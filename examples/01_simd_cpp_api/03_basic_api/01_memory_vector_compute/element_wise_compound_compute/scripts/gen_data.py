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

np.random.seed(9)


def gen_golden_data(scenario=1):
    """
    根据场景编号生成输入数据和Golden数据

    Args:
        scenario: 场景编号
            1 - AddRelu: half -> half 加法与ReLU激活
            2 - Axpy: half -> half 标量乘法与向量加法
    """
    data_size = 512

    if scenario == 1:
        # 场景1：AddRelu - half输入输出
        # 计算公式：dst = max(src0 + src1, 0)
        input0 = np.random.uniform(-1.0, 1.0, [data_size]).astype(np.float16)
        input1 = np.random.uniform(-1.0, 1.0, [data_size]).astype(np.float16)
        golden = np.maximum(input0 + input1, 0).astype(np.float16)

    elif scenario == 2:
        # 场景2：Axpy - half输入输出
        # 计算公式：dst = dst + src * scalar，scalar=2.0
        # input0为src，input1为初始dst
        input0 = np.random.uniform(-1.0, 1.0, [data_size]).astype(np.float16)
        input1 = np.random.uniform(-1.0, 1.0, [data_size]).astype(np.float16)
        scalar = 2.0
        golden = (input1 + input0 * scalar).astype(np.float16)

    else:
        raise ValueError(f"Invalid scenario: {scenario}, must be 1 or 2")

    os.system("mkdir -p input")
    os.system("mkdir -p output")

    input0.tofile("./input/input0.bin")
    input1.tofile("./input/input1.bin")
    golden.tofile("./output/golden.bin")

    print(f"Generated data for scenario {scenario}: input0 shape={input0.shape}, dtype={input0.dtype}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2], help="Scenario number: 1=AddRelu, 2=Axpy")
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
