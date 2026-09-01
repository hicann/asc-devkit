#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import sys
from pathlib import Path

import numpy as np


def main() -> None:
    if len(sys.argv) > 2:
        raise ValueError("Usage: verify_result.py [output_dir]")
    output_dir = Path(sys.argv[1]) if len(sys.argv) == 2 else Path("output")
    actual = np.fromfile(output_dir / "dst_output.bin", dtype=np.float32)
    golden = np.fromfile(output_dir / "dst_golden.bin", dtype=np.float32)
    if actual.shape != golden.shape:
        raise ValueError(f"expected shape {golden.shape}, got {actual.shape}")
    if not np.allclose(actual, golden, rtol=1e-5, atol=1e-6):
        index = int(
            np.flatnonzero(~np.isclose(actual, golden, rtol=1e-5, atol=1e-6))[0]
        )
        raise ValueError(
            f"mismatch at index {index}, actual={actual[index]}, golden={golden[index]}"
        )
    print("test pass! verified float Reg Tensor operator+ results.")


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(error)
        sys.exit(1)
