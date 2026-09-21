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
    input_shape = [8, 8]
    output_shape = [8, 7]
    output_dtype = np.float32

    input = np.arange(0, input_shape[0] * input_shape[1], 1, input_dtype).reshape(input_shape[0], input_shape[1])

    golden = np.zeros((output_shape[0], output_shape[1]), dtype=input_dtype)

    height = input_shape[0]
    width = input_shape[1]

    left = 0
    right = 1
    for i in range(0, height):
        for j in range(left, width - right):
            golden[i][j - left] = input[i][j]

    os.makedirs("input", exist_ok=True)
    input.tofile("./input/input_x.bin")

    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
