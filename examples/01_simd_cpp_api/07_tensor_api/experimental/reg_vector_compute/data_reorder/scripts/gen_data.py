#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import os
import numpy as np


def gen_golden_data_simple():
    total_length = 256
    data_type = np.float32
    src0 = np.arange(total_length, dtype=data_type)
    src1 = np.arange(1000, 1000 + total_length, dtype=data_type)
    combined = np.array([], dtype=data_type)
    for i in range(4):
        offset = i * 64
        tmp_src0 = src0[offset : offset + 64]
        tmp_src1 = src1[offset : offset + 64]
        combined = np.concatenate((combined, tmp_src0, tmp_src1))
    dst0 = combined[0::2]
    dst1 = combined[1::2]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    src0.tofile("input/input_src0.bin")
    src1.tofile("input/input_src1.bin")
    dst0.tofile("output/golden_dst0.bin")
    dst1.tofile("output/golden_dst1.bin")

    print("dst0:\n", dst0)
    print("dst1:\n", dst1)


if __name__ == "__main__":
    gen_golden_data_simple()
