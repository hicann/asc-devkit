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


MATMUL_M = 16
MATMUL_K = 32
MATMUL_N = 16
HIFLOAT8_ONE = 0x08
FIXPIPE_QUANT_META_BIT = 1 << 46
FIXPIPE_SCALE_MASK = 0xFFFFE000


def float_to_bfloat16(values):
    bits = values.astype(np.float32).view(np.uint32)
    rounded = bits + np.uint32(0x7FFF) + ((bits >> np.uint32(16)) & np.uint32(1))
    return (rounded >> np.uint32(16)).astype(np.uint16)


def gen_data():
    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)

    a = np.full((MATMUL_M, MATMUL_K), HIFLOAT8_ONE, dtype=np.uint8)
    b = np.full((MATMUL_K, MATMUL_N), HIFLOAT8_ONE, dtype=np.uint8)
    bias = np.arange(MATMUL_N, dtype=np.float32)
    scale = np.where(np.arange(MATMUL_N) % 2 == 0, 1.0, 0.5).astype(np.float32)
    scale_bits = scale.view(np.uint32).astype(np.uint64)
    quant = np.uint64(FIXPIPE_QUANT_META_BIT) | (scale_bits & np.uint64(FIXPIPE_SCALE_MASK))

    # The raw HiFloat8 value 0x08 represents 1.0 for both input matrices.
    a_value = np.ones((MATMUL_M, MATMUL_K), dtype=np.float32)
    b_value = np.ones((MATMUL_K, MATMUL_N), dtype=np.float32)
    golden_float = (np.matmul(a_value, b_value) + bias) * scale
    golden = float_to_bfloat16(golden_float)

    a.tofile(input_dir / "a_gm.bin")
    b.tofile(input_dir / "b_gm.bin")
    bias.tofile(input_dir / "bias_gm.bin")
    quant.tofile(input_dir / "scale_gm.bin")
    golden.tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    gen_data()
