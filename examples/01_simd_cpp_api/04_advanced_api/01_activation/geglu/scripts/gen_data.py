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


def gen_golden_data():
    dtype = np.float32
    np.set_printoptions(threshold=sys.maxsize, suppress=True)

    src0 = np.random.uniform(-0.1, 0.1, [1024]).astype(dtype)
    src1 = np.random.uniform(-0.1, 0.1, [1024]).astype(dtype)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    src0.tofile("./input/input_src0.bin")
    src1.tofile("./input/input_src1.bin")

    golden = src0 * (src1 / (1 + np.exp(-0.0713548162726 * src1 * (2.2363860002236e1 + np.power(src1, 2)))))
    golden = golden.astype(dtype)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
