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
ascendc compile v200
"""

import os
import hashlib
from asc_op_compile_base.common.buildcfg import get_current_build_config
from .get_op_tiling import TilingInfo
from asc_op_compile_base.common.utils.log_utils import CompileStage
from .global_storage import global_var_storage
from .ascendc_constants import ASCENDC_OOM
from .ascendc_compile_base import SingleTilingKeyCompileParams
from .ascendc_compile_dfx import DFXSectionGenerator
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_constants import (
    ASCENDC_OOM,
    KernelMetaType,
    TILING_KEY_MACRO,
    KERNEL_TYPE_TO_STR,
)


def judge_valid_for_v200(tiling_key_kernel_type):
    kernel_type_res = 0
    for kernel_type in tiling_key_kernel_type.values():
        if kernel_type == KernelMetaType.KERNEL_TYPE_AICORE:
            kernel_type_res |= 0b1
        elif kernel_type == KernelMetaType.KERNEL_TYPE_VECTORCORE:
            kernel_type_res |= 0b10
        elif kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AICORE:
            kernel_type_res |= 0b100
        elif kernel_type == KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
            kernel_type_res |= 0b1000
        else:
            raise Exception(
                f"kernel type {KERNEL_TYPE_TO_STR[kernel_type]} is unvaild in v200"
            )

    if kernel_type_res == 0b1:
        return "AiCore"
    elif kernel_type_res == 0b10:
        return "VectorCore"
    elif kernel_type_res in [0b100, 0b101]:
        return "MIX_AICORE"
    elif kernel_type_res in [0b1000, 0b1001]:
        return "MIX_VECTOR_CORE"
    else:
        raise Exception(f"kernel type {kernel_type_res} combination is unvaild in v200")


def gen_compile_cmd_v200(
    src_file: str, dst_file: str, compile_option_tuple, sub_arch: str, tiling_file: str
):
    """
    Generate the compile command for the V200 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :return: the compile command
    """
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        compile_cmd = [
            os.environ.get("ASCENDC_CCACHE_EXECUTABLE"),
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O2",
        ]
    else:
        compile_cmd = [
            global_var_storage.get_variable("ascendc_compiler_path"),
            "-c",
            "-O2",
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
        "-cce-aicore-record-overflow=false",
        "-mllvm",
        "-cce-aicore-mask-opt=false",
    ]

    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        compile_cmd += ["--cce-enable-sanitizer", "-g"]
        compile_cmd += [
            "-mllvm",
            "-cce-aicore-long-call",
            "-mllvm",
            "-cce-aicore-jump-expand=true",
        ]
    else:
        if "-cce-aicore-jump-expand=true" not in compile_option_tuple.compile_options:
            compile_cmd += ["-mllvm", "-cce-aicore-jump-expand=false"]

    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    compile_cmd += ["--cce-mask-opt"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def call_bisheng_v200_dynamic(param: SingleTilingKeyCompileParams, kernel_type):
    dst_file = param.compile_info.dst_file[:-2] + "_%s.o" % param.tiling_key
    compile_cmd = gen_compile_cmd_v200(
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
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
    ]:
        kernel_func_name = (
            param.compile_info.kernel_name[:-7]
            + param.tiling_key
            + param.compile_info.kernel_name[-8:]
        )
    else:
        kernel_func_name = param.compile_info.kernel_name + "_%s" % param.tiling_key
    compile_cmd += [
        f"-Dauto_gen_{param.compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    section_content = DFXSectionGenerator().generate_dfx_section(
        param.tiling_key, param.tiling_info, kernel_func_name, param.compile_info, True
    )
    return dst_file, compile_cmd, section_content


def call_bisheng_v200_static(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    sub_arch: str,
    kernel_type: KernelMetaType,
):
    """generate static bisheng cmd instead of _build_aicore_compile_cmd
        since tbe set davinci-m200-{sub_core} in build_cce.cc

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
        sub_arch (str): davinci-m200-cube or davinci-m200-vec
    """
    compile_cmd = gen_compile_cmd_v200(
        compile_info.gen_kernel_func_file,
        compile_info.dst_file,
        compile_option_tuple,
        sub_arch,
        tiling_info.tiling_data_file_path,
    )
    # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
    ]:
        kernel_name = compile_info.kernel_name
    else:
        kernel_name = compile_info.get_kernel_func_name()
    compile_cmd += [f"-Dauto_gen_{compile_info.origin_func_name}_kernel={kernel_name}"]
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

    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    new_sources = sources[:-1]
    new_sources += DFXSectionGenerator().generate_dfx_section(
        str(tiling_info.tiling_key), tiling_info, kernel_name, compile_info, True
    )
    new_sources += "#endif\n"
    # add dfx info section to sourse file
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)
    CommonUtility.run_cmd_inner(
        compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
    )
    return
