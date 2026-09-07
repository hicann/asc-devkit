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
import sys
import numpy as np


# for float32
relative_tol = 1e-4
absolute_tol = 1e-4
# This sample outputs only M * N = 2100 elements; 1/2100 already exceeds 1e-4,
# so any single out-of-tolerance element fails the check.
# Write 0 directly to make the zero tolerance explicit, so the printed tolerance does not suggest a usable ratio margin
error_tol = 0.0


def verify_result(output, golden):
    output = np.fromfile(output, dtype=np.float32).reshape(-1)
    golden = np.fromfile(golden, dtype=np.float32).reshape(-1)
    if output.size != golden.size:
        raise ValueError(
            "[ERROR] output and golden have different element counts: %d vs %d"
            % (output.size, golden.size)
        )
    different_element_results = np.isclose(
        output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True
    )
    different_element_indexes = np.where(different_element_results == False)[0]
    for index in range(min(len(different_element_indexes), 100)):
        real_index = different_element_indexes[index]
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f, adiff: %-.9f"
            % (
                real_index,
                golden[real_index],
                output[real_index],
                abs(output[real_index] - golden[real_index]),
            )
        )
    error_ratio = float(different_element_indexes.size) / golden.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, error_tol))
    return error_ratio <= error_tol


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=str)
    parser.add_argument("golden", type=str)
    args = parser.parse_args()
    try:
        res = verify_result(args.output, args.golden)
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
