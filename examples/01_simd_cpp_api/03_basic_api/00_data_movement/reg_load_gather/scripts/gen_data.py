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
import argparse
import numpy as np


def gen_golden_data_gather_scenario1():
    """场景1：Gather接口 - 从UB地址按索引收集元素"""
    total_length = 128
    src_length = 1024
    data_type = np.float16
    x = np.random.uniform(0, 1, [1, src_length]).astype(data_type)
    index = np.random.randint(0, src_length, [1, total_length]).astype(np.uint16)
    golden = x.flatten()[index.flatten()]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    index.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_gatherb_scenario2():
    """场景2：GatherB接口 - 按32字节DataBlock随机收集"""
    total_length = 128
    src_length = 1024
    data_type = np.float16
    elements_per_block = 32 // np.dtype(data_type).itemsize
    num_output_blocks = total_length // elements_per_block
    num_src_blocks = src_length // elements_per_block
    x = np.random.uniform(0, 1, [1, src_length]).astype(data_type)
    block_indices = np.random.randint(0, num_src_blocks, num_output_blocks).astype(np.uint32)
    index = block_indices * 32
    x_flat = x.flatten()
    golden = np.zeros(total_length, dtype=data_type)
    for i, block_idx in enumerate(block_indices):
        start = block_idx * elements_per_block
        golden[i * elements_per_block : (i + 1) * elements_per_block] = x_flat[start : start + elements_per_block]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    index.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1)
    args = parser.parse_args()
    if args.scenarioNum == 1:
        gen_golden_data_gather_scenario1()
    elif args.scenarioNum == 2:
        gen_golden_data_gatherb_scenario2()
