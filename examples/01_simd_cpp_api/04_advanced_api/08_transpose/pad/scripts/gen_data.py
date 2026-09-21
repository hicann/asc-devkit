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


def gen_golden_data_simple():
    input_dtype = np.float32
    output_dtype = np.float32
    input_shape = [16, 31]
    output_shape = [16, 32]
    input_x = np.arange(0, input_shape[0] * input_shape[1], 1, input_dtype).reshape(input_shape[0], input_shape[1])
    golden = np.arange(0, output_shape[0] * output_shape[1], 1, output_dtype).reshape(output_shape[0], output_shape[1])

    height = input_shape[0]
    width = input_shape[1]
    left_pad = 0
    right_pad = 1
    pad_value = 321

    if width * (np.dtype(input_dtype).itemsize) % 32 == 0:
        ori_width = input_shape[2]
        for i in range(0, height):
            for j in range(0, width):
                if j >= ori_width and j <= ori_width + right_pad:
                    golden[i][j] = pad_value
                    if j < width:
                        input_x[i][j] = 0
                    else:
                        golden[i][j] = input_x[i][j]
    else:
        golden = np.zeros((output_shape[0], output_shape[1]), dtype=input_dtype)
        for i in range(0, height):
            for j in range(0, width + left_pad + right_pad):
                if j <= left_pad - 1 or j >= width + left_pad:
                    golden[i][j] = pad_value
                else:
                    golden[i][j] = input_x[i][j - left_pad]

    os.makedirs("input", exist_ok=True)
    input_x.tofile("./input/input_x.bin")

    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
