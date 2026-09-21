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


ERROR_TOL = 0.0


def verify_result(output, golden, scenario_num):
    if scenario_num == 1:
        output_type = np.int32
    else:
        output_type = np.uint32

    output = np.fromfile(output, dtype=output_type).reshape(-1)
    golden = np.fromfile(golden, dtype=output_type).reshape(-1)

    different_element_indexes = np.where(output != golden)[0]
    for index in range(len(different_element_indexes)):
        real_index = different_element_indexes[index]
        golden_data = golden[real_index]
        output_data = output[real_index]
        print("data index: %06d, expected: %d, actual: %d" % (real_index, golden_data, output_data))
        if index == 100:
            break

    error_ratio = float(different_element_indexes.size) / golden.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, ERROR_TOL))
    return error_ratio <= ERROR_TOL


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=str, help="output file path")
    parser.add_argument("golden", type=str, help="golden file path")
    parser.add_argument("-scenarioNum", type=int, default=1, help="scenario number")
    args = parser.parse_args()

    try:
        res = verify_result(args.output, args.golden, args.scenarioNum)
        if not res:
            raise ValueError("[ERROR] result error")
        else:
            print("test pass!")
    except Exception as e:
        print(e)
        sys.exit(1)
