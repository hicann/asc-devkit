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


relative_tol = 1e-3
absolute_tol = 1e-5
error_tol = 1e-3


def verify_result(scenario_num, output_path, golden_path):
    """
    验证输出结果与Golden数据是否一致
    场景1：验证4个float（GetMrgSortResult值）
    场景2：验证192个float（96*2）
    场景3：验证2048个float（1024*2）
    """
    if scenario_num == 1:
        total_floats = 256
    elif scenario_num == 2:
        total_floats = 192
    elif scenario_num == 3:
        total_floats = 2048
    else:
        raise ValueError(f"Unsupported scenario_num: {scenario_num}")

    output = np.fromfile(output_path, dtype=np.float32).reshape(-1)
    golden = np.fromfile(golden_path, dtype=np.float32).reshape(-1)

    assert output.size >= total_floats, f"output size {output.size} < expected {total_floats}"
    assert golden.size >= total_floats, f"golden size {golden.size} < expected {total_floats}"

    output = output[:total_floats]
    golden = golden[:total_floats]

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
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, error_tol))
    return error_ratio <= error_tol


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 4), help="scenario number (1-3)")
    parser.add_argument("output", type=str, help="output bin file path")
    parser.add_argument("golden", type=str, help="golden bin file path")
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
