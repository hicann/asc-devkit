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
import numpy as np


def gen_golden_data_simple():
    input_type = np.dtype("float16")
    output_type = input_type
    sync_type = np.dtype("int32")
    total_length = 512
    input_x = np.ones(total_length, dtype=input_type)
    input_y = np.ones(total_length, dtype=input_type)
    golden = np.zeros(total_length, dtype=output_type)
    input_sync = np.zeros(total_length // 2, dtype=sync_type)
    golden[0 : total_length // 2] = input_x[0 : total_length // 2] + input_y[0 : total_length // 2]
    golden[total_length // 2 :] = golden[0 : total_length // 2] + input_y[total_length // 2 :]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")
    input_sync.tofile("./input/input_sync.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
