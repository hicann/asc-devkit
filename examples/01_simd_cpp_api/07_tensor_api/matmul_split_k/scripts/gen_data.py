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

from pathlib import Path

import numpy as np


M = 16
K = 1024
N = 16
SEED = 2026


def generate_data() -> None:
    rng = np.random.default_rng(SEED)
    matrix_a = rng.uniform(-1.0, 1.0, size=(M, K)).astype(np.float16)
    matrix_b = rng.uniform(-1.0, 1.0, size=(K, N)).astype(np.float16)
    golden = np.matmul(matrix_a.astype(np.float32), matrix_b.astype(np.float32))

    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)
    matrix_a.tofile(input_dir / "x1_gm.bin")
    matrix_b.tofile(input_dir / "x2_gm.bin")
    golden.astype(np.float32).tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    generate_data()
