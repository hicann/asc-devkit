#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import sys
import argparse
from pathlib import Path
import numpy as np


RELATIVE_TOL = 1e-3
ABSOLUTE_TOL = 1e-5
ERROR_TOL = 1e-3
RELATIVE_TOL_F32 = 1e-5
ABSOLUTE_TOL_F32 = 1e-8


def verify_result(scenarioNum, output, golden):
    output_bytes = Path(output).read_bytes()
    golden_bytes = Path(golden).read_bytes()
    # Scenario 2 stores an FP16 value and a uint16 index in each four-byte record.
    record_size = 2 if scenarioNum == 1 else 4
    if not golden_bytes or len(output_bytes) != len(golden_bytes) or len(golden_bytes) % record_size != 0:
        raise ValueError(f"[ERROR] invalid result file sizes: output={len(output_bytes)}, golden={len(golden_bytes)}")
    dtype = np.float32 if scenarioNum in (3, 4) else np.float16
    output = np.frombuffer(output_bytes, dtype=dtype)
    golden = np.frombuffer(golden_bytes, dtype=dtype)
    if scenarioNum == 2:
        output_indices = np.frombuffer(output_bytes, dtype=np.uint16)[1::2]
        golden_indices = np.frombuffer(golden_bytes, dtype=np.uint16)[1::2]
        index_errors = np.flatnonzero(output_indices != golden_indices)
        if index_errors.size:
            for index in index_errors[:100]:
                print(
                    "index row: %06d, expected: %d, actual: %d" % (index, golden_indices[index], output_indices[index])
                )
            return False
        output, golden = output[::2], golden[::2]

    rtol = RELATIVE_TOL_F32 if scenarioNum in (3, 4) else RELATIVE_TOL
    atol = ABSOLUTE_TOL_F32 if scenarioNum in (3, 4) else ABSOLUTE_TOL

    different_element_results = np.isclose(output, golden, rtol=rtol, atol=atol, equal_nan=True)
    different_element_indexes = np.where(different_element_results == False)[0]
    for index in range(len(different_element_indexes)):
        real_index = different_element_indexes[index]
        golden_data = golden[real_index]
        output_data = output[real_index]
        print("data index: %06d, expected: %-.9f, actual: %-.9f" % (real_index, golden_data, output_data))
        if index == 100:
            break
    error_ratio = float(different_element_indexes.size) / golden.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, ERROR_TOL))
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 5))
    parser.add_argument("output", type=str)
    parser.add_argument("golden", type=str)
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
