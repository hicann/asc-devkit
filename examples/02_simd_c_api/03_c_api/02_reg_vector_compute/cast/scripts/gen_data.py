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


def get_saturation(data, data_type):
    return np.clip(data, np.iinfo(data_type).min, np.iinfo(data_type).max)


def gen_golden_data_simple(scenario_num):
    total_length = 256
    if scenario_num == 1:
        src_data_type = np.float16
        dst_data_type = np.int32
        x = np.random.uniform(-100, 100, [1, total_length]).astype(src_data_type)
        golden = np.floor(x).astype(dst_data_type)
    else:
        src_data_type = np.float32
        dst_data_type = np.int16
        x = np.random.uniform(-100, 100, [1, total_length]).astype(src_data_type)
        golden = get_saturation(np.round(x), dst_data_type).astype(dst_data_type)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        choices=[1, 2],
        help="Scenario number: 1=float16->int32, 2=float32->int16",
    )
    args = parser.parse_args()
    gen_golden_data_simple(args.scenarioNum)
