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


import numpy as np
import os


def densify_and_generate_index(B):
    """稠密化稀疏矩阵B,并生成索引矩阵"""
    N, K = B.shape
    dense_B = np.zeros((N, K // 2), dtype=B.dtype)  # 稠密化后的矩阵
    index_matrix = np.zeros((N, K // 2), dtype=np.uint8)  # 索引矩阵
    index_mask_matrix = np.zeros((N, K // 2), dtype=np.uint32)  # index mask矩阵

    for row in range(N):
        dense_row = []
        index_row = []
        index_mask_row = []

        for i in range(0, K, 4):
            block = B[row, i : i + 4]
            nonzero_positions = [j for j in range(4) if block[j] != 0]

            # 记录第1和第2个非零元素的索引
            if len(nonzero_positions) == 0:
                index_1 = 0
                index_2 = 0
                index_mask_row.extend([i, i])
            elif len(nonzero_positions) == 1:
                index_1 = nonzero_positions[0] if nonzero_positions[0] < 3 else 0
                index_2 = 0 if nonzero_positions[0] < 3 else 2
                index_mask_row.extend([nonzero_positions[0] + i, i])
            else:
                index_1 = nonzero_positions[0]
                index_2 = nonzero_positions[1] - 1
                index_mask_row.extend([nonzero_positions[0] + i, nonzero_positions[1] + i])

            # 记录稠密化后的块
            dense_block = [block[pos] for pos in nonzero_positions[:2]]
            if len(dense_block) < 2:
                dense_block += [0] * (2 - len(dense_block))
            dense_row.extend(dense_block)

            # 记录索引
            index_row.extend([index_1, index_2])
        dense_B[row, :] = dense_row
        index_matrix[row, :] = index_row
        index_mask_matrix[row, :] = index_mask_row

    return dense_B, index_matrix, index_mask_matrix


def construct_sparse_matrix_B(shape):
    """生成一个指定形状的稀疏矩阵B, 每行的每4个元素块至少包含2个零"""
    N, K = shape
    B = np.zeros((N, K), dtype=np.int8)  # 初始化矩阵B为全零

    for row in range(N):
        for i in range(0, K, 4):
            block = np.zeros(4, dtype=np.int8)
            # 随机选择2个位置放置非零元素
            non_zero_positions = np.random.choice(4, 2, replace=False)
            block[non_zero_positions[0]] = np.random.randint(1, 10, dtype=np.int8)
            block[non_zero_positions[1]] = np.random.randint(1, 10, dtype=np.int8)
            # 放置到矩阵B的当前行
            B[row, i : i + 4] = block
    return B


def gen_sparse_golden(A, dense_B, index_mask_matrix):
    result_type = np.int32
    M = A.shape[0]
    N = dense_B.shape[0]
    C = np.zeros((M, N), dtype=result_type)
    # 遍历 b 和 index 的每一行
    for r in range(N):
        # 从 a 中根据 index 的第 r 行提取数据
        selected_columns = index_mask_matrix[r]  # 第 r 行的索引
        a_selected = A[:, selected_columns]  # 提取对应列

        # 当前 b 第 r 行与提取后的 a_selected 计算矩阵乘法
        C[:, r] = np.dot(a_selected.astype(result_type), dense_B[r].astype(result_type)).astype(result_type)
    return C


def gen_uint2_zn_idx(index_matrix):
    # K1 = K // 2
    N, K1 = index_matrix.shape
    K2 = K1 // 4
    index = np.zeros((N, K2), dtype=np.uint8)
    for row in range(N):
        index_bytes = []
        index_row = index_matrix[row]
        # 4个uint2 拼成一个unint8
        for j in range(0, len(index_row), 4):
            indices = index_row[j : j + 4]
            uint8_value = sum((index << (2 * k)) for k, index in enumerate(indices))
            index_bytes.append(uint8_value)

        index[row, :] = index_bytes

    # nd->nz 等价 dn->zn
    ceil_N = int(np.ceil(N / 16) * 16)
    pad_N = int(ceil_N - N)
    ceil_K = int(np.ceil(K2 / 8) * 8)
    pad_K = int(ceil_K - K2)
    index_matrix_nz = np.zeros((ceil_N, ceil_K), dtype=np.uint8)
    index_matrix_nz[:N, :K2] = index

    nz_shape = (ceil_N // 16, 16, ceil_K // 8, 8)
    index_matrix_nz = index_matrix_nz.reshape(nz_shape)
    index_matrix_nz = index_matrix_nz.transpose(2, 0, 1, 3)
    return index_matrix_nz


def gen_golden_data():
    M = 128
    N = 128
    K = 64

    A_gm = np.random.randint(1, 10, [M, K]).astype(np.int8)
    # 构造稀疏B矩阵, 确保每4个元素中至少包含2个零
    B_gm = construct_sparse_matrix_B((N, K)).astype(np.int8)
    # 4选2稠密化B矩阵, 并生成对应的uint8类型的索引矩阵与sparse golden生成中A矩阵稠密化需要的矩阵index_mask_matrix
    dense_B, index_matrix, index_mask_matrix = densify_and_generate_index(B_gm)
    # 生成稀疏矩阵乘golden
    golden = gen_sparse_golden(A_gm, dense_B, index_mask_matrix)
    # 将uint8类型的索引矩阵转换成uint2类型的索引矩阵，并将其转换成Zn
    idx_gm = gen_uint2_zn_idx(index_matrix)

    c0Size = 32
    x1_gm = A_gm.reshape((int(M / 16), 16, int(K / c0Size), c0Size)).transpose(2, 0, 1, 3).astype(np.int8)
    x2_gm = dense_B.reshape((int(N / 16), 16, int(K / 2 / c0Size), c0Size)).transpose(2, 0, 1, 3).astype(np.int8)

    os.system("mkdir -p input")
    os.system("mkdir -p output")
    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    idx_gm.tofile("./input/idx_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
