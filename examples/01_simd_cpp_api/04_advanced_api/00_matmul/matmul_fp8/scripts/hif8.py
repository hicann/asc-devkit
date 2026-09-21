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
from collections import namedtuple
import logging
import math
import struct
from multiprocessing import Process, Queue
import numpy as np


def _get_binary_pos_str(f32_binary_str, start_pos, end_pos):
    if start_pos == end_pos:
        return f32_binary_str[start_pos]
    else:
        return f32_binary_str[start_pos : end_pos + 1]


def _binary_to_int(binary_str):
    decimal_num = int(binary_str, 2)
    return decimal_num


def test_decode_fp32(input_float):
    sign_start_pos = 0
    sign_end_pos = 0
    exponent_start_pos = 1
    exponent_end_pos = 8
    fraction_start_pos = 9
    fraction_end_pos = 32

    float_binary = _float_to_binary(input_float)
    f_sign_bin_str = _get_binary_pos_str(float_binary, sign_start_pos, sign_end_pos)
    f_exponent_bin_str = _get_binary_pos_str(float_binary, exponent_start_pos, exponent_end_pos)
    f_fraction_bin_str = _get_binary_pos_str(float_binary, fraction_start_pos, fraction_end_pos)
    f_exponent_bin_int = _binary_to_int(f_exponent_bin_str)
    f_fraction_bin_int = _binary_to_int(f_fraction_bin_str)


def _float_to_binary(float_num):
    # Pack float_num into 4 bytes using struct, then convert to unsigned long integer
    packed = struct.pack("!f", float_num)
    # Convert packed result to 32-bit binary string
    return "".join(f"{c:08b}" for c in packed)


def _binary_to_float(binary_str):
    # Convert binary string to bytes type
    binary_bytes = bytes.fromhex(hex(int(binary_str, 2))[2:])
    # Use struct.unpack to convert bytes type to float type
    return struct.unpack("!f", binary_bytes)[0]


def _get_hif8_fraction_bits_number(exponent):
    # Return dot value(4bits), exponent size, fraction size
    if exponent < -22:
        # zero
        return -1, 3, 0
    if -22 <= exponent < -15:
        # dml
        return 0, 3, 0
    if exponent == 0:
        # d0
        return 1, 0, 3
    if abs(exponent) == 1:
        # d1
        return 2, 1, 3
    if 2 <= abs(exponent) <= 3:
        # d2
        return 4, 2, 3
    if 4 <= abs(exponent) <= 7:
        # d3
        return 8, 3, 2
    if 8 <= abs(exponent) <= 15:
        # d4
        return 12, 4, 1
    if exponent > 15:
        # overflow
        return 12, 4, -1
    logging.info("[ERROR] unknown exponent value")
    return 0, 0, 0


def _fp32_ta_round_to_hif8(fraction32_int, hif8_bits_num):
    # fp32 fraction is 23,keep hif8_bits_num + 1 bits
    hif8_value_tmp = fraction32_int >> (23 - (hif8_bits_num + 1))
    if hif8_value_tmp == pow(2, hif8_bits_num + 1) - 1:
        # carry exponent
        return True, 0
    elif hif8_value_tmp == 0:
        # zero
        return False, 0
    elif hif8_value_tmp % 2 == 1:
        # carrys bits
        hif8_value_tmp += 1
        return False, hif8_value_tmp >> 1
    else:
        return False, hif8_value_tmp >> 1


def _fp32_ssr_round_to_hif8(fraction32_int, hif8_bits_num):
    # fp32 fraction is 23,keep hif8_bits_num bits
    hif8_value = fraction32_int >> (23 - hif8_bits_num)
    f14_t14 = fraction32_int - (hif8_value << (23 - hif8_bits_num))
    # current do not deal hif8_bits_num > 9
    f14_values = f14_t14 >> (23 - hif8_bits_num - 14)
    # b11111111111111
    t14_mask = 16383
    t14_values = f14_t14 & t14_mask
    if f14_values >= t14_values:
        # carry bits
        if hif8_value == pow(2, hif8_bits_num) - 1:
            # carry exponent:
            return True, 0
        else:
            hif8_value += 1
            return False, hif8_value
    else:
        return False, hif8_value


def _fp32_sr_round_to_hif8(fraction32_int, hif8_bits_num, random_t):
    # leave it alone
    return _fp32_ssr_round_to_hif8(fraction32_int, hif8_bits_num)


