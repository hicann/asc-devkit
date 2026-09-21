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
import argparse

import numpy as np


RELATIVE_TOL = 1e-4
ABSOLUTE_TOL = 1e-5
ERROR_TOL = 1e-4


def verify_result(output, golden):
    output_data = np.fromfile(output, dtype=np.float16).reshape(-1)
    golden_data = np.fromfile(golden, dtype=np.float16).reshape(-1)
    if output_data.size != golden_data.size:
        raise ValueError(f"size mismatch: output={output_data.size}, golden={golden_data.size}")

    compare = np.isclose(output_data, golden_data, rtol=RELATIVE_TOL, atol=ABSOLUTE_TOL, equal_nan=True)
    diff_indexes = np.where(compare == False)[0]
    for index, real_index in enumerate(diff_indexes[:100]):
        expect = golden_data[real_index]
        actual = output_data[real_index]
        denominator = expect if expect != 0 else np.float16(1.0)
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f, rdiff: %-.6f"
            % (real_index, expect, actual, abs(actual - expect) / denominator)
        )
        if index == 99:
            break

    error_ratio = float(diff_indexes.size) / golden_data.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, ERROR_TOL))
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=str)
    parser.add_argument("golden", type=str)
    args = parser.parse_args()
    try:
        res = verify_result(args.output, args.golden)
        if not res:
            raise ValueError("[ERROR] result error")
        print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
