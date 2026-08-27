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

def gen_golden_data_simple(scenario_num):
    total_length = 256
    data_type = np.float32
    scalar = 0.0
    # 生成两个 [1, 256] 矩阵
    x = np.random.uniform(-2, 2, [1, total_length]).astype(data_type)
    y = np.random.uniform(-2, 2, [1, total_length]).astype(data_type)
    # 计算逐元素取较大值
    if scenario_num == 1:
        golden = np.maximum(x, y)
    else:
        golden = y.copy()
        golden[x > scalar] = x[x > scalar]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile('./input/input_x.bin')
    y.tofile('./input/input_y.bin')
    golden.tofile('./output/golden.bin')

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-scenarioNum', type=int, default=1, choices=[1, 2],
                        help='Scenario number: 1=Max(x,y), 2=Max(x,scalar)')
    args = parser.parse_args()
    gen_golden_data_simple(args.scenarioNum)
