#!/usr/bin/env python3

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
import numpy as np

M = 1024
K = 1024
N = 1024


def main():
    rng = np.random.default_rng(20260806)
    matrix_a = rng.uniform(-0.1, 0.1, (M, K)).astype(np.float16)
    matrix_b = rng.uniform(-0.1, 0.1, (K, N)).astype(np.float16)
    golden = np.matmul(matrix_a.astype(np.float32), matrix_b.astype(np.float32))

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    matrix_a.tofile("input/x1_gm.bin")
    matrix_b.transpose().copy().tofile("input/x2_gm.bin")
    golden.astype(np.float32).tofile("output/golden.bin")


if __name__ == "__main__":
    main()
