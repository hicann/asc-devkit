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


def split_and_reorder(data):
    data_uint8 = data.astype(np.uint8)
    low_bits_unsigned = data_uint8 & 0x0F
    high_bits_unsigned = (data_uint8 >> 4) & 0x0F
    low_bits_signed = np.where(low_bits_unsigned > 7, low_bits_unsigned - 16, low_bits_unsigned)
    high_bits_signed = np.where(high_bits_unsigned > 7, high_bits_unsigned - 16, high_bits_unsigned)
    result = np.empty(len(data) * 2, dtype=np.int8)
    result[0::2] = low_bits_signed
    result[1::2] = high_bits_signed
    return result


def gen_golden_data_simple():
    x1_gm_type = np.int8
    x2_gm_type = np.int8
    l0c_type = np.int32
    used_core_num = 1
    m = 256
    n = 256
    k = 256
    single_core_m = 256
    single_core_n = 256
    single_core_k = 256
    base_m = 128
    base_n = 256
    base_k = 128
    depth_a1 = 4
    depth_b1 = 2
    step_m = 2
    step_n = 1
    step_ka = 2
    step_kb = 2
    a1_length = base_m * base_k * np.dtype(x1_gm_type).itemsize
    b1_length = base_k * base_n * np.dtype(x2_gm_type).itemsize
    co1_length = base_m * base_n * np.dtype(l0c_type).itemsize
    trans_length = np.max([a1_length, b1_length, co1_length])
    iterate_order = 0

    x1_gm_int4x2 = np.random.randint(1, 10, [256, 128]).astype(np.int8)
    x2_gm_int4x2 = np.random.randint(1, 10, [256, 128]).astype(np.int8)

    x1_gm_unzipped = np.zeros([256, 256], dtype=x1_gm_type)
    x2_gm_unzipped = np.zeros([256, 256], dtype=x2_gm_type)
    for i in range(256):
        x1_gm_unzipped[i] = split_and_reorder(x1_gm_int4x2[i])
        x2_gm_unzipped[i] = split_and_reorder(x2_gm_int4x2[i])

    tiling = np.zeros((32), np.int32)
    l1_size = 0
    l0c_size = 0
    ub_size = 0
    l1_size += depth_a1 * a1_length
    l1_size += depth_b1 * b1_length
    iterate_size = 1
    l0c_size += iterate_size * co1_length
    ub_size += iterate_size * trans_length
    ub_size += trans_length
    ub_size += base_n * base_m * np.dtype(l0c_type).itemsize
    tiling[0:32] = [
        used_core_num,
        m,
        n,
        k,
        k,
        single_core_m,
        single_core_n,
        single_core_k,
        base_m,
        base_n,
        base_k,
        depth_a1,
        depth_b1,
        step_m,
        step_n,
        0,
        trans_length,
        iterate_order,
        0,
        l1_size,
        l0c_size,
        ub_size,
        1,
        1,
        1,
        1,
        step_ka,
        step_kb,
        1,
        1,
        1,
        0,
    ]

    golden = np.matmul(x1_gm_unzipped.astype(np.int32), x2_gm_unzipped.astype(np.int32)).astype(l0c_type)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    tiling.tofile("./input/input_tiling.bin")
    golden.tofile("./output/golden.bin")
    x1_gm_int4x2.tofile("./input/input_x4.bin")
    x2_gm_int4x2.tofile("./input/input_y4.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
