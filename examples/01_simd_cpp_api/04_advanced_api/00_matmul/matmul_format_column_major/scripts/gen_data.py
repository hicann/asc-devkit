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
import tensorflow as tf


is_bias = True
is_trans_a = False
is_trans_b = False
a_format = "COLUMN_MAJOR"
b_format = "COLUMN_MAJOR"
c_format = "COLUMN_MAJOR"


def due_overflow(data):
    data = np.maximum(data, -65504)
    data = np.minimum(data, 65504)
    return data


def tf_matmul(x1_gm_fp32, x2_gm_fp32, bias_gm_fp32=None):
    tf.compat.v1.disable_eager_execution()
    x1 = tf.compat.v1.placeholder(np.float32, shape=x1_gm_fp32.shape)
    x2 = tf.compat.v1.placeholder(np.float32, shape=x2_gm_fp32.shape)
    res_tf = tf.matmul(x1, x2, transpose_a=is_trans_a, transpose_b=is_trans_b)
    if is_bias:
        bias = tf.compat.v1.placeholder(np.float32, shape=bias_gm_fp32.shape)
        res_tf = tf.add(res_tf, bias)

    with tf.compat.v1.Session() as sess:
        feed_dict = {x1: x1_gm_fp32, x2: x2_gm_fp32}
        if is_bias:
            feed_dict[bias] = bias_gm_fp32
        res_tf = sess.run(res_tf, feed_dict=feed_dict)
    y_gm_fp32 = due_overflow(res_tf)
    return y_gm_fp32


def gen_golden_data(dst_type=np.float32):
    m = 428
    n = 479
    k = 528
    src_type = np.float16

    x1_shape = [m, k]
    x2_shape = [k, n]
    bias_shape = [1, n]

    x1_gm = np.random.uniform(-1, 1, x1_shape).astype(src_type)
    x1_gm_fp32 = x1_gm.astype(np.float32)
    x2_gm = np.random.uniform(-1, 1, x2_shape).astype(src_type)
    x2_gm_fp32 = x2_gm.astype(np.float32)
    if is_bias:
        bias_gm = np.random.uniform(-1, 1, [1, n]).astype(dst_type)
        bias_gm_fp32 = bias_gm.astype(np.float32)
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32, bias_gm_fp32)
    else:
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32)

    if a_format == "COLUMN_MAJOR":
        x1_gm = x1_gm.transpose()
    if b_format == "COLUMN_MAJOR":
        x2_gm = x2_gm.transpose()

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    x1_gm.tofile("./input/x1_gm.bin")
    x2_gm.tofile("./input/x2_gm.bin")
    if is_bias:
        bias_gm.tofile("./input/bias_gm.bin")
    y_gm = y_gm_fp32.astype(dst_type)
    if c_format == "COLUMN_MAJOR":
        y_gm = y_gm.transpose()
    y_gm.tofile("./output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
