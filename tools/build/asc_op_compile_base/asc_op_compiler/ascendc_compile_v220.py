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
ascendc compile v220
"""

import os
import stat
import subprocess
import hashlib
from asc_op_compile_base.common.buildcfg import get_current_build_config
from .global_storage import global_var_storage
from .ascendc_common_utility import (
    CommonUtility,
    CompileInfo,
    get_kernel_fun_name_with_tiling_key_and_kernel_type,
)
from .get_op_tiling import TilingInfo
from asc_op_compile_base.common.utils.log_utils import CompileStage
from .ascendc_constants import (
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
    ASCENDC_OOM,
    KernelMetaType,
    MIX_CORE_MACRO,
    TILING_KEY_MACRO,
    InferChannelParams,
)
from .ascendc_compile_base import (
    fatbin_objs,
    compile_multi_tilingkey,
    SingleTilingKeyCompileParams,
)
from .ascendc_compile_dfx import DFXSectionGenerator
from .super_kernel_sub_op_compile import gen_sub_kernel_name


def _gen_compile_cmd_c310(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    sub_arch: str,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the c310 compiler.
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
        "-D__DAV_C310__",
        "--cce-aicore-only",
        "-o",
        dst_file,
        "-mllvm",
        "-cce-aicore-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-function-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-record-overflow=false",
        "-mllvm",
        "-cce-aicore-addr-transform",
        "-mllvm",
        "--cce-aicore-jump-expand=true",
        "-mllvm",
        "-cce-aicore-dcci-insert-for-scalar=false",
    ]
    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        compile_cmd += ["-gline-tables-only"]
        compile_cmd += [
            "-mllvm",
            "-cce-aicore-long-call",
            "-mllvm",
            "-cce-aicore-jump-expand=true",
        ]

    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    if with_tiling_file and tiling_file != "":
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += ["-mllvm", "-cce-block-local-relocate=false"]
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def _gen_compile_cmd_v220(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    sub_arch: str,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the V220 compiler.
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
        "-cce-aicore-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-function-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-record-overflow=false",
        "-mllvm",
        "-cce-aicore-addr-transform",
        "-mllvm",
        "-cce-aicore-dcci-insert-for-scalar=false",
    ]
    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        compile_cmd += ["--cce-enable-sanitizer", "-gline-tables-only"]
        compile_cmd += [
            "-mllvm",
            "-cce-aicore-long-call",
            "-mllvm",
            "-cce-aicore-jump-expand=true",
        ]

    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    if with_tiling_file and tiling_file != "":
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def _gen_compile_cmd_m510(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the m510 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :param with_tiling_file: whether with the tiling file
    :return: the compile command
    """
    """
    Generate the compile command for the m510 compiler.
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
        "--cce-aicore-arch=dav-510r2",
        "--cce-aicore-only",
        "-o",
        dst_file,
        "-mllvm",
        "-cce-aicore-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-function-stack-size=0x8000",
        "-mllvm",
        "-cce-aicore-record-overflow=false",
        "-mllvm",
        "-cce-aicore-addr-transform",
        "-mllvm",
        "--cce-aicore-jump-expand=true",
        "-mllvm",
        "-cce-aicore-dcci-insert-for-scalar=false",
    ]
    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        compile_cmd += ["--cce-enable-sanitizer", "-gline-tables-only"]
        compile_cmd += [
            "-mllvm",
            "-cce-aicore-long-call",
            "-mllvm",
            "-cce-aicore-jump-expand=true",
        ]

    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]

    if with_tiling_file and tiling_file != "":
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    return compile_cmd


def gen_compile_cmd_v220(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    sub_arch: str,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the V220/C310 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :param with_tiling_file: whether with the tiling file
    :return: the compile command
    """
    if CommonUtility.is_c310():
        return _gen_compile_cmd_c310(
            src_file,
            dst_file,
            compile_option_tuple,
            sub_arch,
            tiling_file,
            with_tiling_file,
        )
    elif CommonUtility.is_m510():
        return _gen_compile_cmd_m510(
            src_file, dst_file, compile_option_tuple, tiling_file, with_tiling_file
        )
    else:
        return _gen_compile_cmd_v220(
            src_file,
            dst_file,
            compile_option_tuple,
            sub_arch,
            tiling_file,
            with_tiling_file,
        )


