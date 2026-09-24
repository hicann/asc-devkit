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


"""Generate matmul+bias+residual inputs and the float32 reference result."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np


# CMake writes the same full-size sample_config.json for both run modes.
# The constants below keep standalone script execution consistent with it.
DEFAULT_M = 1024
DEFAULT_K = 256
DEFAULT_N = 1024
DEFAULT_SEED = 2026


def configured_dimensions() -> tuple[int, int, int]:
    config_path = Path("sample_config.json")
    if not config_path.is_file():
        return DEFAULT_M, DEFAULT_K, DEFAULT_N
    config = json.loads(config_path.read_text(encoding="utf-8"))
    return int(config["m"]), int(config["k"]), int(config["n"])


def generate_data(m: int, k: int, n: int, seed: int) -> None:
    if min(m, k, n) <= 0:
        raise ValueError("matrix dimensions must be positive")

    rng = np.random.default_rng(seed)
    a = rng.uniform(-1.0, 1.0, size=(m, k)).astype(np.float16)
    b = rng.uniform(-1.0, 1.0, size=(k, n)).astype(np.float16)
    bias = rng.uniform(-1.0, 1.0, size=n).astype(np.float32)
    residual = rng.uniform(-1.0, 1.0, size=(m, n)).astype(np.float32)

    # The device accumulates half inputs into float output and then AIV adds
    # residual. Compute the reference in float32 in the same order.
    golden = np.matmul(a.astype(np.float32), b.astype(np.float32))
    golden += bias[None, :]
    golden += residual

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)
    a.tofile(input_dir / "a.bin")
    b.tofile(input_dir / "b.bin")
    bias.tofile(input_dir / "bias.bin")
    residual.tofile(input_dir / "residual.bin")
    golden.astype(np.float32).tofile(output_dir / "golden.bin")


def parse_args() -> argparse.Namespace:
    default_m, default_k, default_n = configured_dimensions()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--m", type=int, default=default_m, help=f"rows of A/output (default: {default_m})")
    parser.add_argument("--k", type=int, default=default_k, help=f"inner dimension (default: {default_k})")
    parser.add_argument("--n", type=int, default=default_n, help=f"columns of B/output (default: {default_n})")
    parser.add_argument("--seed", type=int, default=DEFAULT_SEED, help=f"random seed (default: {DEFAULT_SEED})")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    generate_data(args.m, args.k, args.n, args.seed)
