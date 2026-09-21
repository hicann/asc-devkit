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


def gen_golden_data():
    m = 256
    n = 7680
    k = 128

    x1_gm = np.random.uniform(-5, 5, [m, k]).astype(np.int8)
    x2_gm = np.random.uniform(-5, 5, [k, n]).astype(np.int8)
    bias_gm = np.random.uniform(-5, 5, [n]).reshape([n]).astype(np.int32)
    golden = np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32)).astype(np.int32) + bias_gm

    x1_gm_int4 = np.zeros(shape=[m, k // 2]).astype(np.int8)
    for i in range(m):
        for j in range(k):
            if j % 2 == 0:
                x1_gm_int4[i][j // 2] = (x1_gm[i][j + 1] << 4) + (x1_gm[i][j] & 0x0F)

    x2_gm_int4 = np.zeros(shape=[k, n // 2]).astype(np.int8)
    for i in range(k):
        for j in range(n):
            if j % 2 == 0:
                x2_gm_int4[i][j // 2] = (x2_gm[i][j + 1] << 4) + (x2_gm[i][j] & 0x0F)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm_int4.tofile("./input/x1_gm.bin")
    x2_gm_int4.tofile("./input/x2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
