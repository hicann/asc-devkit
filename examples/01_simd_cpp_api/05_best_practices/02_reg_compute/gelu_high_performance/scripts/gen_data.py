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


COEFF_TANH_CUBIC = 0.044715
CHECK_INPUTS = np.array([-3, -2, -1, 0, 1, 2, 3], dtype=np.float32)


def gen_golden_data_simple():
    input_x = np.random.uniform(-10, 10, [8192, 8192]).astype(np.float32)
    input_x.flat[: CHECK_INPUTS.size] = CHECK_INPUTS
    golden = np.empty_like(input_x)
    # Independent float64 tanh reference, evaluated row by row to limit memory use.
    for row in range(input_x.shape[0]):
        reference_x = input_x[row].astype(np.float64)
        tanh_arg = np.sqrt(2 / np.pi) * (reference_x + COEFF_TANH_CUBIC * reference_x**3)
        golden[row] = 0.5 * reference_x * (1 + np.tanh(tanh_arg))

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
