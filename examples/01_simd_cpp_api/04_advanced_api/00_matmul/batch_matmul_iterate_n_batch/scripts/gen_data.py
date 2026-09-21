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
import numpy as np


def gen_golden_data():
    m, n, k, b = 32, 256, 64, 3
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    src_type, dst_type = np.float16, np.float32

    a_layout_info = {"b": 1, "s": m, "n": 1, "g": b, "d": k}
    b_layout_info = {"b": 1, "s": n, "n": 1, "g": b, "d": k}
    c_layout_info = {"b": 1, "n": 1, "g": 1, "d": n}

    x1_gm = np.random.uniform(-1, 1, [a_layout_info["s"] * a_layout_info["g"], a_layout_info["d"]]).astype(src_type)
    x2_gm = np.random.uniform(-1, 1, [b_layout_info["d"], b_layout_info["g"] * b_layout_info["s"]]).astype(src_type)
    input_bias = np.random.uniform(-1, 1, [1, n]).astype(dst_type)

    a_shape = [a_layout_info["b"], a_layout_info["s"], a_layout_info["n"], a_layout_info["g"], a_layout_info["d"]]
    b_shape = [b_layout_info["b"], b_layout_info["s"], b_layout_info["n"], b_layout_info["g"], b_layout_info["d"]]
    bias_shape = [c_layout_info["b"], 1, c_layout_info["n"], c_layout_info["g"], c_layout_info["d"]]

    a = x1_gm.astype(dst_type).reshape(a_shape)
    b = x2_gm.astype(dst_type).reshape(b_shape)
    input_bias = input_bias.astype(dst_type).reshape(bias_shape)

    a_t = np.transpose(a, axes=(0, 2, 3, 1, 4))
    b_t = np.transpose(b, axes=(0, 2, 3, 4, 1))
    input_bias = np.transpose(input_bias, axes=(0, 2, 3, 1, 4))

    a_broadcast_shape = [
        max(a_layout_info["b"], b_layout_info["b"]),
        max(a_layout_info["n"], b_layout_info["n"]),
        max(a_layout_info["g"], b_layout_info["g"]),
        a_layout_info["s"],
        a_layout_info["d"],
    ]
    b_broadcast_shape = [
        max(a_layout_info["b"], b_layout_info["b"]),
        max(a_layout_info["n"], b_layout_info["n"]),
        max(a_layout_info["g"], b_layout_info["g"]),
        b_layout_info["d"],
        b_layout_info["s"],
    ]
    a_broadcast = np.broadcast_to(a_t, a_broadcast_shape)
    b_broadcast = np.broadcast_to(b_t, b_broadcast_shape)

    golden = np.matmul(a_broadcast, b_broadcast).astype(dst_type)
    golden = golden + input_bias
    golden = np.transpose(golden, axes=(0, 3, 1, 2, 4))

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    input_bias.tofile("./input/bias_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
