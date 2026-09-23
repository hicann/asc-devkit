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

import argparse
import os
import numpy as np


DEFAULT_ROWS = 8192
DEFAULT_COLS = 197


def parse_args():
    parser = argparse.ArgumentParser(description="Generate fp16 softmax input and golden binary files.")
    parser.add_argument("rows", nargs="?", type=int, default=DEFAULT_ROWS, help="Flattened row count.")
    parser.add_argument("cols", nargs="?", type=int, default=DEFAULT_COLS, help="Softmax last dimension, in [1, 512].")
    parser.add_argument("--seed", type=int, default=2026, help="Random seed.")
    parser.add_argument("--low", type=float, default=-1.0, help="Input lower bound.")
    parser.add_argument("--high", type=float, default=1.0, help="Input upper bound.")
    return parser.parse_args()


def resolve_shape(args):
    if args.rows <= 0:
        raise ValueError("rows must be positive")
    if args.cols <= 0 or args.cols > 512:
        raise ValueError("cols must be in [1, 512]")
    return args.rows, args.cols


def gen_golden_data(rows, cols, seed, low, high):
    rng = np.random.default_rng(seed)
    input_x = rng.uniform(low, high, size=(rows, cols)).astype(np.float16)

    input_f32 = input_x.astype(np.float32)
    reduce_max = np.max(input_f32, axis=1, keepdims=True)
    exp_x = np.exp(input_f32 - reduce_max)
    golden = (exp_x / np.sum(exp_x, axis=1, keepdims=True)).astype(np.float16)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")
    print(
        f"[INFO] generated input/input_x.bin and output/golden.bin, "
        f"shape=[{rows},{cols}], dtype=float16, range=[{low},{high}], seed={seed}"
    )


if __name__ == "__main__":
    options = parse_args()
    shape_rows, shape_cols = resolve_shape(options)
    gen_golden_data(shape_rows, shape_cols, options.seed, options.low, options.high)
