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

np.random.seed(9)


def gen_golden_data(scenarioNum=1):
    """
    根据场景编号生成输入数据和Golden数据
    场景1：ReduceDataBlock<MAX, half>，输入[1, 256]，输出[1, 16]
    场景2：ReduceDataBlock<MIN, half>，输入[4, 128]，输出[4, 8]
    场景3：ReduceDataBlock<SUM, float>，输入[1, 128]，输出[1, 16]
    """
    if scenarioNum == 1:
        input_type = np.float16
        src_length = 256
        dst_length = 16
    elif scenarioNum == 2:
        input_type = np.float16
        src_length = 512
        dst_length = 32
    else:
        input_type = np.float32
        src_length = 128
        dst_length = 16

    one_data_block_items = 32 // np.dtype(input_type).itemsize
    block_num = src_length // one_data_block_items

    input_x = np.random.uniform(-10, 10, [1, src_length]).astype(input_type)
    golden = np.zeros([1, dst_length]).astype(input_type)

    for i in range(block_num):
        block_data = input_x[0, i * one_data_block_items : (i + 1) * one_data_block_items]
        if scenarioNum == 1:
            golden[0, i] = np.max(block_data)
        elif scenarioNum == 2:
            golden[0, i] = np.min(block_data)
        elif scenarioNum == 3:
            golden[0, i] = np.sum(block_data)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 4))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