def _fp16_ta_round_to_hif8(fraction16_int, hif8_bits_num):
    # fp16 fraction is 10,keep hif8_bits_num + 1 bits
    hif8_value_tmp = fraction16_int >> (10 - (hif8_bits_num + 1))
    if hif8_value_tmp == pow(2, hif8_bits_num + 1) - 1:
        # carry exponent
        return True, 0
    elif hif8_value_tmp == 0:
        # zero
        return False, 0
    elif hif8_value_tmp % 2 == 1:
        # carrys bits
        hif8_value_tmp += 1
        return False, hif8_value_tmp >> 1
    else:
        return False, hif8_value_tmp >> 1


def _fp16_ssr_round_to_hif8(fraction16_int, hif8_bits_num):
    # fp16 fraction is 10,keep hif8_bits_num bits
    hif8_value = fraction16_int >> (10 - hif8_bits_num)
    f2_t2 = fraction16_int - (hif8_value << (10 - hif8_bits_num))
    # current do not deal hif8_bits_num > 7
    f2_values = f2_t2 >> (10 - hif8_bits_num - 2)
    # b1
    t2_mask = 1
    t2_values = (f2_t2 & t2_mask) * 2 + 1
    if f2_values >= t2_values:
        # carry bits
        if hif8_value == pow(2, hif8_bits_num):
            # carry exponent:
            return True, 0
        else:
            hif8_value += 1
            return False, hif8_value
    else:
        return False, hif8_value


def _fp16_sr_round_to_hif8(fraction16_int, hif8_bits_num, random_t):
    # leave it alone
    return _fp16_ssr_round_to_hif8(fraction16_int, hif8_bits_num)


def get_cut_bit_type(round_mode, exponent):
    if round_mode == "hybrid":
        if abs(exponent) < 4:
            cut_bit_type = "TA"
        else:
            cut_bit_type = "SSR"
    elif round_mode == "round":
        cut_bit_type = "TA"
    elif round_mode == "storound":
        cut_bit_type = "SSR"
    else:
        cut_bit_type = "TA"
    return cut_bit_type


def get_over_flow_res(sign, over_mode):
    if sign:
        if over_mode:
            # b11101111
            return 239
        else:
            # b11101110
            return 238
    else:
        if over_mode:
            # b01101111
            return 111
        else:
            # b01101110
            return 110
    return -1


Hif8Collection = namedtuple(
    "Hif8Collection",
    ["dot_hif8_value", "sign_int_value", "hif8_frac_value", "exponent_hif8_bits", "fraction_hif8_bits"],
)


def cvt_float16_to_hifuint8(x, round_mode="round", over_mode=True):
    ec = 0
    over_value = 1.25 * pow(2.0, 15 + ec)
    sign = False
    sign_int_value = 0
    x_abs = math.fabs(x)
    if np.isinf(x) or x_abs >= over_value:
        res = get_over_flow_res(sign, over_mode)
        if res > 0:
            return res
    if np.isnan(x):
        if over_mode:
            # b10000000
            return 128
        else:
            return 0
    if x < 0.0:
        sign = True
        sign_int_value = 128
    if x_abs == 0.0:
        return 0
    exponent = math.floor(math.log2(x_abs))

    cut_bit_type = get_cut_bit_type(round_mode, exponent)
    # precheck
    fraction_int = int(x_abs * pow(2, 10) * pow(2, -exponent) - pow(2, 10))
    dot_hif8_value, exponent_hif8_bits, fraction_hif8_bits = _get_hif8_fraction_bits_number(exponent)
    if cut_bit_type == "TA":
        carry_exp_status, hif8_frac_value = _fp16_ta_round_to_hif8(fraction_int, fraction_hif8_bits)
    elif cut_bit_type == "SSR":
        carry_exp_status, hif8_frac_value = _fp16_ssr_round_to_hif8(fraction_int, fraction_hif8_bits)
    elif cut_bit_type == "SR":
        random_t = 0
        carry_exp_status, hif8_frac_value = _fp16_sr_round_to_hif8(fraction_int, fraction_hif8_bits, random_t)
    else:
        logging.info("[ERROR] unknown round type")
        return 0
    if carry_exp_status:
        exponent += 1
        dot_hif8_value, exponent_hif8_bits, fraction_hif8_bits_new = _get_hif8_fraction_bits_number(exponent)
        hif8_frac_value = hif8_frac_value >> (fraction_hif8_bits - fraction_hif8_bits_new)
        fraction_hif8_bits = fraction_hif8_bits_new
    if fraction_hif8_bits == -1:
        # overflow
        res = get_over_flow_res(sign, over_mode)
        if res > 0:
            return res
    if dot_hif8_value == -1:
        # small than dmz
        return 0
    hif8_collect = Hif8Collection(
        dot_hif8_value, sign_int_value, hif8_frac_value, exponent_hif8_bits, fraction_hif8_bits
    )
    return get_hif8_int_value(exponent, hif8_collect)


