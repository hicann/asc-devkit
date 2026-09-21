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


def gen_golden_data():
    m = 32
    n = 256
    k = 64
    b = 3
    src_type = np.float16
    dst_type = np.float32

    is_bias = True
    is_trans_a = False
    is_trans_b = False

    bias_gm = np.random.uniform(-1, 1, [1, n]).astype(dst_type)

    for idx in range(b):
        x1_gm_left = np.random.uniform(-1, 1, [m, k]).astype(src_type)
        x2_gm_right = np.random.uniform(-1, 1, [k, n]).astype(src_type)
        golden_one = np.matmul(x1_gm_left.astype(dst_type), x2_gm_right.astype(dst_type)).astype(dst_type)
        if is_bias:
            golden_one = golden_one + bias_gm.astype(dst_type)
        if is_trans_a:
            x1_tmp = x1_gm_left.transpose()
        else:
            x1_tmp = x1_gm_left
        if is_trans_b:
            x2_tmp = x2_gm_right.transpose()
        else:
            x2_tmp = x2_gm_right
        if idx == 0:
            x1_gm = x1_tmp
            x2_gm = x2_tmp
            golden = golden_one
        else:
            x1_gm = np.vstack((x1_gm, x1_tmp))
            x2_gm = np.vstack((x2_gm, x2_tmp))
            golden = np.vstack((golden, golden_one))

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
