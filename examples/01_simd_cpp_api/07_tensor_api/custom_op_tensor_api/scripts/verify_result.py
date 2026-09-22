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

import sys

import numpy as np


def verify_result(output_path, golden_path):
    output = np.fromfile(output_path, dtype=np.float16)
    golden = np.fromfile(golden_path, dtype=np.float16)
    if output.size != golden.size:
        print(f"result size mismatch: expected {golden.size}, actual {output.size}")
        return False

    close_mask = np.isclose(output, golden, rtol=1e-3, atol=1e-3, equal_nan=True)
    error_indexes = np.flatnonzero(~close_mask)
    for index in error_indexes[:10]:
        print(f"data index: {index:06d}, expected: {golden[index]:.6f}, actual: {output[index]:.6f}")

    error_ratio = error_indexes.size / golden.size
    print(f"error ratio: {error_ratio:.6f}")
    return error_ratio <= 1e-3


if __name__ == "__main__":
    if len(sys.argv) != 3 or not verify_result(sys.argv[1], sys.argv[2]):
        sys.exit(1)
    print("test pass!")
