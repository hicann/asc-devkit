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

import os
import numpy as np


FLOAT32_MIN = np.finfo(np.float32).min


def softmax_py_float(x):
    """Compute the softmax function for each channel of the input x."""
    orig_shape = x.shape
    x_max = np.max(x, axis=-1)
    x_max = np.reshape(x_max, [orig_shape[0], 1])

    x_sub = x - x_max
    x_exp = np.exp(x_sub)

    x_exp1 = np.reshape(x_exp, [orig_shape[0], orig_shape[1]])
    x_sum = np.sum(x_exp1, axis=-1)
    x_sum = np.reshape(x_sum, [orig_shape[0], 1])
    x_div = x_exp / x_sum
    out = np.reshape(x_div, [orig_shape[0], orig_shape[1]])
    return out, x_max, x_sum


def adjust_softmax_res(res, max_val):
    """Adjust softmax results based on max values."""
    matched_rows = max_val[:, 0] == FLOAT32_MIN
    res[matched_rows, :] = np.float32(0.0)
    return res


def gen_golden_data():
    """Generate golden data for SoftMax + AdjustSoftMaxRes."""
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x_shape = (32, 32)
    workspace_shape = (1024,)
    x = np.random.uniform(-1, 1, x_shape).astype(np.float32)
    x[0, :] = FLOAT32_MIN
    workspace = np.zeros(workspace_shape, dtype=np.uint32)

    softmax_out, max_val, sum_val = softmax_py_float(x)
    softmax_out = adjust_softmax_res(softmax_out, max_val)

    x.tofile("./input/input_x.bin")
    workspace.tofile("./input/workspace.bin")
    softmax_out.tofile("./output/golden.bin")

    print(f"Generated data: input_x {x.shape}, golden {softmax_out.shape}")


if __name__ == "__main__":
    gen_golden_data()
