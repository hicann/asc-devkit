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
import sys
import numpy as np

ML_DTYPES_IMPORT_ERROR = None
try:
    import ml_dtypes

    fp8_e4m3 = ml_dtypes.float8_e4m3fn
    fp8_e5m2 = ml_dtypes.float8_e5m2
except ImportError as error:
    ML_DTYPES_IMPORT_ERROR = error

EN_DTYPES_IMPORT_ERROR = None
try:
    import en_dtypes

    fp4_e1m2 = en_dtypes.float4_e1m2
    fp4_e2m1 = en_dtypes.float4_e2m1
except ImportError as error:
    EN_DTYPES_IMPORT_ERROR = error


def pack_fp4_to_fp4x2(fp4_data):
    row = fp4_data.shape[0]
    col = fp4_data.shape[1]
    fp4_flat = fp4_data.flatten()
    fp4_high = fp4_flat[::2].view(np.uint8)
    fp4_low = fp4_flat[1::2].view(np.uint8)
    low_bits = (fp4_low & 0x0F) << 4
    high_bits = fp4_high & 0x0F
    combined = low_bits | high_bits
    packed = combined.reshape(row, col // 2)
    return packed


def mx_decompress(fp_data, scale_data, block_size=32):
    scale_exp = scale_data.astype(np.float32) - 127.0
    scale_factor = np.power(2.0, scale_exp)

    result = np.zeros(fp_data.shape, dtype=np.float32)

    for i in range(fp_data.shape[1]):
        block_idx = i // block_size
        result[:, i] = fp_data[:, i].astype(np.float32) * scale_factor[:, block_idx]

    return result


def mx_decompress_b(fp_data, scale_data, block_size=32):
    scale_exp = scale_data.astype(np.float32) - 127.0
    scale_factor = np.power(2.0, scale_exp)

    result = np.zeros(fp_data.shape, dtype=np.float32)

    for row in range(fp_data.shape[0]):
        for col in range(fp_data.shape[1]):
            block_idx = row // block_size
            result[row, col] = (
                fp_data[row, col].astype(np.float32) * scale_factor[block_idx, col]
            )

    return result


def layout_scale_a_trans(scale_data):
    m, scale_k = scale_data.shape
    return scale_data.reshape(m, scale_k // 2, 2).transpose(1, 0, 2).copy()


def layout_scale_b_notrans(scale_data):
    scale_k, n = scale_data.shape
    return scale_data.reshape(scale_k // 2, 2, n).transpose(0, 2, 1).copy()


def gen_golden_data_fp4(scenario_num, m, n, k):
    if EN_DTYPES_IMPORT_ERROR is not None:
        raise ImportError(
            "FP4 scenarios require en_dtypes. Install en_dtypes==0.0.4."
        ) from EN_DTYPES_IMPORT_ERROR

    scale_ceil_number = 32
    scale_align_number = 2
    scale_k_unaligned = (k + scale_ceil_number - 1) // scale_ceil_number
    sk = (
        (scale_k_unaligned + scale_align_number - 1) // scale_align_number
    ) * scale_align_number

    if scenario_num == 1:
        a_dtype = fp4_e1m2
        b_dtype = fp4_e2m1
    elif scenario_num == 2:
        a_dtype = fp4_e2m1
        b_dtype = fp4_e1m2
    else:
        raise ValueError(f"Invalid scenario_num {scenario_num} for FP4")

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm = np.random.uniform(-2, 2, [m, k]).astype(a_dtype)
    x2_gm = np.random.uniform(-2, 2, [k, n]).astype(b_dtype)

    x1_scale_gm = np.random.randint(127, 130, [m, sk]).astype(np.uint8)
    x2_scale_gm = np.random.randint(127, 130, [sk, n]).astype(np.uint8)

    x1_full = mx_decompress(x1_gm.astype(np.float32), x1_scale_gm, 32)
    x2_full = mx_decompress_b(x2_gm.astype(np.float32), x2_scale_gm, 32)

    golden = np.matmul(x1_full.astype(np.float64), x2_full.astype(np.float64)).astype(
        np.float32
    )
    if scenario_num == 2:
        bias_gm = np.random.randint(-10, 10, [n]).astype(np.float32)
        golden = golden + bias_gm
        bias_gm.tofile("./input/bias_gm.bin")

    print("B/scaleB transpose")
    x2_gm = x2_gm.transpose()
    x2_scale_gm = x2_scale_gm.transpose().copy()

    x1_packed = pack_fp4_to_fp4x2(x1_gm.view(np.uint8))
    x2_packed = pack_fp4_to_fp4x2(x2_gm.view(np.uint8))

    x1_packed.tofile("./input/x1_gm.bin")
    x2_packed.tofile("./input/x2_gm.bin")
    x1_scale_gm.tofile("./input/x1_scale_gm.bin")
    x2_scale_gm.tofile("./input/x2_scale_gm.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_fp8(scenario_num, m, n, k):
    if ML_DTYPES_IMPORT_ERROR is not None:
        raise ImportError(
            "FP8 scenarios require ml_dtypes. Install ml_dtypes==0.2.0."
        ) from ML_DTYPES_IMPORT_ERROR

    scale_ceil_number = 32
    scale_align_number = 2
    scale_k_unaligned = (k + scale_ceil_number - 1) // scale_ceil_number
    sk = (
        (scale_k_unaligned + scale_align_number - 1) // scale_align_number
    ) * scale_align_number

    if scenario_num != 3:
        raise ValueError(f"Invalid scenario_num {scenario_num} for FP8")
    a_dtype = fp8_e4m3
    b_dtype = fp8_e5m2

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm = np.random.uniform(-10, 10, [m, k]).astype(a_dtype)
    x2_gm = np.random.uniform(-10, 10, [k, n]).astype(b_dtype)

    x1_scale_gm = np.random.randint(127, 130, [m, sk]).astype(np.uint8)
    x2_scale_gm = np.random.randint(127, 130, [sk, n]).astype(np.uint8)

    x1_full = mx_decompress(x1_gm.astype(np.float32), x1_scale_gm, 32)
    x2_full = mx_decompress_b(x2_gm.astype(np.float32), x2_scale_gm, 32)

    golden = (
        np.matmul(x1_full.astype(np.float64), x2_full.astype(np.float64)).astype(
            np.float32
        )
        * 2
    )

    print("B/scaleB transpose")
    x2_gm = x2_gm.transpose()
    x2_scale_gm = x2_scale_gm.transpose().copy()

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    x1_scale_gm.tofile("./input/x1_scale_gm.bin")
    x2_scale_gm.tofile("./input/x2_scale_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    m, n, k = 40, 50, 70

    scenario_num = 1
    if len(sys.argv) > 1:
        scenario_num = int(sys.argv[1].split("=")[1])

    if scenario_num in [1, 2]:
        gen_golden_data_fp4(scenario_num, m, n, k)
    elif scenario_num == 3:
        gen_golden_data_fp8(scenario_num, m, n, k)
    else:
        raise ValueError(f"Invalid scenario_num {scenario_num}")
