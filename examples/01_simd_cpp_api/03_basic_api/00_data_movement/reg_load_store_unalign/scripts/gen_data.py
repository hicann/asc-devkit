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


def gen_golden_data_scenario1():
    """
    Scenario 1: Single-core non-aligned data copy
    Input: 1024 float32 numbers
    Output: 1024 float32 numbers
    - Kernel: CopyVF copies from xLocal[1] to yLocal[1], 128 floats in 2 iterations
    - Iteration 0: Load 64 floats from xLocal[1:65], Store to yLocal[1:65]
    - Iteration 1: Load 64 floats from xLocal[65:129], Store to yLocal[65:129]
    - Result: yLocal[1:129] = xLocal[1:129], yLocal[0] and yLocal[129:] are zeros
    """
    count = 1024
    x = np.random.randn(count).astype(np.float32)
    x.tofile("./input/input_x.bin")

    # Golden: 1024 floats, with copied data at indices 1-128
    golden = np.zeros(count, dtype=np.float32)
    golden[1:129] = x[1:129].copy()  # indices 1-128 inclusive
    golden.tofile("./output/golden.bin")
    print(f"Scenario 1: Generated input_x.bin ({count} floats) and golden.bin ({count} floats)")


def gen_golden_data_scenario2():
    """
    Scenario 2: Multi-core ReduceDataBlock MAX
    Input: 14x255 = 3570 uint16 numbers
    Output: 14x16 = 224 uint16 numbers (max of each DataBlock)
    Each row has 255 elements, split into DataBlocks of 16 elements each.
    The last DataBlock has 15 elements (255 = 15*16 + 15)
    """
    total_rows = 14
    row_length = 255
    data_block_size = 16
    results_per_row = 16

    # Generate random input data
    total_elements = total_rows * row_length
    x = np.random.randint(0, 1000, size=total_elements, dtype=np.uint16)
    x.tofile("./input/input_x.bin")

    # Compute golden output
    golden = np.zeros(total_rows * results_per_row, dtype=np.uint16)

    for row in range(total_rows):
        row_start = row * row_length
        for block_idx in range(results_per_row):
            block_start = row_start + block_idx * data_block_size
            block_end = min(block_start + data_block_size, row_start + row_length)

            # Max of all elements in this DataBlock
            block_max = np.max(x[block_start:block_end]).astype(np.uint16)
            golden[row * results_per_row + block_idx] = block_max

    golden.tofile("./output/golden.bin")
    print(f"Scenario 2: Generated input_x.bin ({total_elements} uint16) and golden.bin ({len(golden)} uint16)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        choices=[1, 2],
        help="Scenario number: 1=Single-core copy, 2=Multi-core ReduceDataBlock",
    )
    args = parser.parse_args()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if args.scenarioNum == 1:
        gen_golden_data_scenario1()
    elif args.scenarioNum == 2:
        gen_golden_data_scenario2()
