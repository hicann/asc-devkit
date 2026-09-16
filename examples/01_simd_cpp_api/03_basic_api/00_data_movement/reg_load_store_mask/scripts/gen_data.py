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
import argparse
import numpy as np


def gen_data_1():
    src = np.zeros(1024, dtype=np.uint8)
    src[0] = 1
    src[31] = 1
    dst = np.zeros(1024, dtype=np.uint8)
    dst[0] = 2
    dst[248] = 2
    dst[256 : 256 + 32] = 255
    return src, dst


def gen_data_2():
    total_length = 256
    data_type = np.float32

    # float32 是 4 字节
    dtype_size = np.dtype(data_type).itemsize
    # 对于b32数据类型，每次repeat处理64个元素，共4次repeat
    repeat = total_length // (256 // dtype_size)
    # 每次repeat对应一个32B的MaskReg
    mask_length = repeat * 32
    # 生成两个 [1, 256] 矩阵
    x = np.random.uniform(0, 2, [1, total_length]).astype(data_type)
    y = np.random.uniform(0, 2, [1, total_length]).astype(data_type)
    mask = np.random.randint(0, 255, [1, mask_length]).astype(np.uint8)
    golden = np.zeros([1, total_length]).astype(data_type)

    # 对于b32数据类型，mask有效位数间隔4bit，golden中每个uint8_t元素对应2个mask值。
    mask_num = 8 // dtype_size
    for i in range(total_length):
        mask_i = mask[0, i // mask_num] >> (i % mask_num * dtype_size) & 1
        golden[0, i] = x[0, i] if mask_i == 1 else y[0, i]
    return x, y, mask, golden


def gen_golden_data(scenario_num):
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    if scenario_num == 1:
        src, golden = gen_data_1()
        src.tofile("./input/input.bin")
        golden.tofile("./output/golden.bin")
    elif scenario_num == 2:
        x, y, mask, golden = gen_data_2()
        x.tofile("./input/input_x.bin")
        y.tofile("./input/input_y.bin")
        mask.tofile("./input/input_mask.bin")
        golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        choices=[1, 2],
        help="Scenario number: 1-2",
    )
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
