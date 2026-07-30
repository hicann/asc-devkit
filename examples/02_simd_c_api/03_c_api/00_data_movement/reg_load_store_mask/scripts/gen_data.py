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


def generate_scenario1():
    src = np.zeros(1024, dtype=np.uint8)
    src[0] = 1
    src[31] = 1
    golden = np.zeros(1024, dtype=np.uint8)
    golden[0] = 2
    golden[248] = 2
    golden[256:288] = 255
    return src, golden


def generate_scenario2():
    src0 = np.linspace(0.0, 2.0, 256, dtype=np.float32)
    src1 = np.linspace(2.0, 4.0, 256, dtype=np.float32)
    mask = np.arange(128, dtype=np.uint8)
    golden = np.empty(256, dtype=np.float32)
    for index in range(256):
        mask_bit = (mask[index // 2] >> ((index % 2) * 4)) & 1
        golden[index] = src0[index] if mask_bit else src1[index]
    return src0, src1, mask, golden


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, choices=(1, 2), required=True)
    args = parser.parse_args()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if args.scenarioNum == 1:
        src, golden = generate_scenario1()
        src.tofile("./input/input.bin")
    else:
        src0, src1, mask, golden = generate_scenario2()
        src0.tofile("./input/input_x.bin")
        src1.tofile("./input/input_y.bin")
        mask.tofile("./input/input_mask.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    main()
