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

M = 30
K = 40
N = 70


def gen_golden_data(scenarioNum=1):
    if scenarioNum == 1:
        x1_gm = np.random.randint(1, 10, [M, K]).astype(np.int8)
        x2_gm = np.random.randint(1, 10, [K, N]).astype(np.int8)
        golden = np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32))
    elif scenarioNum == 2:
        x1_gm = np.random.randint(1, 10, [M, K]).astype(np.int8)
        x2_gm = np.random.randint(1, 10, [K, N]).astype(np.int8)
        golden = np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32))
        x1_gm = x1_gm.transpose()
    elif scenarioNum == 3:
        x1_gm = np.random.uniform(1, 10, [M, K]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [K, N]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
    elif scenarioNum == 4:
        x1_gm = np.random.uniform(1, 10, [M, K]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [K, N]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
        x1_gm = x1_gm.transpose()
    elif scenarioNum == 5:
        x1_gm = np.random.uniform(1, 10, [M, K]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [K, N]).astype(np.float32)
        golden = (np.matmul(x1_gm, x2_gm)).astype(np.float32)
    elif scenarioNum == 6:
        x1_gm = np.random.uniform(1, 10, [M, K]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [K, N]).astype(np.float32)
        golden = (np.matmul(x1_gm, x2_gm)).astype(np.float32)
        x1_gm = x1_gm.transpose()

    os.system("mkdir -p input")
    os.system("mkdir -p output")
    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 7))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
