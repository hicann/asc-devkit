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
    # 生成一个 [1, 256] 向量
    x = np.random.uniform(1, 2, [1, total_length]).astype(data_type)
    # PairReduceElem: 相邻奇偶元素对求和
    # golden[0] = x[0] + x[1], golden[1] = x[2] + x[3], ...
    # 结果数量为输入的一半
    golden = np.zeros(total_length // 2, dtype=data_type)
    for i in range(total_length // 2):
        golden[i] = x[0, 2 * i] + x[0, 2 * i + 1]
    # 补齐到8个float以满足32B对齐
    num_results = total_length // 2
    aligned_size = ((num_results + 7) // 8) * 8
    golden_aligned = np.zeros(aligned_size, dtype=data_type)
    golden_aligned[:num_results] = golden
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    golden_aligned.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
