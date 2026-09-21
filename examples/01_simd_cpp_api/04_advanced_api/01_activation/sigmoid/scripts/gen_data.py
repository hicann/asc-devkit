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
import sys
import numpy as np


def gen_golden_data_simple():
    dtype = np.float32
    np.set_printoptions(threshold=sys.maxsize)

    np.random.seed(123321)
    src_type = dtype
    src_shape = [1024]
    calcount = 1024
    api_mode = 0
    min_value = np.finfo(src_type).min
    max_value = np.finfo(src_type).max
    src = np.random.uniform(min_value, max_value, src_shape).astype(src_type)
    golden = np.zeros(src_shape).astype(src_type)

    if src_type in [np.float16]:
        src_tmp = src.astype(np.float32)
    else:
        src_tmp = src

    with np.errstate(over="ignore"):
        if api_mode == 1 or api_mode == 2:
            golden[:calcount] = (1.0 / (1.0 + np.exp(-src_tmp[:calcount]))).astype(src_type)
        else:
            golden = (1.0 / (1.0 + np.exp(-src_tmp))).astype(src_type)

    if src_type in [np.float16]:
        golden = golden.astype(src_type)

    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_src.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
