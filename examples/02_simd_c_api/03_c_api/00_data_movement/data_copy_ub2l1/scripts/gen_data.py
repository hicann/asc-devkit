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


M = 127
K = 129
N = 130
C0_SIZE = 16


def ceil_align(value, factor):
    return (value + factor - 1) // factor * factor


M_ALIGN = ceil_align(M, C0_SIZE)
K_ALIGN = ceil_align(K, C0_SIZE)
N_ALIGN = ceil_align(N, C0_SIZE)


def pad_matrix(data_nd, rows, columns):
    padded = np.zeros((rows, columns), dtype=data_nd.dtype)
    padded[: data_nd.shape[0], : data_nd.shape[1]] = data_nd
    return padded


def nd_to_nz(data_nd):
    rows, columns = data_nd.shape
    return (
        data_nd.reshape(rows // C0_SIZE, C0_SIZE, columns // C0_SIZE, C0_SIZE)
        .transpose(2, 0, 1, 3)
        .copy()
    )


def generate_data(scenario_num=1):
    np.random.seed(9)
    a_nd = np.random.uniform(-1, 1, (M, K)).astype(np.float16)
    b_nd = np.random.uniform(-1, 1, (K, N)).astype(np.float16)
    golden = np.matmul(a_nd.astype(np.float32), b_nd.astype(np.float32)).astype(
        np.float32
    )

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    a_nz = nd_to_nz(pad_matrix(a_nd, M_ALIGN, K_ALIGN))
    b_nz = nd_to_nz(pad_matrix(b_nd, K_ALIGN, N_ALIGN))

    if scenario_num in (1, 3):
        a_nz.tofile("./input/x1_gm.bin")
    else:
        a_nd.tofile("./input/x1_gm.bin")

    if scenario_num == 1:
        b_nz.tofile("./input/x2_gm.bin")
    elif scenario_num == 2:
        b_nd.tofile("./input/x2_gm.bin")

    if scenario_num == 3:
        a_nz.tofile("./output/golden.bin")
    else:
        golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=(1, 2, 3))
    args = parser.parse_args()
    generate_data(args.scenarioNum)
