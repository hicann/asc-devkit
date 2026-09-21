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
import sys


def get_range_by_dtype(input_type):
    try:
        if input_type == np.float16 or input_type == np.float32 or input_type == np.float64:
            return np.finfo(input_type).min, np.finfo(input_type).max
        else:
            return np.iinfo(input_type).min, np.iinfo(input_type).max
    except ValueError:
        print(f"Unsupported data type:{input_type}")


def gen_golden_data(scenario_num):
    input_type = np.float16
    output_type = input_type
    """
    根据场景编号生成输入数据和Golden数据：
    场景1：普通转置，对[16, 16]的二维矩阵进行转置
    场景2：增强转置，[N,C,H,W]与[N,H,W,C]两个数据格式互相转换
    场景3：5HD格式转换，NCHW格式转换成NC1HWC0格式
    """

    if scenario_num == 1:
        # 场景1：普通转置
        mat_len = 16
        min_val, max_val = get_range_by_dtype(input_type)
        input_shape = [mat_len, mat_len]
        input = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = input.T
    elif scenario_num == 2:
        # 场景2：增强转置
        n = 3
        c = 3
        h = 2
        w = 8
        min_val, max_val = get_range_by_dtype(input_type)
        input_shape = [n, c, h, w]
        input = np.random.uniform(min_val, max_val, input_shape).astype(input_type)
        golden = np.transpose(input, (0, 2, 3, 1))
    elif scenario_num == 3:
        # 场景3：5HD格式转换
        input_x = np.random.randn(2, 32, 16, 16).astype(np.float16)
        # 将NCHW (2, 32, 16, 16) 转换为NC1HWC0 (2, 2, 16, 16, 16)
        # 1. 将C维度拆分为C1和C0
        # 2. 调整维度顺序
        input = input_x
        golden = input_x.reshape(2, 2, 16, 16, 16).transpose(0, 1, 3, 4, 2)
    else:
        print(
            "Invalid scenario number: {}. Supported scenarios: 1 (common), 2 (enhanced), 3 (5hd)".format(scenario_num)
        )
        sys.exit(1)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenario_num", type=int, default=1, choices=range(1, 4))
    args = parser.parse_args()
    gen_golden_data(args.scenario_num)
