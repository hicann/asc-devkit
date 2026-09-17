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
import logging
import ml_dtypes
from hif8 import trans_np_float_tensor_to_hifuint8
import numpy as np
import tensorflow as tf

m = 428
n = 479
k = 158
b = 1
is_trans_a = False
is_trans_b = False
is_bias = True

a_format = "ND"
b_format = "ND"
c_format = "ND"


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
        feed_dict = {
            x1: x1_gm_fp32,
            x2: x2_gm_fp32,
        }
        if is_bias:
            feed_dict[bias] = bias_gm_fp32
        res_tf = sess.run(res_tf, feed_dict=feed_dict)
    y_gm_fp32 = due_overflow(res_tf)
    return y_gm_fp32


def savebinfile(work_dir, x1_gm, x2_gm, y_gm, bias_gm):
    x1_gm.tofile(work_dir + "/input/x1_gm.bin")
    x2_gm.tofile(work_dir + "/input/x2_gm.bin")
    y_gm.tofile(work_dir + "/output/golden.bin")
    if is_bias:
        bias_gm.tofile(work_dir + "/input/bias_gm.bin")


def gen_golden_data(work_dir, scenario_num):
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    data_type_str = "hif8_hif8_float"

    if scenario_num == 1:
        data_type_str = "fp8_e4m3fn_fp8_e4m3fn_float"
    elif scenario_num == 2:
        data_type_str = "fp8_e5m2_fp8_e5m2_float"
    elif scenario_num == 3:
        data_type_str = "fp8_e4m3fn_fp8_e5m2_float"
    elif scenario_num == 4:
        data_type_str = "fp8_e5m2_fp8_e4m3fn_float"

    if data_type_str == "float16_float32":
        gen_golden_data_fp16(work_dir)
    elif data_type_str == "float16_float16":
        gen_golden_data_fp16(work_dir, np.float16)
    elif data_type_str == "hif8_hif8_float":
        gen_golden_data_hif8(work_dir)
    elif "fp8" in data_type_str:
        gen_golden_data_fp8(work_dir, data_type_str)
    else:
        logging.info("[ERROR] can't support data type %s" % (data_type_str))
        return -1
    return 0


def gen_golden_data_hif8(work_dir, dst_type=np.float32):
    src_type = np.uint8
    c0size = 32
    if is_trans_a:
        x1_shape = [b, k, m]
    else:
        x1_shape = [b, m, k]
    if is_trans_b:
        x2_shape = [b, n, k]
    else:
        x2_shape = [b, k, n]
    x1_gm = np.random.uniform(0, 5, x1_shape).astype(src_type)
    x1_gm_fp32 = x1_gm.astype(np.float32)
    x2_gm = np.random.uniform(0, 5, x2_shape).astype(src_type)
    x2_gm_fp32 = x2_gm.astype(np.float32)
    if is_bias:
        bias_gm = np.random.uniform(-1, 1, [1, n]).astype(dst_type)
        bias_gm_fp32 = bias_gm.astype(np.float32)

    if is_bias:
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32, bias_gm_fp32)
    else:
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32)

    if a_format == "NZ":
        x1_gm = MatmulGenHifFp8Data.nd_to_nz(x1_gm, x1_shape, src_type, c0size)
    if b_format == "NZ":
        x2_gm = MatmulGenHifFp8Data.nd_to_nz(x2_gm, x2_shape, src_type, c0size)
    if c_format == "NZ":
        c0size = 16
        y_gm = gen_c_data_nz_format(y_gm_fp32, dst_type, c0size)
    else:
        y_gm = y_gm_fp32.astype(dst_type)

    x1_gm_hif8 = trans_np_float_tensor_to_hifuint8(x1_gm)
    x2_gm_hif8 = trans_np_float_tensor_to_hifuint8(x2_gm)

    savebinfile(work_dir, x1_gm_hif8, x2_gm_hif8, y_gm, bias_gm)
    return 0


def gen_golden_data_fp8(work_dir, data_type_str, dst_type=np.float32):
    c0size = 32
    if data_type_str == "fp8_e4m3fn_fp8_e4m3fn_float":
        srca_type = ml_dtypes.float8_e4m3fn
        srcb_type = ml_dtypes.float8_e4m3fn
    elif data_type_str == "fp8_e5m2_fp8_e5m2_float":
        srca_type = ml_dtypes.float8_e5m2
        srcb_type = ml_dtypes.float8_e5m2
    elif data_type_str == "fp8_e4m3fn_fp8_e5m2_float":
        srca_type = ml_dtypes.float8_e4m3fn
        srcb_type = ml_dtypes.float8_e5m2
    elif data_type_str == "fp8_e5m2_fp8_e4m3fn_float":
        srca_type = ml_dtypes.float8_e5m2
        srcb_type = ml_dtypes.float8_e4m3fn

    if is_trans_a:
        x1_shape = [b, k, m]
    else:
        x1_shape = [b, m, k]
    if is_trans_b:
        x2_shape = [b, n, k]
    else:
        x2_shape = [b, k, n]
    x1_gm = np.random.uniform(-1, 1, x1_shape).astype(srca_type)
    x1_gm_fp32 = x1_gm.astype(np.float32)
    x2_gm = np.random.uniform(-1, 1, x2_shape).astype(srcb_type)
    x2_gm_fp32 = x2_gm.astype(np.float32)
    if is_bias:
        bias_gm = np.random.uniform(-1, 1, [1, n]).astype(dst_type)
        bias_gm_fp32 = bias_gm.astype(np.float32)

    if is_bias:
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32, bias_gm_fp32)
    else:
        y_gm_fp32 = tf_matmul(x1_gm_fp32, x2_gm_fp32)

    if a_format == "NZ":
        x1_gm = MatmulGenHifFp8Data.nd_to_nz(x1_gm, x1_shape, srca_type, c0size)
    if b_format == "NZ":
        x2_gm = MatmulGenHifFp8Data.nd_to_nz(x2_gm, x2_shape, srcb_type, c0size)
    if c_format == "NZ":
        c0size = 16
        y_gm = gen_c_data_nz_format(y_gm_fp32, dst_type, c0size)
    else:
        y_gm = y_gm_fp32.astype(dst_type)

    savebinfile(work_dir, x1_gm, x2_gm, y_gm, bias_gm)
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=0,
        choices=[0, 1, 2, 3, 4],
        help="Scenario number: 0 ~ 4",
    )
    args = parser.parse_args()
    gen_golden_data(".", args.scenarioNum)
