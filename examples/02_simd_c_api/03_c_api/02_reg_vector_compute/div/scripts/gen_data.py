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


def gen_golden_data():
    total_length = 1024
    # 最小正规数：1.1754944e-38（np.power(2, -126)） 最小次正规数: 1.401298464324817e-45（np.power(2, -149)）
    # 1. N / N = S
    # 2. N / S = N
    # 3. S / N = S
    # 4. S / N = N
    # 5. S / S = N
    # 5. S / S = S
    x_small = np.array(
        [
            np.power(2.0, -126),
            np.power(2.0, -125),
            np.power(2.0, -130),
            np.power(2.0, -130),
            np.power(2.0, -140),
        ]
    )
    y_small = np.array(
        [4.0, np.power(2.0, -127), 2.0, np.power(2.0, -20), np.power(2.0, -145)]
    )
    reps = int(np.ceil(total_length / len(x_small)))
    x = (np.tile(x_small, reps)[:total_length]).astype(np.float32)
    y = (np.tile(y_small, reps)[:total_length]).astype(np.float32)

    golden = (x.astype(np.float64) / y.astype(np.float64)).astype(np.float32)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
