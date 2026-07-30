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
import numpy as np


def gen_golden_data_simple():
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
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    mask.tofile("./input/input_mask.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
