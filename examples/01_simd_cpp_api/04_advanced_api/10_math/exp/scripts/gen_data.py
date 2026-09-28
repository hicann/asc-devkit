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


def taylor_exp(src, n):
    if n < 1:
        raise ValueError("taylor_exp: n must be >= 1")
    item = np.ones_like(src).astype(np.float32)
    result = np.ones_like(src).astype(np.float32)
    for i in range(n):
        item *= src / (i + 1)
        result += item
    return result


def gen_golden_data_simple(item_num=10):
    dtype = np.float32
    cal_count = 8192
    src_shape = [cal_count]
    np.random.seed(0)
    input_dtype = dtype
    min_num, max_num = input_dtype(-10), input_dtype(10)

    src = np.random.uniform(min_num, max_num, src_shape).astype(input_dtype)
    src_exp = src[:cal_count]
    src_ori = np.zeros(src.size - cal_count).astype(src.dtype)
    if item_num:
        xa = np.floor(src_exp)
        xb = src_exp - xa
        src_exp = np.exp(xa) * taylor_exp(xb, item_num)
    else:
        src_exp = np.exp(src_exp)
    golden = np.concatenate((src_exp, src_ori), axis=None)

    if input_dtype != np.float32:
        golden = golden.astype(np.float16)

    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_x.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
