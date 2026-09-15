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
relative_tol = 1e-6
absolute_tol = 1e-9
error_tol = 1e-4


def verify_result(scenario_num, output, golden):
    output_dtype = np.int8 if scenario_num in (4, 5) else np.float32
    output = np.fromfile(output, dtype=output_dtype).reshape(-1)
    golden = np.fromfile(golden, dtype=output_dtype).reshape(-1)
    if output.size != golden.size:
        print(
            "element count mismatch: expected %d, actual %d"
            % (golden.size, output.size)
        )
        return False
    different_element_results = np.isclose(
        output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True
    )
    different_element_indexes = np.where(different_element_results == False)[0]
    for index in range(len(different_element_indexes)):
        real_index = different_element_indexes[index]
        golden_data = golden[real_index]
        output_data = output[real_index]
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f, rdiff: %-.6f"
            % (
                real_index,
                golden_data,
                output_data,
                abs(output_data - golden_data) / golden_data,
            )
        )
        if index == 100:
            break
    error_ratio = float(different_element_indexes.size) / golden.size
    return error_ratio <= error_tol


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 8))
    parser.add_argument("output")
    parser.add_argument("golden")
    args = parser.parse_args()
    try:
        res = verify_result(args.scenarioNum, args.output, args.golden)
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
