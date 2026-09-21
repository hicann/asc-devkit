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
        scenarioNum: 场景编号(1=GetUBSizeInBytes, 2=GetRuntimeUBSize)
    """
    input_type = np.float16
    output_type = np.float16
    min_val, max_val = get_range_by_dtype(input_type)

    if scenarioNum == 1:
        # 场景1：GetUBSizeInBytes，shape=[16384]
        total_length = 16384
    elif scenarioNum == 2:
        # 场景2：GetRuntimeUBSize，shape=[126976]
        total_length = 126976
    else:
        print(f"scenarioNum {scenarioNum} is not supported!")
        return

    input_x = np.random.uniform(min_val, max_val, [total_length]).astype(input_type)
    golden = np.abs(input_x)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 3))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
