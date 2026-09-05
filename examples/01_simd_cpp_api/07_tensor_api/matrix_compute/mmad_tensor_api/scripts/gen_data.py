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

    # A矩阵: [M, K], half, 不转置
    input_x = np.random.uniform(-2, 2, [M, K]).astype(np.int8)
    # B矩阵: [K, N], half, 转置存储
    input_y = np.random.uniform(-2, 2, [K, N]).astype(np.int8)
    # Bias: [N], float
    quant = np.random.uniform(-2, 2, [N]).astype(np.float32)
    uint32_quant = np.frombuffer(quant, np.uint32)
    uint32_quant &= 0xFFFFE000
    quant_tensor = uint32_quant.astype(np.uint64)
    quant_tensor |= 1 << 46

    golden = np.matmul(input_x, input_y).astype(np.int32)
    golden = golden * quant
    golden = golden.astype(np.half)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    # B转置后保存 [K,N] -> [N,K]
    input_y.transpose().tofile("./input/input_y.bin")
    quant_tensor.tofile("./input/quant.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
