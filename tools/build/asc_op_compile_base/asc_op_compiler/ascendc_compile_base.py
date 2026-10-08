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
ascendc compile base
"""

import os
import hashlib
import stat
import shutil
import subprocess
import multiprocessing
import re
from collections import namedtuple
from dataclasses import dataclass
from asc_op_compile_base.asc_op_compiler import cce_runtime
from asc_op_compile_base.common.buildcfg import get_current_build_config
from asc_op_compile_base.common.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from asc_op_compile_base.common.ccec import CCECInfo
from asc_op_compile_base.common.platform import COMPILER_ARCH, get_soc_spec
from .ascendc_common_utility import (
    CommonUtility,
    CompileInfo,
    write_mk,
    is_enable_ascendc_cov,
    is_enable_build_log,
    is_enable_sanitizer,
)
from .global_storage import global_var_storage
from asc_op_compile_base.common.utils.log_utils import AscendCLogLevel, CompileStage
from .get_op_tiling import OpInfo, TilingInfo
from .ascendc_constants import KernelMetaType, CORE_TYPE_MIX, CORE_TYPE_CUBE, CORE_TYPE_VEC, TILING_KEY_MACRO
from .ascendc_kernel_feature_manager import global_ascendc_kernel_feature_manager
from .ascendc_compile_gen_code import _gen_compile_cmd
from .ascendc_compile_dfx import DFXSectionGenerator
from asc_op_compile_base.asc_op_compiler.cce_runtime import tvm_callback_cce_postproc


def compile_pre_process(op_info: OpInfo, compile_options: list):
    cce_runtime.TBE_WORKSPACE_SIZE_LIST.local_list = []
    cce_runtime.TBE_WORKSPACE_IND_LIST.local_list = []
    cce_runtime.MULTI_CORE_SYNC_WORKSPACE_SIZE_LIST.local_list = []
    cce_runtime.TBE_ATUO_ATOMIC_IND_LIST.local_list = []
    CommonUtility.get_ascendc_compiler_path()
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        CommonUtility.remove_options(compile_options, ["-x", "cce"])
        compile_options.append("--cce-aicore-lang")
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import op_debug_config

    op_debug_config_val = get_current_build_config(op_debug_config)
    compile_options.append("--cce-disable-kernel-global-attr-check")
    global_var_storage.set_variable("ascendc_enable_super_kernel", False)
    global_var_storage.set_variable("ascendc_sk_double_compile", False)
    global_var_storage.set_variable("ascendc_sk_sub_combine_norm_workflow", False)
    global_var_storage.set_variable("ascendc_sub_super_kernel_params", "")
    global_var_storage.set_variable("ascendc_sub_super_kernel_type", "")
    global_var_storage.set_variable("ascendc_sub_super_kernel_fun_names", {})
    global_var_storage.set_variable("ascendc_compile_debug_config", "dump_cce" in op_debug_config_val)
    global_var_storage.set_variable("ascendc_dump_disable_compile_options", "-DASCENDC_DUMP=0" in compile_options)
    global_var_storage.set_variable("ascendc_debug_compile_options", "-DASCENDC_DEBUG" in compile_options)
    global_var_storage.set_variable("ascendc_enable_sanitizer", is_enable_sanitizer(compile_options))
    global_var_storage.set_variable("ascendc_enable_build_log", is_enable_build_log())
    global_var_storage.set_variable("ascendc_enable_coverage", is_enable_ascendc_cov())
    global_var_storage.set_variable("ascendc_time_stamp_compile_options", "-DASCENDC_TIME_STAMP_ON" in compile_options)
    global_var_storage.set_variable(
        "ascendc_enable_super_kernel",
        (bool(get_current_build_config("enable_super_kernel")) and CommonUtility.is_support_super_kernel()),
    )
    global_var_storage.set_variable(
        "ascendc_enable_aicore_exception_restart", "-DAICORE_EXCEPTION_RESTART" in compile_options
    )
    if global_var_storage.get_variable("ascendc_enable_coverage"):
        compile_options.append("-g")
    global_ascendc_kernel_feature_manager.init_available_and_enable_features()
    return compile_options


def get_actual_kernel_type(tiling_key, compile_info, need_ffts, kernel_name):
    code_type = compile_info.code_channel
    default_kernel_type = compile_info.default_kernel_type
    kernel_type = (
        compile_info.tiling_key_kernel_type[tiling_key]
        if tiling_key in compile_info.tiling_key_kernel_type
        else default_kernel_type
    )
    if kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0]:
        return CORE_TYPE_CUBE
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0]:
        return CORE_TYPE_VEC
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2, KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1]:
        return CORE_TYPE_MIX
    if compile_info.no_set_kernel_type and need_ffts:
        return code_type
    else:
        CommonUtility.print_compile_log(
            kernel_name, "Aicore Exception Restart not support this kernel type", AscendCLogLevel.LOG_ERROR
        )
        raise Exception("Aicore Exception Restart not support this kernel type")


SingleTilingKeyCompileParams = namedtuple(
    "SingleTilingKeyCompileParams",
    ["tiling_key", "compile_info", "sub_arch", "tiling_info", "code_channel", "compile_option_tuple"],
)


def link_resource_id_obj(bin_file_path, resource_object, is_debug, compile_log_path=None):
    """Merge the Resource ID metadata object into the kernel object."""
    link_cmd = [CCECInfo.get_exe("ld.lld"), "-m", "aicorelinux", "-r", "-Ttext=0", "-q"]
    if not is_debug:
        link_cmd.append("-x")
    link_cmd += ["%s" % bin_file_path, "%s" % resource_object, "-static", "-o", "%s" % bin_file_path]
    CommonUtility.run_cmd_inner(link_cmd, CompileStage.SPECIALIZATION, compile_log_path)
    if not global_var_storage.get_variable("ascendc_compile_debug_config"):
        CommonUtility.remove_temp_file(resource_object)


def fatbin_objs(obj_files: list, dst_file: str, is_debug: bool, compile_log_path=None):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True and global_var_storage.get_variable(
        "ascendc_is_static_op"
    ):
        return
    compile_cmd = [CCECInfo.get_exe("ld.lld"), "-m", "aicorelinux", "-r", "-Ttext=0", "-q"]
    if not is_debug:
        compile_cmd.append("-x")
    for obj in obj_files:
        compile_cmd += [obj]
    compile_cmd += ["-static", "-o", "%s" % dst_file]
    CommonUtility.run_cmd_inner(compile_cmd, CompileStage.FATBIN, compile_log_path)
    if not global_var_storage.get_variable("ascendc_compile_debug_config") and not global_var_storage.get_variable(
        "super_kenel_save_sub_op_files"
    ):
        for obj in obj_files:
            os.remove(obj)


def link_relocatable(bin_file_path, compile_log_path=None):
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version == "Ascend310B":
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            "%s" % bin_file_path,
            "-static",
            "-o",
            "%s" % bin_file_path,
            "-q",
        ]
    else:
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            "%s" % bin_file_path,
            "-static",
            "-o",
            "%s" % bin_file_path,
            "-q",
        ]
    CommonUtility.run_cmd_inner(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)


def link_relocatable_meta_file(bin_file_path, meta_file_path, compile_log_path=None):
    link_cmd = [
        CCECInfo.get_exe("ld.lld"),
        "-m",
        "aicorelinux",
        "-Ttext=0",
        "%s" % bin_file_path,
        "%s" % meta_file_path,
        "-static",
        "-o",
        "%s" % bin_file_path,
        "-q",
    ]
    CommonUtility.run_cmd_inner(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)


def link_sk_norm_combine(sk_bin_file, norm_bin_file, sk_bind_dst_file, meta_file_path, compile_log_path=None):
    # Step 1: 解压 sk_bin_file (它是由 ar crs 打包的 .o 文件)
    # 创建临时目录用于解压
    temp_extract_dir = os.path.join(os.path.dirname(sk_bin_file), "temp_extract_" + str(os.getpid()))
    os.makedirs(temp_extract_dir, exist_ok=True)

    try:
        # 解压 sk_bin_file 到临时目录
        CommonUtility.print_compile_log(
            "", f"Extracting sk_bin_file: {sk_bin_file} to {temp_extract_dir}", AscendCLogLevel.LOG_DEBUG
        )
        extract_cmd = ["ar", "x", sk_bin_file]
        CommonUtility.dump_compile_log(extract_cmd, CompileStage.LINKRELOCATE, compile_log_path)
        result = subprocess.run(extract_cmd, cwd=temp_extract_dir, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f"Failed to extract sk_bin_file: {result.stderr}")

        # 列出解压出的所有 .o 文件
        extracted_objs = [os.path.join(temp_extract_dir, f) for f in os.listdir(temp_extract_dir) if f.endswith(".o")]
        CommonUtility.print_compile_log(
            "", f"Extracted object files: {[os.path.basename(f) for f in extracted_objs]}", AscendCLogLevel.LOG_DEBUG
        )

        # Step 2: 将解压的 .o 文件、norm_bin_file、sk_bind_dst_file 和可选的 meta_file_path 合并
        merged_obj = os.path.join(temp_extract_dir, "merged_sk_norm_bind.o")
        link_cmd = [CCECInfo.get_exe("ld.lld"), "-r", "-o", merged_obj]
        if CommonUtility.is_c310():
            link_cmd.extend([norm_bin_file, sk_bind_dst_file])
            link_cmd.extend(extracted_objs)
        else:
            link_cmd.extend(extracted_objs)
            link_cmd.extend([norm_bin_file, sk_bind_dst_file])
        if meta_file_path:
            link_cmd.append(meta_file_path)
        CommonUtility.dump_compile_log(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)
        result = subprocess.run(link_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f"Failed to link merged objects: {result.stderr}")

        # Step 3: 自连接 merged_obj 生成最终的 sk_bin_file
        link_cmd = [
            CCECInfo.get_exe("ld.lld"),
            "-m",
            "aicorelinux",
            "-Ttext=0",
            merged_obj,
            "-static",
            "-o",
            sk_bin_file,
            "-q",
        ]
        CommonUtility.dump_compile_log(link_cmd, CompileStage.LINKRELOCATE, compile_log_path)
        result = subprocess.run(link_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f"Failed to self-link: {result.stderr}")

        CommonUtility.print_compile_log(
            "", f"Successfully created final sk_bin_file: {sk_bin_file}", AscendCLogLevel.LOG_INFO
        )

    finally:
        CommonUtility.print_compile_log("", "Successfully sk combine link", AscendCLogLevel.LOG_INFO)


def _get_max_parallel_num():
    cpu_db_num = max(1, 2 * multiprocessing.cpu_count() - 2)
    cpu_sched_num = max(1, 2 * len(os.sched_getaffinity(0)) - 2)
    return min(cpu_db_num, cpu_sched_num)


def _ignore_parallel_job_self_set():
    ascend_self_par_job = os.getenv("TILINGKEY_PARALLEL_JOB")
    ascend_self_par_job_compatible = os.getenv("ASCENDC_PAR_COMPILE_JOB")
    if ascend_self_par_job is not None:
        CommonUtility.print_compile_log(
            "",
            "Detected that the TILINGKEY_PAR_COMPILE enviroment variable has been set, \
ignoring TILINGKEY_PARALLEL_JOB.",
            AscendCLogLevel.LOG_WARNING,
        )
    elif ascend_self_par_job_compatible is not None:
        CommonUtility.print_compile_log(
            "",
            "Detected that the TILINGKEY_PAR_COMPILE enviroment variable has been set, \
ignoring ASCENDC_PAR_COMPILE_JOB.",
            AscendCLogLevel.LOG_WARNING,
        )


def _get_parallel_job_without_op_project(tilingkey_num: int = 1):
    ascend_self_par_job_compatible = os.getenv("ASCENDC_PAR_COMPILE_JOB")
    if ascend_self_par_job_compatible is not None:
        CommonUtility.print_compile_log(
            "",
            "ASCENDC_PAR_COMPILE_JOB enviroment variable is deprecated, \
please use TILINGKEY_PARALLEL_JOB instead!",
            AscendCLogLevel.LOG_WARNING,
        )
    # formally
    ascend_self_par_job = os.getenv("TILINGKEY_PARALLEL_JOB")
    max_job_num = _get_max_parallel_num()
    ascendc_self_par_job_num = 1
    if ascend_self_par_job_compatible is not None:
        ascendc_self_par_job_num = int(ascend_self_par_job_compatible)
        if ascendc_self_par_job_num == 1:
            ascendc_self_par_job_num = max_job_num
        else:
            ascendc_self_par_job_num = min(max(1, ascendc_self_par_job_num), max_job_num)
    elif ascend_self_par_job is not None:
        ascendc_self_par_job_num = min(max(1, int(ascend_self_par_job)), max_job_num)
    else:
        # default is 64 or max job, which may cause host memory exaust
        default_job = 8 if (CommonUtility.is_v100() or CommonUtility.is_v200()) else 64
        ascendc_self_par_job_num = max(1, min(default_job, max_job_num, tilingkey_num))
        CommonUtility.print_compile_log(
            "",
            f"tiling key num is {tilingkey_num}, \
tilingkey parallel compile num is {ascendc_self_par_job_num}",
            AscendCLogLevel.LOG_INFO,
        )
    return ascendc_self_par_job_num


def compile_multi_tilingkey(tiling_key_list, cmds_list, dstfile_name, compile_log_path):
    parallel_compile_check = os.getenv("TILINGKEY_PAR_COMPILE")
    if parallel_compile_check not in [None, "1", "0"]:
        CommonUtility.print_compile_log(
            "",
            "TILINGKEY_PAR_COMPILE ONLY SUPPORT 0 OR 1, current \
TILINGKEY_PAR_COMPILE is {}".format(parallel_compile_check),
            AscendCLogLevel.LOG_WARNING,
        )

    dstfile_with_pid = os.path.join(CommonUtility.get_kernel_meta_dir(), dstfile_name + "_" + str(os.getpid()))
    write_mk(tiling_key_list, cmds_list, dstfile_with_pid, compile_log_path)
    mk_file = f"{dstfile_with_pid}.mk"
    if parallel_compile_check == "1":
        _ignore_parallel_job_self_set()
        cmd = ["make", "-f", mk_file]
    else:
        ascendc_self_par_job_num = _get_parallel_job_without_op_project(len(tiling_key_list))
        cmd = ["make", "-f", mk_file, "-j", f"{ascendc_self_par_job_num}"]
    cmd_str = " ".join(cmd)
    file_name = ""
    if global_var_storage.get_variable("ascendc_enable_build_log") is True:
        file_name, kernel_name, hash_name = CommonUtility.get_build_file_name(cmds_list[0], CompileStage.COMPILE)
        try:
            with open(file_name, mode="at") as f:
                os.chmod(file_name, stat.S_IRUSR + stat.S_IWUSR)
                f.write("%s\n" % (cmd_str))
                cmd.append("2>&1")
                cmd.append("|")
                cmd.append("tee -a")
                cmd.append(file_name)
                cmd_str = " ".join(cmd)
        except Exception as err:
            raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, ("write log failed, reason is:", err))
    ret = os.system(f"{cmd_str} > /dev/null")
    if ret != 0 and global_var_storage.get_variable("ascendc_enable_build_log") is True:
        file_name_parts = file_name.split(".")
        new_file_name = file_name_parts[0] + "_error." + file_name_parts[-1]
        os.rename(file_name, new_file_name)
        CommonUtility.print_compile_log(
            "",
            "Operator {}_{}: errors occurred during compile phase \
of {}, See also {}".format(kernel_name, hash_name, str(CompileStage.COMPILE), new_file_name),
            AscendCLogLevel.LOG_ERROR,
        )
        raise Exception("An error occurred during compile phases of {}".format(str(CompileStage.COMPILE)))
    if not global_var_storage.get_variable("ascendc_compile_debug_config"):
        CommonUtility.remove_temp_file(mk_file)


def search_in_line(line, keywords):
    pattern = re.compile(r"\b(" + "|".join(re.escape(keyword) for keyword in keywords) + r")\b")
    matches = pattern.findall(line)
    if matches:
        return True, f"{', '.join(matches)}"
    return False, ""


def extract_file_path(line):
    pattern = re.compile(r'"([^"]+)"')
    matches = pattern.findall(line)
    return matches[0]


def _kernel_type_for_key(compile_info: CompileInfo, tiling_key: str):
    """Return the explicit Kernel type registered for one Tiling Key."""

    kernel_type = compile_info.tiling_key_kernel_type.get(str(tiling_key))
    if kernel_type is None:
        raise ValueError(f"kernel type is unavailable for tiling key {tiling_key}")
    return kernel_type


def get_compile_core_types(compile_info: CompileInfo, tiling_key: str):
    """Resolve the physical cube/vector compile targets for one Tiling Key."""

    if compile_info.no_set_kernel_type:
        core_types = {CORE_TYPE_MIX: ("cube", "vec"), CORE_TYPE_CUBE: ("cube",), CORE_TYPE_VEC: ("vec",)}
        try:
            return core_types[compile_info.code_channel]
        except KeyError as error:
            raise ValueError(f"unsupported code channel: {compile_info.code_channel}") from error

    kernel_type = _kernel_type_for_key(compile_info, tiling_key)
    if kernel_type in {
        KernelMetaType.KERNEL_TYPE_AIC_ONLY,
        KernelMetaType.KERNEL_TYPE_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    }:
        return ("cube",)
    if kernel_type in {
        KernelMetaType.KERNEL_TYPE_AIV_ONLY,
        KernelMetaType.KERNEL_TYPE_VECTORCORE,
        KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0,
    }:
        return ("vec",)
    if kernel_type in {
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        KernelMetaType.KERNEL_TYPE_MIX_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
    }:
        return ("cube", "vec")
    raise ValueError(f"unsupported kernel type: {kernel_type}")


def add_op_system_run_cfg_option(compile_cmd, compile_info, tiling_info, tiling_key, sub_arch, *, definition_key):
    """Add the g_opSystemRunCfg definition macro to its designated compile command.

    Args:
        compile_cmd: Mutable compiler argument list to update.
        compile_info: Compile metadata, including tiling keys and core channel.
        tiling_info: Tiling metadata used to identify static shapes.
        tiling_key: Tiling key associated with this compile command.
        sub_arch: Target sub-architecture, such as ``davinci-c220-cube``.
        definition_key: Tiling key that owns the global definition.
    """
    if not (CommonUtility.is_v220() or CommonUtility.is_v200()):
        return
    option = "-D__ASCENDC_DEFINE_OP_SYSTEM_RUN_CFG__"
    compile_cmd[:] = [arg for arg in compile_cmd if arg != option]
    if str(tiling_key) != str(definition_key):
        return
    core_types = get_compile_core_types(compile_info, definition_key)
    definition_core = "cube" if "cube" in core_types else "vec"
    if sub_arch == "dav-m200":
        # V200 AiCore has no "-cube" suffix; use "cube" as its compile target name.
        current_core = "cube"
    elif sub_arch:
        current_core = sub_arch.rsplit("-", 1)[-1]
    else:
        current_core_types = get_compile_core_types(compile_info, tiling_key)
        current_core = "cube" if "cube" in current_core_types else "vec"
    if current_core != definition_core:
        return
    source_index = (
        compile_cmd.index(compile_info.gen_kernel_func_file)
        if compile_info.gen_kernel_func_file in compile_cmd
        else len(compile_cmd)
    )
    compile_cmd.insert(source_index, option)


def _compile_single_tiling(tiling_key, compile_info, tiling_info, compile_option_tuple):
    dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
    compile_cmd = _gen_compile_cmd(
        compile_info.gen_kernel_func_file, dst_file, compile_option_tuple, tiling_info.tiling_data_file_path
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{compile_info.origin_func_name}="
            f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [f"-D{compile_info.origin_func_name}={compile_info.origin_func_name}_{tiling_key}_tilingkey"]
    kernel_func_name = compile_info.kernel_name + "_%s" % tiling_key
    compile_cmd += [f"-Dauto_gen_{compile_info.origin_func_name}_kernel={kernel_func_name}"]
    section_content = DFXSectionGenerator().generate_dfx_section(
        tiling_key, tiling_info, kernel_func_name, compile_info, True
    )
    return compile_cmd, section_content


def _compile_ascendc_cce(compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    # JSON postprocessing imports base helpers through super_kernel_utility.
    from .ascendc_compile_gen_json import _dynamic_kernel_list_to_json

    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    if tiling_info.static_shape_flag:
        compile_cmd = _gen_compile_cmd(
            compile_info.gen_kernel_func_file,
            compile_info.dst_file,
            compile_option_tuple,
            tiling_info.tiling_data_file_path,
        )
        # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
        compile_cmd += [f"-Dauto_gen_{compile_info.origin_func_name}_kernel={compile_info.get_kernel_func_name()}"]
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            tiling_data_hash_src = tiling_info.tiling_data
            if isinstance(tiling_data_hash_src, str):
                tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
            elif not tiling_data_hash_src:
                tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
            tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
            compile_cmd += [
                f"-D{compile_info.origin_func_name}="
                f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_info.tiling_key}_tilingkey"
            ]
        compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_info.tiling_key}UL"]
        add_op_system_run_cfg_option(
            compile_cmd, compile_info, tiling_info, tiling_info.tiling_key, None, definition_key=tiling_info.tiling_key
        )
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key), tiling_info, compile_info.get_kernel_func_name(), compile_info, True
        )
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

        CommonUtility.run_cmd_inner(compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path)
        target = "cce_core"
        tvm_callback_cce_postproc(target, compile_info.kernel_name, tiling_info.block_num)
    else:
        obj_files = []
        for tiling_key in compile_info.tiling_key_list:
            dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            obj_files.append(dst_file)
        cmds_list = []
        for tiling_key in compile_info.tiling_key_list:
            compile_cmd, section_content = _compile_single_tiling(
                tiling_key, compile_info, tiling_info, compile_option_tuple
            )
            add_op_system_run_cfg_option(
                compile_cmd, compile_info, tiling_info, tiling_key, None, definition_key=compile_info.tiling_key_list[0]
            )
            cmds_list.append(compile_cmd)
            new_sources += section_content
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)
        # compile binary
        compile_multi_tilingkey(
            compile_info.tiling_key_list,
            cmds_list,
            os.path.basename(compile_info.dst_file)[:-2],
            compile_info.compile_log_path,
        )
        fatbin_objs(obj_files, compile_info.dst_file, compile_info.is_debug, compile_info.compile_log_path)
        target = "cce_core"
        tvm_callback_cce_postproc(target, compile_info.kernel_name, tiling_info.block_num)
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            compile_info.tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )
