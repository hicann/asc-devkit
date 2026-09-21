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
import ml_dtypes
import math
import struct

import numpy as np
import tensorflow as tf

bfloat16 = tf.bfloat16.as_numpy_dtype


def cvt_fp4_e1m2_to_bfloat16(x):
    Fp4e1m2ToBf16 = {
        "0": 0x0,
        "1": 0x3E80,
        "2": 0x3F00,
        "3": 0x3F40,
        "4": 0x3F80,
        "5": 0x3FA0,
        "6": 0x3FC0,
        "7": 0x3FE0,
        "8": 0x8000,
        "9": 0xBE80,
        "10": 0xBF00,
        "11": 0xBF40,
        "12": 0xBF80,
        "13": 0xBFA0,
        "14": 0xBFC0,
        "15": 0xBFE0,
    }

    x = int(x)
    first_fp4val = x & 0x0F
    second_fp4val = (x >> 4) & 0x0F
    first_fp4str = str(first_fp4val)
    second_fp4str = str(second_fp4val)

    return Fp4e1m2ToBf16[first_fp4str], Fp4e1m2ToBf16[second_fp4str]


def trans_np_fp4_e1m2_tensor_to_bfloat16(in_tensor):
    shape_tensor = in_tensor.shape
    multi_shape = np.prod(shape_tensor)
    out_tensor = np.zeros(multi_shape)
    in_tensor = in_tensor.reshape(multi_shape)
    # 1个uint8包含两个fp4， 先拆成两个uint8
    bfloat16_shape = (shape_tensor[0], shape_tensor[1] * 2)
    bfloat16_tensor = np.zeros(multi_shape * 2).astype(np.uint16)
    fp32_tensor = np.zeros(multi_shape * 2).astype(np.float32)

    for i in range(multi_shape):
        bfloat16_tensor[i * 2], bfloat16_tensor[i * 2 + 1] = cvt_fp4_e1m2_to_bfloat16(in_tensor[i])
        fp32_tensor[i * 2] = struct.unpack("!f", struct.pack("!I", bfloat16_tensor[i * 2] << 16))[0]
        fp32_tensor[i * 2 + 1] = struct.unpack("!f", struct.pack("!I", bfloat16_tensor[i * 2 + 1] << 16))[0]

    fp32_tensor = fp32_tensor.reshape(bfloat16_shape)
    return fp32_tensor


def float_to_hex(f):
    return hex(struct.unpack("<I", struct.pack("<f", f))[0])


def IsRoundOne(sign, man, truncLen):
    roundingTruncLen = 64
    if truncLen >= roundingTruncLen:
        mask0 = 0
    else:
        mask0 = 0x1 << truncLen
    if truncLen > roundingTruncLen:
        mask1 = 0
    else:
        mask1 = 0x1 << (truncLen - 1)

    mask2 = mask1 - 1
    # ROUND_TO_NEAREST
    lastBit = (man & mask0) > 0  # Last bit after conversion
    truncHighBit = (man & mask1) > 0  # Highest bit in the truncated part
    truncLeft = (man & mask2) > 0  # Truncated left part (except for the highest bit)
    return truncHighBit and (truncLeft or lastBit)


def cvt_bfloat16_to_fp4_e1m2(x):
    sRet = 0
    if x < 0.0:
        sRet = 1

    x_abs = math.fabs(x)
    x = eval(float_to_hex(x_abs))
    x = x >> 16

    ef = x >> 7 & 0xFF
    mf = x & 0x7F
    mLenDelta = 7 - 2  #
    maxExp = 1  # max E encoding value of e1m2 is 3
    expBias = 1  # Exponent Bias value of e2m1/e1m2 is 1

    eRet = 0
    mRet = 0
    eNorm = 0
    if ef == 0 and mf != 0:
        eNorm = ef - 127 + 1  # the exp bias of subnormal bf16 is 126
    else:
        eNorm = ef - 127  # the exp bias of bf16 is 127

    if (eNorm > (maxExp - expBias)) or ((eNorm == (maxExp - expBias)) and ((mf >> mLenDelta) == 0b11)):
        return (sRet << 3) | 0b111
    elif eNorm <= -(expBias):
        eRet = 0
        mf = mf | 0x80
        mLenDelta -= eNorm + expBias - 1
        needRound = IsRoundOne(sRet, mf, mLenDelta)  # determine if need to carry
        mRet = mf >> mLenDelta
        if needRound:
            mRet += 1
    else:
        eRet = eNorm + expBias
        needRound = IsRoundOne(sRet, mf, mLenDelta)
        mRet = mf >> mLenDelta
        if needRound:
            mRet += 1
        if ((mRet & 0b100) != 0) and (needRound):
            eRet += 1
            mRet = 0

    if eRet >= 1:
        eRet = 1
    elif eRet == 0 and mRet == 0b100:
        eRet += 1
        mRet = 0

    return ((sRet) << 3) | ((eRet) << 2) | ((mRet) & 3)


