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
import os
import numpy as np


relative_tol = 1e-3
absolute_tol = 1e-3
error_tol = 1e-3
M = 128
N = 128


def nz_to_nd(data, block_cols):
    return data.reshape((N // block_cols, M // 16, 16, block_cols)).transpose(1, 2, 0, 3).reshape(-1)


def verify_result(output, golden, scenarioNum):
    if (scenarioNum == "1") or (scenarioNum == "4"):
        golden = np.fromfile(golden, dtype=np.float16).reshape(-1)
        if os.path.getsize(output) == M * N * np.dtype(np.float16).itemsize:
            output = np.fromfile(output, dtype=np.float16).reshape(-1)
            output = nz_to_nd(output, 16)
        else:
            output = np.fromfile(output, dtype=np.float32).reshape(-1)
    else:
        golden = np.fromfile(golden, dtype=np.int8).reshape(-1)
        if os.path.getsize(output) == M * N * np.dtype(np.int8).itemsize:
            output = np.fromfile(output, dtype=np.int8).reshape(-1)
            output = nz_to_nd(output, 32)
        else:
            output = np.fromfile(output, dtype=np.int32).reshape(-1)
    different_element_results = np.isclose(output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True)
    different_element_indexes = np.where(different_element_results == False)[0]
    for index in range(len(different_element_indexes)):
        real_index = different_element_indexes[index]
        golden_data = golden[real_index]
        output_data = output[real_index]
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f, rdiff: %-.6f"
            % (real_index, golden_data, output_data, abs(output_data - golden_data) / golden_data)
        )
        if index == 100:
            break
    error_ratio = float(different_element_indexes.size) / golden.size
    return error_ratio <= error_tol


if __name__ == "__main__":
    try:
        res = verify_result(sys.argv[1], sys.argv[2], sys.argv[3])
        if res == -1:
            print("not supported!")
        elif not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
