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
import numpy as np
import tensorflow as tf


bfloat16 = tf.bfloat16.as_numpy_dtype
dtype_emu = {bfloat16: 0, np.float16: 1, np.float32: 2, np.int8: 3, np.int16: 4, np.int32: 5}


def gen_golden_data_simple():
    dtype = np.int8
    input_shape_x = [17, 1022]
    input_shape_y = [1, 1022]
    input_x = np.random.uniform(-50, 50, input_shape_x).astype(dtype)
    input_y = np.random.uniform(-50, 50, input_shape_y).astype(dtype)
    golden = (input_x + input_y).astype(dtype)

    if np.size(input_x) > np.size(input_y):
        if input_shape_y[0] == 1:
            axis = 0
            coef = np.size(input_y)
        elif input_shape_y[1] == 1:
            axis = 1
            coef = np.size(input_x) / np.size(input_y)
    else:
        if input_shape_x[0] == 1:
            axis = 0
            coef = np.size(input_x)
        elif input_shape_x[1] == 1:
            axis = 1
            coef = np.size(input_y) / np.size(input_x)
    tiling = np.array(
        [input_shape_x[0] * input_shape_x[1], input_shape_y[0] * input_shape_y[1], coef, axis, dtype_emu[dtype]],
        dtype=np.uint32,
    )

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    tiling.tofile("./input/input_tiling.bin")
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
