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
