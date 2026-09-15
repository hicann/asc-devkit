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


def gen_golden_data(scenarioNum=1):
    M = 128
    K = 128
    N = 256
    kRound = 2  # K轴切分2次

    input_type = np.dtype("float16")
    output_type = np.dtype("float32")
    x1_gm = np.random.uniform(-1, 3, [M, K]).astype(input_type)
    x2_gm = np.random.uniform(-1, 3, [K, N]).astype(input_type)
    golden = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(
        np.float32
    )
    # print(golden)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if scenarioNum == 1:
        block_cols = 16
        golden = (
            golden.reshape((int(M / 16), 16, int(N / block_cols), block_cols))
            .transpose(2, 0, 1, 3)
            .astype(output_type)
        )

    if kRound > 1:
        # 将K轴外移
        x1_gm = x1_gm.reshape(M, kRound, K // kRound).transpose(1, 0, 2)
    x1_gm.astype(input_type).tofile("./input/x1_gm.bin")
    x2_gm.astype(input_type).tofile("./input/x2_gm.bin")
    golden.astype(output_type).tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 5))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
