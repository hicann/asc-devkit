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


def softmax_flash_v3(x, height, width, cnt, inmax=None, insum=None, inmean=None, update=False, is_fp16=False):
    rowMeanLocal = np.zeros([height, 8], dtype=(np.float32))
    rowMeanGlobal = np.zeros([height, 8], dtype=(np.float32))
    tmp = np.zeros([height, 8], dtype=(np.float32))
    rowMeanGlobalTmp = np.zeros([height, 8], dtype=(np.float32))
    mean = np.zeros([height, 8], dtype=(np.float32))
    maxTmp = np.zeros([height, 8], dtype=(np.float32))
    maxTmp1 = np.zeros([height, 1], dtype=(np.float32))
    shiftCurr = np.zeros([height, 8], dtype=(np.float32))
    shiftPrev = np.zeros([height, 8], dtype=(np.float32))
    tmpbuffer0 = np.zeros([height, 64], dtype=(np.float32))
    tmpbuffer1 = np.zeros([height, width], dtype=(np.float32))
    alapha = 0.9375
    scalar = alapha / (1 - alapha)
    if is_fp16:
        x = x.astype(np.float32)

    baseK = int(width / 8)

    for i in range(height):
        for j in range(64):
            sum = 0.0
            for t in range(8):
                sum += x[i][t + j * 8]
            tmpbuffer0[i][j] = sum

    remain = int(width / 64 - 8)
    for i in range(height):
        for j in range(int(remain)):
            for k in range(64):
                tmpbuffer0[i][k] = tmpbuffer0[i][k] + x[i][512 + j * 64 + k]

    for i in range(height):
        for j in range(8):
            sum = 0.0
            for k in range(8):
                sum += tmpbuffer0[i][j * 8 + k]
            rowMeanLocal[i][j] = sum / baseK

    for i in range(height):
        sum = 0.0
        for j in range(8):
            sum += rowMeanLocal[(i, j)]
        for j in range(8):
            rowMeanGlobal[i][j] = sum / 8

    rowMeanGlobalTmp = (rowMeanGlobal - rowMeanLocal) * scalar
    for i in range(height):
        for j in range(int(width / baseK)):
            for k in range(baseK):
                tmpbuffer1[i][j * baseK + k] = rowMeanGlobalTmp[i][j]

    x = x - tmpbuffer1

    if update == False:
        x_mean = rowMeanGlobal
    else:
        x_mean = (rowMeanGlobal + inmean * (cnt - 1)) / cnt

    maxTmp1 = np.max(x, axis=(-1), keepdims=True)
    for i in range(height):
        for j in range(8):
            maxTmp[i][j] = maxTmp1[i][0]

    if update == False:
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        x_max = shiftCurr + maxTmp
    else:
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        shiftPrev = (inmean - x_mean) * scalar
        x_max = shiftCurr + maxTmp
        maxTmp = shiftPrev + inmax
        x_max = np.max(np.concatenate((x_max, maxTmp), axis=(-1)), axis=(-1), keepdims=True)

    if update == False:
        maxTmp = x_max - shiftCurr
        for i in range(height):
            for j in range(int(width / 8)):
                for k in range(8):
                    x[i][j * 8 + k] = x[i][j * 8 + k] - maxTmp[i][k]
        x_exp = np.exp(x)
        x_sum = np.sum(x_exp, axis=(-1), keepdims=True)
        exp_max = None
    else:
        exp_max = np.exp(inmax - x_max + shiftPrev)
        maxTmp = x_max - shiftCurr
        for i in range(height):
            for j in range(int(width / 8)):
                for k in range(8):
                    x[i][j * 8 + k] = x[i][j * 8 + k] - maxTmp[i][k]
        x_exp = np.exp(x)
        x_sum = np.sum(x_exp, axis=(-1), keepdims=True)
        x_sum_new = exp_max * insum + x_sum
    if is_fp16:
        x_exp_half = x_exp.astype(np.float16)
        return (x_exp_half, x_max, x_sum, x_mean, exp_max)


def gen_golden_data_simple():
    tiling_shape = [8]
    tiling_dtype = np.uint32
    height = 8
    width = 2048
    ori_height = 8
    ori_width = 2048
    loopcnt = 2
    split_mean_cnt = 8
    en_update = True
    src_dtype = np.float16
    src_shape = [8, 2048]
    dtype = np.float32
    tiling = np.zeros(tiling_shape).astype(tiling_dtype)
    tiling[0] = height
    tiling[1] = width
    tiling[2] = ori_height
    tiling[3] = ori_width
    tiling[4] = loopcnt
    tiling[5] = split_mean_cnt
    tiling[6] = en_update

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    tiling.tofile("./input/tiling.bin")

    loopcnt = tiling[4]
    split_mean_cnt = tiling[5]
    en_update = tiling[6]
    height = tiling[0]
    width = tiling[1]
    mkShape = [height, width]
    if src_dtype == np.float16:
        x1 = np.random.uniform(0, 10, mkShape).astype(src_dtype)
        x2 = np.random.uniform(0, 10, mkShape).astype(src_dtype)
        max_front = np.full([height, 8], 0.5, dtype)
        sum_front = np.full([height, 8], 0.5, dtype)
        mean_front = np.full([height, 8], 0.5, dtype)

        max_front.tofile("./input/inputMax.bin")
        sum_front.tofile("./input/inputSum.bin")
        mean_front.tofile("./input/inputMean.bin")

        if en_update == False:
            x1.tofile("./input/input.bin")
            out_1, max_1, sum_1, mean_1, exp_max_1 = softmax_flash_v3(
                x1, height, width, loopcnt, max_front, sum_front, mean_front, update=False, is_fp16=True
            )
            out_1.astype(np.float32).tofile("./output/golden.bin")
        else:
            x2.tofile("./input/input.bin")
            out_2, max_2, sum_2, mean_2, exp_max_2 = softmax_flash_v3(
                x2, height, width, loopcnt, max_front, sum_front, mean_front, update=True, is_fp16=True
            )
            out_2.astype(np.float32).tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
