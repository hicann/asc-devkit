#!/usr/bin/env python3

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import sys
import numpy as np

RELATIVE_TOL = 1e-3
ABSOLUTE_TOL = 1e-3
EXPECTED_ELEMENTS = 1024 * 1024


def verify_result(output_path, golden_path):
    output = np.fromfile(output_path, dtype=np.float32)
    golden = np.fromfile(golden_path, dtype=np.float32)
    if output.size != EXPECTED_ELEMENTS or golden.size != EXPECTED_ELEMENTS:
        raise ValueError(
            f"size mismatch: expected={EXPECTED_ELEMENTS}, output={output.size}, golden={golden.size}"
        )

    matched = np.isclose(
        output, golden, rtol=RELATIVE_TOL, atol=ABSOLUTE_TOL, equal_nan=True
    )
    mismatch_indices = np.flatnonzero(~matched)
    for index in mismatch_indices[:10]:
        print(
            f"index {index}: expected={golden[index]:.9f}, actual={output[index]:.9f}"
        )

    print(
        f"mismatched elements: {mismatch_indices.size}, "
        f"rtol: {RELATIVE_TOL}, atol: {ABSOLUTE_TOL}, equal_nan: True"
    )
    return mismatch_indices.size == 0


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <output.bin> <golden.bin>")
        sys.exit(2)
    if not verify_result(sys.argv[1], sys.argv[2]):
        print("test failed")
        sys.exit(1)
    print("test pass!")
