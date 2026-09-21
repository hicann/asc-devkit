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


def leaky_relu(x, alpha=0.001):
    return np.maximum(alpha * x, x)


def gen_golden_data_scenario0():
    """
    SCENARIO=0: 纯Vector计算场景，模式0（16个AIV全核同步）
    计算公式：z = x * (0 + NUM_BLOCKS * 2 - 1) * NUM_BLOCKS
    NUM_BLOCKS = 8，x全为1.0
    """
    total_length = 32
    num_blocks = 8
    value_x = 1.0
    data_type = np.float32

    input_x = np.full(total_length, value_x, dtype=data_type)
    golden = np.full(total_length, value_x * (0 + num_blocks * 2 - 1) * num_blocks, dtype=data_type)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_scenario1():
    """
    SCENARIO=1: 纯Vector计算场景，模式1（2个AIV同步）
    计算公式：z = x * (2 + 3) = x * 5
    x全为1.0，AIV 0 blockIdx=2，AIV 1 blockIdx=3
    """
    total_length = 32
    value_x = 1.0
    data_type = np.float32

    input_x = np.full(total_length, value_x, dtype=data_type)
    golden = np.full(total_length, value_x * (2 + 3), dtype=data_type)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_scenario2():
    """
    SCENARIO=2: Cube与Vector融合计算场景
    计算公式：C = LeakyRelu(Cast(A) × Cast(B))
    A: [32, 32] uint8
    B: [32, 64] uint8
    C: [32, 64] float32
    """
    M = 32
    K = 32
    N = 64
    num_blocks = 8
    input_type = np.uint8
    output_type = np.float32

    x1_gm = np.random.uniform(0, 1, [M, K]).astype(input_type)
    x2_gm = np.random.uniform(0, 1, [K, N]).astype(input_type)

    x11_gm = np.zeros_like(x1_gm).astype(np.half)
    x22_gm = np.zeros_like(x2_gm).astype(np.half)

    golden = leaky_relu(np.matmul(x1_gm.astype(output_type), x2_gm.astype(output_type))).astype(output_type)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm = x1_gm.reshape(M, num_blocks, K // num_blocks).transpose(1, 0, 2)

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    x11_gm.tofile("./input/x11_gm.bin")
    x22_gm.tofile("./input/x22_gm.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data(scenario_num):
    if scenario_num == 0:
        gen_golden_data_scenario0()
    elif scenario_num == 1:
        gen_golden_data_scenario1()
    elif scenario_num == 2:
        gen_golden_data_scenario2()
    else:
        raise ValueError(f"Invalid scenario_num: {scenario_num}, must be 0, 1, or 2")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=0, choices=[0, 1, 2], help="Scenario number: 0, 1, or 2")
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
