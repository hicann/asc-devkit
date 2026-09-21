#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
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
op compile
"""

import os
from shutil import copy


current_dir = os.path.abspath(os.getcwd())


def check_args(args: tuple, expect_args: list, msg: str) -> None:
    """
    check args
    """
    if args not in expect_args:
        return False
    return True


def check_and_config_para(input_x1: dict, input_x2: dict, output_z: dict) -> bool:
    """
    check_and_config_para
    """
    # get format and dtype
    format_a = input_x1.get("format")
    format_b = input_x2.get("format")
    format_out = output_z.get("format")
    dtype_a = input_x1.get("dtype").lower()
    dtype_b = input_x2.get("dtype").lower()
    dtype_out = output_z.get("dtype").lower()

    expect_args = [
        ("FRACTAL_NZ", "float16", "FRACTAL_NZ", "float16", "FRACTAL_NZ", "float16"),
        ("ND", "float16", "ND", "float16", "ND", "float16"),
        ("ND", "float16", "FRACTAL_NZ", "float16", "ND", "float16"),
    ]
    return check_args(
        (format_a, dtype_a, format_b, dtype_b, format_out, dtype_out),
        expect_args,
        "format_a, dtype_a, format_b, dtype_b, format_out, dtype_out",
    )


def copy_compile_res_files_to_output(kernel_name):
    """copy .o and .json file to output path"""
    print("stub opc stub files MatMul. copy_compile_res_files_to_output")
    # if output path not exist, creat it
    test_file_dir = os.path.abspath(os.path.dirname(__file__))
    json_res_path = test_file_dir + "/kernel_meta/MatMul_build_res.json"
    o_res_path = test_file_dir + "/kernel_meta/MatMul_build_res.o"

    json_file_name = kernel_name + ".json"
    o_file_name = kernel_name + ".o"

    test_root_dir = os.path.abspath(os.path.join(test_file_dir, "../.."))

    debug_dir = test_root_dir + "/debug_dir"  # same as testcase
    json_output_path = debug_dir + "/kernel_meta/" + json_file_name
    o_output_path = debug_dir + "/kernel_meta/" + o_file_name
    print("json_res_path:")
    print(json_res_path)
    print("json_output_path:")
    print(json_output_path)
    print("current_dir:")
    print(current_dir)

    try:
        copy(os.path.realpath(json_res_path), json_output_path)
        copy(os.path.realpath(o_res_path), o_output_path)
    except Exception as e:
        raise RuntimeError("Copy [%s] to [%s] field, reason: %s." % (json_res_path, json_output_path, str(e)))
    finally:
        pass


def fresnel_cos(inputs, outputs, attrs, e, a, b, c, d, kernel_name):
    """fresnel_cos"""
    print("stub opc stub files fresnel_cos.py fresnel_cos")
    # mat_mul start args:
    # inputs:
    # {'shape': [-1, -1, 16, 16], 'ori_shape': [-1, -1], 'format': 'FRACTAL_NZ', 'ori_format': 'ND', 'dtype': 'float16', 'range': [[4, 7], [4, 7], [16, 16], [16, 16]], 'ori_range': [[49, 112], [49, 112]]}
    # outputs:
    # {'shape': [-1, -1, 16, 16], 'ori_shape': [-1, -1], 'format': 'FRACTAL_NZ', 'ori_format': 'ND', 'dtype': 'float16', 'range': [[4, 7], [4, 7], [16, 16], [16, 16]], 'ori_range': [[49, 112], [49, 112]]}
    # attrs:
    # {'shape': [1, 1, 16, 16], 'ori_shape': [1, 1], 'format': 'FRACTAL_NZ', 'ori_format': 'ND', 'dtype': 'float16', 'range': None, 'ori_range': None}
    # kernel_name:
    # None
    # a:
    # {'shape': [-1, -1, 16, 16], 'ori_shape': [-1, -1], 'format': 'FRACTAL_NZ', 'ori_format': 'ND', 'dtype': 'float16', 'range': [[4, 7], [4, 7], [16, 16], [16, 16]], 'ori_range': [[49, 112], [49, 112]]}
    # b:
    # False
    # c:
    # False
    # d:
    # 0
    # e:
    # MatMul_3c5cc7048dba0ba86df3f19ef07c72edaabcbff7783646e68d8f48d787faf4f7
    if check_and_config_para(inputs, a, outputs):
        print("stub opc stub files MatMul. py mat_mul param check ok")
        copy_compile_res_files_to_output(kernel_name)
        return
    else:
        print("stub opc stub files MatMul.py mat_mul param check fail")
        return
