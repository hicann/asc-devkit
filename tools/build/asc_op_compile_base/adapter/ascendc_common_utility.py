#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
ascendc common utility
"""

import os
import re
import stat
import subprocess
import copy
import json
import shutil
import struct
from dataclasses import dataclass
from enum import Enum
from typing import Optional, Tuple
from tbe.tvm.contrib.ccec import _build_aicore_compile_cmd, switching_compilation_mode
from tbe.common.buildcfg import get_current_build_config
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from tbe.common.utils.op_tiling import tiling_so_arch_path
from .log_utils import LogUtil, AscendCLogLevel, CompileStage
from .global_storage import global_var_storage
from .ascendc_constants import (
    CORE_TYPE_MIX,
    KernelMetaType,
    InferChannelParamsFromIFile,
    CustomizedConfig,
    TilingKeyConfig,
)

BIN_FILENAME_HASHED_FLAG_ENV = "BIN_FILENAME_HASHED"


class CompileCommandMode(Enum):
    """Control whether a compile command is executed, recorded, or both."""

    EXECUTE = "execute"
    EXECUTE_AND_RECORD = "execute_and_record"
    RECORD_ONLY = "record_only"


@dataclass(frozen=True)
class KernelCompileCommand:
    """Immutable description of one physical kernel compile command."""

    tiling_key: str
    compiled_symbol: str
    core_type: str
    source_path: str
    argv: Tuple[str, ...]
    output_path: str
    compile_tiling_key: Optional[str] = None
    compile_symbol: Optional[str] = None
    object_type: Optional[str] = None
    constant_info_files: Tuple[str, ...] = ()


@dataclass(frozen=True)
class SkBindCommand:
    """Immutable description of the generated SK_BIND compile command."""

    source_path: str
    argv: Tuple[str, ...]
    output_path: str


class CompileCommandSession:
    """Record compile commands and expose whether callers should execute them."""

    def __init__(self, mode=CompileCommandMode.EXECUTE):
        if not isinstance(mode, CompileCommandMode):
            raise TypeError("mode must be a CompileCommandMode")
        self._mode = mode
        self._records = []
        self._sk_bind_records = []

    @property
    def mode(self):
        return self._mode

    @property
    def should_execute(self):
        return self._mode is not CompileCommandMode.RECORD_ONLY

    @property
    def records(self):
        return tuple(self._records)

    @property
    def sk_bind_records(self):
        return tuple(self._sk_bind_records)

    def submit(self, command):
        if self._mode is not CompileCommandMode.EXECUTE:
            self._records.append(command)
        return self.should_execute

    def submit_sk_bind(self, command):
        if self._mode is not CompileCommandMode.EXECUTE:
            self._sk_bind_records.append(command)
        return self.should_execute

    def extend(self, commands):
        if self._mode is not CompileCommandMode.EXECUTE:
            self._records.extend(commands)

    def fork(self):
        # Sub-core compilation inherits the mode but must not share recorded state.
        return CompileCommandSession(self._mode)


class CompileInfo:
    def __init__(self):
        self.src_file: str = ""
        self.dst_file: str = ""
        self.kernel_name: str = ""  # func_name with md5
        self.origin_func_name: str = ""  # func_name written by user, without md5
        self.op_type: str = ""
        self.code_channel: int = CORE_TYPE_MIX
        self.gen_kernel_func_file: str = ""
        self.tiling_key_list: list = None
        self.tiling_key_group_map: dict = {}
        self.is_debug: bool = False
        self.compile_log_path = None
        self.hard_sync: bool = False
        self.enable_deterministic: bool = False
        self.tiling_key_kernel_type: dict = {}
        self.tiling_key_deterministic: dict = {}
        self.raw_tiling_key_kernel_type: dict = {}
        self.no_set_kernel_type: bool = True
        self.default_kernel_type: KernelMetaType = KernelMetaType.KERNEL_TYPE_MAX
        self.dump_info: dict = {}
        self.sub_core_type: int = -1
        self.template_tiling_info: dict = {}
        self.tiling_key_struct_map: dict = {}
        self.register_tiling_struct: set = (
            set()
        )  # tiling struct found in REGISTER_TILING_XXX
        self.tpl_tiling_struct: set = set()  # tiling struct found in TPL
        self.enable_final_super_kernel_compile: bool = False
        # if enable_final_super_kernel_compile is True and super_kernel_objs is empty
        # means no fatbin, dst_file is the final file
        self.super_kernel_objs: list = []
        self.super_kernel_early_start_set_flag: bool = False
        self.super_kernel_early_start_wait_flag: bool = False
        self.super_kernel_info: dict = {}
        self.is_super_kernel_compile = (
            False  # used to judge super kernel compile or sub super kernel compile
        )
        self.max_tiling_size: int = 0
        self.compiled_tiling_key_data_size_map: dict = {}
        self.tiling_and_dfx_utils_file: str = (
            ""  # used when tling struct is not register
        )
        self.tiling_and_dfx_utils_bin_path: str = ""
        self.global_kernel_symbols: list = []
        self.global_kernel_attribute: str = ""
        # Dynamic compilation owns command production; Manifest generation consumes it.
        self.compile_command_session = CompileCommandSession()
        self.last_compiled_symbol: str = ""

    def __str__(self):
        return ",".join(
            "{}={}".format(key, getattr(self, key)) for key in self.__dict__.keys()
        )

    def get_kernel_func_name(self):
        """add "__kernel0" for kernel func name, tbe-dsl do this in build_cce.cc"""
        return self.kernel_name + "__kernel0"


def _trans_compile_cmds_to_precompile(cmds_i: list):
    if len(cmds_i) < 2:
        raise Exception(f"invalid compile cmds = {cmds_i}")
    compiler_index = 0
    for cmd_idx, cmd in enumerate(cmds_i):
        if cmd.endswith(".o"):
            cmds_i[cmd_idx] = cmd[:-2] + ".i"
        if compiler_index == 0 and (cmd.endswith("bisheng") or cmd.endswith("ccec")):
            compiler_index = cmd_idx
    cmds_i.insert(compiler_index + 1, "-E")


def _trans_compile_cmds_to_assemble(cmds_s: list):
    if len(cmds_s) < 2:
        raise Exception(f"invalid compile cmds = {cmds_s}")
    compiler_index = 0
    for cmd_idx, cmd in enumerate(cmds_s):
        if cmd.endswith(".o"):
            cmds_s[cmd_idx] = cmd[:-2] + ".s"
        if compiler_index == 0 and (cmd.endswith("bisheng") or cmd.endswith("ccec")):
            compiler_index = cmd_idx
    cmds_s.insert(compiler_index + 1, "-S")


def write_mk(tiling_key_list, cmds_list, dst, compile_log_path):
    file = f"{dst}.mk"
    try:
        with open(file, mode="wt") as f:
            os.chmod(file, stat.S_IRUSR + stat.S_IWUSR)
            i = 0
            f.write("ALL: ")
            for tiling_key in tiling_key_list:
                f.write(f"{dst}_{tiling_key} ")
            f.write("\n\n")
            for cmds in cmds_list:
                f.write(f"{dst}_{tiling_key_list[i]}:\n")
                if global_var_storage.get_variable("ascendc_compile_debug_config"):
                    cmds_i = copy.deepcopy(cmds)
                    _trans_compile_cmds_to_precompile(cmds_i)
                    cmds_i_str = " ".join(cmds_i)
                    CommonUtility.dump_compile_log(
                        cmds_i, CompileStage.DEBUG_PRECOMPILE, compile_log_path
                    )
                    f.write(f"\t{cmds_i_str}\n")
                cmds_str = " ".join(cmds)
                CommonUtility.dump_compile_log(
                    cmds, CompileStage.COMPILE, compile_log_path
                )
                if CommonUtility.is_v200():
                    f.write(f"\t{cmds_str} 2>/dev/null || ")
                    cmds_list_long = ["-mllvm", "-cce-aicore-jump-expand=true"]
                    cmds_long = copy.deepcopy(cmds)
                    cmds_long.extend(cmds_list_long)
                    cmds_str_long = " ".join(cmds_long)
                    f.write(f" {cmds_str_long}\n")
                    CommonUtility.dump_compile_log(
                        cmds_long, CompileStage.COMPILE, compile_log_path
                    )
                else:
                    f.write(f"\t{cmds_str}\n")
                i += 1
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("write mk file failed, reason is:", err)
        )


class CommonUtility:
    """
    This class defines some common tool function methods.
    """

    def __init__(self):
        pass

    # write the cmpile_cmd to log
    @staticmethod
    def dump_compile_log(compile_cmd, stage: CompileStage, log_file=None):
        LogUtil.dump_compile_log(compile_cmd, stage, log_file)
        return

    @staticmethod
    def dump_log(log_str, log_file=None, level="[INFO] : "):
        LogUtil.dump_log(log_str, log_file, level)
        return

    @staticmethod
    def print_compile_log(kernel_name: str, msg_info: str, log_level: AscendCLogLevel):
        LogUtil.print_compile_log(kernel_name, msg_info, log_level)
        return

    @staticmethod
    def remove_temp_file(file):
        if os.path.isfile(file):
            os.remove(file)

    @staticmethod
    def get_ascendc_compiler_path():
        if global_var_storage.get_variable("ascendc_compiler_path") is not None:
            return
        ascendc_enable_ccache = os.environ.get("ASCENDC_CCACHE_EXECUTABLE")
        global_var_storage.set_variable(
            "ascendc_compiler_path", shutil.which("bisheng")
        )
        if ascendc_enable_ccache is not None and (
            str(ascendc_enable_ccache).endswith("ccache")
        ):
            global_var_storage.set_variable("ascendc_enable_ccache", True)
        if global_var_storage.get_variable("ascendc_compiler_path") is None:
            raise Exception("cannot find bisheng, please set cann pkg env bash.")

    @staticmethod
    def run_cmd_stack_spill_case_ascendc(cmd, cmd_type, cce_file=None):
        """Run a shell commond (only support linux) for stack spill case in
        aicore.

        Parameters
        ----------
        cmd : list
            Command to be run.

        cmd_type : str
            Comand type uesd in running.

        Return
        ------
        out : str
            Standard output.
        returncode : int
            success or fail
        """
        cmd.append("-mllvm")
        cmd.append("-disable-lsr")
        cmd.append("-fno-unroll-loops")
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out, _) = proc.communicate()
        return out, proc.returncode

    @staticmethod
    def run_cmd_stackoverflow_ascnendc(cmd, cmd_type, cce_file=None):
        """Run a shell commond (only support linux) for stack overflow case in
        aicore.
        The current compiler's hardware instructions are not friendly to large
        immediate Numbers.
        Add compile options "-mllvm", "-disable-machine-licm" can avoid this
        problem, but it can degrade performance.

        Parameters
        ----------
        cmd : list
            Command to be run.

        cmd_type : str
            Comand type uesd in running.

        Return
        ------
        out : str
            Standard output.
        returncode : int
            success or fail
        """
        from tbe.common.platform.platform_info import ASCEND_310P
        from tbe.common.platform.platform_info import ASCEND_310

        cmd.insert(3, "-mllvm")
        cmd.insert(4, "-disable-machine-licm")
        if "-cce-aicore-jump-expand=false" in cmd:
            cmd[cmd.index("-cce-aicore-jump-expand=false")] = (
                "-cce-aicore-jump-expand=true"
            )
        if global_var_storage.get_variable("ascendc_short_soc_version") == ASCEND_310P:
            cmd.insert(5, "-mllvm")
            cmd.insert(6, "-cce-aicore-function-stack-size=16000")
        elif global_var_storage.get_variable("ascendc_short_soc_version") == ASCEND_310:
            cmd.insert(5, "-mllvm")
            cmd.insert(6, "-cce-aicore-function-stack-size=8192")
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out, _) = proc.communicate()
        if proc.returncode != 0:
            (out, retcode) = CommonUtility.run_cmd_stack_spill_case_ascendc(
                cmd, cmd_type, cce_file
            )
            return out, retcode
        return out, proc.returncode

    @staticmethod
    def run_cmd_ascendc(cmd, cmd_type):
        """Run a shell commond (only support linux).

        Parameters
        ----------
        cmd : list
            Command to be run.

        cmd_type : str
            Comand type uesd in running.

        Return
        ------
        out : str
            Standard output.
        returncode : int
            success or fail
        """
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out, _) = proc.communicate()
        is_compile_cmd = cmd_type == "compile"
        if proc.returncode != 0:
            if is_compile_cmd and "-O0" in cmd and "-g" in cmd:
                switching_compilation_mode()
                cmd[cmd.index("-O0")] = "-O2"
                del cmd[cmd.index("-g")]
                proc_now = subprocess.Popen(
                    cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
                )
                (out, _) = proc_now.communicate()
                if proc_now.returncode != 0:
                    if "--cce-aicore-only" in cmd:
                        (out_tmp, retcode) = (
                            CommonUtility.run_cmd_stackoverflow_ascnendc(cmd, cmd_type)
                        )
                        return out_tmp, retcode
                return out, proc_now.returncode

            # stack overflow case for aicore compile
            if is_compile_cmd and "--cce-aicore-only" in cmd:
                (out_tmp, retcode) = CommonUtility.run_cmd_stackoverflow_ascnendc(
                    cmd, cmd_type
                )
                if retcode != 0:
                    return out_tmp, retcode
        return out, proc.returncode

    @staticmethod
    def run_cmd_inner(cmds: list, stage: CompileStage, compile_log_path=None):
        if stage == CompileStage.COMPILE:
            if global_var_storage.get_variable("ascendc_compile_debug_config"):
                # common compile .o cmds -> .i cmds
                cmds_i = copy.deepcopy(cmds)
                _trans_compile_cmds_to_precompile(cmds_i)
                CommonUtility.dump_compile_log(
                    cmds_i, CompileStage.DEBUG_PRECOMPILE, compile_log_path
                )
                CommonUtility.run_cmd_ascendc(cmds_i, "compile")
                if CommonUtility.is_c310():
                    # common compile .o cmds -> .s cmds
                    cmds_s = copy.deepcopy(cmds)
                    _trans_compile_cmds_to_assemble(cmds_s)
                    CommonUtility.dump_compile_log(
                        cmds_s, CompileStage.DEBUG_ASSEMBLE, compile_log_path
                    )
                    CommonUtility.run_cmd_ascendc(cmds_s, "compile")
            output, ret_code = CommonUtility.run_cmd_ascendc(cmds, "compile")
            if CommonUtility.is_v200():
                if ret_code != 0:
                    cmds += ["-mllvm", "-cce-aicore-jump-expand=true"]
                    output_long, ret_code = CommonUtility.run_cmd_ascendc(
                        cmds, "compile"
                    )
                    if ret_code == 0:
                        output = output_long
            CommonUtility.dump_compile_log(cmds, CompileStage.COMPILE, compile_log_path)
            CommonUtility.dump_build_log(output.decode(), cmds, stage, ret_code)
            if output.decode().find("unsupported API") != -1:
                CommonUtility.print_compile_log(
                    "", output.decode(), AscendCLogLevel.LOG_ERROR
                )
        elif stage == CompileStage.PRECOMPILE:
            CommonUtility.dump_compile_log(cmds, stage, compile_log_path)
            output, ret_code = CommonUtility.run_cmd_ascendc(cmds, "compile")
            CommonUtility.dump_build_log(output.decode(), cmds, stage, ret_code)
        elif stage == CompileStage.FATBIN:
            CommonUtility.dump_compile_log(cmds, stage, compile_log_path)
            output, ret_code = CommonUtility.run_cmd_ascendc(cmds, "compile")
            CommonUtility.dump_build_log(output.decode(), cmds, stage, ret_code)
        elif stage == CompileStage.LINKRELOCATE:
            CommonUtility.dump_compile_log(cmds, stage, compile_log_path)
            output, ret_code = CommonUtility.run_cmd_ascendc(cmds, "link")
            CommonUtility.dump_build_log(output.decode(), cmds, stage, ret_code)

    @staticmethod
    def get_distinct_filename_tag() -> str:
        if (
            global_var_storage.get_variable("ascendc_enable_ccache") is True
            or os.getenv(BIN_FILENAME_HASHED_FLAG_ENV) == "1"
        ):
            return ""
        return "_" + str(os.getpid())

    @staticmethod
    def ascendc_build_aicore_compile_cmd(src_file, dst_file, name=""):
        return _build_aicore_compile_cmd(src_file, dst_file, name)

    @staticmethod
    def ascendc_raise_python_err(err_code, msg):
        CommonUtility.print_compile_log("", f"{msg}", AscendCLogLevel.LOG_ERROR)
        raise_tbe_python_err(err_code, msg)

    @staticmethod
    def ascendc_read_file(file_path: str):
        try:
            with open(file_path, "r") as f:
                sources = f.readlines()
                return sources
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                (f"read gen_kernel_func_file: {file_path} failed, reason is: {err}"),
            )
        return ""

    @staticmethod
    def ascendc_write_file(file_path: str, context: str):
        try:
            with open(file_path, "w") as f:
                f.writelines(context)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE,
                (f"write gen_kernel_func_file: {file_path} failed, reason is: {err}"),
            )

    @staticmethod
    def is_v220():
        """return if current soc version is V220

        Returns:
            res: True means V220
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend910B", "Ascend910_93"]:
            return True
        return False

    @staticmethod
    def is_support_super_kernel():
        """return if current soc version support super kernel

        Returns:
            res: True means current soc support super kernel
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend910B", "Ascend910_93", "Ascend950"]:
            return True
        return False

    @staticmethod
    def is_support_workspace_offset():
        """return if current soc version support workspace offset way

        Returns:
            res: True means soc version support workspace offset way
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend950", "MC62", "MC32DM11A"]:
            return True
        return False

    @staticmethod
    def is_v200():
        """return if current soc version is V200

        Returns:
            res: True means V200
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend310P", "Ascend610"]:
            return True
        return False

    @staticmethod
    def is_v100():
        """return if current soc version is V100

        Returns:
            res: True means V100
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend910"]:
            return True
        return False

    @staticmethod
    def is_regbase():
        """return if current soc version is V300

        Returns:
            res: True means V300
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend310B", "Ascend610Lite"]:
            return True
        return False

    @staticmethod
    def is_v300():
        """return if current soc version is V300

        Returns:
            res: True means V300
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend310B"]:
            return True
        return False

    @staticmethod
    def is_c310():
        """return if current soc version is c310

        Returns:
            res: True means c310
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Ascend950", "Ascend350"]:
            return True
        return False

    @staticmethod
    def is_has_ffts_mode():
        """return if current soc version is has ffts mode

        Returns:

        Returns:
            res: True means has ffts addr
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version not in ["Ascend950", "Ascend350"]:
            return True
        return False

    @staticmethod
    def is_m510():
        """return if current soc version is m510

        Returns:
            res: True means m510
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["MC62", "MC32DM11A"]:
            return True
        return False

    @staticmethod
    def is_l300():
        """return if current soc version is l300

        Returns:
            res: True means l300
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["KirinX90"]:
            return True
        return False

    @staticmethod
    def is_l311():
        """return if current soc version is l311

        Returns:
            res: True means l311
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["Kirin9030"]:
            return True
        return False

    @staticmethod
    def is_l510():
        """return if current soc version is l510

        Returns:
            res: True means l311
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["KirinDev0000"]:
            return True
        return False

    @staticmethod
    def is_l516():
        """return if current soc version is l516

        Returns:
            res: True means l311
        """
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        if short_soc_version in ["KirinDev0001", "KirinDev0002", "KirinDev0003"]:
            return True
        return False

    @staticmethod
    def get_chip_version():
        """get chip version for (c220/c310/510r2)

        Returns:
            chip_version: chip version
        """
        chip_version = "c220"
        if CommonUtility.is_c310():
            chip_version = "c310"
        elif CommonUtility.is_m510():
            chip_version = "510r2"
        return chip_version

    @staticmethod
    def get_kernel_meta_parent_dir():
        parent_dir = get_current_build_config("kernel_meta_parent_dir")
        try:
            from tbe.common.buildcfg import get_current_build_config as _tbe_get_cfg

            _tbe_parent = _tbe_get_cfg("kernel_meta_parent_dir")
            if _tbe_parent and _tbe_parent != ".":
                parent_dir = _tbe_parent
        except Exception:
            pass
        return parent_dir

    @staticmethod
    def get_kernel_meta_dir():
        """assembly kernel meta directory, and create directory if not exists

        Returns:
            kernel_meta_dir (str): kernel meta directory
        """
        kernel_meta_dir = os.path.join(
            CommonUtility.get_kernel_meta_parent_dir(), "kernel_meta"
        )
        if not os.path.exists(kernel_meta_dir):
            try:
                os.makedirs(
                    kernel_meta_dir,
                    stat.S_IRWXU + stat.S_IRGRP + stat.S_IXGRP,
                    exist_ok=True,
                )
            except OSError as err:
                raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, err)
        return kernel_meta_dir

    @staticmethod
    def remove_options(compile_options: list, to_be_removed: list):
        i = 0
        while i < len(compile_options):
            if compile_options[i] in to_be_removed:
                del compile_options[i]
            else:
                i += 1

    @staticmethod
    def check_debug_options(compile_options: list) -> bool:
        from tbe.common.buildcfg.buildcfg_mapping import op_debug_config

        op_debug_config_info = get_current_build_config(op_debug_config)
        if "ccec_O0" in op_debug_config_info:
            CommonUtility.remove_options(compile_options, ["-O2", "-O3"])
            compile_options.append("-O0")
        if "ccec_g" in op_debug_config_info:
            compile_options.append("-g")
            return True
        if "-g" in compile_options:
            return True
        return False

    @staticmethod
    def get_build_file_name(cmds: list, stage: CompileStage):
        target_name = None
        for cmd in cmds:
            if cmd.endswith(".o") or cmd.endswith(".i"):
                target_name = cmd[:-2]
                break
        if target_name is None:
            return None, None, None
        target_name = target_name.split("/")[-1]
        split_name = target_name.split("_")
        md5sum_pos = 0
        md5sum_len = 32
        # check the splited names from last to avoid kernel name length is 32
        for i in range(len(split_name))[::-1]:
            if len(split_name[i]) == md5sum_len and split_name[i].isalnum():
                md5sum_pos = i
        kernel_name = split_name[md5sum_pos - 1]
        hash_name = split_name[md5sum_pos]
        file_name = "{}_{}.log".format(kernel_name, hash_name)
        if global_var_storage.get_variable("ascendc_build_log_path") is not None:
            file_name = os.path.join(
                global_var_storage.get_variable("ascendc_build_log_path"), file_name
            )
        return file_name, kernel_name, hash_name

    @staticmethod
    def dump_build_log(output: str, cmds: list, stage: CompileStage, ret_code: int):
        file_name, kernel_name, hash_name = CommonUtility.get_build_file_name(
            cmds, stage
        )
        op_name = "{}_{}".format(kernel_name, hash_name)
        if ret_code != 0:
            LogUtil.print_compile_log(op_name, output, AscendCLogLevel.LOG_ERROR)
            if not global_var_storage.get_variable("ascendc_enable_build_log"):
                raise Exception(
                    "An error occurred during compile phases of {}, msg is {}".format(
                        str(stage), output
                    )
                )
        else:
            if cmds[0] != "llvm-objdump" and output != "":
                if "WARNING" in output.upper():
                    LogUtil.print_compile_log(
                        op_name, output, AscendCLogLevel.LOG_WARNING
                    )
                else:
                    LogUtil.print_compile_log(op_name, output, AscendCLogLevel.LOG_INFO)
        if not global_var_storage.get_variable("ascendc_enable_build_log"):
            return
        open_mode = "at"
        if (
            global_var_storage.get_variable("ascendc_build_log_list").count(file_name)
            == 0
        ):
            tmp_log_list = global_var_storage.get_variable("ascendc_build_log_list")
            tmp_log_list.append(file_name)
            global_var_storage.set_variable("ascendc_build_log_list", tmp_log_list)
            open_mode = "wt"
        try:
            with open(file_name, mode=open_mode) as f:
                os.chmod(file_name, stat.S_IRUSR + stat.S_IWUSR)
                if stage == CompileStage.FATBIN or stage == CompileStage.LINKRELOCATE:
                    f.write("[LD] %s\n" % (" ".join(cmds)))
                else:
                    f.write("[CC] %s\n" % (" ".join(cmds)))
                if cmds[0] != "llvm-objdump":
                    f.write(output)
        except Exception as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, ("write log failed, reason is:", err)
            )

        if ret_code != 0:
            file_name_parts = file_name.split(".")
            new_file_name = file_name_parts[0] + "_error." + file_name_parts[-1]
            os.rename(file_name, new_file_name)
            LogUtil.print_compile_log(
                "",
                'Operator {}_{}: errors occurred during compile phase \
of {}, See also "{}"'.format(kernel_name, hash_name, str(stage), new_file_name),
                AscendCLogLevel.LOG_ERROR,
            )
            raise Exception(
                "An error occurred during compile phases of {}, msg is {}".format(
                    str(stage), output
                )
            )
        if stage == CompileStage.LINKRELOCATE and ret_code == 0:
            file_name_parts = file_name.split(".")
            new_file_name = file_name_parts[0] + "_success." + file_name_parts[-1]
            os.rename(file_name, new_file_name)

    @staticmethod
    def parser_uint64_hex_num(hex_num):
        def reverser_hex_str(data):
            hex_num_res = ""
            for i in range(0, len(data), 2):
                hex_num_res = data[i : i + 2] + hex_num_res
            return hex_num_res

        hex_num_str_list = list(map(reverser_hex_str, hex_num[::-1]))
        hex_num_str = "".join(hex_num_str_list)
        return hex_num_str

    @staticmethod
    def get_dump_core_num():
        if CommonUtility.is_c310():
            return 108
        return 75


