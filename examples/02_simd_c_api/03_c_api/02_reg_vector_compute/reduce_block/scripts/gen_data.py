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
    # ReduceDataBlock: 每8个float(DataBlock=32B)求和产生一个结果
    # 结果依次存储在dstReg的最低位，即位置0,1,2,...,31
    num_blocks = total_length // 8
    golden = np.zeros(num_blocks, dtype=data_type)
    for i in range(num_blocks):
        golden[i] = np.sum(x[0, i * 8 : (i + 1) * 8])
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
