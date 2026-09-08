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

import numpy as np
from scipy.special import ndtri


def gen_golden_data_simple(size=2048, include_special=True):
    dtype = np.float32

    rng = np.random.default_rng(20260715)
    input_x = rng.uniform(0.0, 1.0, size).astype(dtype)
    min_probability = np.nextafter(dtype(0.0), dtype(1.0))
    max_probability = np.nextafter(dtype(1.0), dtype(0.0))
    input_x = np.clip(input_x, min_probability, max_probability)

    center_left = dtype(0.075)
    center_right = dtype(0.925)
    far_tail_boundary = dtype(np.exp(-25.0))
    min_normal = dtype(np.finfo(dtype).tiny)
    max_subnormal = np.nextafter(min_normal, dtype(0.0))
    min_subnormal = np.nextafter(dtype(0.0), dtype(1.0))

    fixed_values = np.array(
        [
            0.0,
            1.0,
            0.5,
            min_subnormal,
            max_subnormal,
            min_normal,
            np.nextafter(dtype(1.0), dtype(0.0)),
            1e-30,
            1e-20,
            1e-10,
            np.nextafter(center_left, dtype(0.0)),
            center_left,
            np.nextafter(center_left, dtype(1.0)),
            np.nextafter(center_right, dtype(0.0)),
            center_right,
            np.nextafter(center_right, dtype(1.0)),
            np.nextafter(far_tail_boundary, dtype(0.0)),
            far_tail_boundary,
            np.nextafter(far_tail_boundary, dtype(1.0)),
            1e-2,
            1.0 - 1e-2,
            1e-5,
            1.0 - 1e-5,
            -0.1,
            1.1,
            -np.inf,
            np.inf,
            np.nan,
        ],
        dtype=dtype,
    )
    fixed_count = min(size, fixed_values.size) if include_special else 0
    if fixed_count > 0:
        input_x[:fixed_count] = fixed_values[:fixed_count]

    tail_count = min(max(size // 8, 0), max(size - fixed_count, 0))
    if tail_count > 0:
        left_count = (tail_count + 1) // 2
        left_tail = np.logspace(-45, -2, left_count, dtype=dtype)
        input_x[fixed_count : fixed_count + left_count] = left_tail
        right_count = tail_count - left_count
        if right_count > 0:
            right_delta = np.logspace(-7, -2, right_count, dtype=dtype)
            input_x[fixed_count + left_count : fixed_count + tail_count] = (
                dtype(1.0) - right_delta
            )

    golden = ndtri(input_x).astype(dtype)

    os.makedirs("./input", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    os.makedirs("./output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    size = int(sys.argv[1]) if len(sys.argv) > 1 else 2048
    test_mode = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    gen_golden_data_simple(size, include_special=(test_mode == 1))
