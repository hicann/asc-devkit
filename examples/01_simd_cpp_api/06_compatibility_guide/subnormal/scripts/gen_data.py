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


def gen_golden_data_simple():
    count = 1024
    dtype = np.float16
    # 构造包含Subnormal half数据的输入：
    # Subnormal half的指数位全0且尾数非0，取值范围为(0, 2^-14)即(0, 6.10e-5)
    # 0x0001~0x000A均为正Subnormal half
    sub_bits = np.arange(1, 11, dtype=np.uint16)
    sub_vals = sub_bits.view(dtype)
    # 正常正数（大于2^-14，非Subnormal）
    normal_vals = np.array([2.0, 0.5, 0.25, 0.125, 0.0625], dtype=dtype)

    input_x = np.empty(count, dtype=dtype)
    # 前半部分填入Subnormal值，后半部分填入正常值
    for i in range(count):
        if i < count // 2:
            input_x[i] = sub_vals[i % len(sub_vals)]
        else:
            input_x[i] = normal_vals[i % len(normal_vals)]

    # 真值：以高精度(float32)计算自然对数后转回half，保留Subnormal输入的真实对数结果
    golden = np.log(input_x.astype(np.float32)).astype(dtype)
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data_simple()
