#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
    M = 1024
    N = 1024
    K = 256

    dtype = np.half

    # A矩阵: [M, K], B矩阵: [K, N]
    input_x = np.random.uniform(-2, 2, [M, K]).astype(dtype)
    input_y = np.random.uniform(-2, 2, [K, N]).astype(dtype)
    # Bias: [N], float
    bias = np.random.uniform(-2, 2, [N]).astype(np.float32)

    # 计算golden: C = A @ B + bias, float
    golden = np.matmul(input_x.astype(np.float32), input_y.astype(np.float32)).astype(
        np.float32
    )
    golden = golden + bias

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")

    bias.tofile("./input/bias.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
