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
import argparse
import numpy as np

np.random.seed(9)


def gen_golden_data(scenario_num=1):
    m = 40
    n = 50
    k = 70

    x1_gm = None
    x2_gm = None
    golden = None

    if scenario_num == 1:
        # int8_t, A不转置B转置: A[m,k], B[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.int32
        )
        x2_gm = x2_gm.transpose()
    elif scenario_num == 2 or scenario_num == 7:
        # int8_t, A转置B不转置: A[k,m], B[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.int32
        )
        x1_gm = x1_gm.transpose()
    elif scenario_num == 3:
        # half, A不转置B转置: A[m,k], B[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.float32
        )
        x2_gm = x2_gm.transpose()
    elif scenario_num == 4:
        # half, A转置B不转置: A[k,m], B[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.float32
        )
        x1_gm = x1_gm.transpose()
    elif scenario_num == 5:
        # float, A不转置B转置: A[m,k], B[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.float32
        )
        x2_gm = x2_gm.transpose()
    elif scenario_num == 6:
        # float, A转置B不转置: A[k,m], B[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(
            np.float32
        )
        x1_gm = x1_gm.transpose()

    if scenario_num <= 2 or scenario_num == 7:
        golden = golden.astype(np.int32)
    else:
        golden = golden.astype(np.float32)

    print("x1_gm == ", x1_gm)
    print("x2_gm == ", x2_gm)

    os.system("mkdir -p input")
    os.system("mkdir -p output")

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenario_num", type=int, default=1, choices=range(1, 8))
    args = parser.parse_args()
    gen_golden_data(args.scenario_num)
