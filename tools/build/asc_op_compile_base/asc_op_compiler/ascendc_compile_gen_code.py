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
ascendc compile gen code
"""

import re
import os
from functools import reduce
from asc_op_compile_base.common.buildcfg import get_current_build_config
from .get_op_tiling import TilingInfo
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_constants import (
    TILING_KEY_MACRO,
    CORE_TYPE_CUBE,
    INPUT_OUTPUT_DTYPE_LEN,
    ASCENDC_OOM,
    KernelMetaType,
    CORE_TYPE_VEC,
    CORE_TYPE_MIX,
)
from .get_op_tiling import OpInfo
from .global_storage import global_var_storage
from .ascendc_compile_dfx import DFXSectionGenerator
import stat
import struct
import subprocess

from asc_op_compile_base.common.context import get_context
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel
from .ascendc_constants import MIX_CORE_MACRO, TILING_KEY_SEARCH_KEYWORD
from .ascendc_common_utility import (
    gen_func_align_attribute,
    get_kernel_fun_name_with_tiling_key_and_kernel_type,
)
from .ascendc_compile_base import get_actual_kernel_type
from .ascendc_compile_dfx import (
    DFXArgInfo,
    DFXParamType,
    DFXPointType,
)
from .ascendc_compile_utils import check_custom_dcci_end_false, check_if_gen_placehoder
from .ascendc_compile_v220 import get_v220_kernel_type_mix_flag


def add_time_stamp_codes(desc_id, space_len: int = 1):
    source = "#ifdef ASCENDC_TIME_STAMP_ON\n"
    source += (
        "    " * space_len
        + f"AscendC::PrintTimeStamp(static_cast<uint32_t>(AscendC::TimeStampId::{desc_id}));\n"
    )
    source += "#endif\n"
    return source


def gen_init_dump_code():
    return "    AscendC::SetSysWorkspaceForce(workspace);\n"


def gen_usr_origin_kernel_function_call(
    func_name: str, opinfo: OpInfo, tiling_info: TilingInfo, has_template: bool = False
):
    # call usr kernel function
    if has_template:
        source = f"    {func_name}<TEMPLATE_PARAMS>("
    else:
        source = f"    {func_name}("
    for origin_input in opinfo.origin_inputs:
        if origin_input is not None:
            if isinstance(origin_input, (list, tuple)):
                if len(origin_input) == 0:
                    source += " nullptr, "
                else:
                    source += "{}, ".format(origin_input[0]["param_name"])
            else:
                source += "{}, ".format(origin_input["param_name"])
        else:
            source += " nullptr, "
    for output in opinfo.outputs:
        if output is not None:
            source += "{}, ".format(output["param_name"])
        else:
            source += " nullptr, "

    if (
        opinfo.output_shape_depend_on_compute is not None
        and len(opinfo.output_shape_depend_on_compute) > 0
    ):
        source += "__ascendc_output_shape, "

    # static shape need pass nullptr
    if tiling_info.static_shape_flag:
        source += "usrWorkspace, nullptr);\n"
    else:
        source += "usrWorkspace, tiling);\n"
    return source


def gen_template_tiling_params(compile_info):
    source = "#define TEMPLATE_PARAMS -1\n"
    source += "#define TEMPLATE_PARAMS_LEN 0\n\n"
    if not compile_info.template_tiling_info:
        return source
    for (
        template_tiling_key,
        template_tiling_info,
    ) in compile_info.template_tiling_info.items():
        if not template_tiling_info or not template_tiling_info.get("paramArgs", []):
            continue
        template_tiling_info_str = ", ".join(
            [str(i) for i in template_tiling_info.get("paramArgs", [])]
        )
        source += f"#if {TILING_KEY_MACRO} == {template_tiling_key}UL\n"
        source += "#undef TEMPLATE_PARAMS\n"
        source += f"#define TEMPLATE_PARAMS {template_tiling_info_str}\n"
        source += "#undef TEMPLATE_PARAMS_LEN\n"
        source += f"#define TEMPLATE_PARAMS_LEN {len(template_tiling_info.get('paramArgs', []))}\n"
        source += "#endif\n\n"
    return source


def gen_global_isolation_macro(compile_info: CompileInfo, tiling_info: TilingInfo):
    tiling_key = compile_info.tiling_key_list[0]
    if tiling_info.static_shape_flag:
        tiling_key = tiling_info.tiling_key

    if CommonUtility.is_v220():
        macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_VEC__) && __NPU_ARCH__ == 2201)\n"
        # judge operator is aic only
        if compile_info.no_set_kernel_type is False:
            kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
            if kernel_type.value in [1, 3, 5, 6, 7]:
                macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201)\n"
        elif compile_info.code_channel == CORE_TYPE_CUBE:
            macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201)\n"
    elif CommonUtility.is_v200():
        macro_branch_statment = (
            f"#if {TILING_KEY_MACRO} == {tiling_key}UL && defined(__DAV_M200__)\n"
        )
        if compile_info.no_set_kernel_type is False:
            kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
            if kernel_type.value in [9]:
                macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && defined(__DAV_M200_VEC__)\n"
    elif CommonUtility.is_c310():
        macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_VEC__) && __NPU_ARCH__ == 3510)\n"
        # judge operator is aic only
        if compile_info.no_set_kernel_type is False:
            kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
            if kernel_type.value in [1, 3, 5, 6, 7]:
                macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_CUBE__) && __NPU_ARCH__ == 3510)\n"
        elif compile_info.code_channel == CORE_TYPE_CUBE:
            macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL && (defined(__DAV_CUBE__) && __NPU_ARCH__ == 3510)\n"
    else:
        macro_branch_statment = f"#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
    return macro_branch_statment


def get_code_for_l2_cache(compile_info: CompileInfo, source, tiling_info: TilingInfo):
    source += gen_global_isolation_macro(compile_info, tiling_info)
    source += f"    __gm__ struct OpSystemRunCfg g_opSystemRunCfg = {{{0}}};\n"
    source += "#else\n"
    source += "    extern __gm__ struct OpSystemRunCfg g_opSystemRunCfg;\n"
    source += "#endif\n\n"
    return source


def skip_mc2_context_size(opinfo: OpInfo):
    content = ""
    if opinfo.mc2_ctx:
        for _ in opinfo.mc2_ctx:
            content += "    tmpTilingSizeForOOM += 8;\n"
    return content


def match_options(options, compile_options):
    result = []
    for option in options:
        match = re.search(rf"{option}=(\w+)", " ".join(compile_options))
        if match:
            result.append(match.group(1))
        else:
            result.append(None)
    return result


def add_dtype_fmt_option_single(x_n, is_ref: bool = False):
    options = []
    x_n_in_kernel = x_n + "_REF" if is_ref else x_n
    options.append("DORIG_DTYPE_{n}".format(n=x_n_in_kernel))
    return options


def get_dtype_fmt_options(opinfo: OpInfo):
    options = []
    unique_param_name_set = set()

    inputs_length = len(opinfo.inputs)
    for idx, x in enumerate(opinfo.inputs):
        if x is None:
            options += [None]
            continue
        unique_param_name_set.add(x["param_name"])
        if opinfo.param_type_list[idx] == "dynamic":
            tmp = x["param_name"]
            res = tmp[: tmp.index("_in")].upper()
            options += add_dtype_fmt_option_single(res)
        else:
            options += [None]

    for idx, x in enumerate(opinfo.outputs):
        if x is None:
            options += [None]
            continue
        if opinfo.param_type_list[idx + inputs_length] == "dynamic":
            tmp = x["param_name"]
            res = tmp[: tmp.index("_out")].upper()
            if x["param_name"] in unique_param_name_set:
                options += add_dtype_fmt_option_single(res, True)
            else:
                options += add_dtype_fmt_option_single(res)
        else:
            options += [None]
    return options


data_type_map = {
    "DT_INT4": (2 << 16) + 1,
    "DT_INT8": 1,
    "DT_UINT8": 1,
    "DT_FLOAT16": 2,
    "DT_BF16": 2,
    "DT_INT16": 2,
    "DT_UINT16": 2,
    "DT_FLOAT": 4,
    "DT_INT32": 4,
    "DT_UINT32": 4,
    "DT_INT64": 8,
    "DT_UINT64": 8,
    "DT_HIFLOAT8": 1,
    "DT_DOUBLE": 8,
    "DT_BOOL": 1,
    "DT_COMPLEX64": 8,
    "DT_UINT1": (8 << 16) + 1,
    "DT_INT2": (4 << 16) + 1,
    "DT_COMPLEX32": 4,
    "DT_FLOAT8_E5M2": 1,
    "DT_FLOAT8_E4M3FN": 1,
    "DT_FLOAT8_E8M0": 1,
    "DT_FLOAT4_E2M1": (2 << 16) + 1,
    "DT_FLOAT4_E1M2": (2 << 16) + 1,
}


def get_value(key):
    if key in data_type_map:
        return data_type_map[key]
    else:
        return None


def update_tiling_size_for_oom(
    compile_info: CompileInfo, tiling_info: TilingInfo, dyn_input_shape_offset
):
    content = ""
    # use user-defined tiling struct
    if len(compile_info.tiling_key_struct_map) > 0:
        for tiling_key in tiling_info.tiling_key_list:
            content += f"#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
            content += f"    uint64_t tmpTilingSizeForOOM = sizeof({compile_info.tiling_key_struct_map[tiling_key]});\n"
            content += "    tmpTilingSizeForOOM = (tmpTilingSizeForOOM + 7) / 8 * 8;\n"
            content += "#endif\n"
    elif global_var_storage.get_variable("ascendc_tiling_no_register"):
        for tiling_key in tiling_info.tiling_key_list:
            content += f"#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
            content += f"    uint64_t tmpTilingSizeForOOM = g_custom_tiling_size_meta_{tiling_key};\n"
            content += "    tmpTilingSizeForOOM = (tmpTilingSizeForOOM + 7) / 8 * 8;\n"
            content += "#endif\n"
    else:
        if len(tiling_info.tiling_key_data_size) == 0:
            content += "    uint64_t tmpTilingSizeForOOM = {};\n".format(
                int(dyn_input_shape_offset)
            )
        else:
            for tiling_key in tiling_info.tiling_key_list:
                content += f"#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
                if tiling_key not in tiling_info.tiling_key_data_size:
                    content += "    uint64_t tmpTilingSizeForOOM = {};\n".format(
                        int(tiling_info.default_tiling_size)
                    )
                else:
                    content += "    uint64_t tmpTilingSizeForOOM = {};\n".format(
                        int(tiling_info.tiling_key_data_size[tiling_key])
                    )
                content += "#endif\n"
    return content


def set_workspace_param(opinfo: OpInfo, tiling_info: TilingInfo):
    # set workspace addr && workspace len
    source = ""
    if tiling_info.static_shape_flag:
        if (
            opinfo.output_shape_depend_on_compute is not None
            and len(opinfo.output_shape_depend_on_compute) > 0
        ):
            # each output needs 9 uint64 elements
            output_shape_len = (
                (9 * 8 * len(opinfo.output_shape_depend_on_compute) + 32 - 1) // 32 * 32
            )
            source += (
                "    AscendC::OOMCheckAddrRange(__ascendc_output_shape, {});\n".format(
                    output_shape_len
                )
            )
        source += "    AscendC::OOMCheckAddrRange(workspace, {});\n".format(
            tiling_info.static_workspace_size
        )
    else:
        if (
            opinfo.output_shape_depend_on_compute is not None
            and len(opinfo.output_shape_depend_on_compute) > 0
        ):
            output_shape_len = (
                "*((__gm__ uint64_t *)((__gm__ uint8_t *)tiling + tmpTilingSizeForOOM))"
            )
            source += (
                "    AscendC::OOMCheckAddrRange(__ascendc_output_shape, {});\n".format(
                    output_shape_len
                )
            )
            source += "    tmpTilingSizeForOOM += 8;\n"
        workspace_len = (
            "*((__gm__ uint64_t *)((__gm__ uint8_t *)tiling + tmpTilingSizeForOOM))"
        )
        source += "    AscendC::OOMCheckAddrRange(workspace, {});\n".format(
            workspace_len
        )
    source += "#endif\n"
    return source


def add_op_param_to_workspace(
    opinfo: OpInfo,
    tiling_info: TilingInfo,
    source: str,
    compile_options: list,
    compile_info: CompileInfo,
):
    input_output_info = []
    for io_info in [opinfo.inputs, opinfo.outputs]:
        if list(io_info):
            input_output_info += io_info
    dyn_input_shape_offset = tiling_info.tiling_data_size
    dyn_input_shape_offset = (dyn_input_shape_offset + 8 - 1) // 8 * 8
    count = 0
    source += "#if defined(ASCENDC_OOM) && ASCENDC_OOM == 1\n"
    source += "    AscendC::OOMInit();\n"

    options = get_dtype_fmt_options(opinfo)
    dtype_char = match_options(options, compile_options)
    dtype_int = list(map(get_value, dtype_char))

    source += update_tiling_size_for_oom(
        compile_info, tiling_info, dyn_input_shape_offset
    )
    source += skip_mc2_context_size(opinfo)

    for io_index, op_param in enumerate(input_output_info):
        if op_param is None:
            continue
        if opinfo.param_type_list[io_index] == "dynamic":
            if dtype_int[io_index]:
                source += "    AscendC::OOMCheckTensorListRange({}, {});\n".format(
                    op_param.get("param_name"), dtype_int[io_index]
                )
        else:
            if tiling_info.static_shape_flag:
                input_shape_len = reduce(
                    lambda x, y: x * y, op_param.get("shape")
                ) * INPUT_OUTPUT_DTYPE_LEN.get(op_param.get("dtype"))
                input_shape_len = (input_shape_len + 32 - 1) // 32 * 32
            else:
                input_shape_len = "*((__gm__ uint64_t *)((__gm__ uint8_t *)tiling + tmpTilingSizeForOOM))"
            source += "    AscendC::OOMCheckAddrRange({}, {});\n".format(
                op_param.get("param_name"), input_shape_len
            )
        source += "    tmpTilingSizeForOOM += 8;\n"
        count = count + 1
    source += set_workspace_param(opinfo, tiling_info)
    count = count + 1
    if count > 128:
        raise Exception("input and output num exceed 128")
    return source


def _gen_compile_cmd(
    src_file: str,
    dst_file: str,
    compile_option_tuple,
    tiling_file: str,
    with_tiling_file: bool = True,
):
    """
    Generate the compile command for the v100/v200 compiler.
    :param src_file: the source file
    :param dst_file: the destination file
    :param extra_options: the extra options
    :param with_tiling_file: whether with the tiling file
    :return: the compile command
    """
    jump_expand_flag = (
        "-cce-aicore-jump-expand=true" in compile_option_tuple.compile_options
    )
    compile_cmd = CommonUtility.ascendc_build_aicore_compile_cmd(src_file, dst_file, "")
    if global_var_storage.get_variable("ascendc_enable_ccache") == True:
        compile_cmd = [os.environ.get("ASCENDC_CCACHE_EXECUTABLE")] + compile_cmd
    to_del_idx = []
    for cmd_idx, cmd in enumerate(compile_cmd):
        if "-fcce-vf-vl=256" in cmd:
            to_del_idx.append(cmd_idx - 1)
            to_del_idx.append(cmd_idx)
        if "-cce-aicore-fp-ceiling" in cmd:
            to_del_idx.append(cmd_idx - 1)
            to_del_idx.append(cmd_idx)
        # whether auto sync or not, it should be ascendc`s charge
        elif "--cce-auto-sync" in cmd:
            to_del_idx.append(cmd_idx)
        # if customize set op jump open, then change jump expand setting which was auto generated
        elif (
            jump_expand_flag
            or global_var_storage.get_variable("ascendc_enable_sanitizer")
        ) and "-cce-aicore-jump-expand=false" == cmd:
            compile_cmd[cmd_idx] = "-cce-aicore-jump-expand=true"
        elif cmd == "ccec":
            compile_cmd[cmd_idx] = global_var_storage.get_variable(
                "ascendc_compiler_path"
            )
    for idx in reversed(to_del_idx):
        del compile_cmd[idx]

    # v100 / v200 add stack size compile_cmd = [cmd.replace('16000', '32000') for cmd in compile_cmd]
    compile_cmd_front = compile_cmd[:3]
    compile_cmd_backend = compile_cmd[3:]
    for option in compile_option_tuple.compile_options:
        compile_cmd_front += [option]
    compile_cmd = compile_cmd_front + compile_cmd_backend
    for opt in compile_option_tuple.mllvm_options:
        compile_cmd += [opt]
    if global_var_storage.get_variable("ascendc_enable_sanitizer"):
        compile_cmd += ["--cce-enable-sanitizer", "-g"]
        compile_cmd += [
            "-mllvm",
            "-cce-aicore-long-call",
            "-mllvm",
            "-cce-aicore-jump-expand=true",
        ]
    if with_tiling_file:
        compile_cmd += ["-include", tiling_file]
    compile_cmd += ["-std=c++17"]
    compile_cmd += ["--cce-mask-opt"]
    if "oom" in get_current_build_config("tir.op_debug_config"):
        compile_cmd += [f"-D{ASCENDC_OOM}={1}"]
    compile_cmd += ["--cce-long-call=true"]
    return compile_cmd


def get_tiling_key_struct_size_map(
    tiling_key_struct_size_map, name_part, compile_info, dec_data
):
    if "_" in name_part:
        tiling_struct, tiling_key_value = name_part.rsplit("_", 1)
        if tiling_key_value.endswith("UL"):
            tiling_key_value = tiling_key_value[:-2]
        tiling_key_struct_size_map[tiling_key_value] = (tiling_struct, dec_data)
        if compile_info.tiling_key_group_map is None:
            return tiling_key_struct_size_map
        if tiling_key_value in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key_value]:
                tiling_key_struct_size_map[tiling_key_slave] = (tiling_struct, dec_data)
    return tiling_key_struct_size_map


def gen_tiling_struct_and_dfx_section_head():
    source = "#undef __global__\n"
    source += "#define __global__ inline\n"
    source += '#include "kernel_common.h"\n'
    source += "#undef __global__\n"
    source += "#if ASCENDC_CPU_DEBUG\n"
    source += "#define __global__\n"
    source += "#else\n"
    source += "#define __global__ __attribute__((cce_kernel))\n"
    source += "#endif\n\n"
    return source


def gen_tiling_struct_size_for_group_key_no_size(compile_info: CompileInfo):
    source = ""
    for tiling_key in compile_info.tiling_key_list:
        source += f"extern __gm__ uint64_t g_custom_tiling_size_meta_{tiling_key};\n"
        if compile_info.tiling_key_group_map is None:
            continue
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                source += f"extern __gm__ uint64_t g_custom_tiling_size_meta_{tiling_key_slave};\n"
    return source


def gen_tiling_struct_size_for_group_key(
    compile_info: CompileInfo, tiling_key_struct_size_map: dict
):
    source = ""
    for tiling_key in compile_info.tiling_key_list:
        tiling_struct_info = tiling_key_struct_size_map.get(str(tiling_key), None)
        if tiling_struct_info is None:
            continue
        _, tiling_struct_size = tiling_struct_info
        source += f"__gm__ uint64_t g_custom_tiling_size_meta_{tiling_key} = {tiling_struct_size};\n"
        if compile_info.tiling_key_group_map is None:
            continue
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                source += f"__gm__ uint64_t g_custom_tiling_size_meta_{tiling_key_slave} = {tiling_struct_size};\n"
    return source


def gen_dfx_section_for_one_tiling_key_static(
    compile_info: CompileInfo,
    tiling_key,
    tiling_info: TilingInfo,
    tiling_key_struct_size_map: dict,
):
    source = ""
    if compile_info.no_set_kernel_type is False:
        kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
        if kernel_type in [
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
            KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        ]:
            cube_marker = "_mix_aic"
            kernel_name = compile_info.kernel_name + cube_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
            vec_marker = "_mix_aiv"
            kernel_name = compile_info.kernel_name + vec_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        else:
            current_kernel_name = compile_info.get_kernel_func_name()
            kernel_name = current_kernel_name
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
    else:
        if compile_info.code_channel == CORE_TYPE_MIX:
            cube_marker = "_mix_aic"
            kernel_name = compile_info.kernel_name + cube_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
            vec_marker = "_mix_aiv"
            kernel_name = compile_info.kernel_name + vec_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        elif compile_info.hard_sync and compile_info.code_channel in [
            CORE_TYPE_VEC,
            CORE_TYPE_CUBE,
        ]:
            core_type_marker = (
                "_mix_aic"
                if compile_info.code_channel == CORE_TYPE_CUBE
                else "_mix_aiv"
            )
            kernel_name = compile_info.kernel_name + core_type_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        else:
            kernel_name = compile_info.get_kernel_func_name()
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
    return source


def gen_dfx_section_for_one_tiling_key_dynamic(
    compile_info: CompileInfo,
    tiling_key,
    tiling_info: TilingInfo,
    tiling_key_struct_size_map: dict,
):
    source = ""
    if compile_info.no_set_kernel_type is False:
        kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
        if kernel_type.value >= 6 and kernel_type.value <= 7:
            cube_marker = "_mix_aic"
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key + cube_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
            vec_marker = "_mix_aiv"
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key + vec_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        elif kernel_type.value >= 2 and kernel_type.value <= 5:
            if kernel_type in [
                KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
                KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
            ]:
                sub_marker = "_mix_aic"
            else:
                sub_marker = "_mix_aiv"
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key + sub_marker
        elif kernel_type.value >= 0 and kernel_type.value <= 1:
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
    else:
        if compile_info.code_channel == CORE_TYPE_MIX:
            cube_marker = "_mix_aic"
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key + cube_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
            vec_marker = "_mix_aiv"
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key + vec_marker
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        elif compile_info.hard_sync and compile_info.code_channel in [
            CORE_TYPE_VEC,
            CORE_TYPE_CUBE,
        ]:
            core_type_marker = (
                "_mix_aic"
                if compile_info.code_channel == CORE_TYPE_CUBE
                else "_mix_aiv"
            )
            kernel_name = (
                compile_info.kernel_name + "_%s" % tiling_key + core_type_marker
            )
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
        else:
            kernel_name = compile_info.kernel_name + "_%s" % tiling_key
            source += (
                DFXSectionGenerator().generate_dfx_section_without_tiling_register(
                    tiling_key, tiling_info, tiling_key_struct_size_map, kernel_name
                )
            )
    return source


def _omit_super_kernel_workspace(tiling_info: TilingInfo):
    # Only omit an explicitly empty workspace for static A5 sub-operators.
    return (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and CommonUtility.is_c310()
        and CommonUtility.is_support_workspace_offset()
        and tiling_info.static_shape_flag
        and (tiling_info.raw_run_info or {}).get("workspaces") == [0]
    )


def _gen_kernel_func_declare_head_with_workspace(
    tiling_info: TilingInfo, super_kernel_params, func_params
):
    dfx_generator = DFXSectionGenerator()
    # static shape do not have tiling
    if CommonUtility.is_v100() or CommonUtility.is_v200():
        if tiling_info.static_shape_flag:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR overflowStatus")
        else:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR tiling")
            func_params.append("GM_ADDR overflowStatus")
            dfx_generator.insert_param(DFXArgInfo("tiling", DFXParamType.TILING))
    else:
        if tiling_info.static_shape_flag:
            if not _omit_super_kernel_workspace(tiling_info):
                func_params.append("GM_ADDR workspace")
                super_kernel_params.append("workspace")
        else:
            func_params.append("GM_ADDR workspace")
            func_params.append("GM_ADDR tiling")
            super_kernel_params.append("workspace")
            super_kernel_params.append("tiling")
            dfx_generator.insert_param(DFXArgInfo("tiling", DFXParamType.TILING))
    return super_kernel_params, func_params


def _gen_kernel_func_declare_head(
    is_mix: bool,
    is_single_and_using_hard_sync: bool,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
):
    # generate kernel function
    source = ""
    dfx_generator = DFXSectionGenerator()
    func_params = []
    super_kernel_params = []
    needs_ffts = (is_mix or is_single_and_using_hard_sync) and not (
        CommonUtility.is_c310() or CommonUtility.is_m510()
    )
    workspace_idx = 0
    if needs_ffts:
        func_params.append("GM_ADDR ffts_addr")
        workspace_idx += 1
        super_kernel_params.append("ffts_addr")
        dfx_generator.insert_param(DFXArgInfo("ffts", DFXParamType.FFTS))

    if opinfo.mc2_ctx:
        for ctx_name in opinfo.mc2_ctx:
            func_params.append("GM_ADDR {}".format(ctx_name))
            workspace_idx += 1
            super_kernel_params.append(str(ctx_name))
            dfx_generator.insert_param(DFXArgInfo(ctx_name, DFXParamType.MC2CTX))

    for input in opinfo.inputs:
        if input is None:
            continue
        func_params.append("GM_ADDR {}".format(input["param_name"]))
        workspace_idx += 1
        super_kernel_params.append(input["param_name"])
        dfx_generator.insert_param(DFXArgInfo(input["param_name"], DFXParamType.INPUT))

    for output in opinfo.outputs:
        if output is None:
            continue
        func_params.append("GM_ADDR {}".format(output["param_name"]))
        workspace_idx += 1
        super_kernel_params.append(output["param_name"])
        dfx_generator.insert_param(
            DFXArgInfo(output["param_name"], DFXParamType.OUTPUT)
        )

    if (
        opinfo.output_shape_depend_on_compute is not None
        and len(opinfo.output_shape_depend_on_compute) > 0
    ):
        func_params.append("GM_ADDR __ascendc_output_shape")
        workspace_idx += 1
        super_kernel_params.append("__ascendc_output_shape")
        dfx_generator.insert_param(
            DFXArgInfo("shape_tensor", DFXParamType.SHAPE_TENSOR)
        )
        # modify point type for OutputShapeDependOnCompute output
        for index in opinfo.output_shape_depend_on_compute:
            parameter: DFXArgInfo = dfx_generator.get_param(
                opinfo.outputs[index]["param_name"]
            )
            parameter.point_type = DFXPointType.LEVEL_1_FOR_SHAPE_TENSOR
        # for static shape, set size to max value, len(OutputShapeDependOnCompute output) * mix dim(8) * uint64_t(8)
        if tiling_info.static_shape_flag:
            dfx_generator.set_size_of_dfx_info(
                "shape_tensor", len(opinfo.output_shape_depend_on_compute) * 8 * 8
            )

    # dynamic: must add workspace, static: if workspace_size >= 0 add workspace
    if not tiling_info.static_shape_flag or tiling_info.static_workspace_size >= 0:
        dfx_generator.insert_param(DFXArgInfo("workspace", DFXParamType.WORKSPACE))

    super_kernel_params, func_params = _gen_kernel_func_declare_head_with_workspace(
        tiling_info, super_kernel_params, func_params
    )
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        global_var_storage.set_variable(
            "ascendc_sub_super_kernel_params", super_kernel_params
        )
        context = get_context()
        if (
            context is None
            or context.get_addition("super_kernel_sub_combine") is not True
        ):
            called_func_params = "args_offset"
            called_func_params_type = "uint64_t args_offset"
            source += "uint64_t args_offset) {\n"
            source += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
            for param in func_params:
                source += f"    {param} = param_base[args_offset++];\n"
        else:
            called_func_params = "param, sargs"
            called_func_params_type = "__gm__ uint64_t *param, sk::SkSystemArgs *sargs"
            source += "__gm__ uint64_t *param, sk::SkSystemArgs *sargs) {\n"
            source += (
                "    g_super_kernel_early_start_config = sargs->SkGetTaskSyncCfg();\n"
            )
            source += "    uint32_t __asc_index = 0;\n"
            for param in func_params:
                source += f"    {param} = (GM_ADDR)param[__asc_index++];\n"

    else:
        source += ", ".join(func_params) + ") {\n"
        called_func_params_type = ", ".join(func_params)
        called_func_params = called_func_params_type.replace("GM_ADDR ", "")
    return source, workspace_idx, called_func_params, called_func_params_type


def _gen_usr_workspace_codes():
    workspace_ptr = "AscendC::GetUserWorkspace(workspace)"
    if CommonUtility.is_support_workspace_offset():
        workspace_ptr = "workspace + AscendC::RESERVED_WORKSPACE"
        if CommonUtility.is_c310():
            return (
                "#if ENABLE_CV_COMM_VIA_SSBUF != 0 && __MIX_CORE_AIC_RATION__ != 1\n"
                "    GM_ADDR usrWorkspace = workspace;\n"
                "#else\n"
                f"    GM_ADDR usrWorkspace = {workspace_ptr};\n"
                "#endif\n"
            )
    return f"    GM_ADDR usrWorkspace = {workspace_ptr};\n"


def _gen_set_workspace_codes(
    is_mix: bool,
    is_single_and_using_hard_sync: bool,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
    compile_options: list,
    compile_info: CompileInfo,
):
    source = _gen_usr_workspace_codes()
    if "oom" in get_current_build_config("tir.op_debug_config"):
        source = add_op_param_to_workspace(
            opinfo, tiling_info, source, compile_options, compile_info
        )

    needs_ffts = (
        is_mix or is_single_and_using_hard_sync
    ) and not CommonUtility.is_c310()
    # set ffts_addr for ascend910b mix op or is_single_and_using_hard_sync scene
    if needs_ffts:
        source += "    icache_preload(1);\n"
        source += "    if (ffts_addr != nullptr) {\n"
        source += "        set_ffts_base_addr((uint64_t)ffts_addr);\n"
        source += "    }\n"

    # restart enable begin position
    if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
        source += "do {\n"

    # is_single_and_using_hard_sync scene not need clear workspace
    if is_mix and (not CommonUtility.is_c310()):  # c310 doesn't need clearWorkspace
        source += f"#ifdef {MIX_CORE_MACRO} \n"
        source += "    if constexpr (g_coreType == AscendC::AIC) {\n"
        source += "        matmul::clearWorkspace(workspace);\n"
        source += "    }\n"
        source += "#endif\n"
    return source


def _gen_set_mc2_ctx_param(opinfo: OpInfo):
    if opinfo.mc2_ctx is None:
        return ""
    source = ""
    index = 0
    for ctx_name in opinfo.mc2_ctx:
        source += f"    AscendC::SetHcclContext<{index}>({ctx_name});\n"
        index += 1
    return source


# When TPL struct is not registered through macro REGISTER_TILING, need to insert section code
def _gen_tpl_tiling_struct_section(compile_info: CompileInfo, tiling_info: TilingInfo):
    source = gen_global_isolation_macro(compile_info, tiling_info)
    tiling_struct_set = set()
    counter = 0
    for tiling_key in compile_info.tiling_key_list:
        original_tiling_struct = compile_info.tiling_key_struct_map[tiling_key]
        if (
            original_tiling_struct in compile_info.tpl_tiling_struct
            and original_tiling_struct not in compile_info.register_tiling_struct
            and original_tiling_struct not in tiling_struct_set
        ):
            source += f"static const uint64_t __ascendc_TPL_tiling_struct_{counter} __attribute__"
            source += f'((used, section(".ascendc_tiling.{original_tiling_struct}"))) = sizeof({original_tiling_struct});\n'
            counter += 1
        tiling_struct_set.add(original_tiling_struct)
    source += "#endif\n\n"
    return source


def gen_meta_info_section(compile_info, op_info):
    meta_info = {}
    section_var = ""
    out_file = compile_info.gen_kernel_func_file
    kernel_name = op_info.kernel_name

    # version
    section_var += f"static const struct BinaryMetaVersion {kernel_name}_kernel_metainfo_version_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += " {{B_TYPE_BIN_VERSION_INFO, sizeof(unsigned int)}, 0x01};\n"

    # debug
    debug_options = 0
    debug_buf_size = 0
    debug_options_table = {
        "printf": 0x001,
        "dumptensor": 0x001,
        "assert": 0x002,
        "timestamp": 0x004,
        "oom": 0x008,
    }

    if "oom" in get_current_build_config("tir.op_debug_config"):
        debug_options |= debug_options_table["oom"]
    section_var += f"static const struct BinaryMetaDebug {kernel_name}_kernel_metainfo_debug_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += (
        f" {{{{B_TYPE_DEBUG_INFO, 8}}, {debug_buf_size}, {debug_options}}};\n"
    )

    # dynamicparam
    dynamic_param = (
        1
        if "param_type_dynamic" in op_info._fields and op_info.param_type_dynamic
        else 0
    )
    section_var += "static const struct BinaryMetaDynamicParam "
    section_var += f"{kernel_name}_kernel_metainfo_dynamicparam_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += f" {{{{B_TYPE_DYNAMIC_PARAM, 4}}, 0, {dynamic_param}}};\n"

    # optinalparam
    optional_input_mode = 1 if check_if_gen_placehoder(op_info, True) else 0
    optional_output_mode = 1 if check_if_gen_placehoder(op_info, False) else 0
    section_var += "static const struct BinaryMetaOptionalParam "
    section_var += f"{kernel_name}_kernel_metainfo_optionalparam_section __attribute__ "
    section_var += '((used, section (".ascend.meta"))) = '
    section_var += f" {{{{B_TYPE_OPTIONAL_PARAM, 4}}, {optional_input_mode}, {optional_output_mode}}};\n"

    global_var_storage.set_variable("ascendc_meta_info", section_var)


def gen_kernel_fun(
    compile_info: CompileInfo,
    func_name: str,
    opinfo: OpInfo,
    tiling_info: TilingInfo,
    compile_option_tuple,
):
    compile_options = compile_option_tuple.compile_options
    src_file = compile_info.src_file
    out_file = compile_info.gen_kernel_func_file

    file_name = os.path.basename(src_file)
    file_name_without_ext = os.path.splitext(file_name)[0]
    # begin generate code
    # File Isolation Macro
    source = f"#ifndef __{file_name_without_ext.upper()}__KERNEL_FUN_H__\n"
    source += f"#define __{file_name_without_ext.upper()}__KERNEL_FUN_H__\n\n"
    # replace __global micro for usr kernel function, and recover after usr kernel function
    source += "#undef __global__\n"
    source += "#define __global__ inline\n"
    source += f'#include "{src_file}"\n'
    source += '#include "kernel_common.h"\n'

    source += "#undef __global__\n"
    source += "#if ASCENDC_CPU_DEBUG\n"
    source += "#define __global__\n"
    source += "#else\n"
    source += "#define __global__ __attribute__((cce_kernel))\n"
    source += "#endif\n\n"

    if global_var_storage.get_variable("ascendc_tiling_no_register"):
        source += gen_tiling_struct_size_for_group_key_no_size(compile_info)

    # add template_param
    source += gen_template_tiling_params(compile_info)

    is_mix, is_single_and_using_hard_sync = get_v220_kernel_type_mix_flag(
        compile_info, tiling_info
    )

    # generate code for l2 cache
    if (
        global_var_storage.get_variable("ascendc_enable_sanitizer") is False
        and global_var_storage.get_variable("ascendc_debug_compile_options") is False
        and global_var_storage.get_variable("ascendc_enable_super_kernel") is False
    ):
        if CommonUtility.is_v220() or CommonUtility.is_v200():
            source = get_code_for_l2_cache(compile_info, source, tiling_info)

    # generate kernel function
    auto_gen_kernel_func = f"auto_gen_{func_name}_kernel"

    gen_func_attributes = "__global__"
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        align_size = compile_info.super_kernel_info["sp_options"].get("func-align", 512)
        gen_func_attributes = gen_func_align_attribute(align_size)
        gen_func_attributes += " __sk__"
    else:
        gen_func_attributes += " [aicore]"

    kernel_func_dec = f'extern "C" {gen_func_attributes} void {auto_gen_kernel_func}('
    compile_info.global_kernel_attribute = gen_func_attributes
    kernel_func_dec_pub = f"__aicore__ inline __attribute__((always_inline)) void ascendc_{auto_gen_kernel_func}("

    source_declare_pub, workspace_idx, called_func_params, called_func_params_type = (
        _gen_kernel_func_declare_head(
            is_mix, is_single_and_using_hard_sync, opinfo, tiling_info
        )
    )
    source += kernel_func_dec_pub
    source += source_declare_pub
    source_declare_pub_fun = (
        f"ascendc_{auto_gen_kernel_func}(" + called_func_params + ");"
    )

    # init dump and system workspace
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is False:
        source += gen_init_dump_code()
        # set mc2 context
        source += _gen_set_mc2_ctx_param(opinfo)
        # implicit add aicore exception restart begin position
        # set workspace
        source += _gen_set_workspace_codes(
            is_mix,
            is_single_and_using_hard_sync,
            opinfo,
            tiling_info,
            compile_options,
            compile_info,
        )
    else:
        source += _gen_set_mc2_ctx_param(opinfo)
        if _omit_super_kernel_workspace(tiling_info):
            source += "    GM_ADDR usrWorkspace = nullptr;\n"
        else:
            source += "    AscendC::SetSysWorkspaceForce(workspace);\n"
            source += (
                "    GM_ADDR usrWorkspace = AscendC::GetUserWorkspace(workspace);\n"
            )

        # restart enable begin position
        if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
            source += "do {\n"

    need_ffts = is_mix or is_single_and_using_hard_sync
    context = get_context()
    enable_inner_core_sync_check = (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and context is not None
        and context.get_addition("super_kernel_sub_combine") is True
        and compile_info.super_kernel_info["sp_options"].get(
            "debug-per-op-max-core-num", "0"
        )
        == "1"
    )

    # call usr kernel function call
    if enable_inner_core_sync_check:
        source += "    AscendC::g_superKernelSetWaitFlagCountDifference = "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE;\n"
    source += "#if defined(TEMPLATE_PARAMS_LEN) && TEMPLATE_PARAMS_LEN != 0\n"
    source += gen_usr_origin_kernel_function_call(
        func_name, opinfo, tiling_info, has_template=True
    )
    source += "#else\n"
    source += gen_usr_origin_kernel_function_call(
        func_name, opinfo, tiling_info, has_template=False
    )
    source += "#endif\n"
    if enable_inner_core_sync_check:
        source += "    if (AscendC::g_superKernelSetWaitFlagCountDifference > "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE) {\n"
        source += '        assert(false, "SuperKernel: current operator has %d more '
        source += 'SetFlag calls than WaitFlag calls; Please check synchronization within the current operator.\\n", '
        source += "AscendC::g_superKernelSetWaitFlagCountDifference - "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE);\n"
        source += "    } else if (AscendC::g_superKernelSetWaitFlagCountDifference < "
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE) {\n"
        source += '        assert(false, "SuperKernel: current operator has %d more '
        source += 'WaitFlag calls than SetFlag calls; Please check synchronization within the current operator.\\n", '
        source += "AscendC::SUPER_KERNEL_SET_WAIT_FLAG_COUNT_INITIAL_VALUE - "
        source += "AscendC::g_superKernelSetWaitFlagCountDifference);\n"
        source += "    }\n"

    if len(compile_info.tiling_key_struct_map) > 0:
        source += _gen_tpl_tiling_struct_section(compile_info, tiling_info)

    # aicore exception restart main block
    if global_var_storage.get_variable("ascendc_enable_aicore_exception_restart"):
        for key in tiling_info.tiling_key_list:
            source += f"#if {TILING_KEY_MACRO} == {key}UL"
            source += "\n"

            actual_kernel_type = get_actual_kernel_type(
                key, compile_info, need_ffts, opinfo.kernel_name
            )

            if actual_kernel_type == CORE_TYPE_CUBE:
                source += "    if ASCEND_IS_AIC {\n"
                source += "        AscendC::PipeBarrier<PIPE_ALL>();\n"
                source += "        AscendC::CrossCoreSetFlag<0, PIPE_FIX>(AscendC::SYNC_AIC_FLAG);\n"
                source += (
                    "        AscendC::CrossCoreWaitFlag(AscendC::SYNC_AIC_FLAG);\n"
                )
                source += "    }\n"
            elif actual_kernel_type == CORE_TYPE_VEC:
                source += "    AscendC::SyncAll();\n"
            elif actual_kernel_type == CORE_TYPE_MIX:
                source += "    AscendC::SyncAll<false>();\n"
            source += "#endif\n"

        ctx_num = 0
        if opinfo.mc2_ctx is not None:
            ctx_num = len(opinfo.mc2_ctx)
        source += f"    auto __ascendc_is_restart = AscendC::GetRestart({ctx_num});\n"
        source += "    if (__ascendc_is_restart > 0) {\n"
        source += "        AscendC::PipeBarrier<PIPE_ALL>();\n"
        source += "        dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE);\n"
        # add corresponding sync all by kernel type
        for key in tiling_info.tiling_key_list:
            source += f"#if {TILING_KEY_MACRO} == {key}UL"
            source += "\n"

            actual_kernel_type = get_actual_kernel_type(
                key, compile_info, need_ffts, opinfo.kernel_name
            )

            if actual_kernel_type == CORE_TYPE_CUBE:
                source += "        if ASCEND_IS_AIC {\n"
                source += "            AscendC::PipeBarrier<PIPE_ALL>();\n"
                source += "            AscendC::CrossCoreSetFlag<0, PIPE_FIX>(AscendC::SYNC_AIC_FLAG);\n"
                source += (
                    "            AscendC::CrossCoreWaitFlag(AscendC::SYNC_AIC_FLAG);\n"
                )
                source += "        }\n"
            elif actual_kernel_type == CORE_TYPE_VEC:
                source += "        AscendC::SyncAll();\n"
            elif actual_kernel_type == CORE_TYPE_MIX:
                source += "        AscendC::SyncAll<false>();\n"
            source += "#endif\n"
        source += f"        AscendC::SetRestart({ctx_num});\n"
        source += "    } else {\n"
        source += "        break;\n"
        source += "    }\n"
        source += "} while(1);\n"

    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import status_check

    if get_current_build_config(status_check) and (
        CommonUtility.is_v200() or CommonUtility.is_v100()
    ):
        source += "    AscendC::WriteBackOverflow(overflowStatus);\n"

    if not global_var_storage.get_variable("ascendc_enable_super_kernel") and (
        CommonUtility.is_c310() or CommonUtility.is_m510()
    ):
        check_custom_dcci_end_false(compile_option_tuple)

    source += "}\n\n"
    source += kernel_func_dec
    source += called_func_params_type + ") {\n"
    source += "    " + source_declare_pub_fun + "\n"
    source += "}\n\n"

    for tiling_key in compile_info.tiling_key_list:
        if compile_info.tiling_key_group_map is not None:
            if tiling_key in compile_info.tiling_key_group_map.keys():
                source += gen_kernel_fun_with_tiling_key_slave(
                    compile_info,
                    tiling_key,
                    source_declare_pub_fun,
                    gen_func_attributes,
                    source_declare_pub,
                )
    source += "#endif\n"
    # write code into file
    try:
        with os.fdopen(
            os.open(
                out_file,
                os.O_TRUNC | os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(source)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("gen kernel func file failed, reason is:", err),
        )
    return workspace_idx


def gen_kernel_fun_with_tiling_key_slave(
    compile_info: CompileInfo,
    tiling_key: int,
    source_declare_pub_fun: str,
    gen_func_attributes: str,
    source_declare: str,
):
    source = ""
    if CommonUtility.is_v220() or CommonUtility.is_c310():
        chip_version = CommonUtility.get_chip_version().upper()
        cube_core_type = f"__DAV_{chip_version}_CUBE__"
        vec_core_type = f"__DAV_{chip_version}_VEC__"
    else:
        cube_core_type = "__DAV_M200__"
        vec_core_type = "__DAV_M200_VEC__"
    if compile_info.tiling_key_group_map is not None:
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                if compile_info.sub_core_type == CORE_TYPE_CUBE:
                    source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({cube_core_type})\n"
                elif compile_info.sub_core_type == CORE_TYPE_VEC:
                    source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({vec_core_type})\n"
                if tiling_key not in compile_info.tiling_key_kernel_type.keys():
                    raise Exception(f"kernel type of tiling key {tiling_key} not found")
                else:
                    kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
                    if kernel_type in [
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
                        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
                    ]:
                        source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({cube_core_type})\n"
                        kernel_name_of_tk = (
                            get_kernel_fun_name_with_tiling_key_and_kernel_type(
                                compile_info, tiling_key_slave
                            )
                        )
                        kernel_func_dec = f'extern "C" {gen_func_attributes} void {kernel_name_of_tk}('
                        source += kernel_func_dec
                        if (
                            global_var_storage.get_variable(
                                "ascendc_enable_super_kernel"
                            )
                            is True
                        ):
                            source += "uint64_t args_offset) {\n"
                        else:
                            source += source_declare
                        source += f"    {source_declare_pub_fun}\n"
                        source += "}\n"
                        source += "#endif\n"
                        source += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL && defined({vec_core_type})\n"
                        kernel_name_of_tk = (
                            get_kernel_fun_name_with_tiling_key_and_kernel_type(
                                compile_info, tiling_key_slave
                            )
                        )
                        kernel_name_of_tk = kernel_name_of_tk[:-1] + "v"
                        kernel_func_dec = f'extern "C" {gen_func_attributes} void {kernel_name_of_tk}('
                        source += kernel_func_dec
                        if (
                            global_var_storage.get_variable(
                                "ascendc_enable_super_kernel"
                            )
                            is True
                        ):
                            source += "uint64_t args_offset) {\n"
                        else:
                            source += source_declare
                        source += f"    {source_declare_pub_fun}\n"
                        source += "}\n"
                        source += "#endif\n"
                    else:
                        raise Exception(
                            f"unsupported kernel type {kernel_type} for tiling key {tiling_key}"
                        )
    return source


def gen_tiling_struct_size_and_dfx_section_file(
    compile_info: CompileInfo, tiling_info: TilingInfo, tiling_key_struct_size_map: dict
):
    out_file = compile_info.tiling_and_dfx_utils_file
    source = gen_tiling_struct_and_dfx_section_head()
    source += gen_tiling_struct_size_for_group_key(
        compile_info, tiling_key_struct_size_map
    )

    if tiling_info.static_shape_flag:
        source += gen_dfx_section_for_one_tiling_key_static(
            compile_info,
            tiling_info.tiling_key,
            tiling_info,
            tiling_key_struct_size_map,
        )
        if compile_info.tiling_key_group_map is not None:
            if tiling_info.tiling_key in compile_info.tiling_key_group_map.keys():
                for tiling_key_slave in compile_info.tiling_key_group_map[
                    tiling_info.tiling_key
                ]:
                    source += gen_dfx_section_for_one_tiling_key_static(
                        compile_info,
                        tiling_key_slave,
                        tiling_info,
                        tiling_key_struct_size_map,
                    )
    else:
        for tiling_key in compile_info.tiling_key_list:
            source += gen_dfx_section_for_one_tiling_key_dynamic(
                compile_info, tiling_key, tiling_info, tiling_key_struct_size_map
            )
            if compile_info.tiling_key_group_map is not None:
                if tiling_key in compile_info.tiling_key_group_map.keys():
                    for tiling_key_slave in compile_info.tiling_key_group_map[
                        tiling_key
                    ]:
                        source += gen_dfx_section_for_one_tiling_key_dynamic(
                            compile_info,
                            tiling_key_slave,
                            tiling_info,
                            tiling_key_struct_size_map,
                        )

    try:
        with os.fdopen(
            os.open(
                out_file,
                os.O_TRUNC | os.O_RDWR | os.O_CREAT,
                stat.S_IWUSR | stat.S_IRUSR,
            ),
            "w",
        ) as ofd:
            ofd.write(source)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE,
            ("gen kernel func file failed, reason is:", err),
        )


def _get_tiling_struct_size(compile_info):
    tiling_struct_set = set()
    tiling_struct_size_map = {}
    max_tiling_size = 0
    for _, tiling_struct in compile_info.tiling_key_struct_map.items():
        tiling_struct_set.add(tiling_struct)

    for tiling_struct in tiling_struct_set:
        objdump_cmd = [
            "llvm-objdump",
            "-s",
            "-j",
            ".ascendc_tiling.{}".format(tiling_struct),
            "{}".format(compile_info.dst_file),
        ]
        proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
        (out, _) = proc.communicate()
        """
        e.g.
        Contents of section .ascend.meta.TilingData:         # main_tiling_info[0]
        0000 50000000 00000000                    P.......   # main_tiling_info[1] that needs to be parsed
        """
        tiling_str_info = out.decode("utf-8")
        if (
            TILING_KEY_SEARCH_KEYWORD in tiling_str_info
        ):  # key words from llvm-objdump .ascendc_tiling.
            main_line_start_index = tiling_str_info.index(TILING_KEY_SEARCH_KEYWORD)
            main_tiling_info = tiling_str_info[main_line_start_index:].split("\n")
            hex_num = main_tiling_info[1].split(" ")[2:4]
            hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
            bytes_data = bytes.fromhex(hex_num_str)
            dec_data = struct.unpack(">Q", bytes_data)[0]
            tiling_struct_size_map[tiling_struct] = dec_data
            max_tiling_size = max(max_tiling_size, dec_data)

    # Preserve exact per-key sizes for Manifest constants before removing sections.
    compile_info.compiled_tiling_key_data_size_map = {
        str(tiling_key): int(tiling_struct_size_map[tiling_struct])
        for tiling_key, tiling_struct in compile_info.tiling_key_struct_map.items()
        if tiling_struct in tiling_struct_size_map
    }
    # The sk sub operator failed to rm tiling section because llvm-objcopy could not correctly process the ar obj.
    if (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and compile_info.is_super_kernel_compile is False
    ):
        CommonUtility.print_compile_log(
            compile_info.kernel_name,
            "[Superkernel]In sk sub kernel compile, do not need rm tiling seciton!",
            AscendCLogLevel.LOG_INFO,
        )
        return max_tiling_size
    # remove ascendc_tiling section
    objdump_cmd = [
        "llvm-objcopy",
        "--remove-section=.ascendc_tiling.*",
        "{}".format(compile_info.dst_file),
    ]
    proc = subprocess.Popen(
        objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    CommonUtility.print_compile_log(
        compile_info.kernel_name, "need rm tiling seciton!", AscendCLogLevel.LOG_INFO
    )
    (out, _) = proc.communicate()
    return max_tiling_size


def _get_tiling_struct_without_register_size(compile_info: CompileInfo):
    section_name_set = set()
    tiling_key_struct_size_map = {}  # tiling_key -> (tiling_struct, struct_size)
    max_tiling_size = 0
    objdump_cmd = ["llvm-objdump", "-s", f"{compile_info.dst_file}"]
    proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
    (out, _) = proc.communicate()
    tiling_str_info = out.decode("utf-8")
    tiling_lines = [
        line for line in tiling_str_info.splitlines() if ".ascendc_tiling" in line
    ]
    pattern = re.compile(r"\.ascendc_tiling\.[^\s]+")

    for line in tiling_lines:
        for match in pattern.findall(line):
            # match eg. ".ascendc_tiling.optiling::TilingData1_2UL.0"
            match = match.rstrip(":;,")
            section_name_set.add(match)
            name_part = match.split(".ascendc_tiling.", 1)[1]
            name_part = name_part.rsplit(".", 1)[0]

            tiling_key_struct_size_map = get_tiling_key_struct_size_map(
                tiling_key_struct_size_map, name_part, compile_info, 0
            )

    for section_name in section_name_set:
        objdump_cmd = [
            "llvm-objdump",
            "-s",
            "-j",
            "{}".format(section_name),
            "{}".format(compile_info.dst_file),
        ]
        proc = subprocess.Popen(objdump_cmd, stdout=subprocess.PIPE, stderr=None)
        (out, _) = proc.communicate()
        tiling_str_info = out.decode("utf-8")
        if (
            TILING_KEY_SEARCH_KEYWORD in tiling_str_info
        ):  # key words from llvm-objdump .ascendc_tiling.
            main_line_start_index = tiling_str_info.index(TILING_KEY_SEARCH_KEYWORD)
            main_tiling_info = tiling_str_info[main_line_start_index:].split("\n")
            hex_num = main_tiling_info[1].split(" ")[2:4]
            hex_num_str = CommonUtility.parser_uint64_hex_num(hex_num)
            bytes_data = bytes.fromhex(hex_num_str)
            dec_data = struct.unpack(">Q", bytes_data)[0]
            name_part = section_name.split(".ascendc_tiling.", 1)[1].rsplit(".", 1)[0]
            tiling_key_struct_size_map = get_tiling_key_struct_size_map(
                tiling_key_struct_size_map, name_part, compile_info, dec_data
            )
            max_tiling_size = max(max_tiling_size, dec_data)
    compile_info.max_tiling_size = max_tiling_size
    # REGISTER_NONE_TILING sections encode both the struct name and byte size.
    compile_info.compiled_tiling_key_data_size_map = {
        str(tiling_key): int(value[1])
        for tiling_key, value in tiling_key_struct_size_map.items()
        if isinstance(value, tuple) and len(value) == 2 and value[1] > 0
    }
    return tiling_key_struct_size_map


def delete_tiling_section(compile_info: CompileInfo):
    # The sk sub operator failed to rm tiling section because llvm-objcopy could not correctly process the ar obj.
    if (
        global_var_storage.get_variable("ascendc_enable_super_kernel") is True
        and compile_info.is_super_kernel_compile is False
    ):
        CommonUtility.print_compile_log(
            compile_info.kernel_name,
            "[Superkernel]In sk sub kernel compile, do not need rm tiling seciton!",
            AscendCLogLevel.LOG_INFO,
        )
        return
    # remove ascendc_tiling section
    objdump_cmd = [
        "llvm-objcopy",
        "--remove-section=.ascendc_tiling.*",
        "{}".format(compile_info.dst_file),
    ]
    proc = subprocess.Popen(
        objdump_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    CommonUtility.print_compile_log(
        compile_info.kernel_name, "need rm tiling seciton!", AscendCLogLevel.LOG_INFO
    )
    (out, _) = proc.communicate()
    return


def gen_op_stub_kernel_func(
    compile_info: CompileInfo,
    op_info: OpInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    distinct_tag,
    kernel_meta_dir,
):
    # generate kernel fun for ffts_addr, overflow, workspace
    msg_info = "<{}> <{}> generate kernel stub start".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    file_name_tag = distinct_tag + "_kernel.cpp"

    # for aclnn sk sub operator combine norm workflow, generate norm kernel file
    if global_var_storage.get_variable("ascendc_sk_sub_combine_norm_workflow") is True:
        file_name_tag = distinct_tag + "_norm_kernel.cpp"
    compile_info.gen_kernel_func_file = os.path.join(
        kernel_meta_dir, op_info.kernel_name + file_name_tag
    )

    if CommonUtility.is_c310():
        gen_meta_info_section(compile_info, op_info)
    workspace_idx = gen_kernel_fun(
        compile_info,
        compile_info.origin_func_name,
        op_info,
        tiling_info,
        compile_option_tuple,
    )

    msg_info = "<{}> <{}> generate kernel stub end".format(
        compile_info.op_type, compile_info.tiling_key_list
    )
    LogUtil.detail_log_print(op_info.kernel_name, msg_info, AscendCLogLevel.LOG_INFO)
    return workspace_idx
