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

dtype_size = 2  # b16数据类型
fractal_shape = [16, 32 // dtype_size]  # 分形形状
fractal_size = fractal_shape[0] * fractal_shape[1]  # 分形元素数量


def load2D(src, dst, repeatTimes, srcStride, dstGap, startIndex=0, addrMode=0, ifTranspose=False):
    addr_calc_mode = 1 if addrMode == 0 else -1
    for i in range(repeatTimes):
        src_tmp_addr = startIndex * fractal_size + addr_calc_mode * srcStride * fractal_size * i
        dst_tmp_addr = (dstGap + 1) * fractal_size * i
        for j in range(fractal_shape[0]):
            dst_block_addr = dst_tmp_addr + fractal_shape[1] * j
            if ifTranspose is False:
                src_block_addr = src_tmp_addr + fractal_shape[1] * j
                dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[
                    src_block_addr : src_block_addr + fractal_shape[1]
                ]
            else:
                src_block_addr = src_tmp_addr + j
                dst[dst_block_addr : dst_block_addr + fractal_shape[1]] = src[
                    src_block_addr : src_block_addr + fractal_size
                ][:: fractal_shape[1]]


if __name__ == "__main__":
    np.set_printoptions(threshold=np.inf)
    dtype = np.int16  # b16
    if dtype_size == 1:
        dtype = np.int8  # b8
    elif dtype_size == 4:
        dtype = np.int32  # b32
    src = np.arange(4096).astype(dtype)
    dst = np.zeros(4096).astype(dtype)
    load2D(src, dst, repeatTimes=1, srcStride=1, dstGap=0, startIndex=0, addrMode=0, ifTranspose=False)
    print("dst", dst)
