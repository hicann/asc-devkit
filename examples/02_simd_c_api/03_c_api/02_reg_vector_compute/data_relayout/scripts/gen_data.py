#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import os
import argparse
import numpy as np


def gen_golden_data_interleave():
    """场景1：Interleave - 将两个uint16向量交织"""
    total_length = 128
    data_type = np.uint16
    # 生成两个 [1, 128] 的 uint16 向量
    x = np.random.uniform(0, 65535, [1, total_length]).astype(data_type)
    y = np.random.uniform(0, 65535, [1, total_length]).astype(data_type)
    x_flat = x.flatten()
    y_flat = y.flatten()
    # Interleave 将VL(128)分成高低两半(各64)分别交织：
    # dst0 = 低半交织：[src0[0], src1[0], src0[1], src1[1], ..., src0[63], src1[63]]
    # dst1 = 高半交织：[src0[64], src1[64], src0[65], src1[65], ..., src0[127], src1[127]]
    dst0 = np.zeros(total_length, dtype=data_type)
    dst1 = np.zeros(total_length, dtype=data_type)
    dst0[0::2] = x_flat[:64]
    dst0[1::2] = y_flat[:64]
    dst1[0::2] = x_flat[64:]
    dst1[1::2] = y_flat[64:]
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    y.tofile("./input/input_y.bin")
    dst0.tofile("./output/golden_dst0.bin")
    dst1.tofile("./output/golden_dst1.bin")


def gen_golden_data_pack():
    """场景2：Pack - 从uint32_t向量中提取低16位"""
    total_length = 128
    data_type_32 = np.uint32
    data_type_16 = np.uint16
    # 生成1个 [1, 128] 的 uint32 向量
    x = np.random.uniform(0, 0xFFFFFFFF, [1, total_length]).astype(data_type_32)
    # Pack LOWEST：提取每个 uint32_t 的低16位
    golden = (x & 0xFFFF).astype(data_type_16).flatten()
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate golden data for data_relayout sample"
    )
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        help="Scenario number: 1=Interleave, 2=Pack",
    )
    args = parser.parse_args()
    if args.scenarioNum == 1:
        gen_golden_data_interleave()
    else:
        gen_golden_data_pack()
