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
    input_length = 64
    src = np.arange(input_length, dtype=np.float32) * np.float32(0.25) - np.float32(5.0)
    golden = np.array([np.sum(src, dtype=np.float32), np.max(src), np.min(src)], dtype=np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    src.tofile("input/input.bin")
    golden.tofile("output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
