#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import os
import numpy as np


def gen_golden_data_gather():
    """Gather（源操作数为寄存器）- 根据索引从half向量中收集元素"""
    total_length = 128
    data_type = np.float16
    index_type = np.uint16
    # 生成1个 [1, 128] 的 half 向量作为源数据
    x = np.random.uniform(0, 1, [1, total_length]).astype(data_type)
    # 生成1个 [1, 128] 的 uint16 索引向量，范围 [0, 127]
    y = np.random.uniform(0, total_length, [1, total_length]).astype(index_type)
    x_flat = x.flatten()
    y_flat = y.flatten()
    # Gather：dst[i] = src[index[i]]
    golden = x_flat[y_flat]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_gather()