def is_enable_sanitizer(compile_options):
    """
    get whether enable sanitizer

    Args:
        compile_options (list): compile options for bisheng
    """

    from tbe.common.buildcfg.buildcfg_mapping import op_debug_config

    op_debug_config_val = get_current_build_config(op_debug_config)
    asan_op_debug_config = "sanitizer" in op_debug_config_val
    enable_compile_asan_option = "-sanitizer" in compile_options or asan_op_debug_config
    if "-sanitizer" in compile_options:
        CommonUtility.remove_options(compile_options, ["-sanitizer"])
    if enable_compile_asan_option is False:
        return False
    ascend_home_path = os.environ.get("ASCEND_HOME_PATH")

    if ascend_home_path is None:
        return False
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    pre_asan_obj_path = {}

    js = {
        "Sanitizer": [
            {
                "Ascend910B": [
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-c220-vec.a",
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-c220-cube.a",
                ],
                "Ascend910_93": [
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-c220-vec.a",
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-c220-cube.a",
                ],
                "Ascend310P": [
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-m200.a",
                    "${ASCEND_HOME_PATH}/tools/mssanitizer/lib64/libsanitizer_stub_dav-m200-vec.a",
                ],
                "Ascend950": [],
            }
        ]
    }

    if short_soc_version in js["Sanitizer"][0]:
        pre_asan_obj_path[short_soc_version] = js["Sanitizer"][0][short_soc_version]
    else:
        return False

    soc_obj_path = []
    for obj_path in pre_asan_obj_path[short_soc_version]:
        tmp_path = obj_path.replace("${ASCEND_HOME_PATH}", ascend_home_path)
        if os.path.exists(tmp_path) is False:
            raise Exception("asan obj file not exist")
        soc_obj_path.append(tmp_path)
    global_var_storage.set_variable(
        "ascendc_asan_obj_path", {short_soc_version: soc_obj_path}
    )
    return True


