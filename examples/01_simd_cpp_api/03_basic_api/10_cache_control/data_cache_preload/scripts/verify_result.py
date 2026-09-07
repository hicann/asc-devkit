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
import numpy as np


def verify_result(output, golden):
    """
    验证核函数的输出结果是否与预期一致。

    output: output.bin，包含1个uint32_t累加和
    golden: golden.bin，包含1个预期累加和
    """
    output = np.fromfile(output, dtype=np.uint32).reshape(-1)
    golden = np.fromfile(golden, dtype=np.uint32).reshape(-1)

    if output.size != golden.size:
        print("output size: %d, golden size: %d" % (output.size, golden.size))
        return False

    diff = np.where(output != golden)[0]
    for index in range(len(diff)):
        real_index = diff[index]
        golden_data = golden[real_index]
        output_data = output[real_index]
        print(
            "data index: %06d, expected: %d, actual: %d"
            % (real_index, golden_data, output_data)
        )
        if index == 100:
            break
    error_ratio = float(diff.size) / golden.size
    print("error ratio: %.4f, tolerance: 0.0" % error_ratio)
    return diff.size == 0


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
