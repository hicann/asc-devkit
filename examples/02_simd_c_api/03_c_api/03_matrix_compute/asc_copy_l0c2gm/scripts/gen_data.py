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
import argparse
import copy
import struct
import numpy as np

np.random.seed(9)


def extract_relu_params(relu_pre):
    relu_pre = int(relu_pre)
    relu_alpha_bits = (
        relu_pre >> 13
    ) & 0xFFFFF  # 提取M2的20位[31:13]，0xFFFFF是20位掩码
    sign_bit = (relu_alpha_bits >> 18) & 0x1
    exponent = (relu_alpha_bits >> 10) & 0xFF
    mantissa = relu_alpha_bits & 0x3FF
    exponent_bias = 127  # 假设指数偏倚量为127，与float32一致
    relu_alpha = (
        (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - exponent_bias))
    )
    return relu_alpha


def extract_quant_params(quant_pre):
    """
    从uint64类型的quant_gm中提取M1、offset、sign参数
    param:
        quant_pre:uint64类型的整数
    return:
        quant_alpha:自定义格式(1,8,10)的浮点数
        offset:9位整数
        sign:1位布尔值(0或1)
    """
    quant_pre = int(quant_pre)
    quant_alpha_bits = (
        quant_pre >> 13
    ) & 0xFFFFF  # 提取M1的20位[31:13]，0xFFFFF是20位掩码
    mode_control_bit = (
        quant_pre >> 36
    ) & 0x1  # 提取mode_ctrl bit的一位[36]，0x1是1位掩码
    offset = (quant_pre >> 37) & 0x1FF  # 提取offset的9位[45:37]，0x1FF是9位掩码
    sign = (quant_pre >> 46) & 0x1  # 提取sign的一位[46]，0x1是1位掩码
    n = (quant_pre >> 32) & 0xF
    # 解析M1为(1,8,10)格式的浮点数
    sign_bit = (quant_alpha_bits >> 18) & 0x1
    exponent = (quant_alpha_bits >> 10) & 0xFF
    mantissa = quant_alpha_bits & 0x3FF
    exponent_bias = 127  # 假设指数偏倚量为127，与float32一致
    quant_alpha = (
        (-1) ** sign_bit * (1 + mantissa / 1024) * (2 ** (exponent - exponent_bias))
    )
    return quant_alpha, offset, sign, n, mode_control_bit


def saturation(value, min_val, max_val, target_type):
    """
    将输入的浮点数进行饱和处理，并转换为目标类型
    """
    x_clamped = np.clip(value, min_val, max_val)
    return np.round(x_clamped).astype(target_type)


def qf322b8_pre(data, quant_pre, relu_pre):
    """
    float32 -> int8/uint8
    """
    quant_alpha, offset, sign, n_shift, mode_control_bit = extract_quant_params(
        quant_pre
    )
    # sign = 0
    relu_alpha = extract_relu_params(relu_pre)
    if mode_control_bit == 1:
        data = data / (2**n_shift)
        data = saturation(data, -32768, 32767, np.int16)
    data = data.astype(np.float32)
    if data >= 0:
        data = data * quant_alpha
    else:
        data = data * relu_alpha
    quant_data = saturation(data, -256, 255, np.int16) + offset
    if sign:
        return saturation(quant_data, -128, 127, np.int8)
    else:
        return saturation(quant_data, 0, 255, np.uint8)


def pre_quant_relu(golden, dst_type, m, n, pre_quant_mode, pre_relu_mode):
    quant_scalar = 2
    norm_relu_alpha = 0

    relu_alpha = 1
    if pre_relu_mode == 0:
        relu_alpha = quant_scalar
    elif pre_relu_mode == 1:
        relu_alpha = norm_relu_alpha
    relu_alpha = struct.unpack("!I", struct.pack("!f", relu_alpha))[0]

    # 1 * n 量化系数为全为quant scalar的量化tensor
    temp_quant_tensor = (
        (quant_scalar * np.ones((1, n), dtype=np.float32)).astype(np.float32)
    )[0]
    temp_quant_tensor_api = copy.deepcopy(temp_quant_tensor).astype(np.uint64)
    for i, _ in enumerate(temp_quant_tensor_api):
        temp_quant_tensor_api[i] = struct.unpack(
            "!I", struct.pack("!f", temp_quant_tensor[i])
        )[0]
        if dst_type == np.int8:
            # np.int8 sign is true
            temp_quant_tensor_api[i] = temp_quant_tensor_api[i] | np.uint64(
                0x400000000000
            )
    quant_tensor = np.frombuffer(temp_quant_tensor_api, np.uint64)
    quant_tensor = quant_tensor.astype(np.uint64)
    if pre_quant_mode == 2:
        quant_tensor.tofile("./input/quant_pre.bin")
    quant_golden = np.zeros((m, n), dtype=np.int8)
    for i in range(m):
        for j in range(n):
            quant_golden[i, j] = qf322b8_pre(golden[i, j], quant_tensor[j], relu_alpha)
    return quant_golden


def gen_golden_data(scenarioNum=1):
    M = 128
    K = 128
    N = 256
    kRound = 2  # K轴切分2次

    input_type = np.dtype("float16")
    output_type = np.dtype("float32")
    x1_gm = np.random.uniform(-1, 3, [M, K]).astype(input_type)
    x2_gm = np.random.uniform(-1, 3, [K, N]).astype(input_type)
    golden = np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32)).astype(
        np.float32
    )
    # print(golden)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if scenarioNum == 1:
        block_cols = 16
        golden = (
            golden.reshape((int(M / 16), 16, int(N / block_cols), block_cols))
            .transpose(2, 0, 1, 3)
            .astype(output_type)
        )
    elif scenarioNum == 2:
        pass
    elif scenarioNum == 3:
        golden = golden.transpose(1, 0)
    elif scenarioNum == 4:
        output_type = np.dtype("int8")
        golden = pre_quant_relu(golden, output_type, M, N, 1, 0)  # scalar
    elif scenarioNum == 5:
        output_type = np.dtype("int8")
        golden = pre_quant_relu(golden, output_type, M, N, 2, 0)  # vector
    elif scenarioNum == 6:
        golden = np.maximum(golden, 0).astype(output_type)
    elif scenarioNum == 7:
        block_cols = 8
        golden = (
            golden.reshape((int(M / 16), 16, int(N / block_cols), block_cols))
            .transpose(2, 0, 1, 3)
            .astype(output_type)
        )

    if kRound > 1:
        # 将K轴外移
        x1_gm = x1_gm.reshape(M, kRound, K // kRound).transpose(1, 0, 2)
    x1_gm.astype(input_type).tofile("./input/x1_gm.bin")
    x2_gm.astype(input_type).tofile("./input/x2_gm.bin")
    golden.astype(output_type).tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 8))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