def is_enable_build_log():
    """
    get build log path from env
    """
    build_log_path = os.environ.get("ASCENDC_BUILD_LOG_DIR")
    if build_log_path is None:
        return False
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version is None:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("get soc version failed, reason is:", err)
        )
    build_log_path = os.path.join(build_log_path, short_soc_version.lower())
    if not os.path.exists(build_log_path):
        try:
            os.makedirs(
                build_log_path,
                stat.S_IRWXU + stat.S_IRGRP + stat.S_IXGRP,
                exist_ok=True,
            )
        except OSError as err:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, ("make dir failed, reason is:", err)
            )
    global_var_storage.set_variable("ascendc_build_log_path", build_log_path)
    return True


def is_enable_ascendc_cov():
    """
    get ascendc cov options from env
    """
    ascendc_cov_flag = os.environ.get("ASCENDC_COV")
    if ascendc_cov_flag is None:
        return False
    if int(ascendc_cov_flag) == 1:
        return True
    else:
        return False


def get_op_tiling_so_path(tiling_so_path):
    if not os.path.exists(tiling_so_path):
        tmp_path = ""
        for sub_path in tiling_so_path.split("/"):
            tmp_path += sub_path + "/"
            if sub_path == "tbe":
                break
        tiling_so_path_new = tmp_path + "/".join(tiling_so_arch_path.split("/")[2:])
        return tiling_so_path_new
    else:
        return tiling_so_path


