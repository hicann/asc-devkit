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
ascendc compile regbase
"""

import hashlib
import os

from tbe.tvm.contrib.ccec import CCECInfo
from tbe.tvm.runtime.cce_runtime import tvm_callback_cce_postproc
from tbe.common.buildcfg import get_current_build_config
from tbe.common.platform.platform_info import get_soc_spec, set_soc_spec
from tbe.tvm import var
from .log_utils import CompileStage
from .get_op_tiling import TilingInfo
from .global_storage import global_var_storage
from .ascendc_constants import ASCENDC_OOM, TILING_KEY_MACRO
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_compile_dfx import DFXSectionGenerator
from .ascendc_compile_base import (
    SingleTilingKeyCompileParams,
    compile_multi_tilingkey,
    fatbin_objs,
)
from .ascendc_compile_gen_code import _gen_compile_cmd
from .ascendc_compile_gen_json import (
    _dynamic_regbase_kernel_list_to_json,
    _static_regbase_kernel_list_to_json,
    _gen_non_mix_sub_json,
)


def _compile_ascendc_cce_regbase(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    soc_arch_map = {"Ascend310B": "dav-m300", "Ascend610Lite": "dav-m310"}
    arch = soc_arch_map.get(
        global_var_storage.get_variable("ascendc_short_soc_version")
    )
    value = get_soc_spec("cube_vector_combine")
    value_str_list = value.split(",")
    enable_mix_for_profiling = False
    if (
        value_str_list[0] == "unknown"
        or ("fuse" in value_str_list and len(value_str_list)) == 1
    ):
        enable_mix_for_profiling = True
    if enable_mix_for_profiling:
        sub_core_type = "AIC"
        optional_core = "AiCore"
    else:
        sub_core_type = "AIV"
        optional_core = "VectorCore"  # do the same work with SetOptionalCoreType in cpp
    set_soc_spec(optional_core)
    tiling_key_list = _call_bisheng_regbase(
        compile_info, compile_option_tuple, tiling_info, arch, compile_info.code_channel
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_regbase_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            enable_mix_for_profiling,
            compile_info.tiling_key_deterministic,
        )
    else:
        _static_regbase_kernel_list_to_json(compile_info.kernel_name)


def _gen_compile_cmd_regbase(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    sub_arch: str,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the V300 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :param with_tiling_file: whether with the tiling file
    :return: the compile command
    """
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        compile_cmd = [
            os.environ.get("ASCENDC_CCACHE_EXECUTABLE"),
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]
    else:
        compile_cmd = [
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O3",
        ]

    for option in compile_option_tuple.compile_options:
        compile_cmd += [option]
    compile_cmd += [
        src_file,
        "--cce-aicore-arch=%s" % sub_arch,
        "--cce-aicore-only",
        "-o",
        dst_file,
        "-mllvm",
        "-cce-aicore-function-stack-size=16000",
        "--cce-long-call=true",
        "-mllvm",
        "-cce-aicore-addr-transform",
        "-mllvm",
        "--cce-aicore-or-combine=false",
        "-mllvm",
        "-instcombine-code-sinking=false",
        "-mllvm",
        "-cce-aicore-jump-expand=false",
        "-mllvm",
        "-cce-aicore-mask-opt=false",
    ]
    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    if with_tiling_file:
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def _compile_single_tiling_regbase(param: SingleTilingKeyCompileParams):
    dst_file = param.compile_info.dst_file[:-2] + "_%s.o" % param.tiling_key
    compile_cmd = _gen_compile_cmd_regbase(
        param.compile_info.gen_kernel_func_file,
        dst_file,
        param.compile_option_tuple,
        param.sub_arch,
        param.tiling_info.tiling_data_file_path,
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={param.tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = param.tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = param.tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{param.compile_info.origin_func_name}="
            f"{param.compile_info.origin_func_name}_{tiling_data_hash}_{param.tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{param.compile_info.origin_func_name}="
            f"{param.compile_info.origin_func_name}_{param.tiling_key}_tilingkey"
        ]
    kernel_func_name = param.compile_info.kernel_name + "_%s" % param.tiling_key
    compile_cmd += [
        f"-Dauto_gen_{param.compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    section_content = DFXSectionGenerator().generate_dfx_section(
        param.tiling_key, param.tiling_info, kernel_func_name, param.compile_info, True
    )
    return compile_cmd, section_content


def _mssanitizer_link(src_file, dst_file, compile_log_path=None):
    """Build the mssanitize link command before link.
    Parameters
    ----------
    src_file : str
        The src object file.

    dst_file : str
        The dst object file.
    """
    short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
    if short_soc_version not in global_var_storage.get_variable(
        "ascendc_asan_obj_path"
    ):
        raise Exception("asan config file not support asan.a path")
    asan_obj_paths = global_var_storage.get_variable("ascendc_asan_obj_path")[
        short_soc_version
    ]
    if asan_obj_paths == []:
        return
    if not isinstance(src_file, list):
        src_file = [src_file]
    cmd = [CCECInfo.get_exe("ld.lld"), "-m", "aicorelinux", "-r", "-Ttext=0"]
    cmd.extend(src_file)
    cmd.extend(["--dependent-libraries"])
    cmd.extend(asan_obj_paths)
    cmd.extend(
        [
            "-r",
            "-o",
            "%s" % dst_file,
        ]
    )
    CommonUtility.run_cmd_inner(cmd, CompileStage.FATBIN, compile_log_path)


def _call_bisheng_regbase(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    sub_arch: str,
    code_channel: int,
):
    """generate bisheng cmd instead of _build_aicore_compile_cmd, since tbe set davinci-m300-{sub_core} in build_cce.cc

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
        sub_arch (str): m300 arch info
    """
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
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={compile_info.get_kernel_func_name()}"
        ]
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
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key),
            tiling_info,
            compile_info.get_kernel_func_name(),
            compile_info,
            True,
        )
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
        target = "cce_core"
        core_type_info = {var("core_type"): var("")}

        tvm_callback_cce_postproc(
            target,
            compile_info.kernel_name,
            tiling_info.block_num,
            0,
            "",
            None,
            None,
            core_type_info,
            None,
            None,
            False,
            1,
            None,
            None,
        )
    else:
        obj_files = []
        for tiling_key in compile_info.tiling_key_list:
            dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            obj_files.append(dst_file)
        cmds_list = []
        for tiling_key in compile_info.tiling_key_list:
            param = SingleTilingKeyCompileParams(
                tiling_key,
                compile_info,
                sub_arch,
                tiling_info,
                code_channel,
                compile_option_tuple,
            )
            compile_cmd, section_content = _compile_single_tiling_regbase(param)
            cmds_list.append(compile_cmd)
            new_sources += section_content
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )
        # compile binary
        compile_multi_tilingkey(
            compile_info.tiling_key_list,
            cmds_list,
            os.path.basename(compile_info.dst_file)[:-2],
            compile_info.compile_log_path,
        )
        fatbin_objs(
            obj_files,
            compile_info.dst_file,
            compile_info.is_debug,
            compile_info.compile_log_path,
        )
        return compile_info.tiling_key_list
