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


import argparse
import os
import numpy as np


def gen_golden_data(mode=0):
    m, n, k, base_m, base_n, is_bias = 2558, 2045, 128, 80, 64, True
    x1_gm = np.random.uniform(-1, 1, [m, k]).astype(np.float16)
    x2_gm = np.random.uniform(-1, 1, [k, n]).astype(np.float16)
    bias_gm = np.random.uniform(-10, 10, [n]).reshape([n]).astype(np.float32)
    golden = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(np.float32) + bias_gm
    y_gm = np.random.uniform(-1, 1, [m, n]).astype(np.float32)

    for i in range(m):
        for j in range(n):
            upper_triangle_ignore_data = mode == 0 and (int((i + base_m) / base_m) > int((j + base_n) / base_n))
            lower_triangle_ignore_data = mode == 1 and (int((i + base_m) / base_m) < int((j + base_n) / base_n))
            if upper_triangle_ignore_data or lower_triangle_ignore_data:
                golden[i][j] = y_gm[i][j]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    y_gm.tofile("./input/y_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", type=int, default=0, choices=[0, 1])
    args = parser.parse_args()
    gen_golden_data(args.m)