def check_func_align(s):
    try:
        num = int(s)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("Invalid func-align option, reason is: ", err),
        )

    if num < 0:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            f"Invalid func-align option, func-align should be a positive integer, but got {num}",
        )
    elif num > 0 and (num & (num - 1)) != 0:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            f"Invalid func-align option, func-align should be a power of two, but got {num}",
        )
    return


def gen_func_align_attribute(align_size):
    check_func_align(align_size)
    if int(align_size) == 0:
        return ""
    else:
        return f"__attribute__((aligned({align_size})))"


def add_compile_options(tiling_so_path, compile_options: list):
    if not os.path.exists(tiling_so_path):
        CommonUtility.print_compile_log(
            "",
            "Not found tiling so file, use default api soc verison",
            AscendCLogLevel.LOG_WARNING,
        )
        compile_options.append("-DASCENDC_API_VERSION=20250330")
        return

    objdump_cmd = [
        "llvm-objdump",
        "-s",
        "-j",
        ".ascendc.api.version",
        "{}".format(tiling_so_path),
    ]
    proc = subprocess.Popen(
        objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    (out, _) = proc.communicate()

    lines = out.decode("utf-8").split("\n")
    if len(lines) >= 5:
        find_flag = False
        line_content = ""
        for line in lines:
            if ".ascendc.api.version" in line:
                find_flag = True
                continue
            if find_flag is True:
                line_content = line
                find_flag = False
        if line_content == "":
            compile_options.append("-DASCENDC_API_VERSION=20250330")
            CommonUtility.print_compile_log(
                "",
                "not found api version, use default api soc verison",
                AscendCLogLevel.LOG_INFO,
            )
            return
        hex_num = line_content.split(" ")[2:4]
        hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
        bytes_data = bytes.fromhex(hex_num_str)
        dec_data = struct.unpack(">Q", bytes_data)[0]
        compile_options.append(f"-DASCENDC_API_VERSION={dec_data}")
        CommonUtility.print_compile_log(
            "", f"set api soc verison: {dec_data}", AscendCLogLevel.LOG_INFO
        )
    else:
        compile_options.append("-DASCENDC_API_VERSION=20250330")
        CommonUtility.print_compile_log(
            "",
            "not found api version, use default api soc verison",
            AscendCLogLevel.LOG_INFO,
        )


def process_ascendc_api_version(cce_file: str, compile_options: list, extend_options):
    for sub_option in compile_options:
        if "-DASCENDC_API_VERSION" in sub_option:
            CommonUtility.print_compile_log(
                "", "Api soc verison has been set", AscendCLogLevel.LOG_INFO
            )
            return

    pyf_path = ""
    if extend_options.get("opp_kernel_hidden_dat_path", None) is not None:
        for sub_option in compile_options:
            if "../ascendc/common" in sub_option:
                pyf_path = sub_option.replace("../ascendc/common", "")
                pyf_path = pyf_path[2:]

    tiling_so_path = ""
    if pyf_path != "":
        pyf_file_dir = os.path.abspath(pyf_path)
        tiling_so_path = os.path.join(
            pyf_file_dir, "../../" + "/".join(tiling_so_arch_path.split("/")[2:])
        )
    else:
        cce_file_dir = os.path.dirname(os.path.abspath(cce_file))
        tiling_so_path = os.path.join(cce_file_dir, "../../op_tiling/liboptiling.so")
        tiling_so_path = get_op_tiling_so_path(tiling_so_path)
    add_compile_options(tiling_so_path, compile_options)


def convert_customized_config_to_inferchannel(config: CustomizedConfig):
    tiling_key_infos = config.tiling_key_infos
    first_tiling_info = next(iter(tiling_key_infos.values()))
    tiling_key_list = list(tiling_key_infos.keys())
    hard_sync = False
    no_kfc_server_flag = not first_tiling_info.use_kfc
    enable_deterministic = first_tiling_info.enable_deterministic
    tiling_key_kernel_type = {k: v.kernel_type for k, v in tiling_key_infos.items()}
    no_set_kernel_type = False
    default_kernel_type = first_tiling_info.kernel_type
    code_channel = -1
    dump_info = {"dump_type": ""}
    template_tiling_info = {}
    default_tiling_struct = config.default_tiling_struct_name
    tiling_struct_expr_map = {}
    tiling_key_struct_map = {
        k: str(v.tiling_struct_name)
        for k, v in tiling_key_infos.items()
        if str(v.tiling_struct_name) != ""
    }
    register_tiling_struct = set()
    tpl_tiling_struct = set()
    set_task_bar = False
    wait_task_bar = False
    tiling_key_deterministic = {
        k: str(v.enable_deterministic).lower() for k, v in tiling_key_infos.items()
    }

    return InferChannelParamsFromIFile(
        tiling_key_list,
        code_channel,
        hard_sync,
        no_kfc_server_flag,
        enable_deterministic,
        tiling_key_kernel_type,
        no_set_kernel_type,
        default_kernel_type,
        dump_info,
        template_tiling_info,
        default_tiling_struct,
        tiling_struct_expr_map,
        tiling_key_struct_map,
        register_tiling_struct,
        tpl_tiling_struct,
        set_task_bar,
        wait_task_bar,
        tiling_key_deterministic,
        None,
    )


def get_kernel_fun_name_with_tiling_key_and_kernel_type(
    compile_info: CompileInfo, tiling_key: int
):
    full_kernel_name = compile_info.kernel_name
    if (
        re.search(r"mix_aic", full_kernel_name)
        or re.search(r"mix_aiv", full_kernel_name)
    ) and len(full_kernel_name) > 8:
        full_kernel_name = (
            full_kernel_name[:-8] + f"_{tiling_key}" + full_kernel_name[-8:]
        )
        return full_kernel_name
    full_kernel_name += f"_{tiling_key}"
    kernel_type = None
    if compile_info.tiling_key_kernel_type is not None:
        if tiling_key in compile_info.tiling_key_kernel_type.keys():
            kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
    if kernel_type is None:
        raise Exception(
            f"get kernel type failed for tiling key {tiling_key} in TILING_KEY_LIST"
        )
    suffix_marker = ""
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    ]:
        suffix_marker = "_mix_aic"
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0]:
        suffix_marker = "_mix_aiv"
    full_kernel_name += suffix_marker
    return full_kernel_name
