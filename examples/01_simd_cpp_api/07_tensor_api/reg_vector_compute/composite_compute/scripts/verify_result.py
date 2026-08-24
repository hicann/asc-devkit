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
    ("half", np.float16, 1e-3, 1e-3),
    ("float", np.float32, 1e-4, 1e-4),
)


def main() -> None:
    if len(sys.argv) > 2:
        raise ValueError("Usage: verify_result.py [output_dir]")
    output_dir = Path(sys.argv[1]) if len(sys.argv) == 2 else Path("output")
    golden_dir = output_dir
    for name, dtype, rtol, atol in cases:
        actual = np.fromfile(output_dir / f"{name}_output.bin", dtype=dtype)
        golden = np.fromfile(golden_dir / f"{name}_golden.bin", dtype=dtype)
        if actual.shape != golden.shape:
            raise ValueError(f"{name}: expected shape {golden.shape}, got {actual.shape}")
        matches = np.isclose(actual, golden, rtol=rtol, atol=atol)
        if np.all(matches):
            continue
        index = int(np.flatnonzero(~matches)[0])
        raise ValueError(f"{name}: mismatch at index {index}, actual={actual[index]}, golden={golden[index]}")
    print("test pass! verified fma for half and float.")


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(error)
        sys.exit(1)
