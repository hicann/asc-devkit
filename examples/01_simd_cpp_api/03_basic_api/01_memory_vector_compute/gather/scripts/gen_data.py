#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025-2026 Huawei Technologies Co., Ltd.
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


def get_range_by_dtype(input_type):
    try:
        if input_type == np.float16 or input_type == np.float32 or input_type == np.float64:
            return np.finfo(input_type).min, np.finfo(input_type).max
        else:
            return np.iinfo(input_type).min, np.iinfo(input_type).max
    except ValueError:
        print(f"Unsupported data type:{input_type}")
        return None, None


def ensure_dirs():
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)


def save_data(input_x, input_y=None, golden=None):
    ensure_dirs()
    input_x.tofile("./input/input_x.bin")
    if input_y is not None:
        input_y.tofile("./input/input_y.bin")
    if golden is not None:
        golden.tofile("./output/golden.bin")


def gen_golden_data(scenario_num):
    """
    根据场景编号生成输入数据和Golden数据
    """

    if scenario_num == 1:
        input_x = np.arange(1, 257).astype(np.uint32)
        gdst = np.arange(2, 257, 2).astype(np.uint32)
        gzero = np.zeros(128).astype(np.uint32)
        golden = np.concatenate([gdst, gzero]).astype(np.uint32)
        save_data(input_x, golden=golden)
    elif scenario_num == 2:
        gen_golden_data_custom()
    elif scenario_num == 3:
        count = 128
        dataSize = 2
        input_x = np.arange(0, count, 1, np.float16)
        input_y = np.arange(0, count * dataSize, dataSize, np.uint32)
        input_y = np.flipud(input_y)
        golden = np.flipud(input_x)
        save_data(input_x, input_y, golden)
    elif scenario_num == 4:
        input_x = np.arange(128).astype(np.uint16)
        input_y = np.arange(8)[::-1] * 32
        input_y = input_y.astype(np.uint32)
        data_size = 2
        golden = np.zeros(128).astype(np.uint16)
        count = 0
        for i in range(8):
            for j in range(int(32 / data_size)):
                golden[count] = input_x[int(input_y[i] / data_size + j)]
                count += 1
        save_data(input_x, input_y, golden)


def gen_golden_data_custom():
    one_repeat_size = 256
    data_block_size = 32
    input_type = np.uint32
    output_type = input_type
    type_size = np.dtype(input_type).itemsize
    block_length = 256
    mask = 70
    src0_block_stride = 1
    repeat_times = 2
    src0_repeat_stride = 4
    src1_repeat_stride = 0
    one_data_block_items = data_block_size // type_size

    min_val, max_val = get_range_by_dtype(input_type)
    if min_val is None or max_val is None:
        raise ValueError(f"Failed to get range for data type: {input_type}")
    input_x_shape = [block_length]
    input_y_shape = [block_length // 8]
    input_x = np.random.uniform(min_val, max_val, input_x_shape).astype(input_type)
    input_y = 0x7E7C00A5 * np.ones(input_y_shape).astype(input_type)
    input_mask = np.unpackbits(input_y.view(np.uint8), bitorder="little").astype(bool)
    golden = np.zeros(input_x_shape).astype(input_type)
    for i in range(repeat_times):
        base = i * (src0_repeat_stride // src0_block_stride) * one_data_block_items
        base_m = i * src1_repeat_stride * one_data_block_items
        src0_iter = input_x[base : base + mask]
        mask_slice = input_mask[base_m : base_m + mask]
        selected = src0_iter[mask_slice]
        base_g = i * selected.size
        golden[base_g : base_g + selected.size] = selected
    save_data(input_x, input_y, golden)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenario_num", type=int, default=1, choices=range(1, 5))
    args = parser.parse_args()
    gen_golden_data(args.scenario_num)