def gen_compile_cmd_for_meta_info(
    src_file: str, dst_file: str, compile_option_tuple, sub_arch: str
):
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
    ]
    if CommonUtility.is_c310():
        compile_cmd += ["-D__DAV_C310__"]
    compile_cmd += ["-std=c++17"]
    return compile_cmd


def get_v220_kernel_type_mix_flag(compile_info: CompileInfo, tiling_info: TilingInfo):
    is_v220_flag = CommonUtility.is_v220() or CommonUtility.is_c310()
    kernel_type_res = 0
    is_single_and_using_hard_sync = (
        is_v220_flag
        and compile_info.hard_sync
        and compile_info.code_channel in [CORE_TYPE_VEC, CORE_TYPE_CUBE]
    )
    if is_v220_flag is False:
        return False, is_single_and_using_hard_sync

    is_mix = compile_info.code_channel == CORE_TYPE_MIX
    if compile_info.no_set_kernel_type:
        return is_mix, is_single_and_using_hard_sync

    used_tiling_keys = []
    if tiling_info.static_shape_flag:
        used_tiling_keys.append(str(tiling_info.tiling_key))
    else:
        used_tiling_keys = compile_info.tiling_key_list

    for tiling_key in used_tiling_keys:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        if kernel_type == KernelMetaType.KERNEL_TYPE_AIV_ONLY:
            kernel_type_res = kernel_type_res | 0x1
        elif kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY:
            kernel_type_res = kernel_type_res | 0x2
        elif (
            kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC
            or kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
        ):
            kernel_type_res = kernel_type_res | 0x4
        else:
            kernel_type_res = kernel_type_res | 0x8
    if kernel_type_res == 2 or kernel_type_res == 1:
        return False, False
    else:
        return True, is_single_and_using_hard_sync


def set_dynamic_sub_func_names_of_super_kernel_with_kernel_type(
    tiling_key, arch, kernel_type, kernel_func_name
):
    sub_super_kernel_func_names = global_var_storage.get_variable(
        "ascendc_sub_super_kernel_fun_names"
    )
    sub_super_kernel_func_names.setdefault("dynamic_func_names", {}).setdefault(
        tiling_key, {}
    )[arch] = kernel_func_name
    sub_super_kernel_func_names["dynamic_func_names"][tiling_key]["kernel_type"] = (
        kernel_type
    )
    global_var_storage.set_variable(
        "ascendc_sub_super_kernel_fun_names", sub_super_kernel_func_names
    )
    return


def set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group(
    tiling_key, arch, kernel_type, kernel_func_name, compile_info: CompileInfo
):
    set_dynamic_sub_func_names_of_super_kernel_with_kernel_type(
        tiling_key, arch, kernel_type, kernel_func_name
    )
    if (
        compile_info.tiling_key_group_map is not None
        and len(compile_info.tiling_key_group_map) > 0
    ):
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                kernel_type_slave = compile_info.tiling_key_kernel_type[
                    str(tiling_key_slave)
                ]
                kernel_func_name = get_kernel_fun_name_with_tiling_key_and_kernel_type(
                    compile_info, str(tiling_key_slave)
                )
                set_dynamic_sub_func_names_of_super_kernel_with_kernel_type(
                    tiling_key_slave, arch, kernel_type_slave.name, kernel_func_name
                )
    return


