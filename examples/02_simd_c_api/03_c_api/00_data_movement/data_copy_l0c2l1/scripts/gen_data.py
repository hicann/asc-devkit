#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
K_ROUND = 2
VECTOR_QUANT_SCENARIOS = (2, 4, 6)
NZ_OUTPUT_SCENARIOS = (2, 3, 6)
RELU_SCENARIOS = (1, 4, 6)


def decode_quant_pre(quant_pre):
    quant_pre = int(quant_pre)
    alpha_bits = (quant_pre >> 13) & 0xFFFFF
    sign_bit = (alpha_bits >> 18) & 0x1
    exponent = (alpha_bits >> 10) & 0xFF
    mantissa = alpha_bits & 0x3FF
    alpha = (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - 127))
    offset = (quant_pre >> 37) & 0x1FF
    sign = (quant_pre >> 46) & 0x1
    shift = (quant_pre >> 32) & 0xF
    mode_control = (quant_pre >> 36) & 0x1
    return alpha, offset, sign, shift, mode_control


def decode_relu_pre(relu_pre):
    alpha_bits = (int(relu_pre) >> 13) & 0xFFFFF
    sign_bit = (alpha_bits >> 18) & 0x1
    exponent = (alpha_bits >> 10) & 0xFF
    mantissa = alpha_bits & 0x3FF
    return (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - 127))


def saturate(value, min_value, max_value, dtype):
    return np.round(np.clip(value, min_value, max_value)).astype(dtype)


def snap_near_half_integer(value, epsilon=1e-5):
    absolute_value = abs(value)
    fractional_part = absolute_value - np.floor(absolute_value)
    if 0.5 - epsilon < fractional_part < 0.5:
        return np.sign(value) * (np.floor(absolute_value) + np.float32(0.5))
    return value


def apply_deqf16(value, quant_pre, relu_pre):
    alpha, offset, _, shift, mode_control = decode_quant_pre(quant_pre)
    if mode_control:
        value = saturate(
            value >> shift,
            np.finfo(np.float16).min,
            np.finfo(np.float16).max,
            np.float16,
        )
    value = np.float32(value)
    value *= alpha if value >= 0 else decode_relu_pre(relu_pre)
    value = (
        saturate(
            value,
            np.finfo(np.float16).min,
            np.finfo(np.float16).max,
            np.float16,
        )
        + offset
    )
    return saturate(
        value,
        np.finfo(np.float16).min,
        np.finfo(np.float16).max,
        np.float16,
    )


def apply_qf322b8(value, quant_pre, relu_pre):
    alpha, offset, sign, shift, mode_control = decode_quant_pre(quant_pre)
    if mode_control:
        value = saturate(value / (2**shift), -32768, 32767, np.int16)
    value = np.float32(value)
    value *= alpha if value >= 0 else decode_relu_pre(relu_pre)
    value = snap_near_half_integer(value)
    value = saturate(value, -256, 255, np.int16) + offset
    return (
        saturate(value, -128, 127, np.int8)
        if sign
        else saturate(value, 0, 255, np.uint8)
    )


def apply_req8(value, quant_pre, relu_pre):
    alpha, offset, sign, shift, mode_control = decode_quant_pre(quant_pre)
    if mode_control:
        value = saturate(value >> shift, -32768, 32767, np.int16)
    value = np.float32(value)
    value *= alpha if value >= 0 else decode_relu_pre(relu_pre)
    value = snap_near_half_integer(value)
    value = saturate(value, -256, 255, np.int16) + offset
    return (
        saturate(value, -128, 127, np.int8)
        if sign
        else saturate(value, 0, 255, np.uint8)
    )


def make_quant_params():
    alpha_bits = struct.unpack("!I", struct.pack("!f", 2.0))[0]
    return np.full(N, np.uint64(alpha_bits | (1 << 46)), dtype=np.uint64)


def apply_prequant(matrix, scenario_num, quant_params):
    relu_alpha = 0.0 if scenario_num in RELU_SCENARIOS else 2.0
    relu_pre = struct.unpack("!I", struct.pack("!f", relu_alpha))[0]
    result_dtype = np.float16 if scenario_num in (1, 2) else np.int8
    result = np.zeros((M, N), dtype=result_dtype)
    for row in range(M):
        for column in range(N):
            if scenario_num in (1, 2):
                result[row, column] = apply_deqf16(
                    matrix[row, column], quant_params[column], relu_pre
                )
            elif scenario_num in (3, 4):
                result[row, column] = apply_qf322b8(
                    matrix[row, column], quant_params[column], relu_pre
                )
            else:
                result[row, column] = apply_req8(
                    matrix[row, column], quant_params[column], relu_pre
                )
    return result


def gen_golden_data(scenario_num):
    np.random.seed(9)
    if scenario_num in (1, 2, 5, 6):
        input_dtype = np.int8
        mm_dtype = np.int32
        output_dtype = np.float16 if scenario_num in (1, 2) else np.int8
        x1 = np.random.randint(-3, 3, (M, K)).astype(input_dtype)
        x2 = np.random.randint(-3, 3, (K, N)).astype(input_dtype)
    else:
        input_dtype = np.float16
        mm_dtype = np.float32
        output_dtype = np.int8
        x1 = np.random.uniform(-3.0, 3.0, (M, K)).astype(input_dtype)
        x2 = np.random.uniform(-3.0, 3.0, (K, N)).astype(input_dtype)

    golden = np.matmul(x1.astype(mm_dtype), x2.astype(mm_dtype)).astype(mm_dtype)
    quant_params = make_quant_params()
    golden = apply_prequant(golden, scenario_num, quant_params)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    if scenario_num in VECTOR_QUANT_SCENARIOS:
        quant_params.tofile("./input/quant_pre.bin")

    if scenario_num in NZ_OUTPUT_SCENARIOS:
        block_columns = 16 if output_dtype == np.float16 else 32
        golden = golden.reshape(
            M // 16, 16, N // block_columns, block_columns
        ).transpose(2, 0, 1, 3)

    # The kernel consumes A as two consecutive [M, K / K_ROUND] chunks.
    x1.reshape(M, K_ROUND, K // K_ROUND).transpose(1, 0, 2).astype(input_dtype).tofile(
        "./input/x1_gm.bin"
    )
    x2.astype(input_dtype).tofile("./input/x2_gm.bin")
    golden.astype(output_dtype).tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 7))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
