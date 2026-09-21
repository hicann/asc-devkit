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
import numpy as np


RELATIVE_TOL = 1e-6
ABSOLUTE_TOL = 1e-9
ERROR_TOL = 1e-4


def verify_result(output, golden, scenarioNum=1):
    # 根据场景选择数据类型
    if scenarioNum == 2:
        output_type = np.float32
    elif scenarioNum == 4 or scenarioNum == 5 or scenarioNum == 6:
        output_type = np.int8
    else:
        output_type = np.float16

    np.set_printoptions(threshold=np.inf)
    output = np.fromfile(output, dtype=output_type).reshape(-1)
    golden = np.fromfile(golden, dtype=output_type).reshape(-1)

    different_element_results = np.isclose(
        output.flatten(), golden.flatten(), rtol=RELATIVE_TOL, atol=ABSOLUTE_TOL, equal_nan=True
    )
    different_element_indexes = np.where(different_element_results == False)[0]

    for index in range(len(different_element_indexes)):
        real_index = different_element_indexes[index]
        golden_data = golden.flatten()[real_index]
        output_data = output.flatten()[real_index]
        print("data index: %06d, expected: %-.9f, actual: %-.9f" % (real_index, golden_data, output_data))
        if index == 100:
            break

    error_ratio = float(different_element_indexes.size) / len(golden.flatten())
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, ERROR_TOL))
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output_file", help="Output file path")
    parser.add_argument("golden_file", help="Golden file path")
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2, 3, 4, 5, 6], help="Scenario number")
    args = parser.parse_args()

    try:
        res = verify_result(args.output_file, args.golden_file, args.scenarioNum)
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
