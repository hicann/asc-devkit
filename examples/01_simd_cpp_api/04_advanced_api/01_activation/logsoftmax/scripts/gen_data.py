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


def softmax_2d_py_float(x, inmax=None, insum=None, update=None, log=None):
    x.astype(np.float64)
    orig_shape = x.shape
    x_max = np.max(x, axis=-1)
    x_max = np.reshape(x_max, [orig_shape[0], 1])
    x_sub = x - x_max
    x_exp = np.exp(x_sub)
    x_exp1 = np.reshape(x_exp, [orig_shape[0], orig_shape[1]])
    x_sum = np.sum(x_exp1, axis=-1)
    x_sum = np.reshape(x_sum, [orig_shape[0], 1])
    x_div = x_exp / x_sum
    if log:
        x_div = np.log10(x_div)
    if update:
        x_max_new = np.max(np.concatenate((inmax, x_max), axis=-1), axis=-1).reshape([orig_shape[0], 1])
        x_exp_new = np.exp(x_max.reshape([orig_shape[0], 1]) - x_max_new.reshape([orig_shape[0], 1]))
        exp_max = np.exp(inmax - x_max_new)
        x_sum_new = exp_max * insum + x_exp_new * x_sum
        exp_max = exp_max * insum / x_sum_new
        softmax_ratio = x_sum * x_exp_new / x_sum_new
        out_new = x_div * softmax_ratio
        return out_new, x_max_new, x_sum_new, exp_max
    else:
        out = np.reshape(x_div, [orig_shape[0], orig_shape[1]])
        exp_max = None
        return out, x_max, x_sum, exp_max


def softmax_flash_v2(x, inmax=None, insum=None, update=False, is_fp16=False):
    orig_shape = x.shape
    if update == False:
        if is_fp16:
            x = x.astype(np.float32)
        x_max = np.max(x, axis=-1, keepdims=True)
        x_sub = x - x_max
        x_exp = np.exp(x_sub)
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        exp_max = None
        if is_fp16:
            x_exp = x_exp.astype(np.float16)
            x_max = x_max.astype(np.float16)
            x_sum = x_sum.astype(np.float16)
        return x_exp, x_max, x_sum, exp_max
    else:
        if is_fp16:
            x = x.astype(np.float32)
            inmax = inmax.astype(np.float32)
            insum = insum.astype(np.float32)
        x_max = np.max(np.concatenate((inmax, x), axis=-1), axis=-1, keepdims=True)
        x_exp = np.exp(x - x_max)
        exp_max = np.exp(inmax - x_max)
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        x_sum_new = exp_max * insum + x_sum
        if is_fp16:
            x_exp = x_exp.astype(np.float16)
            x_max = x_max.astype(np.float16)
            x_sum_new = x_sum_new.astype(np.float16)
            exp_max = exp_max.astype(np.float16)
        return x_exp, x_max, x_sum_new, exp_max


def gen_golden_data_simple():
    shapeinfo_dtype = np.uint32
    dtype = np.float32
    src_dtype = dtype
    height = 128
    width = 128
    shapeinfo = np.random.uniform(1, 1, [8]).astype(shapeinfo_dtype)
    shapeinfo[0] = height
    shapeinfo[1] = width
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    shapeinfo.tofile("./input/shapeInfo.bin")
    mkShape = [height, width]
    oriWidth = width
    oriHeight = height
    x1 = np.random.uniform(-10, 10, mkShape).astype(src_dtype)
    orimkShape = [oriHeight, oriWidth]
    orix1 = np.zeros(mkShape, dtype=src_dtype)
    for i in range(oriHeight):
        for j in range(oriWidth):
            orix1[i][j] = x1[i][j]
    orix2 = np.zeros(orimkShape, dtype=src_dtype)
    for i in range(oriHeight):
        for j in range(oriWidth):
            orix2[i][j] = x1[i][j]
    max_front = np.zeros([height, 8], dtype=src_dtype)
    sum_front = np.zeros([height, 8], dtype=src_dtype)
    out_1, max_1, sum_1, exp_max_1 = softmax_2d_py_float(orix2, max_front, sum_front, update=False, log=True)
    newout = np.zeros(mkShape, dtype=src_dtype)
    for i in range(oriHeight):
        for j in range(oriWidth):
            newout[i][j] = out_1[i][j]
    orix1.tofile("./input/input_src.bin")
    newout.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