def get_hif8_int_value(exponent, hif8_collect):
    if exponent < 0:
        sig_exp = 1
    else:
        sig_exp = 0

    if hif8_collect.dot_hif8_value == 1:
        # d0
        dot_int_value = hif8_collect.dot_hif8_value << 3
        hif8_int_value = hif8_collect.sign_int_value + dot_int_value + hif8_collect.hif8_frac_value
    elif hif8_collect.dot_hif8_value == 0:
        # dml
        hif8_int_value = hif8_collect.sign_int_value + exponent + 23
    else:
        abs_exponent = abs(exponent)
        abs_exponent = abs_exponent - pow(2, hif8_collect.exponent_hif8_bits - 1)
        exponent_int_value = abs_exponent << hif8_collect.fraction_hif8_bits
        sig_exp = sig_exp << (hif8_collect.exponent_hif8_bits - 1 + hif8_collect.fraction_hif8_bits)
        dot_int_value = hif8_collect.dot_hif8_value << 3
        hif8_int_value = (
            hif8_collect.sign_int_value + dot_int_value + sig_exp + exponent_int_value + hif8_collect.hif8_frac_value
        )
    return hif8_int_value


def cvt_float32_to_hifuint8(x, round_mode="round", over_mode=True):
    sign = False
    sign_int_value = 0
    x_abs = math.fabs(x)
    ec = 0
    over_value = 1.25 * pow(2.0, 15 + ec)
    if x < 0.0:
        sign = True
        sign_int_value = 128
    if np.isinf(x) or x_abs >= over_value:
        res = get_over_flow_res(sign, over_mode)
        if res > 0:
            return res
    if np.isnan(x):
        if over_mode:
            # b10000000
            return 128
        else:
            return 0
    if x_abs == 0.0:
        return 0
    exponent = math.floor(math.log2(x_abs))
    cut_bit_type = get_cut_bit_type(round_mode, exponent)

    # precheck
    fraction_int = int(x_abs * pow(2, 23) * pow(2, -exponent) - pow(2, 23))
    dot_hif8_value, exponent_hif8_bits, fraction_hif8_bits = _get_hif8_fraction_bits_number(exponent)
    if cut_bit_type == "TA":
        carry_exp_status, hif8_frac_value = _fp32_ta_round_to_hif8(fraction_int, fraction_hif8_bits)
    elif cut_bit_type == "SSR":
        carry_exp_status, hif8_frac_value = _fp32_ssr_round_to_hif8(fraction_int, fraction_hif8_bits)
    elif cut_bit_type == "SR":
        random_t = 0
        carry_exp_status, hif8_frac_value = _fp32_sr_round_to_hif8(fraction_int, fraction_hif8_bits, random_t)
    else:
        logging.info("[ERROR] unknown round type")
        return 0
    if carry_exp_status:
        exponent += 1
        dot_hif8_value, exponent_hif8_bits, fraction_hif8_bits_new = _get_hif8_fraction_bits_number(exponent)
        fraction_hif8_bits = fraction_hif8_bits_new
    if exponent < -22:
        # zero b00000000
        return 0
    hif8_collect = Hif8Collection(
        dot_hif8_value, sign_int_value, hif8_frac_value, exponent_hif8_bits, fraction_hif8_bits
    )
    return get_hif8_int_value(exponent, hif8_collect)


def cvt_hifuint8_to_float(x, over_mode=True):
    if x == 0:
        return float(0)
    elif x == 128:
        if over_mode:
            return np.nan
        else:
            return float(0)
    elif x == 239:
        if over_mode:
            return -np.inf
        else:
            return -32768
    elif x == 111:
        if over_mode:
            return np.inf
        else:
            return 32768
    else:
        if x >= 128:
            sign = -1.0
        else:
            sign = 1.0
        return cvt_hifuint8_to_float_else(x, sign)


