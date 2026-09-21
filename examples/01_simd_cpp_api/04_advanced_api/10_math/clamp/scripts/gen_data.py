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
import enum
import random
import numpy as np


class ScalarType(enum.Enum):
    BOTH_TENSOR = 1
    TENSOR_SCALAR = 2
    SCALAR_TENSOR = 3
    BOTH_SCALAR = 4


def random_set_boundary(src_seq):
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = np.inf
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = -np.inf
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = -np.nan
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = np.nan
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = 0
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 20):
            seq[i] = -0
    return


def gen_golden_data_simple():
    dtype = np.float32
    count = int(128)
    data_size = int(128)
    scalar_type = int(4)
    is_dynamic = int(0)

    src = np.random.uniform(-100, 100, [128]).astype(dtype)
    src_min = np.random.uniform(-80, 30, [128]).astype(dtype)
    src_max = np.random.uniform(0, 100, [128]).astype(dtype)
    tiling = np.array([count, data_size, scalar_type], dtype="uint32")

    if dtype == np.half or dtype == np.float32 or dtype == ml_dtypes.bfloat16:
        random_set_boundary([src])
    golden = np.zeros([128]).astype(dtype)
    if scalar_type == ScalarType.BOTH_TENSOR.value:
        golden[:count] = np.clip(src[:count], src_min[:count], src_max[:count]).astype(dtype)
    elif scalar_type == ScalarType.TENSOR_SCALAR.value:
        golden[:count] = np.clip(src[:count], src_min[:count], src_max[0]).astype(dtype)
    elif scalar_type == ScalarType.SCALAR_TENSOR.value:
        golden[:count] = np.clip(src[:count], src_min[0], src_max[:count]).astype(dtype)
    else:
        golden[:count] = np.clip(src[:count], src_min[0], src_max[0]).astype(dtype)

    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_src.bin")
    src_min.tofile("./input/input_src_min.bin")
    src_max.tofile("./input/input_src_max.bin")
    tiling.tofile("./input/input_tiling.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
