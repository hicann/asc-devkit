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
from ml_dtypes import bfloat16


def gen_golden_data(scenario_num=1):
    m = 30
    k = 70
    n = 40
    if scenario_num == 1:
        x1_gm = np.random.randint(1, 10, [m, k]).astype(np.int8)
        x2_gm = np.random.randint(1, 10, [k, n]).astype(np.int8)
        bias_gm = np.random.randint(1, 10, [n]).astype(np.int32)
        golden = (
            np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32)).astype(np.int32)
            + bias_gm
        )
    elif scenario_num == 2:
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(bfloat16)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(bfloat16)
        golden = (
            np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(
                np.float32
            )
            * 2
        )
        x2_gm = x2_gm.transpose()
    elif scenario_num == 3:
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
        bias_gm = np.random.uniform(1, 10, [n]).astype(np.float32)
        golden = (
            np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(
                np.float32
            )
            + bias_gm
        )
        x1_gm = x1_gm.transpose()
        x2_gm = x2_gm.transpose()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    if scenario_num in [1, 3]:
        bias_gm.tofile("./input/bias_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2, 3])
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