def cvt_hifuint8_to_float_else(x, sign):
    dot_4_bits = x & 120
    dot_4_value = dot_4_bits >> 3
    if dot_4_value >= 12:
        exponet = x & 30
        exponet_int = exponet >> 1
        if exponet_int >= 8:
            exponet_value = -exponet_int
        else:
            exponet_value = exponet_int + 8

        fra_int = x & 1
        m_value = 1.0 + fra_int * 0.5
    elif dot_4_value >= 8:
        exponet = x & 28
        exponet_int = exponet >> 2
        if exponet_int >= 4:
            # b100
            exponet_value = -exponet_int
        else:
            exponet_value = exponet_int + 4
        fra_int = x & 3
        m_value = 1.0 + fra_int * 0.25
    elif dot_4_value >= 4:
        # b0100
        exponet = x & 24
        exponet_int = exponet >> 3
        if exponet_int >= 2:
            # b10
            exponet_value = -exponet_int
        else:
            exponet_value = exponet_int + 2
        fra_int = x & 7
        m_value = 1.0 + fra_int * 0.125
    elif dot_4_value >= 2:
        # b0010
        exponet = x & 8
        exponet_sign = exponet >> 3
        if exponet_sign >= 1:
            # b10
            exponet_value = -1
        else:
            exponet_value = 1
        fra_int = x & 7
        m_value = 1.0 + fra_int * 0.125
    elif dot_4_value == 1:
        # d0
        exponet_value = 0
        fra_int = x & 7
        m_value = 1.0 + fra_int * 0.125
    elif dot_4_value == 0:
        # dml
        m_value = 1
        exponet_value = (x & 7) - 23
    else:
        logging.info("[ERROR] error, dot error")
        m_value = 0.0
        exponet_value = 0
    return sign * pow(2.0, exponet_value) * m_value


def worker_cvt_fp32_to_hif8(queuein, queueout):
    while True:
        in_data = queuein.get()
        if in_data is None:
            break
        index = in_data["index"]
        value = in_data["value"]
        out_data = {}
        out_data["index"] = index
        out_data["value"] = cvt_float32_to_hifuint8(value)
        queueout.put(out_data)


def trans_np_float_tensor_to_hifint8_mp(in_tensor):
    shape_tensor = in_tensor.shape
    multi_shape = np.prod(shape_tensor)
    out_tensor = np.zeros(multi_shape)
    in_tensor = in_tensor.reshape(multi_shape)
    processes_num = 4
    queue_in = Queue()
    queue_out = Queue()
    processes = []
    for _ in range(processes_num):
        p = Process(target=worker_cvt_fp32_to_hif8, args=(queue_in, queue_out))
        p.start()
        processes.append(p)
    for i in range(multi_shape):
        data = {}
        data["index"] = i
        data["value"] = in_tensor[i]
        queue_in.put(data)

    for _ in range(len(processes)):
        queue_in.put(None)

    count = 0
    while count < multi_shape:
        if not queue_out.empty():
            data_out = queue_out.get()
            out_tensor[data_out["index"]] = data_out["value"]
            count += 1
        else:
            continue

    for p in processes:
        p.join()

    out_tensor = out_tensor.reshape(shape_tensor).astype(np.int8)
    return out_tensor


def trans_np_float_tensor_to_hifuint8(in_tensor, round_mode="round", over_mode=True):
    shape_tensor = in_tensor.shape
    multi_shape = np.prod(shape_tensor)
    out_tensor = np.zeros(multi_shape)
    in_tensor = in_tensor.reshape(multi_shape)
    if in_tensor.dtype == np.float32:
        for i in range(multi_shape):
            out_tensor[i] = cvt_float32_to_hifuint8(in_tensor[i], round_mode, over_mode)
    else:
        for i in range(multi_shape):
            out_tensor[i] = cvt_float16_to_hifuint8(in_tensor[i], round_mode, over_mode)
    out_tensor = out_tensor.astype(np.uint8)
    out_tensor = out_tensor.reshape(shape_tensor)
    return out_tensor


def trans_np_hifuint8_tensor_to_float32(in_tensor):
    shape_tensor = in_tensor.shape
    multi_shape = np.prod(shape_tensor)
    out_tensor = np.zeros(multi_shape).astype(np.float32)
    in_tensor = in_tensor.reshape(multi_shape)
    for i in range(multi_shape):
        out_tensor[i] = cvt_hifuint8_to_float(in_tensor[i])
    out_tensor = out_tensor.reshape(shape_tensor).astype(np.float32)
    return out_tensor
