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

import os
import sys
import numpy as np


# for float32
relative_tol = 1e-6
absolute_tol = 1e-9
error_tol = 1e-4


def verify_result(output_path, golden_path):
    output_size = os.path.getsize(output_path)
    golden_size = os.path.getsize(golden_path)
    if output_size != golden_size:
        print("output and golden file sizes differ: %d != %d" % (output_size, golden_size))
        return False
    if golden_size == 0:
        print("output and golden files are empty")
        return False

    if golden_size == 128 * 256 * np.dtype(np.int8).itemsize:
        output = np.fromfile(output_path, dtype=np.int8).reshape(-1)
        golden = np.fromfile(golden_path, dtype=np.int8).reshape(-1)
        different_element_indexes = np.where(output != golden)[0]
        for real_index in different_element_indexes[:101]:
            print("data index: %06d, expected: %d, actual: %d" % (real_index, golden[real_index], output[real_index]))
        return different_element_indexes.size == 0

    output = np.fromfile(output_path, dtype=np.float32).reshape(-1)
    golden = np.fromfile(golden_path, dtype=np.float32).reshape(-1)
    different_element_results = np.isclose(output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True)
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
                abs(output_data - golden_data) / max(abs(golden_data), absolute_tol),
            )
        )
        if index == 100:
            break
    error_ratio = float(different_element_indexes.size) / golden.size
    return error_ratio <= error_tol


if __name__ == "__main__":
    try:
        res = verify_result(sys.argv[1], sys.argv[2])
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
