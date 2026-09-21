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


def gen_layernorm_data():
    # 统一的参数设置
    B = 2
    S = 32
    H = 16
    eps = 0.0001
    seed = 42
    dtype = np.float32
    np.random.seed(seed)

    # 1. 生成输入数据
    # inputX: [B, S, H]
    inputX = np.random.uniform(-100, 100, [B, S, H]).astype(dtype)

    # gamma: [H]
    gamma = np.random.uniform(-100, 100, [H]).astype(dtype)

    # beta: [H]
    beta = np.random.uniform(-100, 100, [H]).astype(dtype)

    # dy: 上游梯度 [B, S, H]
    dy = np.random.uniform(-100, 100, [B, S, H]).astype(dtype)

    # 2. LayerNorm前向计算
    reduce_axis = 2
    mean = np.mean(inputX, axis=reduce_axis, keepdims=True)  # [B, S, 1]
    variance = np.mean(np.power((inputX - mean), 2), axis=reduce_axis, keepdims=True)  # [B, S, 1]

    # 归一化
    x_minus_mean = inputX - mean
    tmp2 = 1.0 / np.sqrt(variance + eps)
    res_for_gamma = x_minus_mean * tmp2
    y = gamma * res_for_gamma + beta

    # 3. LayerNormGrad反向计算
    pd_xl = dy * gamma
    reciprocal = 1.0 / H

    # 计算pd_var
    pd_var = np.sum(
        np.multiply(np.multiply(np.multiply(-0.5, pd_xl), x_minus_mean), np.power(np.add(variance, eps), -1.5)),
        axis=reduce_axis,
        keepdims=True,
    )

    # 计算pd_mean
    pd_mean = np.add(
        np.sum(np.multiply(np.multiply(-1.0, pd_xl), tmp2), axis=reduce_axis, keepdims=True),
        np.multiply(
            np.multiply(pd_var, reciprocal), np.sum(np.multiply(-2.0, x_minus_mean), axis=reduce_axis, keepdims=True)
        ),
    )

    # 计算pd_x
    pd_x = np.add(
        np.add(np.multiply(pd_mean, reciprocal), np.multiply(np.multiply(pd_var, 2.0 * reciprocal), x_minus_mean)),
        np.multiply(pd_xl, tmp2),
    )

    # 4. LayerNormGradBeta参数梯度计算
    pd_gamma = np.sum(dy * res_for_gamma, axis=(0, 1), keepdims=False)
    pd_beta = np.sum(dy, axis=(0, 1), keepdims=False)

    # 5. 保存输入文件
    os.makedirs("input", exist_ok=True)
    inputX.tofile("./input/input_inputX.bin")
    gamma.tofile("./input/input_gamma.bin")
    beta.tofile("./input/input_beta.bin")
    dy.tofile("./input/input_dy.bin")

    os.makedirs("output", exist_ok=True)

    y.tofile("./output/golden_Y.bin")
    mean.squeeze(-1).tofile("./output/golden_Mean.bin")
    variance.squeeze(-1).tofile("./output/golden_Var.bin")
    pd_x.tofile("./output/golden_PdX.bin")
    pd_gamma.tofile("./output/golden_PdGamma.bin")
    pd_beta.tofile("./output/golden_PdBeta.bin")


if __name__ == "__main__":
    gen_layernorm_data()
