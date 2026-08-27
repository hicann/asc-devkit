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


def numpy_fmod(src0_gm, src1_gm):
    new_dtype = np.float32
    golden = np.fmod(src0_gm.astype(new_dtype), src1_gm.astype(new_dtype)).astype(
        src0_gm.dtype
    )
    return golden


def generate_random_data(min_range, max_range, shape, dtype):
    full_value = None
    if min_range in [np.inf, -np.inf] or np.isnan(min_range):
        full_value = min_range
    elif max_range in [np.inf, -np.inf] or np.isnan(max_range):
        full_value = max_range
    if full_value is not None:
        return np.full(shape, full_value, dtype=dtype)
    if min_range == max_range:
        return np.full(shape, min_range, dtype=dtype)
    return np.random.uniform(min_range, max_range, shape).astype(dtype)


def gen_golden_data_simple():
    np.random.seed(123321)
    dtype = np.float32
    src_shape = [159]
    src0 = generate_random_data(-10.0, 10.0, src_shape, dtype)
    src1 = generate_random_data(0.5, 5.0, src_shape, dtype)
    src1[1::2] = -src1[1::2]
    src0[:4] = [2.0, 2.0, -3.0, -3.0]
    src1[:4] = [1.5, -1.5, 1.1, -1.1]
    golden_gm = np.zeros([159]).astype(dtype)
    golden = numpy_fmod(src0, src1)
    valid_len = 159
    cal_count = 159
    golden_gm[0:valid_len] = golden[0:valid_len]

    os.makedirs("input", exist_ok=True)
    src0.tofile("./input/input_src0.bin")
    src1.tofile("./input/input_src1.bin")
    os.makedirs("output", exist_ok=True)
    golden_gm.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
