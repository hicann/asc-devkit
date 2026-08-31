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

import argparse
import os

import numpy as np


ROW = 32
COL = 32
LEFT_PADDING = 3
RIGHT_PADDING = 5
PADDING_VALUE = -9


def gen_golden_data(scenario_num):
    input_col = COL - LEFT_PADDING - RIGHT_PADDING if scenario_num == 5 else COL
    data = ((np.arange(ROW * input_col, dtype=np.int16) * 13 + 7) % 251 - 125).astype(
        np.int8
    )

    golden = data
    if scenario_num == 5:
        golden = np.full((ROW, COL), PADDING_VALUE, dtype=np.int8)
        golden[:, LEFT_PADDING : COL - RIGHT_PADDING] = data.reshape(ROW, input_col)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    data.tofile("input/input_x.bin")
    golden.tofile("output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenario_num", type=int, default=1, choices=range(1, 6))
    args = parser.parse_args()
    gen_golden_data(args.scenario_num)