def set_dynamic_sub_func_names_of_super_kernel(
    tiling_key, compile_info, arch, kernel_func_name
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is False:
        return
    if compile_info.code_channel == CORE_TYPE_MIX or (
        compile_info.hard_sync
        and compile_info.code_channel in [CORE_TYPE_VEC, CORE_TYPE_CUBE]
    ):
        kernel_type = (
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2.name
            if compile_info.code_channel == CORE_TYPE_MIX
            else (
                KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0.name
                if compile_info.code_channel == CORE_TYPE_VEC
                else KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0.name
            )
        )
    else:
        kernel_type = (
            KernelMetaType.KERNEL_TYPE_AIC_ONLY.name
            if arch == "dav-c220-cube"
            else KernelMetaType.KERNEL_TYPE_AIV_ONLY.name
        )
        arch = "AiCore"
    sub_super_kernel_func_names = global_var_storage.get_variable(
        "ascendc_sub_super_kernel_fun_names"
    )
    sub_super_kernel_func_names.setdefault("dynamic_func_names", {}).setdefault(
        tiling_key, {}
    )[arch] = kernel_func_name
    sub_super_kernel_func_names["dynamic_func_names"][tiling_key]["kernel_type"] = (
        kernel_type
    )
    global_var_storage.set_variable(
        "ascendc_sub_super_kernel_fun_names", sub_super_kernel_func_names
    )


def compile_single_tiling_v220(param: SingleTilingKeyCompileParams):
    dst_file = param.compile_info.dst_file[:-2] + "_%s.o" % param.tiling_key
    compile_cmd = gen_compile_cmd_v220(
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
    if param.code_channel == CORE_TYPE_MIX or (
        param.compile_info.hard_sync
        and param.compile_info.code_channel in [CORE_TYPE_VEC, CORE_TYPE_CUBE]
    ):
        kernel_func_name = (
            param.compile_info.kernel_name[:-7]
            + param.tiling_key
            + param.compile_info.kernel_name[-8:]
        )
    else:
        kernel_func_name = param.compile_info.kernel_name + "_%s" % param.tiling_key
    if param.code_channel == CORE_TYPE_MIX:
        compile_cmd += [f"-D{MIX_CORE_MACRO}={1}"]
    if CommonUtility.is_c310() or CommonUtility.is_m510():
        if param.code_channel == CORE_TYPE_MIX:
            compile_cmd += ["-D__ASCENDC_ENABLE_VEC_TAIL_TILING_COPY__"]
        raw_kernel_type = param.compile_info.raw_tiling_key_kernel_type.get(
            str(param.tiling_info.tiling_key)
        )
        if raw_kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY:
            compile_cmd += ["-DRAW_AIC_ONLY_DUMP_TENSOR"]
    compile_cmd += [
        f"-Dauto_gen_{param.compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    set_dynamic_sub_func_names_of_super_kernel(
        param.tiling_key, param.compile_info, param.sub_arch, kernel_func_name
    )
    return compile_cmd, kernel_func_name


def gen_current_kernel_name(
    compile_info: CompileInfo, sub_arch: str, code_channel: int
):
    if code_channel == CORE_TYPE_MIX or (
        compile_info.hard_sync
        and compile_info.code_channel in [CORE_TYPE_VEC, CORE_TYPE_CUBE]
    ):
        # if code_mix, do not add __kernel0 when compile
        current_kernel_name = compile_info.kernel_name
        current_kernel_name = gen_sub_kernel_name(
            current_kernel_name,
            sub_arch,
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2.name
            if code_channel == CORE_TYPE_MIX
            else (
                KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0.name
                if code_channel == CORE_TYPE_VEC
                else KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0.name
            ),
            compile_info.dst_file,
        )
    else:
        current_kernel_name = compile_info.get_kernel_func_name()
        if sub_arch is not None:
            current_kernel_name = gen_sub_kernel_name(
                current_kernel_name,
                "AiCore",
                KernelMetaType.KERNEL_TYPE_AIC_ONLY.name
                if "cube" in sub_arch
                else KernelMetaType.KERNEL_TYPE_AIV_ONLY.name,
                compile_info.dst_file,
            )
    return current_kernel_name


def get_compile_cmd_for_kernel_name(
    compile_info: CompileInfo,
    current_kernel_name: str,
    code_channel: int,
    tiling_info: TilingInfo,
):
    compile_cmd = [
        f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
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
    if code_channel == CORE_TYPE_MIX:
        compile_cmd += [f"-D{MIX_CORE_MACRO}={1}"]
    if CommonUtility.is_c310() or CommonUtility.is_m510():
        if code_channel == CORE_TYPE_MIX:
            compile_cmd += ["-D__ASCENDC_ENABLE_VEC_TAIL_TILING_COPY__"]
        raw_kernel_type = compile_info.raw_tiling_key_kernel_type.get(
            str(tiling_info.tiling_key)
        )
        if raw_kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY:
            compile_cmd += ["-DRAW_AIC_ONLY_DUMP_TENSOR"]
    return compile_cmd


def call_bisheng_v220(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    sub_arch: str,
    code_channel: int,
):
    """generate bisheng cmd instead of _build_aicore_compile_cmd, since tbe set davinci-c220-{sub_core} in build_cce.cc

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
        sub_arch (str): davinci-c220-cube or davinci-c220-vec
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    new_sources = sources[:-1]
    if tiling_info.static_shape_flag:
        compile_cmd = gen_compile_cmd_v220(
            compile_info.gen_kernel_func_file,
            compile_info.dst_file,
            compile_option_tuple,
            sub_arch,
            tiling_info.tiling_data_file_path,
        )
        # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
        current_kernel_name = gen_current_kernel_name(
            compile_info, sub_arch, code_channel
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.append(current_kernel_name)
        compile_cmd += get_compile_cmd_for_kernel_name(
            compile_info, current_kernel_name, code_channel, tiling_info
        )
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key),
            tiling_info,
            current_kernel_name,
            compile_info,
            True,
        )
        if compile_info.code_channel == CORE_TYPE_MIX:
            if "vec" in sub_arch:
                new_sources += global_var_storage.get_variable("ascendc_meta_info")
        else:
            new_sources += global_var_storage.get_variable("ascendc_meta_info")
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

        compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_info.tiling_key}UL"]
        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
        return [f"{tiling_info.tiling_key}"]
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
            compile_cmd, kernel_name = compile_single_tiling_v220(param)
            new_sources += DFXSectionGenerator().generate_dfx_section(
                tiling_key, tiling_info, kernel_name, compile_info, True
            )
            cmds_list.append(compile_cmd)
        if compile_info.code_channel == CORE_TYPE_MIX:
            if "vec" in sub_arch:
                new_sources += global_var_storage.get_variable("ascendc_meta_info")
        else:
            new_sources += global_var_storage.get_variable("ascendc_meta_info")
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

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


def get_ktype_section_head(variable_name: str):
    section_var_head = ""
    if "mix_aic" in variable_name:
        if CommonUtility.is_v220():
            section_var_head += "#if (defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201)\n"
        elif CommonUtility.is_c310():
            section_var_head += "#if (defined(__DAV_CUBE__) && __NPU_ARCH__ == 3510)\n"
    elif "mix_aiv" in variable_name:
        if CommonUtility.is_v220():
            section_var_head += "#if (defined(__DAV_VEC__) && __NPU_ARCH__ == 2201)\n"
        elif CommonUtility.is_c310():
            section_var_head += "#if (defined(__DAV_VEC__) && __NPU_ARCH__ == 3510)\n"
    return section_var_head


def get_ktype_section_variable(
    variable_name: str, section_func_name: str, kernel_meta_type: KernelMetaType
):
    section_var = ""
    if kernel_meta_type == KernelMetaType.KERNEL_TYPE_AIV_ONLY:
        section_var += (
            f"static const struct FunLevelKType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_AIV} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY:
        section_var += (
            f"static const struct FunLevelKType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_AIC} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC:
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIV_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 0, 1} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC:
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 0} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0:
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIV_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 0, 1} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0:
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 0} };\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1:
        section_var += get_ktype_section_head(variable_name)
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 1} };\n"
        section_var += "#endif\n"
    elif kernel_meta_type == KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2:
        section_var += get_ktype_section_head(variable_name)
        section_var += (
            f"static const struct FunLevelMixCoreType {variable_name} __attribute__ "
        )
        section_var += f'((used, section (".ascend.meta.{section_func_name}"))) = '
        section_var += "{ {{F_TYPE_KTYPE, sizeof(unsigned int)}, K_TYPE_MIX_AIC_MAIN},\
    {{F_TYPE_MIX_TASK_RATION, sizeof(unsigned int)}, 1, 2} };\n"
        section_var += "#endif\n"
    else:
        raise Exception(f"invalid kernel meta type: {kernel_meta_type}")
    return section_var


def _is_hard_sync_instr(inst: str):
    if inst[6:] != "41":
        return False
    if inst[4] == "e" and (int(inst[5], 16) & 0x4 == 0x4):
        # WAIT_FLAG_DEV -> wait_flag_dev
        return True
    if inst[4] == "f" and (int(inst[5], 16) & 0x8 == 0x0):
        # SET_CROSS_CORE -> ffts_cross_core_sync
        return True
    return False


def v220_mode(inst) -> int:
    if len(inst) != 8:
        return 0
    if inst[6] == "f" or inst[6] == "c":
        # inst[6] == 'f' means MATRIX and inst[6] == 'c' means FIXP
        return CORE_TYPE_CUBE
    elif inst[6] == "8" or inst[6] == "9":
        is_move_mask = (
            inst[4] == "4" and inst[5] == "0" and inst[6] == "8" and inst[7] == "0"
        )
        if is_move_mask:
            return 0
        # SIMD INST
        return CORE_TYPE_VEC
    # DMA MOVE
    elif inst[6] == "6":
        if inst[7] == "b" and (int(inst[4], 16) & 0x8) == 0x8:
            # MOV_{SRC}_TO_{DST}_ALIGN
            return CORE_TYPE_VEC
        else:
            # MOV_CUB1
            return CORE_TYPE_CUBE
    # DMA MOVE
    elif inst[6] == "7":
        if inst[7] == "0" and (int(inst[4], 16) & 0x8) == 0x8:
            # MOV_UB_TO_XX
            return CORE_TYPE_VEC
        elif (int(inst[0], 16) & 0x7) == 0 and (int(inst[1], 16) & 0x8) == 0x8:
            # MOV_XX_TO_UB
            return CORE_TYPE_VEC
        else:
            # MOV_CUB2
            return CORE_TYPE_CUBE
    # SCALAR
    return 0


def v310_mode_vec_ofile(little_endian, binary_32) -> int:
    vf_high_low_map = {
        "01000010011000100": "00",  # wait_intra_block
        "01000010010000100": "00",  # set_intra_block
        "01000000101000100": "00",  # set_flag
        "01000000110000100": "00",  # wait_flag
    }
    # DMA
    vec_high_low_map = {
        "0110101010": "1",  # ND_DMA_DCI
    }
    vec_high_map = {
        "0110111001",  # ND_DMA_OUT_TO_UB
        "0111010010",  # MOV_OUT_TO_UB_ALIGN_V2
        "0111010011",  # MOV_UB_TO_OUT_ALIGN_V2
    }
    vec_high_mid_map = {
        "011100001": "0100",  # MOV_UB_TO_L1
    }
    high_9 = binary_32[:9]
    high_10 = binary_32[:10]
    mid_36 = binary_32[25:29]
    low_1 = binary_32[31]

    conditions = [
        # PIPE_V: exclude movemask
        (
            little_endian[0] == "1"
            and little_endian[1] == "5"
            and not (
                binary_32[:11] == "00010101110"
                and binary_32[16:26] == "0000000000"
                and binary_32[27:] == "10011"
            )
        ),
        # set, wait
        (
            binary_32[:17] in vf_high_low_map
            and binary_32[30:] == vf_high_low_map[binary_32[:17]]
        ),
        # DMA
        (high_10 in vec_high_low_map and low_1 == vec_high_low_map[high_10]),
        (high_10 in vec_high_map),
        (high_9 in vec_high_mid_map and mid_36 == vec_high_mid_map[high_9]),
    ]

    if any(conditions):
        return CORE_TYPE_VEC

    return 0


def v310_mode_cube_ofile(little_endian, binary_32) -> int:
    cube_high_low_map = {
        "0110000001": "10",  # SET_L1_2D
        "0110011000": "10",  # LOAD_OUT_TO_L1_2Dv2
    }
    cube_high_low2_map = {
        "0110110100": "1",  # LOAD_L1_TO_L0B_2D_TRANSPOSE
        "0110110101": "1",
        "0110110110": "1",
        "0110110111": "1",
    }
    cube_high_map = {
        "0110110000",  # LOAD_L1_TO_L0A_2Dv2和LOAD_L1_TO_L0B_2Dv2
        "0110110001",
        "0110110010",
        "0110110011",
        "0110101000",  # MOV_OUT _TO_L1 _MULTI_DN2NZ
        "0110101100",  # MOV_OUT _TO_L1 _MULTI_ND2NZ
        "0110111010",  # MOV_OUT_TO_L1_V2
        "0111011000",  # LOAD_L1_TO_L0A_MX_2Dv2和LOAD_L1_TO_L0B_MX_2Dv2
        "0110011100",  # LOAD_L1_TO_L0A_3Dv2和LOAD_L1_TO_L0B_3Dv2
        "0110011101",
        "0110010100",
        "0110010101",
    }
    high_10 = binary_32[:10]

    conditions = [
        # Fixpipe
        (little_endian[0] == "c" and little_endian[1] in "0123"),
        # matrix instr
        (
            little_endian[0] == "e"
            and little_endian[1] in "012345"
            and binary_32[30:] == "00"
        ),
        (little_endian[0] == "f" and little_endian[1] in "01"),
        (
            little_endian[0] == "f"
            and little_endian[1] == "6"
            and binary_32[30:] == "00"
        ),
        (
            little_endian[0] == "f"
            and little_endian[1] in "2345abcd"
            and binary_32[30] == "0"
        ),
        # DMA
        (
            binary_32[:9] == "011100100" and binary_32[25:29] in ("0001", "0101")
        ),  # DMA move inst, include MOV L1 TO UB
        (high_10 in cube_high_low_map and binary_32[30:] == cube_high_low_map[high_10]),
        (
            high_10 in cube_high_low2_map
            and binary_32[31] == cube_high_low2_map[high_10]
        ),
        (high_10 in cube_high_map),
    ]

    if any(conditions):
        return CORE_TYPE_CUBE

    return 0


def v310_mode(inst, cubemode) -> int:
    if len(inst) != 8:
        return 0

    little_endian = f"{int.from_bytes(int(inst, 16).to_bytes(4, 'little'), 'big'):08x}"
    binary_32 = bin(int(little_endian, 16))[2:].zfill(32)

    return (
        v310_mode_cube_ofile(little_endian, binary_32)
        if cubemode
        else v310_mode_vec_ofile(little_endian, binary_32)
    )


def decode_mode(mode) -> int:
    # 3 means insts contain cube and vector instruct
    if mode == 3:
        return CORE_TYPE_MIX
    # 0 means insts is scalar instruct
    elif mode == 0:
        return CORE_TYPE_VEC
    else:
        return mode


def get_code_channel_v220_by_first_tiling_key(params: InferChannelParams):
    if params.tiling_info.static_shape_flag:
        # static shape use tiling_key itself
        infer_tiling_key = f"{params.tiling_info.tiling_key}"
    else:
        # dynamic shape use first tiling_key
        infer_tiling_key = params.tiling_key
    hardware_sync_in_asm = False
    mode = 0
    chip_version = CommonUtility.get_chip_version()
    for arch in [f"dav-{chip_version}-cube", f"dav-{chip_version}-vec"]:
        dst_file = params.dst_file_header + f"_{arch}_{params.tiling_key}.o"
        compile_cmd = gen_compile_cmd_v220(
            params.src_file,
            dst_file,
            params.compile_option_tuple,
            arch,
            params.tiling_info.tiling_data_file_path,
        )
        compile_cmd += [
            f"-D{TILING_KEY_MACRO}={infer_tiling_key}UL",
            "-DASCENDC_DUMP=0",
            "-UASCENDC_ACC_DUMP",
        ]
        CommonUtility.remove_options(
            compile_cmd, ["-DASCENDC_DUMP", "-DASCENDC_DUMP=1"]
        )
        compile_cmd += ["-D__GET_CODE_CHANNEL__"]
        if "vec" in arch and params.no_kfc_server_flag:
            compile_cmd += ["-D__NO_KFC_SERVER__"]
        CommonUtility.dump_compile_log(
            compile_cmd, CompileStage.INFERCHANNEL, params.compile_log_path
        )
        proc = subprocess.Popen(
            compile_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        )
        (out, _) = proc.communicate()
        CommonUtility.dump_build_log(
            out.decode(), compile_cmd, CompileStage.INFERCHANNEL, proc.returncode
        )
        objdump_cmd = ["llvm-objdump", "-s", "-j", ".text", "{}".format(dst_file)]

        CommonUtility.dump_compile_log(
            objdump_cmd, CompileStage.INFERCHANNEL, params.compile_log_path
        )
        proc = subprocess.Popen(
            objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        )
        (out, _) = proc.communicate()
        CommonUtility.dump_build_log(
            out.decode(), objdump_cmd, CompileStage.INFERCHANNEL, proc.returncode
        )

        os.chmod(dst_file, stat.S_IRUSR | stat.S_IWUSR)
        if not global_var_storage.get_variable("ascendc_compile_debug_config"):
            os.remove(dst_file)
        lines = out.decode("utf-8").split("\n")
        for line in lines:
            insts = line.strip().split()
            if len(insts) < 5:
                continue
            for inst in insts[1:5]:
                hardware_sync_in_asm = hardware_sync_in_asm or _is_hard_sync_instr(inst)
                if CommonUtility.is_c310():
                    mode |= v310_mode(inst, arch == f"dav-{chip_version}-cube")
                else:
                    mode |= v220_mode(inst)
    code_channel = decode_mode(mode)
    return code_channel, hardware_sync_in_asm
