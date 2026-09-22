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


def generate_data():
    batch = 16
    sequence_length = 64
    hidden_size = 256
    projection_size = 3 * hidden_size

    rng = np.random.default_rng(0)
    input_data = rng.uniform(-1.0, 1.0, (batch, sequence_length, hidden_size)).astype(np.float16)
    weight = rng.uniform(-1.0, 1.0, (hidden_size, projection_size)).astype(np.float16)
    bias = rng.uniform(-1.0, 1.0, (projection_size,)).astype(np.float16)

    flattened_input = input_data.reshape(batch * sequence_length, hidden_size).astype(np.float32)
    golden = flattened_input @ weight.astype(np.float32) + bias.astype(np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_data.tofile("input/input.bin")
    weight.tofile("input/weight.bin")
    bias.tofile("input/bias.bin")
    golden.astype(np.float16).tofile("output/golden.bin")


if __name__ == "__main__":
    generate_data()
