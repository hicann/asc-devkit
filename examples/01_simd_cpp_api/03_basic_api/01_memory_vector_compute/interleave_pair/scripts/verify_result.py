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


RELATIVE_TOL = 1e-6
ABSOLUTE_TOL = 1e-9
ERROR_TOL = 1e-4


def verify_result(output0, output1, golden0, golden1):
    output_type = np.float16
    output0 = np.fromfile(output0, dtype=output_type).reshape(-1)
    output1 = np.fromfile(output1, dtype=output_type).reshape(-1)
    golden0 = np.fromfile(golden0, dtype=output_type).reshape(-1)
    golden1 = np.fromfile(golden1, dtype=output_type).reshape(-1)

    different0 = np.isclose(output0, golden0, rtol=RELATIVE_TOL, atol=ABSOLUTE_TOL, equal_nan=True)
    different1 = np.isclose(output1, golden1, rtol=RELATIVE_TOL, atol=ABSOLUTE_TOL, equal_nan=True)

    diff_idx0 = np.where(different0 == False)[0]
    diff_idx1 = np.where(different1 == False)[0]

    for idx in diff_idx0[:100]:
        print("dst0 index: %06d, expected: %-.9f, actual: %-.9f" % (idx, golden0[idx], output0[idx]))
    for idx in diff_idx1[:100]:
        print("dst1 index: %06d, expected: %-.9f, actual: %-.9f" % (idx, golden1[idx], output1[idx]))

    error_ratio = float(diff_idx0.size + diff_idx1.size) / (golden0.size + golden1.size)
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, ERROR_TOL))
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    try:
        res = verify_result(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
