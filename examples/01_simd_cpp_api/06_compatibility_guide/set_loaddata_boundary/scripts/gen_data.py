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
    input_shape_x = [32, 32]
    input_shape_y = [32, 32]
    dtype = np.half
    input_x = np.eye(32).astype(dtype)
    mid = 32 // 2  # 找到中间列
    input_x[mid:, :mid] = input_x[:mid, :mid]
    input_x[mid:, mid:] = input_x[:mid, :mid]
    input_x[:mid, mid:] = input_x[:mid, :mid]
    input_y = np.arange(32 * 32).reshape([32, 32]).astype(dtype)
    golden = np.matmul(input_x.astype(np.float32), input_y.astype(np.float32)).astype(np.float32)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
