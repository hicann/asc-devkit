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
AscendC adapter, check_op
"""

import ctypes
import json

from tbe.common.utils.errormgr import get_error_message


def check_op_cap(check_type: str, optype: str, inputs: list, outputs: list, attrs: list = None):
    """Check whether an operator has a certain capability based on the operator type / inputs / outputs information.

    Args:
        check_type (str): including check_supported/op_select_format/get_op_specific_info/get_op_support_info/
        optype (str): operator type
        inputs (list): shape/ori_shape/format/ori_format/dtype/name, type including required/optional/dynamic,
                        if shape of inputs is const, there will be an extra field "const value".
        outputs (list): shape/ori_shape/format/ori_format/dtype/name, type including required/optional/dynamic
        attrs (list): name/dtype/value.

    Raises:
        RuntimeError: If the function is called incorrectly, raise the error information.

    Returns:
        res_info: A dictionary containing results
                i.e. "check_supported": {
                        "ret_code": 1,
                        "reason": "unsupported"
                    },
                    "op_select_format": {
                        "op_info": "xxx"
                    },
    """
    check_type_c = check_type.encode("utf_8")
    optype_c = optype.encode("utf_8")
    inputs_c = json.dumps(inputs).encode("utf_8")
    outputs_c = json.dumps(outputs).encode("utf_8")
    attrs_c = json.dumps(attrs).encode("utf_8")
    max_run_info_size = 1024 * 64
    result_info_buf = ctypes.create_string_buffer(max_run_info_size)
    result_info_buf_size = ctypes.c_size_t(max_run_info_size)

    from .get_op_tiling import load_lib

    load_lib()

    libregister = ctypes.CDLL("libregister.so")
    check_func = libregister.AscendCPyInterfaceCheckOp
    res = check_func(check_type_c, optype_c, inputs_c, outputs_c, attrs_c, result_info_buf, result_info_buf_size)

    if not res:
        dict_args = {}
        inputs_str = "\n".join(tuple(map(str, inputs)))
        outputs_str = "\n".join(tuple(map(str, outputs)))
        dict_args["errCode"] = "E90003"
        dict_args["detailed_cause"] = (
            f"check_op_cap func failed, check_type: {check_type}, op_type:{optype} failed, "
            f"failure details:\n"
            f"Compile_info: empty_compile_info\n"
            f"Inputs: {inputs_str}\n"
            f"Outputs: {outputs_str}\n"
            f"Attrs: {attrs}"
        )
        raise RuntimeError(dict_args, get_error_message(dict_args))

    return result_info_buf.value


def generalize_op_params(optype: str, inputs: list, outputs: list, attrs: list = None, generalize_config: str = ""):
    """
    This is a common interface. You can use this interface to query the customized functions registered by an operator.
    Refer to "tbe_register.register_param_generalization"

    Args:
        optype (str): operator type
        inputs (list): shape/ori_shape/format/ori_format/dtype/name, type including required/optional/dynamic,
                        if shape of inputs is const, there will be an extra field "const value".
        outputs (list): shape/ori_shape/format/ori_format/dtype/name, type including required/optional/dynamic
        attrs (list): name/dtype/value.
        generalize_config (str): including mode.

    Raises:
        RuntimeError: If the function is called incorrectly, raise the error information.

    Returns:
        generalized_op_params (list): including generalized inputs/outputs/attrs.
    """

    optype_c = optype.encode("utf_8")
    inputs_c = json.dumps(inputs).encode("utf_8")
    outputs_c = json.dumps(outputs).encode("utf_8")
    attrs_c = json.dumps(attrs).encode("utf_8")
    generalize_config_c = json.dumps(generalize_config).encode("utf_8")
    libregister = ctypes.CDLL("libregister.so")
    generalize_func = libregister.AscendCPyInterfaceGeneralized

    max_run_info_size = 1024 * 64
    result_info_buf = ctypes.create_string_buffer(max_run_info_size)
    result_info_buf_size = ctypes.c_size_t(max_run_info_size)
    res = generalize_func(
        optype_c, inputs_c, outputs_c, attrs_c, generalize_config_c, result_info_buf, result_info_buf_size
    )

    if not res:
        dict_args = {}
        inputs_str = "\n".join(tuple(map(str, inputs)))
        outputs_str = "\n".join(tuple(map(str, outputs)))
        dict_args["errCode"] = "E90003"
        dict_args["detailed_cause"] = (
            f"generalize_op_params func of op_type {optype} failed, failure details:\n"
            f"Compile_info: empty_compile_info\n"
            f"Inputs: {inputs_str}\n"
            f"Outputs: {outputs_str}\n"
            f"Attrs: {attrs}"
            f"generalize_config: {generalize_config}"
        )
        raise RuntimeError(dict_args, get_error_message(dict_args))

    return result_info_buf.value
