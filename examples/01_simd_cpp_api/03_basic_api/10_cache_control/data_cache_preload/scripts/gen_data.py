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
import numpy as np


def gen_golden_data():
    """
    生成DataCachePreload样例的输入数据和预期输出。

    数据布局：
      - data.bin:    64KB的uint8_t随机数据缓冲区（1024个Cache Line）
      - indices.bin: 256个uint32_t随机索引，每个索引指向data中的一个Cache Line对齐位置
      - golden.bin:  1个uint32_t值：预期累加和
    """
    count = 256
    data_len = 65536  # 64KB = 1024 * 64B

    np.random.seed(42)

    # 生成64KB的随机uint8_t数据
    data = np.random.randint(0, 256, size=data_len, dtype=np.uint8)

    # 生成随机索引：每个索引指向Cache Line对齐的起始位置 (0, 64, 128, ...)
    # 使用np.arange(0, data_len, 64)确保覆盖所有可能对齐位置后shuffle
    all_line_starts = np.arange(0, data_len, 64, dtype=np.uint32)
    np.random.shuffle(all_line_starts)
    indices = all_line_starts[:count]

    # 计算预期累加和：sum(data[indices[i]])
    golden_sum = np.uint32(0)
    for idx in indices:
        golden_sum += data[idx]

    # 输出golden：累加和（两种场景算法一致，结果相同）
    golden = np.array([golden_sum], dtype=np.uint32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    data.tofile("./input/data.bin")
    indices.tofile("./input/indices.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
