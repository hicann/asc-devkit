#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import os
import logging

import numpy as np
import tensorflow as tf

IS_OUTPUT_TXT = False


class MatmulGenData:
    def __init__(
        self,
        m,
        n,
        k,
        b,
        is_trans_a,
        is_trans_b,
        is_bias,
        data_type_str,
        a_format="ND",
        b_format="ND",
        c_format="ND",
        is_channel_split=False,
    ):
        self.m = m
        self.n = n
        self.k = k
        self.b = b
        self.is_trans_a = is_trans_a
        self.is_trans_b = is_trans_b
        self.is_bias = is_bias
        self.data_type_str = data_type_str
        self.a_format = a_format
        self.b_format = b_format
        self.c_format = c_format
        self.is_channel_split = is_channel_split

    @staticmethod
    def due_overflow(data):
        data = np.maximum(data, -65504)
        data = np.minimum(data, 65504)
        return data

    @staticmethod
    def nd_to_nz(matrix, shape, data_type, c0size):
        matrix = (
            matrix.reshape((int(shape[1] / 16), 16, int(shape[2] / c0size), c0size))
            .transpose(2, 0, 1, 3)
            .astype(data_type)
        )
        return matrix

    def tf_matmul(self, x1_gm_fp32, x2_gm_fp32, bias_gm_fp32=None):
        tf.compat.v1.disable_eager_execution()
        x1 = tf.compat.v1.placeholder(np.float32, shape=x1_gm_fp32.shape)
        x2 = tf.compat.v1.placeholder(np.float32, shape=x2_gm_fp32.shape)
        res_tf = tf.matmul(x1, x2, transpose_a=self.is_trans_a, transpose_b=self.is_trans_b)
        if self.is_bias:
            bias = tf.compat.v1.placeholder(np.float32, shape=bias_gm_fp32.shape)
            res_tf = tf.add(res_tf, bias)

        with tf.compat.v1.Session() as sess:
            feed_dict = {x1: x1_gm_fp32, x2: x2_gm_fp32}
            if self.is_bias:
                feed_dict[bias] = bias_gm_fp32
            res_tf = sess.run(res_tf, feed_dict=feed_dict)
        y_gm_fp32 = MatmulGenData.due_overflow(res_tf)
        return y_gm_fp32

    def gen_c_data_nz_format(self, y_gm_fp32, dst_type, c0size):
        nz_fractal_m = 16
        nz_fractal_n = 16
        if self.is_channel_split:
            nz_fractal_n = 8
            c0size = 8
        align_m = int(int((self.m + nz_fractal_m - 1) / nz_fractal_m) * nz_fractal_m)
        align_n = int(int((self.n + nz_fractal_n - 1) / nz_fractal_n) * nz_fractal_n)
        y_gm_pad = np.zeros([align_m, align_n])
        y_gm_pad[0 : self.m, 0 : self.n] = y_gm_fp32
        y_gm = y_gm_pad.astype(dst_type)
        y_shape = [self.b, align_m, align_n]
        y_gm = MatmulGenData.nd_to_nz(y_gm, y_shape, dst_type, c0size)
        return y_gm

    def savebinfile(self, work_dir, x1_gm, x2_gm, y_gm, bias_gm=None):
        x1_gm.tofile(work_dir + "/input/x1_gm.bin")
        x2_gm.tofile(work_dir + "/input/x2_gm.bin")
        y_gm.tofile(work_dir + "/output/golden.bin")
        if self.is_bias:
            bias_gm.tofile(work_dir + "/input/bias_gm.bin")

    def savetxtfile(self, work_dir, x1_gm_fp32, x2_gm_fp32, y_gm_fp32, bias_gm_fp32=None):
        if IS_OUTPUT_TXT:
            np.savetxt(work_dir + "/input/x1_gm.txt", x1_gm_fp32.flatten(), fmt="%f", newline="\n")
            np.savetxt(work_dir + "/input/x2_gm.txt", x2_gm_fp32.flatten(), fmt="%f", newline="\n")
            np.savetxt(work_dir + "/output/golden.txt", y_gm_fp32.astype(np.float32).flatten(), fmt="%f", newline="\n")
            if self.is_bias:
                np.savetxt(work_dir + "/input/bias_gm.txt", bias_gm_fp32.flatten(), fmt="%f", newline="\n")

    def gen_golden_data_fp16(self, work_dir, dst_type=np.float32):
        src_type = np.float16
        c0size = 16
        if self.is_trans_a:
            x1_shape = [self.b, self.k, self.m]
        else:
            x1_shape = [self.b, self.m, self.k]
        if self.is_trans_b:
            x2_shape = [self.b, self.n, self.k]
        else:
            x2_shape = [self.b, self.k, self.n]
        x1_gm = np.random.uniform(-1, 1, x1_shape).astype(src_type)
        x1_gm_fp32 = x1_gm.astype(np.float32)
        x2_gm = np.random.uniform(-1, 1, x2_shape).astype(src_type)
        x2_gm_fp32 = x2_gm.astype(np.float32)
        if self.is_bias:
            bias_gm = np.random.uniform(-1, 1, [1, self.n]).astype(dst_type)
            bias_gm_fp32 = bias_gm.astype(np.float32)

        if self.is_bias:
            y_gm_fp32 = self.tf_matmul(x1_gm_fp32, x2_gm_fp32, bias_gm_fp32)
        else:
            y_gm_fp32 = self.tf_matmul(x1_gm_fp32, x2_gm_fp32)

        if self.a_format == "NZ":
            x1_gm = MatmulGenData.nd_to_nz(x1_gm, x1_shape, src_type, c0size)
        if self.b_format == "NZ":
            x2_gm = MatmulGenData.nd_to_nz(x2_gm, x2_shape, src_type, c0size)
        if self.c_format == "NZ":
            y_gm = self.gen_c_data_nz_format(y_gm_fp32, dst_type, c0size)
        else:
            y_gm = y_gm_fp32.astype(dst_type)

        if self.is_bias:
            self.savebinfile(work_dir, x1_gm, x2_gm, y_gm, bias_gm)
            self.savetxtfile(work_dir, x1_gm_fp32, x2_gm_fp32, y_gm_fp32, bias_gm_fp32)
        else:
            self.savebinfile(work_dir, x1_gm, x2_gm, y_gm)
            self.savetxtfile(work_dir, x1_gm_fp32, x2_gm_fp32, y_gm_fp32)
        return 0

    def gen_golden_data(self, work_dir):
        if self.data_type_str == "float16_float32":
            self.gen_golden_data_fp16(work_dir)
        elif self.data_type_str == "float16_float16":
            self.gen_golden_data_fp16(work_dir, np.float16)
        else:
            logging.info("[ERROR] can't support data type %s" % (self.data_type_str))
            return -1
        return 0

    def gen_fake_golden_data(self, work_dir):
        data_type_bytes_ab = 2  # float16
        data_type_bytes_c = 4  # float32

        file_byte = self.b * self.m * self.k * data_type_bytes_ab
        with open(work_dir + "/input/x1_gm.bin", "wb") as file:
            file.truncate(file_byte)

        file_byte = self.b * self.k * self.n * data_type_bytes_ab
        with open(work_dir + "/input/x2_gm.bin", "wb") as file:
            file.truncate(file_byte)

        if self.is_bias:
            file_byte = 1 * self.n * data_type_bytes_c
            with open(work_dir + "/input/bias_gm.bin", "wb") as file:
                file.truncate(file_byte)


if __name__ == "__main__":
    matmul_gen_data = MatmulGenData(64, 256, 384, 1, False, False, False, "float16_float32")
    os.makedirs("./input", exist_ok=True)
    os.makedirs("./output", exist_ok=True)
    matmul_gen_data.gen_golden_data(".")
