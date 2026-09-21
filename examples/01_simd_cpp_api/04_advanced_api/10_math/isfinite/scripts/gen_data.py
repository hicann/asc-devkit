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
import math
import random
import numpy as np


def random_num_spec_range(max_val, min_val, size, dtype, range_ls=None, range_ratio=None):
    """ """
    x = np.random.uniform(min_val, max_val, size=(size,)).astype(dtype)

    offset = 0
    count_ratio = 0
    for (low, high), ratio in zip(range_ls, range_ratio):
        sub_len = math.ceil(size * ratio)
        tmp_arr = np.random.uniform(low, high, sub_len).astype(dtype)
        x[offset : offset + sub_len] = tmp_arr[:]
        count_ratio += ratio
        offset += sub_len
    return x


def random_set_inf(src_seq, src_cpp_type):
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 10):
            sign = random.choice([-1, 1])
            if "float" in str(src_cpp_type):
                seq[i] = sign * np.inf
            elif "half" in str(src_cpp_type):
                seq[i] = sign * np.inf
            elif "bfloat16_t" in str(src_cpp_type):

                def to_bfloat16(x):
                    return np.frombuffer(np.array(x, dtype=np.float32).tobytes()[::2], dtype=np.uint16)

    return


def random_set_nan(src_seq, src_cpp_type, dst_cpp_type):
    for seq in src_seq:
        for i in random.sample(range(len(seq)), len(seq) // 10):
            if "int" in str(dst_cpp_type):
                seq[i] = 0
            else:
                seq[i] = np.nan
    return


def gen_golden_data_simple():
    dtype = np.float32
    src_type = np.float32
    dst_type = np.float32
    count = 1024
    data_size = 1024
    dtype = src_type
    max_val = 65504
    min_val = -65504

    src = random_num_spec_range(max_val, min_val, data_size, dtype, range_ls=[(-10, 10)], range_ratio=[0.2])
    random_set_inf([src], src_type)
    random_set_nan([src], src_type, dst_type)
    golden = np.zeros(data_size).astype(dst_type)

    golden[:count] = np.isfinite(src[:count]).astype(dst_type)

    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_x.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
