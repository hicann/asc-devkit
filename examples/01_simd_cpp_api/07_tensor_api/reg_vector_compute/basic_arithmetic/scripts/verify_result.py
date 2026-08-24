#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the license.

import sys
from pathlib import Path

import numpy as np


cases = (
    ("int8", np.int8, None),
    ("int16", np.int16, None),
    ("half", np.float16, (1e-3, 1e-3)),
    ("int32", np.int32, None),
    ("float", np.float32, (1e-4, 1e-4)),
)


def check_case(output_dir: Path, golden_dir: Path, name: str, dtype: type, tolerance) -> None:
    actual = np.fromfile(output_dir / f"{name}_output.bin", dtype=dtype)
    golden = np.fromfile(golden_dir / f"{name}_golden.bin", dtype=dtype)
    if actual.shape != golden.shape:
        raise ValueError(f"{name}: expected shape {golden.shape}, got {actual.shape}")
    if tolerance is None:
        if np.array_equal(actual, golden):
            return
        index = int(np.flatnonzero(actual != golden)[0])
    else:
        rtol, atol = tolerance
        matches = np.isclose(actual, golden, rtol=rtol, atol=atol)
        if np.all(matches):
            return
        index = int(np.flatnonzero(~matches)[0])
    raise ValueError(f"{name}: mismatch at index {index}, actual={actual[index]}, golden={golden[index]}")


def main() -> None:
    if len(sys.argv) > 2:
        raise ValueError("Usage: verify_result.py [output_dir]")
    output_dir = Path(sys.argv[1]) if len(sys.argv) == 2 else Path("output")
    golden_dir = output_dir
    for case in cases:
        check_case(output_dir, golden_dir, *case)
    print("test pass! verified abs for int8, int16, half, int32, and float.")


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(error)
        sys.exit(1)
