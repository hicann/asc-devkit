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
    m, n, k, is_bias = 7680, 480, 320, True

    a1_gm = np.random.uniform(-10, 10, [m, k]).reshape([m, k]).astype(np.float16)
    b1_gm = np.random.uniform(-10, 10, [k, n]).reshape([k, n]).astype(np.float16)
    a2_gm = np.random.uniform(-10, 10, [m, k]).reshape([m, k]).astype(np.float16)
    b2_gm = np.random.uniform(-10, 10, [k, n]).reshape([k, n]).astype(np.float16)
    bias_gm = np.random.uniform(-10, 10, [n]).reshape([n]).astype(np.float32)

    golden_1 = np.matmul(a1_gm.astype(np.float32), b1_gm.astype(np.float32), dtype=np.float32) + bias_gm
    golden_2 = np.matmul(a2_gm.astype(np.float32), b2_gm.astype(np.float32), dtype=np.float32) + bias_gm

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    a1_gm.tofile("./input/a1_gm.bin")
    b1_gm.tofile("./input/b1_gm.bin")
    a2_gm.tofile("./input/a2_gm.bin")
    b2_gm.tofile("./input/b2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    golden_1.tofile("./output/golden1.bin")
    golden_2.tofile("./output/golden2.bin")


if __name__ == "__main__":
    gen_golden_data()
