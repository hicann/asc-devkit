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
from enum import Enum
import numpy as np
import argparse


class TransDataMode(Enum):
    NCDHW_FractalZ3D = 1
    FractalZ3D_NCDHW = 2  # fractal_z_3d D C1 H W, N1, N0, C0
    NCDHW_NDC1HWC0 = 3
    NDC1HWC0_NCDHW = 4


def gen_golden_data_simple(mode):
    dtype = np.float16
    np.random.seed(123)
    np.set_printoptions(threshold=sys.maxsize)
    np.set_printoptions(linewidth=32)
    n = 16
    c = 16
    d = 1
    h = 3
    w = 5
    n0 = 16
    c0 = 16
    hw0 = 16
    n1 = (n + n0 - 1) // n0
    c1 = (c + c0 - 1) // c0

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if mode == TransDataMode.NCDHW_FractalZ3D.value or mode == TransDataMode.NCDHW_NDC1HWC0.value:
        total_elements = n * c * d * h * w
        # 生成 NCDHW 格式的数组
        ncdhw_array = np.arange(total_elements).reshape(n, c, d, h, w).astype(dtype)
        src_array = ncdhw_array
        # 计算需要补齐到 8 的倍数的填充量
        N_pad = n1 * n0 - n if mode == 1 else 0
        C_pad = c1 * c0 - c
        # 使用 np.pad 补齐 N 轴和 C 轴
        padded_array = np.pad(
            ncdhw_array, ((0, N_pad), (0, C_pad), (0, 0), (0, 0), (0, 0)), mode="constant", constant_values=0
        )
        if mode == TransDataMode.NCDHW_FractalZ3D.value:
            # 调整数组形状
            reshaped_array = padded_array.reshape(n1, n0, c1, c0, d, h, w)
            # 转置数组
            # n1n0 c1c0dhw -> c1c0dhw n1n0
            transposed_array = reshaped_array.transpose(2, 3, 4, 5, 6, 0, 1)
            # c1 c0 dhwn1n0 -> c1 dhwn1n0 c0
            transposed_array = transposed_array.transpose(0, 2, 3, 4, 5, 6, 1)
            # c1 d hwn1n0c0 -> d c1 hwn1n0c0
            transposed_array = transposed_array.transpose(1, 0, 2, 3, 4, 5, 6)
        else:
            reshaped_array = padded_array.reshape(n, c1, c0, d, h, w)
            transposed_array = reshaped_array.transpose(0, 1, 3, 4, 5, 2)
            transposed_array = transposed_array.transpose(0, 2, 1, 3, 4, 5)
        dst_array = transposed_array

        src_array.tofile("./input/input_src.bin")
        dst_array.flatten().tofile("./output/golden.bin")

    elif mode == TransDataMode.FractalZ3D_NCDHW.value:
        hw1 = (h * w + hw0 - 1) // hw0
        src_shape = (d, c1, h, w, n1, n0, c0)
        src_size = int(np.prod(src_shape))
        src = np.random.uniform(-100, 100, size=(src_size,)).astype(dtype)
        # src: d c1 h w n1 n0 c0    (fractal_z_3d)
        golden = src.reshape((d, c1, h * w, n1, n0, c0))
        # d, c1, hw n1 n0 c0 -> c1, d, hw n1 n0 c0
        golden = golden.transpose(1, 0, 2, 3, 4, 5)
        # c1, d hw n1n0, c0 -> c1, c0, d hw n1n0
        golden = golden.transpose(0, 5, 1, 2, 3, 4)
        # c1c0, d hw, n1, n0 -> n1n0, c1c0, d hw
        golden = golden.transpose(4, 5, 0, 1, 2, 3)
        golden = golden.reshape((n1 * n0, c1 * c0, d, h * w))
        # n1n0, c1c0, d hw -> n, c, d, hw
        diff_n = n - n1 * n0
        diff_c = c - c1 * c0
        golden = golden[: (diff_n if diff_n != 0 else None), : (diff_c if diff_c != 0 else None), :, :]
        src.flatten().tofile("./input/input_src.bin")
        golden.flatten().tofile("./output/golden.bin")
    elif mode == TransDataMode.NDC1HWC0_NCDHW.value:
        total_elements = n * c * d * h * w
        ncdhw_array = np.arange(total_elements).reshape(n, c, d, h, w).astype(dtype)
        n_pad = 0
        c_pad = c1 * c0 - c
        padded_array = np.pad(
            ncdhw_array, ((0, n_pad), (0, c_pad), (0, 0), (0, 0), (0, 0)), mode="constant", constant_values=0
        )
        reshaped_array = padded_array.reshape(n, c1, c0, d, h, w)
        transposed_array = reshaped_array.transpose(0, 1, 3, 4, 5, 2)
        transposed_array = transposed_array.transpose(0, 2, 1, 3, 4, 5)

        src_array = transposed_array
        dst_array = ncdhw_array
        src_array.tofile("./input/input_src.bin")
        dst_array.flatten().tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", type=int, choices=range(1, 5), help="指定数据转换的场景，取值1~4")
    args = parser.parse_args()
    mode = args.mode if args.mode else 1
    gen_golden_data_simple(mode)
