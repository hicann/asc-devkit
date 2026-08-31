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
import argparse

import numpy as np


EPSILON = 1e-4


def verify_result(output_path, golden_path, scenario_num):
    dtype = np.float16 if scenario_num == 3 else np.float32
    output_size = os.path.getsize(output_path)
    golden_size = os.path.getsize(golden_path)
    if output_size != golden_size:
        print(
            "output and golden file sizes differ: %d != %d" % (output_size, golden_size)
        )
        return False
    if golden_size == 0 or golden_size % np.dtype(dtype).itemsize != 0:
        print("output and golden files must contain complete %s data" % np.dtype(dtype))
        return False

    output = np.fromfile(output_path, dtype=dtype)
    golden = np.fromfile(golden_path, dtype=dtype)
    if scenario_num == 3:
        different_indices = np.where(output != golden)[0]
        for index in different_indices[:101]:
            golden_value = golden[index]
            output_value = output[index]
            print(
                "data index: %06d, expected: %-.9f, actual: %-.9f"
                % (index, golden_value, output_value)
            )
        return different_indices.size == 0

    absolute_difference = np.abs(output - golden)
    with np.errstate(divide="ignore", invalid="ignore"):
        relative_difference = absolute_difference / np.abs(golden)
    different_indices = np.where(
        (absolute_difference > EPSILON) & (relative_difference > EPSILON)
    )[0]
    for index in different_indices[:101]:
        golden_value = golden[index]
        output_value = output[index]
        print(
            "data index: %06d, expected: %-.9f, actual: %-.9f, rdiff: %-.6f"
            % (index, golden_value, output_value, relative_difference[index])
        )

    return different_indices.size == 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output_path")
    parser.add_argument("golden_path")
    parser.add_argument("-scenarioNum", type=int, default=1, choices=(1, 2, 3))
    args = parser.parse_args()

    try:
        if not verify_result(args.output_path, args.golden_path, args.scenarioNum):
            raise ValueError("[ERROR] result error")
        print("test pass!")
    except Exception as error:
        print(error)
        sys.exit(1)
