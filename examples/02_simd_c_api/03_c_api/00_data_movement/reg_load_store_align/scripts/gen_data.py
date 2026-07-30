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


INPUT_LENGTH = 1024
VECTOR_LENGTH = 128


def generate_golden(scenario_num, src0, src1):
    if scenario_num == 1:
        output = np.zeros(INPUT_LENGTH, dtype=np.float16)
        output[: INPUT_LENGTH - 3] = src0[: INPUT_LENGTH - 3] + src1[: INPUT_LENGTH - 3]
        return output
    if scenario_num in (2, 3):
        return src0 + src1
    if scenario_num == 4:
        return (src0 + src1).reshape(INPUT_LENGTH // 32, 32)[:, :16].reshape(-1)
    if scenario_num == 5:
        return np.repeat(
            src0[::VECTOR_LENGTH] + src1[::VECTOR_LENGTH], VECTOR_LENGTH
        ).astype(np.float16)
    return np.repeat(src0 + src1, 2).astype(np.float16)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, choices=range(1, 7), required=True)
    args = parser.parse_args()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    src0 = np.arange(INPUT_LENGTH, dtype=np.float16)
    src1 = np.arange(INPUT_LENGTH, dtype=np.float16)
    golden = generate_golden(args.scenarioNum, src0, src1)
    src0.tofile("./input/input_x.bin")
    src1.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    main()
