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
import argparse
import numpy as np

np.random.seed(9)


def gen_golden_data(scenarioNum=1):
    """
    根据场景编号生成输入数据和Golden数据
    场景1：128个元素，4条队列合并为1条，输出[1, 256]
    场景2：96个元素，3条队列合并为1条，输出[1, 192]
    场景3：1024个元素，32条队列多轮合并为1条，输出[1, 2048]
    """
    if scenarioNum == 1:
        total_elements = 128
    elif scenarioNum == 2:
        total_elements = 96
    elif scenarioNum == 3:
        total_elements = 1024
    else:
        raise ValueError(f"Unsupported scenarioNum: {scenarioNum}")

    input_x = np.random.uniform(1, 100, [total_elements]).astype(np.float32)
    input_y = np.zeros(total_elements, dtype=np.uint32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")

    if scenarioNum == 1:
        # 场景1：128个元素全局降序排序，(score, index)交替存储
        sorted_indices = np.argsort(input_x)[::-1]
        sorted_arr = input_x[sorted_indices]
        golden = np.zeros(total_elements * 2, dtype=np.float32)
        for i in range(total_elements):
            golden[2 * i] = sorted_arr[i]
    elif scenarioNum == 3:
        # 场景3：1024个元素全局降序排序，(score, index)交替存储
        sorted_indices = np.argsort(input_x)[::-1]
        sorted_arr = input_x[sorted_indices]
        golden = np.zeros(total_elements * 2, dtype=np.float32)
        for i in range(total_elements):
            golden[2 * i] = sorted_arr[i]
    else:
        # 场景2：全局降序排序，(score, index)交替存储
        sorted_indices = np.argsort(input_x)[::-1]
        sorted_arr = input_x[sorted_indices]
        golden = np.zeros(total_elements * 2, dtype=np.float32)
        for i in range(total_elements):
            golden[2 * i] = sorted_arr[i]

    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 4), help="scenario number (1-3)")
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
