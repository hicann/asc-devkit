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
import numpy as np

np.random.seed(9)


def gen_golden_data(scenarioNum=1):
    m = 40
    n = 50
    k = 70
    if scenarioNum == 1:
        # 输入数据类型为int4_t, A矩阵输入为[m,k], B矩阵输入为[n,k]
        x1_gm = np.random.uniform(-8, 7, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(-8, 7, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32))).astype(np.int32)
        x2_gm = x2_gm.transpose()
    elif scenarioNum == 2:
        # 输入数据类型为int4_t, A矩阵输入为[m,k], B矩阵输入为[k,n]
        x1_gm = np.random.uniform(-8, 7, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(-8, 7, [k, n]).astype(np.int8)
        # x1_gm = np.random.randint(1, 2, [m, k]).astype(np.int8)
        # x2_gm = np.random.randint(2, 3, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.int32), x2_gm.astype(np.int32))).astype(np.int32)
    elif scenarioNum == 3 or scenarioNum == 4:
        # 输入数据类型为int8_t, A矩阵输入为[m,k], B矩阵输入为[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.int32)
        x2_gm = x2_gm.transpose()
    elif scenarioNum == 5:
        # 输入数据类型为int8_t, A矩阵输入为[k,m], B矩阵输入为[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.int8)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.int8)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.int32)
        x1_gm = x1_gm.transpose()
    elif scenarioNum == 6 or scenarioNum == 7:
        # 输入数据类型为half, A矩阵输入为[m,k], B矩阵输入为[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
        x2_gm = x2_gm.transpose()
    elif scenarioNum == 8 or scenarioNum == 9 or scenarioNum == 10:
        # 输入数据类型为half, A矩阵输入为[k,m], B矩阵输入为[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float16)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float16)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
        x1_gm = x1_gm.transpose()
    elif scenarioNum == 11 or scenarioNum == 12:
        # 输入数据类型为float, A矩阵输入为[m,k], B矩阵输入为[n,k]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
        x2_gm = x2_gm.transpose()
    elif scenarioNum == 13 or scenarioNum == 14:
        # 输入数据类型为float, A矩阵输入为[k,m], B矩阵输入为[k,n]
        x1_gm = np.random.uniform(1, 10, [m, k]).astype(np.float32)
        x2_gm = np.random.uniform(1, 10, [k, n]).astype(np.float32)
        golden = (np.matmul(x1_gm.astype(np.float32), x2_gm.astype(np.float32))).astype(np.float32)
        x1_gm = x1_gm.transpose()
    if scenarioNum <= 5:
        golden = golden.astype(np.int32)
    else:
        golden = golden.astype(np.float32)

    print("x1_gm == ", x1_gm)
    print("x2_gm == ", x2_gm)

    os.system("mkdir -p input")
    os.system("mkdir -p output")

    # 构造输入bin文件，当输入数据类型为int4时，需要特殊处理
    if scenarioNum <= 2:
        x1_gm_int4 = np.zeros(shape=[x1_gm.shape[0], x1_gm.shape[1] // 2]).astype(np.int8)
        for i in range(x1_gm.shape[0]):
            for j in range(x1_gm.shape[1]):
                if j % 2 == 0:
                    x1_gm_int4[i][j // 2] = (x1_gm[i][j + 1] << 4) + (x1_gm[i][j] & 0x0F)
        x2_gm_int4 = np.zeros(shape=[x2_gm.shape[0], x2_gm.shape[1] // 2]).astype(np.int8)
        for i in range(x2_gm.shape[0]):
            for j in range(x2_gm.shape[1]):
                if j % 2 == 0:
                    x2_gm_int4[i][j // 2] = (x2_gm[i][j + 1] << 4) + (x2_gm[i][j] & 0x0F)
        # print("x1_gm_int4 shape:", x1_gm_int4.shape)
        # print("x1_gm_int4[0][:5]:", x1_gm_int4[0][:5])

        # print("x1_gm_int4 == ", x1_gm_int4)
        # print("x2_gm_int4 == ", x2_gm_int4)

        x1_gm_int4.tofile("./input/x1_gm.bin")
        x2_gm_int4.tofile("./input/x2_gm.bin")
    else:
        x1_gm.tofile("./input/x1_gm.bin")
        x2_gm.tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=range(1, 15))
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
