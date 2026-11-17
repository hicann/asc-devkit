#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import os

import numpy as np


def fast_gelu(value):
    denominator = 1.0 + np.exp(-1.702 * value)
    return value / denominator


def gen_golden_data():
    m = 1024
    n = 1024
    k = 256

    input_x = np.random.uniform(-2, 2, [m, k]).astype(np.int8)
    input_y = np.random.uniform(-2, 2, [k, n]).astype(np.int8)
    channel_scale = np.random.uniform(0.001, 0.01, [n]).astype(np.float32)
    token_scale = np.random.uniform(0.001, 0.01, [m]).astype(np.float32)

    mm_result = np.matmul(input_x, input_y).astype(np.int32)
    dequant_result = mm_result * channel_scale[np.newaxis, :] * token_scale[:, np.newaxis]
    golden = fast_gelu(dequant_result).astype(np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.transpose().tofile("./input/input_y.bin")
    channel_scale.tofile("./input/channel_scale.bin")
    token_scale.tofile("./input/token_scale.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
