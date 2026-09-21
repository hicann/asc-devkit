#!/usr/bin/python3
# coding=utf-8

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
    m, n, k, is_bias = 8192, 8192, 8192, False

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm = np.random.uniform(-1, 1, [m, k]).astype(np.float16)
    x2_gm = np.random.uniform(-1, 1, [k, n]).astype(np.float16)
    matmul_result = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))

    # Apply GELU activation: GELU(x) ≈ x / (1 + e^(-2·√(2/π)·(x + 0.044715·x³)))
    # where -2·√(2/π) ≈ -1.595769
    # Use float64 for intermediate computation to avoid overflow in x³ and exp()
    matmul_f64 = matmul_result.astype(np.float64)
    coeff = -1.595769
    exponent = coeff * (matmul_f64 + 0.044715 * (matmul_f64**3))
    # Clip exponent to [-88, 88] to avoid exp() overflow in float64
    # For exponent > 88: exp()→∞, GELU(x)→0 (x is large negative)
    # For exponent < -88: exp()→0, GELU(x)→x (x is large positive)
    golden = (matmul_f64 / (1.0 + np.exp(np.clip(exponent, -88.0, 88.0)))).astype(np.float32)

    x1_gm.tofile("./input/x1_gm.bin")
    # x2_gm transpose to match B matrix transpose
    x2_gm = x2_gm.transpose()
    x2_gm.tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
