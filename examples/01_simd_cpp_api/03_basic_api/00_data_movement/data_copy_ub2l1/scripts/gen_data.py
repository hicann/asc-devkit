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
import argparse


def nd_to_nz(data_nd, c0size=16):
    """
    将ND格式的矩阵转换为NZ格式（分形间列主序，分形内行主序）
    与仓库中其他matmul样例使用相同的标准NZ转换。
    """
    rows, cols = data_nd.shape
    data_nz = data_nd.reshape((int(rows / 16), 16, int(cols / c0size), c0size)).transpose(2, 0, 1, 3)
    return data_nz


def gen_golden_data_simple(scenarioNum=1):
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    m = 32
    n = 32
    k = 32

    # 生成ND格式的数据，值域为[-1, 1]的均匀分布
    x1_nd = np.random.uniform(-1, 1, (m, k)).astype(np.float16)
    x2_nd = np.random.uniform(-1, 1, (k, n)).astype(np.float16)

    # 转换为NZ格式
    x1_nz = nd_to_nz(x1_nd)
    x2_nz = nd_to_nz(x2_nd)

    # 计算golden数据（使用ND格式计算）
    golden = (np.matmul(x1_nd.astype(np.float32), x2_nd.astype(np.float32))).astype(np.float32)

    if scenarioNum == 1:
        # 保存NZ格式的输入数据
        x1_nz.tofile("./input/input_x.bin")
        x2_nz.tofile("./input/input_y.bin")
    else:
        x1_nd.tofile("./input/input_x.bin")
        x2_nd.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenarioNum", type=int, default=1, choices=[1, 2])
    args = parser.parse_args()
    gen_golden_data_simple(args.scenarioNum)
