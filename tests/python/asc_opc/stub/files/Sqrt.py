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
import os
from shutil import copy
import json


current_dir = os.path.abspath(os.getcwd())


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

    debug_dir = test_root_dir + "/debug_dir/kernel_meta_Sqrt"  # same as testcase
    json_output_path = debug_dir + "/kernel_meta/" + json_file_name
    o_output_path = debug_dir + "/kernel_meta/" + o_file_name
    print("json_res_path:")
    print(json_res_path)
    print("json_output_path:")
    print(json_output_path)
    print("current_dir:")
    print(current_dir)

    try:
        with open(json_res_path, "r", encoding="utf-8") as f:
            data = json.load(f)
        data["binFileName"] = "Sqrt"
        with open(json_output_path, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        copy(os.path.realpath(o_res_path), o_output_path)
    except Exception as e:
        raise RuntimeError("Copy [%s] to [%s] field, reason: %s." % (json_res_path, json_output_path, str(e)))
    finally:
        pass


def sqrt_compute(input_data, output_data, kernel_name="sqrt", impl_mode="high_performance"):
    copy_compile_res_files_to_output(kernel_name)
    return


def sqrt(input_x, output_y, kernel_name="sqrt", impl_mode="high_performance"):
    copy_compile_res_files_to_output(kernel_name)
    return
