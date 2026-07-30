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
from pathlib import Path
import numpy as np


def read_result_file(file_path, label, expected_size):
    path = Path(file_path)
    if not path.is_file():
        raise FileNotFoundError(
            f"{label} file not found: {file_path}. "
            f"Current directory: {Path.cwd()}. "
            "Please run gen_data.py and ./demo in the build directory before verification."
        )

    data = np.fromfile(path, dtype=np.float32)
    if data.size != expected_size:
        raise ValueError(
            f"{label} file size error: expected {expected_size} float32 values, got {data.size}"
        )
    return data


def verify_result(output_file, golden_file):
    m, n = 40, 50
    expected_size = m * n

    output_data = read_result_file(output_file, "output", expected_size)
    golden_data = read_result_file(golden_file, "golden", expected_size)

    output_data = output_data.reshape(m, n)
    golden_data = golden_data.reshape(m, n)

    diff = np.abs(output_data - golden_data)
    max_diff = np.max(diff)
    mean_diff = np.mean(diff)

    threshold = 0.01
    if max_diff < threshold:
        print("test pass!")
        return True
    else:
        print(f"test failed! max_diff={max_diff}, mean_diff={mean_diff}")
        return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=str, nargs="?", default="output/output.bin")
    parser.add_argument("golden", type=str, nargs="?", default="output/golden.bin")
    args = parser.parse_args()

    try:
        if not verify_result(args.output, args.golden):
            sys.exit(1)
    except Exception as e:
        print(e)
        sys.exit(1)
