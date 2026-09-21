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


MATRIX_H = 8192
MATRIX_W = 8192
C0_ELEMS = 16
GLOBAL_C0_COLS = MATRIX_W // C0_ELEMS
RANDOM_SEED = 2026
RANDOM_LOW = -128
RANDOM_HIGH = 128


def build_golden(input_x):
    nd_blocks = input_x.reshape(MATRIX_H, GLOBAL_C0_COLS, C0_ELEMS)
    nz_blocks = nd_blocks.transpose(1, 0, 2)
    return np.ascontiguousarray(nz_blocks).reshape(-1)


def main():
    rng = np.random.default_rng(RANDOM_SEED)
    input_x = rng.integers(RANDOM_LOW, RANDOM_HIGH + 1, size=(MATRIX_H, MATRIX_W), dtype=np.int16).astype(np.float16)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    input_x.tofile("./input/input.bin")
    golden = build_golden(input_x)
    golden.tofile("./output/golden.bin")
    print(f"generate random input: seed={RANDOM_SEED}, range=[{RANDOM_LOW}, {RANDOM_HIGH}]")
    print(f"input sample: {input_x.reshape(-1)[:16]}")


if __name__ == "__main__":
    main()
