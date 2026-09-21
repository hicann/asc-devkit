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


class SparseMatmulGenData:
    def __init__(self, m, n, k, is_trans_a, is_trans_b, is_bias):
        self.m = m
        self.n = n
        self.k = k
        self.is_bias = is_bias
        self.is_trans_a = is_trans_a
        self.is_trans_b = is_trans_b

    @staticmethod
    def index_matrix_nd_to_nz(index_matrix):
        """convert index matrix from nd to nz, fractal size is (16, 8)

        Args:
            b_matrix: index matrix of nd format

        Returns:
            index matrix of nz format
        """
        n = index_matrix.shape[0]
        k = index_matrix.shape[1]

        ceil_n = int(np.ceil(n / 16) * 16)
        pad_n = int(ceil_n - n)
        ceil_k = int(np.ceil(k / 8) * 8)
        pad_k = int(ceil_k - k)

        index_matrix_nz = np.zeros((ceil_n, ceil_k), dtype=np.uint8)
        index_matrix_nz[:n, :k] = index_matrix

        new_shape = (ceil_n // 16, 16, ceil_k // 8, 8)
        index_matrix_nz = index_matrix_nz.reshape(new_shape)
        index_matrix_nz = index_matrix_nz.transpose(2, 0, 1, 3)
        return index_matrix_nz

    def gen_sparse_matrix_b(self):
        """generate sparse matrix b, there are two zero-value element at each row.

        Args: NA

        Returns:
            generated sparse matrix b.
        """
        dtype = np.int8
        b_matrix = np.zeros((self.n, self.k), dtype=dtype)
        for row in range(self.n):
            for i in range(0, self.k, 4):
                block = np.zeros(4, dtype=dtype)
                # random choose two position to put none-zero-value element
                none_zero_positions = np.random.choice(4, 2, replace=False)
                # generate random none-zero-value
                block[none_zero_positions[0]] = np.random.choice([i for i in range(-5, 6) if i != 0])
                block[none_zero_positions[1]] = np.random.choice([i for i in range(-5, 6) if i != 0])
                # put none-zero-value element to current row
                b_matrix[row, i : i + 4] = block
        return b_matrix

    def densify_and_gen_index(self, b_matrix):
        """densify sparse matrix b and generate index matrix

        Args:
            b_matrix: input sparse matrix b

        Returns:
            generated dense matrix b and related index matrix, index matrix mask
        """
        n, k = self.n, self.k
        b_matrix_dense = np.zeros((n, k // 2), dtype=b_matrix.dtype)
        index_matrix = np.zeros((n, k // 8), dtype=np.uint8)  # k / 2 / 4 -> k / 8
        index_matrix_mask = np.zeros((n, k // 2), dtype=np.uint32)  # index matrix mask, use for matrix A when mmad

        for row in range(n):
            dense_row = []
            index_row = []  # relative position
            index_mask_row = []  # absolute position
            for i in range(0, k, 4):
                block = b_matrix[row, i : i + 4]
                none_zero_positions = [j for j in range(4) if block[j] != 0]
                # record the index of first and second none_zero_value element
                # index_1: indicates the relative position of the first non-zero element among the first three elements.
                # index_2: indicates the relative position of the second non-zero element among the last three elements.
                if len(none_zero_positions) == 0:
                    index_1 = 0
                    index_2 = 0
                    index_mask_row.extend([i, i])
                elif len(none_zero_positions) == 1:
                    index_1 = none_zero_positions[0] if none_zero_positions[0] < 3 else 0  # the first three elements
                    index_2 = 0 if none_zero_positions[0] < 3 else 2  # the last three elements
                    index_mask_row.extend([none_zero_positions[0] + i, i])
                else:
                    index_1 = none_zero_positions[0]
                    index_2 = none_zero_positions[1] - 1
                    index_mask_row.extend([none_zero_positions[0] + i, none_zero_positions[1] + i])
                index_row.extend([index_1, index_2])
                dense_block = [block[pos] for pos in none_zero_positions[:2]]
                if len(dense_block) < 2:
                    dense_block += [0] * (2 - len(dense_block))
                dense_row.extend(dense_block)

            # reverse the index order and pack it as int8
            index_bytes = []
            for j in range(0, len(index_row), 4):
                indices = index_row[j : j + 4]
                int8_val = sum((index << (2 * k)) for k, index in enumerate(indices))
                index_bytes.append(int8_val)

            b_matrix_dense[row, :] = dense_row
            index_matrix[row, :] = index_bytes
            index_matrix_mask[row, :] = index_mask_row

        return b_matrix_dense, index_matrix, index_matrix_mask

    def gen_golden_data(self):
        src_type = np.int8
        dst_type = np.int32
        if self.is_trans_a:
            x1_shape = [self.k, self.m]
        else:
            x1_shape = [self.m, self.k]

        # generate input x1, x2
        x1_gm = np.random.randint(-5, 5, x1_shape).astype(src_type)
        x2_gm = self.gen_sparse_matrix_b().astype(src_type)  # sparse matrix b is transposed, [n, k]
        x2_gm_dense, index_matrix, index_matrix_mask = self.densify_and_gen_index(x2_gm)
        if self.is_bias:
            bias_gm = np.random.randint(-2, 2, [1, self.n]).astype(dst_type)

        # generate golden y
        y_gm_int32 = np.zeros((self.m, self.n), dtype=dst_type)
        for r in range(self.n):
            selected_columns = index_matrix_mask[r]
            selected_a_matrix = x1_gm[:, selected_columns]
            y_gm_int32[:, r] = np.dot(selected_a_matrix.astype(dst_type), x2_gm_dense[r].astype(dst_type)).astype(
                dst_type
            )
        index_matrix = SparseMatmulGenData.index_matrix_nd_to_nz(index_matrix)
        os.makedirs("input", exist_ok=True)
        os.makedirs("output", exist_ok=True)

        if self.is_bias:
            y_gm_int32 = (y_gm_int32 + bias_gm).astype(dst_type)
            bias_gm.tofile("./input/bias_gm.bin")
        x1_gm.tofile("./input/x1_gm.bin")
        x2_gm_dense.tofile("./input/x2_gm.bin")
        index_matrix.tofile("./input/index_gm.bin")
        y_gm_int32.tofile("./output/golden.bin")
        return 0


if __name__ == "__main__":
    gen_data = SparseMatmulGenData(m=128, n=7680, k=64, is_trans_a=False, is_trans_b=True, is_bias=False)
    gen_data.gen_golden_data()
