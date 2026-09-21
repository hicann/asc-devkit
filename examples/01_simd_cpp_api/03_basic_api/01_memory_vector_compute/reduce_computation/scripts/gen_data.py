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


def gen_golden_data(scenarioNum):
    """
    生成测试输入数据和真值数据
    Args:
        scenarioNum: 场景编号(1=ReduceMax前n个数据, 2=ReduceMax高维切分, 3=ReduceMin前n个数据, 4=ReduceMin高维切分, 5=ReduceSum前n个数据, 6=ReduceSum高维切分)
    """
    input_type = np.float16
    output_type = input_type
    # 与half对应
    max_index_type = np.uint16
    min_index_type = np.uint16

    if scenarioNum == 1:
        block_length = 288
        output_shape = [16]
        input_shape = [block_length]
        min_val, max_val = get_range_by_dtype(input_type)
        input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = np.zeros(output_shape, dtype=output_type)
        golden[0] = np.max(input_x)
        max_index = np.argmax(input_x)
        max_index = np.uint16(max_index)
        golden[1] = np.float16(max_index.view(np.float16))
    elif scenarioNum == 2:
        block_length = 512
        output_shape = [16]
        input_shape = [block_length]
        min_val, max_val = get_range_by_dtype(input_type)
        input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = np.zeros(output_shape, dtype=output_type)
        golden[0] = np.max(input_x)
        max_index = np.argmax(input_x)
        max_index = np.uint16(max_index)
        golden[1] = np.float16(max_index.view(np.float16))
    elif scenarioNum == 3:
        block_length = 288
        output_shape = [16]
        input_shape = [block_length]
        min_val, max_val = get_range_by_dtype(input_type)
        input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = np.zeros(output_shape, dtype=output_type)
        golden[0] = np.min(input_x)
        min_index = np.argmin(input_x)
        min_index = np.uint16(min_index)
        golden[1] = np.float16(min_index.view(np.float16))
    elif scenarioNum == 4:
        block_length = 512
        output_shape = [16]
        input_shape = [block_length]
        min_val, max_val = get_range_by_dtype(input_type)
        input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = np.zeros(output_shape, dtype=output_type)
        golden[0] = np.min(input_x)
        min_index = np.argmin(input_x)
        min_index = np.uint16(min_index)
        golden[1] = np.float16(min_index.view(np.float16))
    elif scenarioNum == 5:
        block_length = 288
        output_shape = [16]
        input_shape = [block_length]
        input_x = np.ones(input_shape).astype(input_type)
        golden = np.zeros(output_shape).astype(output_type)
        golden[0] = np.sum(input_x)
        golden[1] = golden[0]
    elif scenarioNum == 6:
        block_length = 8320
        output_shape = [16]
        input_shape = [block_length]
        input_x = np.ones(input_shape).astype(input_type)
        golden = np.zeros(output_shape).astype(output_type)
        golden[0] = np.sum(input_x)
    else:
        print(f"Unsupported scenarioNum: {scenarioNum}")
        return

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 7))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
