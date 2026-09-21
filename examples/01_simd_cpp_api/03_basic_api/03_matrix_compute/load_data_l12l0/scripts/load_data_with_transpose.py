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


import numpy as np


class data_type_dict:
    b4 = "b4"
    b8 = "b8"
    b16 = "b16"
    b32 = "b32"


# 设置数据类型为b4
data_type = data_type_dict.b4
fractal_num = None
fractal_shape = None
square_shape = None

# 根据不同数据类型计算分形形状和方阵形状
if data_type == data_type_dict.b4:
    fractal_num = 4
    fractal_shape = [16, 64]
    square_shape = [64, 64]
elif data_type == data_type_dict.b8:
    fractal_num = 2
    fractal_shape = [16, 32]
    square_shape = [32, 32]
elif data_type == data_type_dict.b16:
    fractal_num = 1
    fractal_shape = [16, 16]
    square_shape = [16, 16]
else:
    fractal_num = 2
    fractal_shape = [16, 8]
    square_shape = [16, 16]

# 计算分形大小和方阵大小
fractal_size = fractal_shape[0] * fractal_shape[1]
square_size = fractal_size * fractal_num


# b4数据类型矩阵转置
def load_data_with_transpose_b4(src, dst, startIndex, repeatTimes, srcStride, dstGap, dstFracGap=0):
    src_addr = startIndex * square_size
    dst_addr = 0
    for i in range(repeatTimes):
        src_square_addr = src_addr + srcStride * i * square_size
        dst_square_addr = dst_addr + (dstGap + 1) * i * fractal_size
        for j in range(fractal_num):
            dst_fractal_addr = dst_square_addr + fractal_size * (dstFracGap + 1) * j
            for k in range(fractal_shape[0]):
                src_block_addr = src_square_addr + j * fractal_shape[0] + k  # 这里是方阵的block
                dst_block_addr = dst_fractal_addr + k * fractal_shape[1]
                dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[
                    src_block_addr : src_block_addr + square_size
                ][:: square_shape[0]]


# b8数据类型矩阵转置
def load_data_with_transpose_b8(src, dst, startIndex, repeatTimes, srcStride, dstGap, dstFracGap=0):
    src_addr = startIndex * square_size
    dst_addr = 0
    for i in range(repeatTimes):
        src_square_addr = src_addr + srcStride * i * square_size
        dst_square_addr = dst_addr + (dstGap + 1) * i * fractal_size
        for j in range(fractal_num):
            dst_fractal_addr = dst_square_addr + fractal_size * (dstFracGap + 1) * j
            for k in range(fractal_shape[0]):
                src_block_addr = src_square_addr + j * fractal_shape[0] + k  # 这里是方阵的block
                dst_block_addr = dst_fractal_addr + k * fractal_shape[1]
                dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[
                    src_block_addr : src_block_addr + square_size
                ][:: square_shape[0]]


# b16数据类型矩阵转置
def load_data_with_transpose_b16(src, dst, startIndex, repeatTimes, srcStride, dstGap, dstFracGap=0):
    src_addr = startIndex * square_size
    dst_addr = 0
    for i in range(repeatTimes):
        src_square_addr = src_addr + srcStride * i
        dst_square_addr = dst_addr + (dstGap + 1) * i * fractal_size
        for j in range(fractal_num):
            dst_fractal_addr = dst_square_addr + fractal_size * (dstFracGap + 1) * j
            for k in range(fractal_shape[0]):
                src_block_addr = src_square_addr + j * fractal_shape[0] + k  # 这里是方阵的block
                dst_block_addr = dst_fractal_addr + k * fractal_shape[1]
                dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[
                    src_block_addr : src_block_addr + square_size
                ][:: square_shape[0]]


# b32数据类型矩阵转置
def load_data_with_transpose_b32(src, dst, startIndex, repeatTimes, srcStride, dstGap, dstFracGap=0):
    src_addr = startIndex * square_size
    dst_addr = 0
    offset = fractal_shape[1] * fractal_shape[1]
    for i in range(repeatTimes):
        src_square_addr = src_addr + srcStride * i * square_size
        dst_square_addr = dst_addr + (dstGap + 1) * i * fractal_size
        # 搬运左z分形的上半部分
        for k in range(fractal_shape[1]):
            src_block_addr = src_square_addr + k
            dst_block_addr = dst_square_addr + fractal_shape[1] * k
            dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[src_block_addr : src_block_addr + offset][
                :: fractal_shape[1]
            ]
        # 搬运左z分形的下半部分
        for k in range(fractal_shape[1]):
            src_block_addr = src_square_addr + k + offset
            dst_block_addr = dst_square_addr + (dstFracGap + 1) * fractal_size + fractal_shape[1] * k
            dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[src_block_addr : src_block_addr + offset][
                :: fractal_shape[1]
            ]
        # 搬运右z分形的上半部分
        for k in range(fractal_shape[1]):
            src_block_addr = src_square_addr + fractal_size + k
            dst_block_addr = dst_square_addr + fractal_shape[1] * k + offset
            dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[src_block_addr : src_block_addr + offset][
                :: fractal_shape[1]
            ]
        # 搬运右z分形的下半部分
        for k in range(fractal_shape[1]):
            src_block_addr = src_square_addr + fractal_size + k + offset
            dst_block_addr = dst_square_addr + (dstFracGap + 1) * fractal_size + fractal_shape[1] * k + offset
            dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[src_block_addr : src_block_addr + offset][
                :: fractal_shape[1]
            ]


if __name__ == "__main__":
    np.set_printoptions(threshold=np.inf)
    src = np.arange(10000).astype(np.int32)  # 代表内存单元编号
    dst = np.zeros(10000).astype(np.int32)  # 代表内存单元编号

    if data_type == data_type_dict.b4:
        load_data_with_transpose_b4(src, dst, startIndex=0, repeatTimes=1, srcStride=0, dstGap=0, dstFracGap=0)
    if data_type == data_type_dict.b8:
        load_data_with_transpose_b8(src, dst, startIndex=0, repeatTimes=1, srcStride=0, dstGap=0, dstFracGap=1)
    elif data_type == data_type_dict.b16:
        load_data_with_transpose_b16(src, dst, startIndex=0, repeatTimes=1, srcStride=0, dstGap=0)
    elif data_type == data_type_dict.b32:
        load_data_with_transpose_b32(src, dst, startIndex=0, repeatTimes=2, srcStride=1, dstGap=2, dstFracGap=0)

    print("dst", dst)
