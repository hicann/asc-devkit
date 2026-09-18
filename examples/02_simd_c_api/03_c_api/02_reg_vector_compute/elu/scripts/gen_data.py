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


def gen_elu_golden(x, alpha, scale, input_scale):
    """计算 ELU golden：x > 0 时取 scale*x，否则取 alpha*scale*(exp(x*inputScale)-1)"""
    x_float = x.astype(np.float32)
    # np.where 会先计算两个分支，正数部分 exp 可能溢出为 inf，但不会被选中，用 errstate 屏蔽告警
    with np.errstate(over="ignore", invalid="ignore"):
        golden = np.where(
            x_float > 0,
            scale * x_float,
            alpha * scale * (np.exp(x_float * input_scale) - 1),
        )
    return golden


def gen_golden_data_simple():
    total_length = 256
    # 生成一个 [1, total_length] 向量，数据范围 [-100, 100]，覆盖正负区间
    x = np.random.uniform(-100, 100, [1, total_length]).astype(np.float32)
    alpha = 1.0
    scale = 1.0
    input_scale = 1.0
    golden = gen_elu_golden(x, alpha, scale, input_scale)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


def gen_golden_data_fp16():
    total_length = 256
    # fp16 输入：先按 float 生成，再转 fp16（验证 fp16 算子分支）
    x = np.random.uniform(-100, 100, [1, total_length]).astype(np.float16)
    alpha = 1.0
    scale = 1.0
    input_scale = 1.0
    golden = gen_elu_golden(x, alpha, scale, input_scale).astype(np.float16)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x_fp16.bin")
    golden.tofile("./output/golden_fp16.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
    gen_golden_data_fp16()
