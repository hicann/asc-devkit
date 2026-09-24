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


"""Compare a device result with the float32 matmul+bias+residual golden."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np


DEFAULT_M = 1024
DEFAULT_N = 1024
RELATIVE_TOLERANCE = 1e-3
ABSOLUTE_TOLERANCE = 1e-3


def configured_dimensions() -> tuple[int, int]:
    config_path = Path("sample_config.json")
    if not config_path.is_file():
        return DEFAULT_M, DEFAULT_N
    config = json.loads(config_path.read_text(encoding="utf-8"))
    return int(config["m"]), int(config["n"])


def load_matrix(path: Path, elements: int) -> np.ndarray:
    values = np.fromfile(path, dtype=np.float32)
    if values.size != elements:
        raise ValueError(f"{path} contains {values.size} float32 values, expected {elements}")
    return values.reshape(-1)


def verify(output_path: Path, golden_path: Path, m: int, n: int) -> bool:
    if min(m, n) <= 0:
        raise ValueError("matrix dimensions must be positive")
    expected_elements = m * n
    output = load_matrix(output_path, expected_elements)
    golden = load_matrix(golden_path, expected_elements)
    absolute_error = np.abs(output - golden)
    relative_error = absolute_error / np.maximum(np.abs(golden), np.finfo(np.float32).tiny)
    max_absolute_error = float(np.max(absolute_error)) if output.size else 0.0
    max_relative_error = float(np.max(relative_error)) if output.size else 0.0
    close = np.isclose(output, golden, rtol=RELATIVE_TOLERANCE, atol=ABSOLUTE_TOLERANCE, equal_nan=True)
    if np.all(close):
        print(f"test pass! max absolute error: {max_absolute_error:.6e}, max relative error: {max_relative_error:.6e}")
        return True

    failed = np.flatnonzero(~close)
    first = int(failed[0])
    row, column = divmod(first, n)
    print(
        f"first mismatch at ({row}, {column}): "
        f"expected={golden[first]:.9f}, actual={output[first]:.9f}, "
        f"absolute_error={absolute_error[first]:.6e}, relative_error={relative_error[first]:.6e}"
    )
    print(
        f"[ERROR] {failed.size} of {expected_elements} elements exceed tolerance; "
        f"max absolute error: {max_absolute_error:.6e}, max relative error: {max_relative_error:.6e}"
    )
    return False


def parse_args() -> argparse.Namespace:
    default_m, default_n = configured_dimensions()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    parser.add_argument("golden", type=Path)
    parser.add_argument("--m", type=int, default=default_m, help=f"output rows (default: {default_m})")
    parser.add_argument("--n", type=int, default=default_n, help=f"output columns (default: {default_n})")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    try:
        passed = verify(args.output, args.golden, args.m, args.n)
    except (OSError, ValueError) as error:
        print(f"[ERROR] {error}")
        raise SystemExit(1)
    raise SystemExit(0 if passed else 1)
