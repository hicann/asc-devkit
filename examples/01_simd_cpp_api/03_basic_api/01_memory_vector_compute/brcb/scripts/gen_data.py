#!/usr/bin/python3
# coding=utf-8

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
import numpy as np


def get_range_by_dtype(input_type):
    try:
        if input_type == np.float16 or input_type == np.float32 or input_type == np.float64:
            return np.finfo(input_type).min, np.finfo(input_type).max
        else:
            return np.iinfo(input_type).min, np.iinfo(input_type).max
    except ValueError:
        print(f"Unsupported data type:{input_type}")


def gen_golden_data_simple():
    input_type = np.dtype("uint16")
    output_type = input_type
    min_val, max_val = get_range_by_dtype(input_type)
    block_length = 256
    one_data_block_size = 32  # 一个datablock能处理32B数据

    input_shape = [block_length * input_type.itemsize // one_data_block_size]
    output_shape = [block_length]
    input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
    golden = np.zeros(output_shape).astype(output_type)
    for i in range(len(input_x)):
        golden[
            i * (one_data_block_size // input_type.itemsize) : (i + 1) * (one_data_block_size // input_type.itemsize)
        ] = input_x[i]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
