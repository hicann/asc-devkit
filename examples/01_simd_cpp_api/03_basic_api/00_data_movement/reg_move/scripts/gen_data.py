#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software; you can redistribute it and/or modify it under the terms and conditions of
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
    """
    Scenario 1: MaskGenWithRegTensor downsample
    Input: 64 int32 values
    Output: 32 uint8 mask bytes (256 bits)
    """
    total_length = 64

    cond = np.zeros([1, total_length], dtype=np.uint32)
    cond[0, 0] = 0x00000001
    cond[0, 1] = 0xFFFFFFFF

    cond_bytes = cond.flatten().view(np.uint8)
    src_bytes = cond_bytes[0:8]
    src_bits = []
    for byte in src_bytes:
        for bit in range(8):
            src_bits.append((byte >> bit) & 1)

    mask_bytes = [0] * 32
    for src_bit_idx in range(64):
        src_bit_val = src_bits[src_bit_idx]
        mask_bit_start = src_bit_idx * 4
        for j in range(4):
            mask_bit_idx = mask_bit_start + j
            mask_byte_idx = mask_bit_idx // 8
            mask_bit_in_byte = mask_bit_idx % 8
            if src_bit_val:
                mask_bytes[mask_byte_idx] |= 1 << mask_bit_in_byte

    golden = np.array(mask_bytes, dtype=np.uint8)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    cond.tofile("./input/input_cond.bin")
    golden.tofile("./output/golden.bin")
    print(f"Scenario 1: Generated input_cond.bin ({total_length} int32) and golden.bin (32 uint8)")


def gen_data_2():
    """
    Scenario 2: Select with mask (same as ld_st_reg_mask scenario 2)
    Input: x/y [1, 256] float, mask [1, 8] float(view as uint8)
    Output: z [1, 256] float
    """
    total_length = 256
    data_type = np.float32

    dtype_size = np.dtype(data_type).itemsize
    mask_length = total_length // 8 // dtype_size

    x = np.random.uniform(0, 2, [1, total_length]).astype(data_type)
    y = np.random.uniform(0, 2, [1, total_length]).astype(data_type)
    mask = np.random.randint(0, 255, [1, mask_length * dtype_size]).astype(np.uint8)
    golden = np.zeros([1, total_length]).astype(data_type)

    for i in range(total_length):
        mask_i = (mask[0, i // 8] >> (i % 8)) & 1
        golden[0, i] = x[0, i] if mask_i == 1 else y[0, i]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    mask.tofile("./input/input_mask.bin")
    golden.tofile("./output/golden.bin")
    print(f"Scenario 2: Generated input_x.bin, input_y.bin, input_mask.bin and golden.bin ({total_length} float)")


def gen_golden_data(scenario_num):
    if scenario_num == 1:
        gen_data_1()
    elif scenario_num == 2:
        gen_data_2()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        choices=[1, 2],
        help="Scenario number: 1=MaskGenWithRegTensor, 2=Select with mask",
    )
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
