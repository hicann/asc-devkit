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
import numpy as np
import argparse
from enum import Enum


class CMPMODE(Enum):
    EQ = "EQ"  # 等于（equal to）
    NE = "NE"  # 不等于（not equal to）
    GE = "GE"  # 大于或等于（greater than or equal to）
    LE = "LE"  # 小于或等于（less than or equal to）
    GT = "GT"  # 大于（greater than）
    LT = "LT"  # 小于（less than）


def get_range_by_dtype(input_type):
    try:
        if input_type == np.float16 or input_type == np.float32 or input_type == np.float64:
            return np.finfo(input_type).min, np.finfo(input_type).max
        else:
            return np.iinfo(input_type).min, np.iinfo(input_type).max
    except ValueError:
        print(f"Unsupported data type:{input_type}")


def compare(input_x, input_y, input_size, output_size):
    compare_mode = CMPMODE.LT
    use_core_num = 1
    output_type = np.uint8
    output_shape = [use_core_num, output_size]
    golden = np.ones(output_shape).astype(output_type)

    if compare_mode == CMPMODE.EQ:
        compare_result = (input_x == input_y).astype(output_type)
    elif compare_mode == CMPMODE.NE:
        compare_result = (input_x != input_y).astype(output_type)
    elif compare_mode == CMPMODE.LE:
        compare_result = (input_x <= input_y).astype(output_type)
    elif compare_mode == CMPMODE.LT:
        compare_result = (input_x < input_y).astype(output_type)
    elif compare_mode == CMPMODE.GT:
        compare_result = (input_x > input_y).astype(output_type)
    elif compare_mode == CMPMODE.GE:
        compare_result = (input_x >= input_y).astype(output_type)
    for i in range(use_core_num):
        for j in range(input_size // 8):
            bits = compare_result[i, j * 8 : (j + 1) * 8]
            byte_val = 0
            for k in range(8):
                if bits[k] == 1:
                    byte_val |= 1 << k
            golden[i, j] = byte_val

    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


def gen_golden_data(scenario_num, input_size, input_y_size, output_size):
    """
    生成测试输入数据和真值数据
    Args:
        scenario_num: 场景编号(1=Compare, 2=Compare结果存入寄存器, 3=Compares, 4=Compares灵活标量位置)
    """
    input_type = np.float32
    use_core_num = 1
    input_shape = [use_core_num, input_size]
    input_y_shape = [use_core_num, input_y_size]
    min_val, max_val = get_range_by_dtype(input_type)
    input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
    input_y = np.random.uniform(min_val, max_val, input_y_shape).astype(input_type)

    os.makedirs("input", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")

    if scenario_num == 1 or scenario_num == 2:
        compare(input_x, input_y, input_size, output_size)
    elif scenario_num == 3 or scenario_num == 4:
        compare(input_x, input_y[0][0], input_size, output_size)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenario_num", type=int, default=1, choices=range(1, 5))
    args = parser.parse_args()
    scenario_num = args.scenario_num
    if scenario_num == 1:
        gen_golden_data(scenario_num, 256, 256, 32)
    elif scenario_num == 2:
        gen_golden_data(scenario_num, 64, 64, 32)
    elif scenario_num == 3 or scenario_num == 4:
        gen_golden_data(scenario_num, 256, 16, 32)
