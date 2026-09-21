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

import argparse
import os
import struct

import numpy as np


M = 128
K = 128
N = 256
QUANT_TRANSFER_BYTES = (N * np.dtype(np.uint64).itemsize + 127) // 128 * 128


def extract_relu_alpha(relu_pre):
    relu_alpha_bits = (int(relu_pre) >> 13) & 0xFFFFF
    sign_bit = (relu_alpha_bits >> 18) & 0x1
    exponent = (relu_alpha_bits >> 10) & 0xFF
    mantissa = relu_alpha_bits & 0x3FF
    return (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - 127))


def extract_quant_params(quant_pre):
    quant_pre = int(quant_pre)
    quant_alpha_bits = (quant_pre >> 13) & 0xFFFFF
    mode_control_bit = (quant_pre >> 36) & 0x1
    offset = (quant_pre >> 37) & 0x1FF
    signed_output = (quant_pre >> 46) & 0x1
    n_shift = (quant_pre >> 32) & 0xF
    sign_bit = (quant_alpha_bits >> 18) & 0x1
    exponent = (quant_alpha_bits >> 10) & 0xFF
    mantissa = quant_alpha_bits & 0x3FF
    quant_alpha = (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - 127))
    return quant_alpha, offset, signed_output, n_shift, mode_control_bit


def saturate_and_round(value, minimum, maximum, dtype):
    return np.round(np.clip(value, minimum, maximum)).astype(dtype)


def quantize_f32_to_i8(value, quant_pre, relu_pre):
    quant_alpha, offset, signed_output, n_shift, mode_control_bit = extract_quant_params(quant_pre)
    if mode_control_bit:
        value = saturate_and_round(value / (2**n_shift), -32768, 32767, np.int16)
    value = np.float32(value)
    value *= quant_alpha if value >= 0 else extract_relu_alpha(relu_pre)
    quantized = saturate_and_round(value, -256, 255, np.int16) + offset
    if signed_output:
        return saturate_and_round(quantized, -128, 127, np.int8)
    return saturate_and_round(quantized, 0, 255, np.uint8)


def vector_quantize_to_i8(data):
    quant_scalar = np.float32(2.0)
    relu_pre = struct.unpack("!I", struct.pack("!f", quant_scalar))[0]
    quant_pre = np.full(N, quant_scalar, dtype=np.float32).view(np.uint32).astype(np.uint64)
    # Bit 46 selects signed int8 output in the Fixpipe vector-quant parameter format.
    quant_pre |= np.uint64(0x400000000000)

    quantized = np.empty((M, N), dtype=np.int8)
    for row in range(M):
        for column in range(N):
            quantized[row, column] = quantize_f32_to_i8(data[row, column], quant_pre[column], relu_pre)
    return quantized, quant_pre


def gen_golden_data(scenario_num=1):
    np.random.seed(9)
    x1_gm = np.random.uniform(-1, 3, [M, K]).astype(np.float16)
    x2_gm = np.random.uniform(-1, 3, [K, N]).astype(np.float16)
    golden = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if scenario_num == 1:
        # Store both inputs in Nz before they are copied to L1.
        x1_gm = x1_gm.reshape(M // 16, 16, K // 16, 16).transpose(2, 0, 1, 3)
        x2_gm = x2_gm.reshape(K // 16, 16, N // 16, 16).transpose(2, 0, 1, 3)
    elif scenario_num == 3:
        # DN input is the transpose of the corresponding ND matrix.
        x1_gm = x1_gm.transpose(1, 0)
        x2_gm = x2_gm.transpose(1, 0)
    elif scenario_num == 4:
        golden, quant_pre = vector_quantize_to_i8(golden)
        quant_pre_padded = np.zeros(QUANT_TRANSFER_BYTES // np.dtype(np.uint64).itemsize, dtype=np.uint64)
        quant_pre_padded[:N] = quant_pre
        quant_pre_padded.tofile("./input/quant_pre.bin")

    x1_gm.astype(np.float16).tofile("./input/x1_gm.bin")
    x2_gm.astype(np.float16).tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 5))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
