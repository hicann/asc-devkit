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
from pathlib import Path

import numpy as np


M = 16
N = 16
RELATIVE_TOLERANCE = 1e-3
ABSOLUTE_TOLERANCE = 1e-3


def load_matrix(path: Path) -> np.ndarray:
    matrix = np.fromfile(path, dtype=np.float32)
    expected_size = M * N
    if matrix.size != expected_size:
        raise ValueError(f"{path} contains {matrix.size} elements, expected {expected_size}")
    return matrix.reshape(M, N)


def verify(output_path: Path, golden_path: Path) -> bool:
    output = load_matrix(output_path)
    golden = load_matrix(golden_path)
    close = np.isclose(output, golden, rtol=RELATIVE_TOLERANCE, atol=ABSOLUTE_TOLERANCE, equal_nan=True)
    if np.all(close):
        max_error = float(np.max(np.abs(output - golden)))
        print(f"test pass! max absolute error: {max_error:.6e}")
        return True

    failed = np.argwhere(~close)
    for row, column in failed[:10]:
        print(f"mismatch at ({row}, {column}): expected={golden[row, column]:.9f}, actual={output[row, column]:.9f}")
    print(f"[ERROR] {failed.shape[0]} of {M * N} elements exceed tolerance")
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    parser.add_argument("golden", type=Path)
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    raise SystemExit(0 if verify(args.output, args.golden) else 1)
