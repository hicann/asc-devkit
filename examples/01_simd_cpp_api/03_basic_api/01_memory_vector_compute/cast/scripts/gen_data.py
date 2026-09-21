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
import argparse


def get_range_by_dtype(input_type):
    try:
        if input_type == np.float16 or input_type == np.float32 or input_type == np.float64:
            return np.finfo(input_type).min, np.finfo(input_type).max
        else:
            return np.iinfo(input_type).min, np.iinfo(input_type).max
    except ValueError:
        print(f"Unsupported data type:{input_type}")


def gen_golden_data_simple():
    input_type = np.float16
    output_type = np.int32
    min_val, max_val = get_range_by_dtype(input_type)
    block_length = 512
    input_shape = [block_length]
    output_shape = [block_length]
    input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
    min_val, max_val = get_range_by_dtype(output_type)
    golden = np.clip(input_x, min_val, max_val)
    # AscendC::RoundMode::CAST_CEIL
    golden = np.ceil(golden)
    golden = golden.astype(output_type)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_simple_int4b():
    input_type = np.float16
    output_type = np.int8
    min_val, max_val = get_range_by_dtype(input_type)
    block_length = 512
    input_shape = [block_length]
    output_shape = [block_length // 2]
    input_x = np.random.uniform(-9, 8, input_shape).astype(input_type)
    output_ceil = np.ceil(input_x).astype(output_type)
    output = 0xF & np.clip(output_ceil, -8, 7).astype(output_type)
    golden = (output[::2] | (output[1::2] << 4)).astype(output_type)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, choices=(0, 1), help="选择场景")
    args = parser.parse_args()
    gen_golden_data_simple() if args.scenarioNum else gen_golden_data_simple_int4b()
