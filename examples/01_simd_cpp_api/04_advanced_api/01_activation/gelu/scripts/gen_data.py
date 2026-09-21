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
import sys
import numpy as np


def gen_golden_data_simple():
    dtype = np.float32
    np.set_printoptions(threshold=sys.maxsize, suppress=True)

    src = np.random.uniform(-5, 5, [32]).astype(dtype)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    src.tofile("./input/input_src.bin")

    golden = src / (1 + np.exp(-1.5957691216057308 * (src + 0.044715 * np.power(src, 3))))
    golden = golden.astype(dtype)

    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
