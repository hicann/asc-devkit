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
import sys
import numpy as np


def gen_golden_data_simple(npu_arch):
    np.random.seed(10)

    N = 128
    K = 8
    offsetLength = 1
    groupSize = K // offsetLength

    src = np.random.uniform(-10, 10, [8, 128]).astype(np.int8)
    offset = np.random.uniform(-10, 10, [1, 128]).astype(np.float16)
    scale = np.random.uniform(-10, 10, [1, 128]).astype(np.float16)

    if npu_arch == "dav-2201":
        dst = np.zeros([8, 128]).astype(np.float32)
        for i in range(0, K):
            for j in range(0, N):
                dst[i][j] = scale[i // groupSize][j] * (src[i][j] + offset[i // groupSize][j])
        dst = dst.astype(np.float16)
    elif npu_arch == "dav-3510":
        src_fp32 = src.astype(np.float32)
        offset_fp32 = offset.astype(np.float32)
        scale_fp32 = scale.astype(np.float32)
        dst = np.zeros([8, 128]).astype(np.float32)
        for i in range(0, K):
            for j in range(0, N):
                dst[i][j] = scale_fp32[i // groupSize][j] * (src_fp32[i][j] + offset_fp32[i // groupSize][j])
        dst = dst.astype(np.float16)
    else:
        raise ValueError(f"Unsupported NPU architecture: {npu_arch}")

    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_src.bin")
    offset.tofile("./input/input_offset.bin")
    scale.tofile("./input/input_scale.bin")
    os.makedirs("output", exist_ok=True)
    dst.tofile("./output/golden.bin")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("用法: python3 gen_data.py -DCMAKE_ASC_ARCHITECTURES=dav-2201")
        sys.exit(1)

    arg = sys.argv[1]
    if arg.startswith("-DCMAKE_ASC_ARCHITECTURES="):
        npu_arch = arg.split("=")[1]
    else:
        npu_arch = arg

    gen_golden_data_simple(npu_arch)
