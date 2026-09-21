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

M = 64
N = 7680
K = 64


def gen_c_data_nz_format(y_gm_in):
    nz_fractal_m = 16
    nz_fractal_n = 8
    c0_size = 8

    align_m = int(int((M + nz_fractal_m - 1) / nz_fractal_m) * nz_fractal_m)
    align_n = int(int((N + nz_fractal_n - 1) / nz_fractal_n) * nz_fractal_n)
    y_gm_pad = np.zeros([align_m, align_n])
    y_gm_pad[0:M, 0:N] = y_gm_in
    y_gm = y_gm_pad.astype(np.float32)
    y_gm = (
        y_gm.reshape((int(align_m / 16), 16, int(align_n / c0_size), c0_size)).transpose(2, 0, 1, 3).astype(np.float32)
    )
    return y_gm


def gen_golden_data():
    x1_gm = np.random.uniform(-1, 1, [M, K]).astype(np.float16)
    x2_gm = np.random.uniform(-1, 1, [K, N]).astype(np.float16)
    bias_gm = np.random.uniform(-10, 10, [N]).reshape([N]).astype(np.float32)
    golden = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(np.float32) + bias_gm

    golden_nz = gen_c_data_nz_format(golden)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    bias_gm.tofile("./input/bias_gm.bin")
    golden_nz.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
