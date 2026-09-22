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

VAL_1 = 0
VAL_2 = 1
VAL_3 = 2
VAL_4 = 3


def philox4_round(counter, key, philox_m, len_w, mask_w):
    prod = philox_m[VAL_1] * counter[VAL_1]
    hi_1 = prod >> len_w
    lo_1 = prod & mask_w
    prod = philox_m[VAL_2] * counter[VAL_3]
    hi_2 = prod >> len_w
    lo_2 = prod & mask_w
    counter[VAL_1] = hi_2 ^ counter[VAL_2] ^ key[VAL_1]
    counter[VAL_2] = lo_2
    counter[VAL_3] = hi_1 ^ counter[VAL_4] ^ key[VAL_2]
    counter[VAL_4] = lo_1


def philox(counter, key, philox_round, philox_m, philox_bumpkey, philox_w, len_w, mask_w, rounds):
    for _ in range(rounds - 1):
        philox_round(counter, key, philox_m, len_w, mask_w)
        philox_bumpkey(key, philox_w, mask_w)
    philox_round(counter, key, philox_m, len_w, mask_w)
    return counter


PHILOX_M4_32 = [0xD2511F53, 0xCD9E8D57]
PHILOX_W_32 = [0x9E3779B9, 0xBB67AE85]
MASK_32 = 0xFFFFFFFF


def inc_counter(counter):
    for i in range(4):
        counter[i] = (counter[i] + 1) & MASK_32
        if counter[i] != 0:
            return counter
    return counter


def philox4_bumpkey(key, philox_w, mask_w):
    key[VAL_1] = (key[VAL_1] + philox_w[VAL_1]) & mask_w
    key[VAL_2] = (key[VAL_2] + philox_w[VAL_2]) & mask_w


def philox4_32(counter, key, rounds):
    return philox(counter, key, philox4_round, PHILOX_M4_32, philox4_bumpkey, PHILOX_W_32, 32, MASK_32, rounds)


def philox_random_with_stride(rounds, counter, key, stride, row, column):
    ret = list()
    if stride % 4 != 0 or column % 4 != 0 or column > stride:
        raise Exception(f"not support, {stride} {row} {column}")
    if row <= 0 or column <= 0:
        raise Exception(f"not support, {stride} {row} {column}")
    for _ in range(row):
        for _ in range(column // 4):
            ret.extend(philox4_32(counter[:], key[:], rounds))
            counter = inc_counter(counter[:])
        for _ in range((stride - column) // 4):
            counter = inc_counter(counter[:])
    return np.array(ret)


def uint2float(golden):
    import struct

    ret = list()
    for x in list(golden):
        man = x & 0x7FFFFF
        exp = 127
        val = (exp << 23) | man
        result = struct.unpack("f", struct.pack("I", val))[0]
        ret.append(result - 1.0)
    return np.array(ret).astype(np.float32)


def gen_golden_data_simple():
    dtype = np.float32
    counter = [0, 0, 0, 0]
    key = [0, 0]
    stride = 32
    row = 32
    column = 32

    golden = philox_random_with_stride(10, counter, key, stride, row, column).astype(np.uint32)
    if dtype == np.float32:
        golden = uint2float(golden)
    else:
        golden = golden.astype(dtype)
    golden = np.concatenate((golden, np.zeros(256).astype(dtype)))
    golden = np.tile(golden, 8)

    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
