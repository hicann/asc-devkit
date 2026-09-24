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

"""Verify the compact NZ output produced by the bank-conflict sample."""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


DEFAULT_M = 8192
DEFAULT_N = 8192


def load_data(path: Path, expected_elements: int) -> np.ndarray:
    values = np.fromfile(path, dtype=np.float16)
    if values.size != expected_elements:
        raise ValueError(f"{path} contains {values.size} half values, expected {expected_elements}")
    return values


def verify(output_path: Path, golden_path: Path, m: int, n: int) -> bool:
    if m <= 0 or n <= 0:
        raise ValueError("matrix dimensions must be positive")
    expected_elements = m * n
    output = load_data(output_path, expected_elements)
    golden = load_data(golden_path, expected_elements)
    equal = output == golden
    if np.all(equal):
        print(f"test pass! verified {expected_elements} compact NZ elements")
        return True

    failed = np.flatnonzero(~equal)
    for flat_index in failed[:10]:
        index = int(flat_index)
        print(f"mismatch at flat index {index}: expected={golden[index]}, actual={output[index]}")
    print(f"[ERROR] {failed.size} of {expected_elements} elements differ")
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    parser.add_argument("golden", type=Path)
    parser.add_argument("--m", type=int, default=DEFAULT_M, help=f"input rows (default: {DEFAULT_M})")
    parser.add_argument("--n", type=int, default=DEFAULT_N, help=f"input columns (default: {DEFAULT_N})")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    try:
        passed = verify(args.output, args.golden, args.m, args.n)
    except (OSError, ValueError) as error:
        print(f"[ERROR] {error}")
        raise SystemExit(1)
    raise SystemExit(0 if passed else 1)
