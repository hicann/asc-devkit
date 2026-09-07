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
    # Fix the random seed so input and golden are reproducible
    np.random.seed(0)

    # Dimensions must match M / K / N in matmul.asc
    m = 30
    k = 40
    n = 70

    # Use a positive range: with [-10, 10] the accumulated results cluster around 0,
    # the np.isclose tolerance degrades to absolute_tol, which is on the same order as the float32 accumulation error,
    # and the precision check would fail randomly.
    x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
    x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
    bias_gm = np.random.uniform(1, 10, [n]).astype(np.float32)
    golden = (np.matmul(x1_gm, x2_gm) + bias_gm).astype(np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