def trans_np_bfloat16_tensor_to_fp4_e1m2(in_tensor):
    shape_tensor = in_tensor.shape
    multi_shape = np.prod(shape_tensor)
    out_tensor = np.zeros(multi_shape).astype(np.uint8)
    in_tensor = in_tensor.reshape(multi_shape)

    for i in range(multi_shape):
        out_tensor[i] = cvt_bfloat16_to_fp4_e1m2(in_tensor[i])

    out_tensor = out_tensor.astype(np.uint8)
    # 每两个fp4拼成一个uint8保存
    fp4_shape = (shape_tensor[0], shape_tensor[1] // 2)
    fp4_tensor = np.zeros(multi_shape // 2).astype(np.uint8)
    for i in range(multi_shape // 2):
        fp4_tensor[i] = (out_tensor[i * 2 + 1] << 4) | out_tensor[
            i * 2
        ]  # 按两两交叉顺序保存b4，比如b4两个数：0100 0010 存为b8后为0010 0100

    fp4_tensor = fp4_tensor.reshape(fp4_shape)
    return fp4_tensor


def gen_golden_data():
    m = 64
    n = 128
    k = 128
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    is_bias = False
    is_trans_a = False
    is_trans_b = False
    is_trans_scalea = False
    is_trans_scaleb = True

    src_type = ml_dtypes.float8_e5m2
    src_scale_type = np.uint8
    dst_type = np.float32
    a_format = "NZ"
    b_format = "NZ"
    scalea_format = "NZ"
    scaleb_format = "NZ"
    c0_size = 64
    sk = (int)(np.ceil(k / 64) * 2)

    if is_trans_a:
        x1_shape = [k, m]
    else:
        x1_shape = [m, k]

    if is_trans_b:
        x2_shape = [n, k]
    else:
        x2_shape = [k, n]

    x1_s_shape = [m, sk]
    x2_s_shape = [sk, n]

    x1_ori = np.random.uniform(0, 2, x1_shape).astype(np.float64)
    x1_ori_tmp = trans_np_bfloat16_tensor_to_fp4_e1m2(x1_ori.astype(bfloat16))
    x1_gm = trans_np_fp4_e1m2_tensor_to_bfloat16(x1_ori_tmp).astype(np.float64)

    x2_ori = np.random.uniform(0, 2, x2_shape).astype(np.float64).transpose()
    x2_ori_tmp = trans_np_bfloat16_tensor_to_fp4_e1m2(x2_ori.astype(bfloat16))
    x2_gm = trans_np_fp4_e1m2_tensor_to_bfloat16(x2_ori_tmp).astype(np.float64).transpose()

    bias_gm = np.random.randint(0, 10, (1, n)).astype(dst_type)
    x1_mx_gm = np.random.randint(127, 130, x1_s_shape).astype(src_scale_type)
    x2_mx_gm = np.random.randint(127, 130, x2_s_shape).astype(src_scale_type)

    x1_mx = 2 ** (x1_mx_gm.astype(np.float64) - 127)
    x2_mx = 2 ** (x2_mx_gm.astype(np.float64) - 127)

    x1 = np.zeros(x1_shape, dtype=np.float64)
    x2 = np.zeros(x2_shape, dtype=np.float64)

    for i in range(x1_gm.shape[1]):
        x1[:, i] = x1_gm[:, i] * x1_mx[:, i // 32]
        x2[i, :] = x2_gm[i, :] * x2_mx[i // 32, :]

    y_gm = np.matmul(x1.astype(np.float64), x2.astype(np.float64)).astype(dst_type)

    if is_trans_scalea:
        x1_mx_gm = x1_mx_gm.transpose()
    if is_trans_scaleb:
        x2_mx_gm = x2_mx_gm.transpose()

    if a_format == "NZ":
        # x1_gm nz
        x1_gm = x1_gm.reshape((int(m / 16), 16, int(k / c0_size), c0_size)).transpose(2, 0, 1, 3)
        x1_gm = x1_gm.reshape(x1_gm.shape[0] * x1_gm.shape[1], x1_gm.shape[2] * x1_gm.shape[3])
    if b_format == "NZ":
        # x2_gm nz
        x2_gm = x2_gm.reshape((int(x2_gm.shape[0] / 16), 16, int(x2_gm.shape[1] / c0_size), c0_size)).transpose(
            2, 0, 1, 3
        )
        x2_gm = x2_gm.reshape(x2_gm.shape[0] * x2_gm.shape[1], x2_gm.shape[2] * x2_gm.shape[3])
    if scalea_format == "NZ":
        # scalea nz
        if not is_trans_scalea:
            x1_mx_gm = x1_mx_gm.reshape(int(m / 16), 16, int(sk / 2), 2).transpose(0, 2, 1, 3)
        else:
            x1_mx_gm = x1_mx_gm.reshape(int(sk / 2), 2, int(m / 16), 16).transpose(2, 0, 3, 1)
    else:
        if is_trans_scalea:
            x1_mx_gm = x1_mx_gm.reshape(int(sk / 2), 2, m).transpose(0, 2, 1)
    if scaleb_format == "NZ":
        # scaleb nz
        if not is_trans_scaleb:
            x2_mx_gm = x2_mx_gm.reshape(int(sk / 2), 2, int(n / 16), 16).transpose(2, 0, 3, 1)
        else:
            x2_mx_gm = x2_mx_gm.reshape(int(n / 16), 16, int(sk / 2), 2).transpose(0, 2, 1, 3)
    else:
        if not is_trans_scaleb:
            x2_mx_gm = x2_mx_gm.reshape(int(sk / 2), 2, n).transpose(0, 2, 1)

    x1_ori_shape = np.prod(x1_gm.shape)
    x1_tensor = np.zeros(x1_ori_shape).astype(np.uint8)
    fp4_tensor = trans_np_bfloat16_tensor_to_fp4_e1m2(x1_gm.astype(bfloat16))
    tmp_tensor = fp4_tensor.reshape(np.prod(fp4_tensor.shape))
    x1_tensor[0 : x1_ori_shape // 2] = tmp_tensor[0 : x1_ori_shape // 2]
    x1_tensor[x1_ori_shape // 2 :] = 0
    x1_tensor.tofile("./input/x1_gm.bin")

    x2_ori_shape = np.prod(x2_gm.shape)
    x2_tensor = np.zeros(x2_ori_shape).astype(np.uint8)
    fp4_tensor = trans_np_bfloat16_tensor_to_fp4_e1m2(x2_gm.astype(bfloat16))
    tmp_tensor = fp4_tensor.reshape(np.prod(fp4_tensor.shape))
    x2_tensor[0 : x2_ori_shape // 2] = tmp_tensor[0 : x2_ori_shape // 2]
    x2_tensor[x2_ori_shape // 2 :] = 0
    x2_tensor.tofile("./input/x2_gm.bin")

    x1_mx_gm.tofile("./input/x1_mx_gm.bin")
    x2_mx_gm.tofile("./input/x2_mx_gm.bin")

    y_gm.tofile("./output/golden.bin")
    if is_bias:
        bias_gm.tofile("./input/bias_gm.bin")


if __name__ == "__main__":
    gen_golden_data()
