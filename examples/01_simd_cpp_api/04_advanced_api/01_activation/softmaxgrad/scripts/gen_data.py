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
import argparse
import numpy as np


def softmax_grad_float(grad, src, isFront=None):
    muls_r = grad * src
    muls_r = muls_r.sum(axis=-1, keepdims=True)
    if isFront:
        return muls_r
    sub_r = grad - muls_r
    res = sub_r * src
    return res


def gen_golden_data_simple(front_mode=False):
    x_shape = (960, 960)
    workspace_shape = (1024,)
    x = np.random.uniform(-1, 1, x_shape).astype(np.float32)
    y = np.random.uniform(-1, 1, x_shape).astype(np.float32)
    workspace = np.random.uniform(0, 0, workspace_shape).astype(np.uint32)

    if front_mode:
        # Front 模式：输出 shape 为 [m, 8]，每行 8 个元素值相同
        golden_front = softmax_grad_float(x, y, isFront=True)
        golden = np.broadcast_to(golden_front, (x_shape[0], 8)).copy()
    else:
        golden = softmax_grad_float(x, y, isFront=False)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")
    workspace.tofile("./input/workspace.bin")

    if front_mode:
        print(f"Generated data for SoftmaxGradFront mode: input {x_shape}, output {golden.shape}")
    else:
        print(f"Generated data for SoftmaxGrad mode: input {x_shape}, output {golden.shape}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate test data for SoftmaxGrad/SoftmaxGradFront")
    parser.add_argument("--front-mode", action="store_true", help="Enable SoftmaxGradFront mode")
    args = parser.parse_args()
    gen_golden_data_simple(front_mode=args.front_mode)
