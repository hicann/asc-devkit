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


def generate_element_gather_data():
    src_length = 1024
    output_length = 256
    src = np.random.uniform(0, 1, src_length).astype(np.float16)
    index = np.random.randint(0, src_length, output_length).astype(np.uint16)
    golden = src[index]
    src.tofile("./input/input_x.bin")
    index.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


def generate_datablock_gather_data():
    src_length = 1024
    output_length = 256
    elements_per_block = 32 // np.dtype(np.float16).itemsize
    src = np.random.uniform(0, 1, src_length).astype(np.float16)
    block_index = np.random.randint(
        0, src_length // elements_per_block, output_length // elements_per_block
    ).astype(np.uint32)
    index = block_index * 32
    golden = np.concatenate(
        [
            src[block * elements_per_block : (block + 1) * elements_per_block]
            for block in block_index
        ]
    )
    src.tofile("./input/input_x.bin")
    index.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, choices=(1, 2), default=1)
    args = parser.parse_args()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    if args.scenarioNum == 1:
        generate_element_gather_data()
    else:
        generate_datablock_gather_data()


if __name__ == "__main__":
    main()
