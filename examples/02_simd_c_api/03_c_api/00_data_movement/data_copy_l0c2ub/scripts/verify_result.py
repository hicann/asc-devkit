#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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


RELATIVE_TOL = 1e-6
ABSOLUTE_TOL = 1e-9
FLOAT_OUTPUT_RELATIVE_TOL = 1e-4
FLOAT_OUTPUT_ABSOLUTE_TOL = 1e-4
ERROR_TOL = 1e-4


def get_tolerance(scenario_num):
    if scenario_num == 7:
        return FLOAT_OUTPUT_RELATIVE_TOL, FLOAT_OUTPUT_ABSOLUTE_TOL
    return RELATIVE_TOL, ABSOLUTE_TOL


def verify_result(scenario_num, output_path, golden_path):
    output_dtype = (
        np.float32
        if scenario_num == 7
        else (np.float16 if scenario_num in (1, 2) else np.int8)
    )
    output = np.fromfile(output_path, dtype=output_dtype).reshape(-1)
    golden = np.fromfile(golden_path, dtype=output_dtype).reshape(-1)
    if output.size != golden.size:
        print(
            "element count mismatch: expected %d, actual %d"
            % (golden.size, output.size)
        )
        return False

    relative_tol, absolute_tol = get_tolerance(scenario_num)
    match = np.isclose(
        output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True
    )
    mismatch_indices = np.where(~match)[0]
    for index in mismatch_indices[:101]:
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f"
            % (index, golden[index], output[index])
        )
    error_ratio = float(mismatch_indices.size) / golden.size
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 8))
    parser.add_argument("output", type=str)
    parser.add_argument("golden", type=str)
    args = parser.parse_args()
    if not verify_result(args.scenarioNum, args.output, args.golden):
        print("[ERROR] result error")
        sys.exit(1)
    print("test pass!")
