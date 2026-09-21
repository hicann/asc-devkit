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


def get_range_by_dtype(input_type_type):
    try:
        if input_type_type == np.float16 or input_type_type == np.float32 or input_type_type == np.float64:
            finfo = np.finfo(input_type_type)
            return finfo.min, finfo.max
        else:
            iinfo = np.iinfo(input_type_type)
            return iinfo.min, iinfo.max
    except ValueError:
        print(f"Unsupported data type:{input_type_type}")
        return 0, 0


def gen_golden_data(scenario_num):
    """
    生成测试输入数据和真值数据
    Args:
        scenario_num: 场景编号
    """
    input_type = np.float32
    input_sel_type = np.uint8
    output_type = input_type
    input_size = 256
    input_sel_size = 8 if scenario_num == 1 else 32
    input_y_size = 256 if (scenario_num == 1 or scenario_num == 3) else 8
    output_size = 256
    mask_num = 64  # selMask的有效数值

    input_shape = [input_size]
    input_sel_shape = [input_sel_size]
    output_shape = [output_size]
    input_y_shape = [input_y_size]

    min_val, max_val = get_range_by_dtype(input_sel_type)
    input_sel = np.random.uniform(min_val, max_val, input_sel_shape).astype(input_sel_type)

    min_val, max_val = get_range_by_dtype(input_type)
    input_x = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
    input_y = np.random.uniform(min_val, max_val, input_y_shape).astype(input_type)

    golden = np.ones(output_shape).astype(output_type)

    for i in range(input_size):
        if scenario_num == 1:
            sel_idx = i % mask_num
            byte_idx = sel_idx // 8
            bit_idx = sel_idx % 8
        else:
            byte_idx = i // 8
            bit_idx = i % 8
        if (input_sel[byte_idx] >> bit_idx) & 1:
            golden[i] = input_x[i]
        else:
            golden[i] = input_y[i] if (scenario_num == 1 or scenario_num == 3) else input_y[0]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")
    input_sel.tofile("./input/input_sel.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenario_num", type=int, default=1, choices=range(1, 5))
    args = parser.parse_args()
    gen_golden_data(args.scenario_num)
