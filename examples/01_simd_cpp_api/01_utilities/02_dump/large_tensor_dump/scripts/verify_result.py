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


def verify_result(output_path, golden_path):
    total_length = 65536
    output = np.fromfile(output_path, dtype=np.float32)
    golden = np.fromfile(golden_path, dtype=np.float32)
    if output.size != total_length or golden.size != total_length:
        print(f"Unexpected element count: output={output.size}, golden={golden.size}, expected={total_length}")
        return False
    mismatches = np.flatnonzero(output != golden)
    if mismatches.size:
        index = mismatches[0]
        print(f"data index: {index}, expected: {golden[index]}, actual: {output[index]}")
        return False
    return True


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("Usage: python3 verify_result.py <output.bin> <golden.bin>")
    try:
        if not verify_result(sys.argv[1], sys.argv[2]):
            sys.exit("[ERROR] result error")
        print("test pass!")
    except (OSError, ValueError) as error:
        sys.exit(str(error))
