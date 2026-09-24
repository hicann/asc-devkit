#!/usr/bin/env python3
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

"""Generate an ND input matrix and its compact NZ reference."""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


DEFAULT_M = 8192
DEFAULT_N = 8192
DEFAULT_SEED = 2026
C0 = 16


def nd_to_nz(matrix: np.ndarray) -> np.ndarray:
    m, n = matrix.shape
    if m <= 0 or n <= 0 or n % C0 != 0:
        raise ValueError(f"matrix shape must have positive dimensions and N divisible by {C0}: {matrix.shape}")
    blocks = matrix.reshape(m, n // C0, C0)
    return np.ascontiguousarray(blocks.transpose(1, 0, 2)).reshape(-1)


def generate_data(m: int, n: int, seed: int) -> None:
    if m <= 0 or n <= 0 or m % C0 != 0 or n % C0 != 0:
        raise ValueError(f"M and N must be positive multiples of {C0}, got M={m}, N={n}")

    rng = np.random.default_rng(seed)
    # Integer-valued half input keeps the expected result exactly
    # representable and makes failures easy to inspect in a simulator dump.
    matrix = rng.integers(-128, 129, size=(m, n), dtype=np.int16).astype(np.float16)
    golden = nd_to_nz(matrix)

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)
    matrix.tofile(input_dir / "input.bin")
    golden.tofile(output_dir / "golden.bin")
    print(f"generated ND input: shape=({m}, {n}), seed={seed}")
    print(f"generated NZ golden: elements={golden.size}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--m", type=int, default=DEFAULT_M, help=f"input rows (default: {DEFAULT_M})")
    parser.add_argument("--n", type=int, default=DEFAULT_N, help=f"input columns (default: {DEFAULT_N})")
    parser.add_argument("--seed", type=int, default=DEFAULT_SEED, help=f"random seed (default: {DEFAULT_SEED})")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    generate_data(args.m, args.n, args.seed)
