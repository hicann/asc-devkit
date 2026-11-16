#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from pathlib import Path

import numpy as np

ATOL = 2.0e-2
RTOL = 2.0e-2


def from_bfloat16_bits(bits):
    return (bits.astype(np.uint32) << 16).view(np.float32)


def main():
    actual = from_bfloat16_bits(np.fromfile(Path("output/output.bin"), dtype=np.uint16))
    golden = from_bfloat16_bits(np.fromfile(Path("output/golden.bin"), dtype=np.uint16))
    if actual.size != golden.size:
        print(f"[ERROR] output size {actual.size} does not match golden size {golden.size}")
        return 1

    close = np.isclose(actual, golden, atol=ATOL, rtol=RTOL, equal_nan=True)
    max_error = float(np.max(np.abs(actual - golden))) if actual.size else 0.0
    if np.all(close):
        print("test pass!")
        return 0

    failed = np.flatnonzero(~close)
    print(f"[ERROR] {failed.size} of {actual.size} elements exceed tolerance; max error: {max_error:.6e}")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
