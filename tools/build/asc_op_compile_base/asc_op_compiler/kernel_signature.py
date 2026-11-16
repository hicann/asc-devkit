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
"""Collect kernel arguments and render normal or super-kernel signatures."""

from .ascendc_compile_dfx import DFXArgInfo, DFXParamType, DFXPointType


def _append_tensor_arguments(tensors, param_type, func_params, super_kernel_params, dfx_generator):
    count = 0
    for tensor in tensors:
        if tensor is None:
            continue
        func_params.append("GM_ADDR {}".format(tensor["param_name"]))
        count += 1
        super_kernel_params.append(tensor["param_name"])
        dfx_generator.insert_param(DFXArgInfo(tensor["param_name"], param_type))
    return count


def _append_output_shape_argument(opinfo, tiling_info, func_params, super_kernel_params, dfx_generator):
    if opinfo.output_shape_depend_on_compute is None or len(opinfo.output_shape_depend_on_compute) == 0:
        return 0
    func_params.append("GM_ADDR __ascendc_output_shape")
    super_kernel_params.append("__ascendc_output_shape")
    dfx_generator.insert_param(DFXArgInfo("shape_tensor", DFXParamType.SHAPE_TENSOR))
    for index in opinfo.output_shape_depend_on_compute:
        parameter = dfx_generator.get_param(opinfo.outputs[index]["param_name"])
        parameter.point_type = DFXPointType.LEVEL_1_FOR_SHAPE_TENSOR
    if tiling_info.static_shape_flag:
        dfx_generator.set_size_of_dfx_info("shape_tensor", len(opinfo.output_shape_depend_on_compute) * 8 * 8)
    return 1


def collect_kernel_arguments(needs_ffts, opinfo, tiling_info, dfx_generator):
    func_params = []
    super_kernel_params = []
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
    workspace_idx += _append_tensor_arguments(
        opinfo.inputs, DFXParamType.INPUT, func_params, super_kernel_params, dfx_generator
    )
    workspace_idx += _append_tensor_arguments(
        opinfo.outputs, DFXParamType.OUTPUT, func_params, super_kernel_params, dfx_generator
    )
    workspace_idx += _append_output_shape_argument(opinfo, tiling_info, func_params, super_kernel_params, dfx_generator)
    return func_params, super_kernel_params, workspace_idx


def render_super_kernel_signature(func_params, context):
    if context is None or context.get_addition("super_kernel_sub_combine") is not True:
        called_func_params = "args_offset"
        called_func_params_type = "uint64_t args_offset"
        source = "uint64_t args_offset) {\n"
        source += "    GM_ADDR *param_base = (GM_ADDR *)get_para_base();\n"
        for param in func_params:
            source += f"    {param} = param_base[args_offset++];\n"
    else:
        called_func_params = "param, sargs"
        called_func_params_type = "__gm__ uint64_t *param, sk::SkSystemArgs *sargs"
        source = "__gm__ uint64_t *param, sk::SkSystemArgs *sargs) {\n"
        source += "    sargs->SkInit();\n"
        source += "    uint32_t __asc_index = 0;\n"
        for param in func_params:
            source += f"    {param} = (GM_ADDR)param[__asc_index++];\n"
    return source, called_func_params, called_func_params_type
