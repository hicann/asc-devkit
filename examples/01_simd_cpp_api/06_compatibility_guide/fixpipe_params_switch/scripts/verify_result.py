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


def nd2nz(matrix, cube_block=16):
    m, n = matrix.shape
    m_align = (m + cube_block - 1) // cube_block * cube_block
    n_align = (n + cube_block - 1) // cube_block * cube_block
    padded = np.zeros((m_align, n_align), dtype=matrix.dtype)
    padded[:m, :n] = matrix
    nz = padded.reshape(
        m_align // cube_block, cube_block, n_align // cube_block, cube_block
    )
    nz = nz.transpose(2, 0, 1, 3).reshape(-1)
    return nz


def verify_result(output, golden):
    output = np.fromfile(output, dtype=np.float32).reshape(-1)
    golden = np.fromfile(golden, dtype=np.float32).reshape(128, 256)

    scenario = os.environ.get("SCENARIO_NUM", "1")
    if scenario == "2":
        golden = nd2nz(golden)
    else:
        golden = golden.reshape(-1)

    output = output[: golden.size]
    different_element_results = np.isclose(
        output, golden, rtol=relative_tol, atol=absolute_tol, equal_nan=True
    )
    different_element_indexes = np.where(different_element_results == False)[0]
    for index in range(min(len(different_element_indexes), 100)):
        real_index = different_element_indexes[index]
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f"
            % (real_index, golden[real_index], output[real_index])
        )
    error_ratio = float(different_element_indexes.size) / golden.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, error_tol))
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
