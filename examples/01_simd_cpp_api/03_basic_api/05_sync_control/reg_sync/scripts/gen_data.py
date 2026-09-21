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


def gen_golden_data_simple(scenarioNum=1):
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    count = 1024
    x = np.random.randn(count).astype(np.float32)
    x.tofile("./input/input_x.bin")
    if scenarioNum == 1:
        z = np.exp(x)
        z.tofile("./output/golden.bin")
    elif scenarioNum == 2:
        x = np.random.randn(count).astype(np.float32)
        x.tofile("./input/input_x.bin")
        exp = np.exp(x - np.max(x))
        golden = exp / np.sum(exp)
        golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum", type=int, default=1, choices=[1, 2], help="Scenario number: 1=RegSync, 2=UbSync"
    )
    args = parser.parse_args()
    gen_golden_data_simple(args.scenarioNum)
