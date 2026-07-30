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
import numpy as np


def gen_golden_data_simple():
    total_length = 128
    data_type = np.float32
    x = np.random.uniform(1, 100, [total_length]).astype(data_type)
    y = np.full(total_length, -1.0, dtype=data_type)
    golden = np.zeros(total_length, dtype=data_type)

    one_rep_size = 64
    half_rep_size = one_rep_size // 2
    for i in range(0, total_length, one_rep_size):
        golden[i : i + half_rep_size] = x[i : i + half_rep_size]
        golden[i + half_rep_size : i + one_rep_size] = y[
            i + half_rep_size : i + one_rep_size
        ]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
