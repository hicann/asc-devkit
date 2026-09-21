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
import argparse
import numpy as np

np.random.seed(9)


def gen_golden_data(scenarioNum=1):
    """
    根据场景编号生成输入数据和Golden数据
    场景1：输入[1, 20]，输出[1, 32]，使用SetPadValue填充
    场景2：输入[32, 59]，输出[32, 64]，rightPadding，不使用SetPadValue
    场景3：输入[3, 24]，输出[1, 80]，Compact模式
    场景4：输入[1, 320]，输出[1, 576]，SetLoopModePara loop mode（Compact模式）
    场景5：输入[1, 320]，输出[1, 576]，SetLoopModePara loop mode（Normal模式）
    场景6：输入[2, 4, 3, 128, 126]五维，搬运[2, 2, 2, 64, 126]，UB连续存放[512, 128]
    """

    if scenarioNum == 1:
        src_rows = 1
        src_cols = 20
        dst_cols = 32
        data_type = np.float16
    elif scenarioNum == 2:
        src_rows = 32
        src_cols = 59
        dst_cols = 64
        data_type = np.float32
    elif scenarioNum == 3:
        src_rows = 3
        src_cols = 24
        dst_cols = 80
        data_type = np.float16
    elif scenarioNum == 4:
        src_rows = 1
        src_cols = 320
        dst_cols = 576  # Compact模式：320B数据 + 64B填充 + 192B随机值
        data_type = np.int8
    elif scenarioNum == 5:
        src_rows = 1
        src_cols = 320
        dst_cols = 576  # Normal模式：320B数据 + 192B填充 + 32B外层循环间隔 + 32B未使用
        data_type = np.int8
    elif scenarioNum == 6:
        # 场景6：GM五维 [2, 4, 3, 128, 126]int8
        src_rows = 2 * 4 * 3 * 128 * 126
        src_cols = 1  # 五维数据不适用src_cols概念，设置为1
        dst_cols = 512 * 128  # UB连续存放 [512, 128]int8
        data_type = np.int8

    if scenarioNum == 6:
        # 场景6：五维数据 [2, 4, 3, 128, 126]
        input_x = np.random.uniform(-10, 10, [2, 4, 3, 128, 126]).astype(data_type)
    else:
        input_x = np.random.uniform(-10, 10, [src_rows, src_cols]).astype(data_type)

    if scenarioNum == 1 or scenarioNum == 2:
        golden = np.zeros([src_rows, dst_cols], dtype=data_type)
        for i in range(src_rows):
            for j in range(src_cols):
                golden[i, j] = input_x[i, j]

        if scenarioNum == 1:
            for i in range(src_rows):
                for j in range(src_cols, dst_cols):
                    golden[i, j] = 1

    elif scenarioNum == 3:
        golden = np.zeros([dst_cols], dtype=data_type)
        input_flatten = input_x.flatten()
        golden[:72] = input_flatten
        golden = golden.reshape(1, 80)

    elif scenarioNum == 4:
        # 场景4：Compact模式，每次内层循环搬运80B后填充16B使其96B对齐
        # 先清零UB buffer，确保间隔区域值为0，padding值设为-1
        golden = np.zeros(dst_cols, dtype=data_type)
        input_flatten = input_x.flatten()

        # 外层循环：每次移动 288
        for outer in range(0, dst_cols, 288):
            # 内层循环：每次搬运 80 个数据
            for inner, data_start in enumerate(range(0, 160, 80)):
                dst_start = outer + inner * 128
                dst_data_end = dst_start + 80
                dst_fill_end = dst_data_end + 16

                data_src_start = outer // 288 * 160 + inner * 80
                data_src_end = data_src_start + 80

                golden[dst_start:dst_data_end] = input_flatten[data_src_start:data_src_end]
                golden[dst_data_end:dst_fill_end] = -1
        golden = golden.reshape(1, dst_cols)
    elif scenarioNum == 5:
        # 场景5：Normal模式，每个block搬运40B后填充24B使其64B对齐
        # 参数：LOOP1_SIZE=2, LOOP2_SIZE=2, BLOCK_COUNT=2, BLOCK_LEN=40
        # LOOP1_SRC_STRIDE=80, LOOP1_DST_STRIDE=128, LOOP2_SRC_STRIDE=160, LOOP2_DST_STRIDE=288
        golden = np.zeros(dst_cols, dtype=data_type)
        input_flatten = input_x.flatten()

        # 外层循环：LOOP2_SIZE=2，每次移动LOOP2_DST_STRIDE=288
        for outer_idx in range(2):
            dst_base = outer_idx * 288
            src_base = outer_idx * 160

            # 内层循环：LOOP1_SIZE=2，每次移动LOOP1_DST_STRIDE=128
            for inner_idx in range(2):
                dst_offset = inner_idx * 128
                src_offset = inner_idx * 80

                # 搬运BLOCK_COUNT=2个block，每个block占64B（40B数据+24B填充）
                for block_idx in range(2):
                    block_dst_start = dst_base + dst_offset + block_idx * 64
                    block_src_start = src_base + src_offset + block_idx * 40

                    golden[block_dst_start : block_dst_start + 40] = input_flatten[
                        block_src_start : block_src_start + 40
                    ]
                    golden[block_dst_start + 40 : block_dst_start + 64] = -1

        golden = golden.reshape(1, dst_cols)

    elif scenarioNum == 6:
        # 场景6：五维数据搬运 [2, 4, 3, 128, 126]int8 -> [512, 128]int8
        # 搬运 [2, 2, 2, 64, 126]，补2个0 -> [2, 2, 2, 64, 128]
        # Normal mode，loop mode

        # 输入数据 reshape 为五维
        input_5d = input_x.reshape(2, 4, 3, 128, 126)

        # Golden 数据 [512, 128]
        golden = np.zeros([512, 128], dtype=data_type)

        # 搬运逻辑：
        # 第0维：搬运第0和第1个（for循环2次）
        # 第1维：搬运第0和第1个（for循环2次）
        # 第2维：搬运第0和第1个（loop1循环2次）
        # 第3维：搬运第0到第63个（blockCount=64），从128中选64个
        # 第4维：搬运全部126个，补2个0

        golden_row_idx = 0
        for dim0 in range(2):  # 第0维，搬运2次
            for dim1 in range(2):  # 第1维，搬运2次
                for dim2 in range(2):  # 第2维，搬运2次（loop1）
                    for dim3 in range(64):  # 第3维，搬运64次（从128中选择前64个）
                        # 从第3维的128个中选择第dim3个，搬运第4维的126个数据
                        golden[golden_row_idx, :126] = input_5d[dim0, dim1, dim2, dim3, :]
                        # 补2个0
                        golden[golden_row_idx, 126:128] = 0
                        golden_row_idx += 1

        golden = golden.reshape(1, dst_cols)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2, 3, 4, 5, 6])
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
