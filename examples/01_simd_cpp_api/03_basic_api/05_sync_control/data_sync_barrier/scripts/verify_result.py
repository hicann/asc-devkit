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


def verify_result(output_file, golden_file):
    output = np.fromfile(output_file, dtype=np.int32).reshape(-1)
    golden = np.fromfile(golden_file, dtype=np.int32).reshape(-1)
    if output.size < golden.size:
        raise ValueError(f"output size {output.size} < expected {golden.size}")

    output = output[: golden.size]
    different_indexes = np.where(output != golden)[0]
    for index in different_indexes[:100]:
        print("data index: %06d, expected: %d, actual: %d" % (index, golden[index], output[index]))

    error_ratio = float(different_indexes.size) / golden.size
    print("error ratio: %.4f, tolerance: %.4f" % (error_ratio, 0.0))
    return different_indexes.size == 0


if __name__ == "__main__":
    try:
        if len(sys.argv) != 3:
            raise ValueError("usage: verify_result.py output_file golden_file")
        if not verify_result(sys.argv[1], sys.argv[2]):
            raise ValueError("[ERROR] result error")
        print("test pass!")
    except Exception as err:
        print(err)
        sys.exit(1)
