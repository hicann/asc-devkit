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


def gen_golden_data_int():
    input_total_length = 8192 * 8192
    rng = np.random.default_rng(20260516)
    self_data = rng.integers(-100000, 100001, input_total_length, dtype=np.int32)
    other_data = rng.integers(-127, 128, input_total_length, dtype=np.int32)
    other_data[other_data == 0] = 7

    divisible_indexes = np.arange(0, input_total_length, 64)
    multipliers = rng.integers(-2048, 2049, divisible_indexes.size, dtype=np.int32)
    self_data[divisible_indexes] = other_data[divisible_indexes] * multipliers

    golden = np.zeros(input_total_length, dtype=np.int32)
    chunk_size = 1 << 20
    for start in range(0, input_total_length, chunk_size):
        end = min(start + chunk_size, input_total_length)
        self_chunk = self_data[start:end].astype(np.int64)
        other_chunk = other_data[start:end].astype(np.int64)
        quotient = np.abs(self_chunk) // np.abs(other_chunk)
        quotient = np.where((self_chunk >= 0) == (other_chunk >= 0), quotient, -quotient)
        rem = self_chunk - quotient * other_chunk
        signs_differ = (rem < 0) != (other_chunk < 0)
        golden[start:end] = np.where((signs_differ) & (rem != 0), rem + other_chunk, rem).astype(np.int32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    self_data.tofile("./input/input_self.bin")
    other_data.tofile("./input/input_other.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_int()
