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


def verify_result(scenario_num, output_path, golden_path):
    output_type = np.uint8 if scenario_num in (3, 4, 9, 10) else np.float32
    output = np.fromfile(output_path, dtype=output_type)
    golden = np.fromfile(golden_path, dtype=output_type)
    if output.shape != golden.shape:
        print(
            "[ERROR] output shape %s does not match golden shape %s"
            % (output.shape, golden.shape)
        )
        return False
    if output_type == np.uint8:
        passed = np.array_equal(output, golden)
        matched = output == golden
    else:
        matched = np.isclose(output, golden, rtol=1e-3, atol=2e-6, equal_nan=False)
        passed = np.all(matched)
    if not passed:
        mismatch = np.flatnonzero(~matched)
        for index in mismatch[:20]:
            print(
                "data index: %06d, expected: %s, actual: %s"
                % (index, golden[index], output[index])
            )
    return passed


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 11))
    parser.add_argument("output", type=str)
    parser.add_argument("golden", type=str)
    args = parser.parse_args()
    try:
        if not verify_result(args.scenarioNum, args.output, args.golden):
            raise ValueError("[ERROR] result error")
        print("test pass!")
    except Exception as error:
        print(error)
        sys.exit(1)
