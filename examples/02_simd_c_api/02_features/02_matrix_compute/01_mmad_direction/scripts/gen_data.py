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

import argparse
import os

import numpy as np


RANDOM_SEED = 20260902
QUANT_SCALE = 1.0


def convert_nd_to_nz(matrix, block_n):
    matrix_m, matrix_n = matrix.shape
    return (
        matrix.reshape(matrix_m // 16, 16, matrix_n // block_n, block_n)
        .transpose(2, 0, 1, 3)
        .reshape(-1)
    )


def pack_signed_int4(data):
    data_uint8 = data.astype(np.int8).view(np.uint8) & 0x0F
    return data_uint8[0::2] | (data_uint8[1::2] << 4)


def generate_data(scenario_num):
    matrix_m = 192 if scenario_num in (3, 4) else 256
    matrix_n = 64 if scenario_num in (3, 4) else 256
    matrix_k = 64 if scenario_num in (9, 10) else 128
    rng = np.random.default_rng(RANDOM_SEED)

    values = np.array([-1.0, 0.0, 1.0], dtype=np.float16)
    probabilities = np.array([0.18, 0.64, 0.18])
    a = rng.choice(values, size=(matrix_m, matrix_k), p=probabilities).astype(
        np.float16
    )
    b = rng.choice(values, size=(matrix_k, matrix_n), p=probabilities).astype(
        np.float16
    )
    golden_nd = np.matmul(a.astype(np.float32), b.astype(np.float32))

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    a.tofile("./input/x1_gm.bin")
    b.transpose().copy().tofile("./input/x2_gm.bin")

    if scenario_num in (1, 2):
        convert_nd_to_nz(golden_nd, 16).astype(np.float32).tofile("./output/golden.bin")
    elif scenario_num in (3, 4):
        quantized = np.rint(np.clip(golden_nd * QUANT_SCALE, -8, 7)).astype(np.int8)
        packed = pack_signed_int4(convert_nd_to_nz(quantized, 64))
        packed.tofile("./output/golden.bin")
    elif scenario_num in (5, 6):
        golden_nd.astype(np.float32).tofile("./output/golden.bin")
    elif scenario_num in (7, 8):
        golden_nd.transpose().astype(np.float32).tofile("./output/golden.bin")
    else:
        quantized = np.rint(np.clip(golden_nd * QUANT_SCALE, -128, 127)).astype(np.int8)
        convert_nd_to_nz(quantized, 32).tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 11))
    args = parser.parse_args()
    generate_data(args.scenarioNum)
