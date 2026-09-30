#!/usr/bin/env python3

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Minimal hifloat8 CAST_ROUND encoder/decoder for sample data preparation."""

import math

import numpy as np


def _layout(exponent):
    if exponent < -22:
        return 0, 0, 0
    if exponent <= -16:
        return 0, 0, 0
    if exponent == 0:
        return 1, 0, 3
    if abs(exponent) == 1:
        return 2, 1, 3
    if abs(exponent) <= 3:
        return 4, 2, 3
    if abs(exponent) <= 7:
        return 8, 3, 2
    if abs(exponent) <= 15:
        return 12, 4, 1
    return 12, 4, -1


def float_to_hif8(value):
    value = float(value)
    if math.isnan(value):
        return 128
    if math.isinf(value) or abs(value) >= 1.25 * (2.0**15):
        return 239 if math.copysign(1.0, value) < 0 else 111
    if value == 0.0:
        return 0

    sign = 128 if value < 0 else 0
    magnitude = abs(value)
    exponent = math.floor(math.log2(magnitude))
    if exponent < -22:
        return 0
    if exponent <= -16:
        return sign + exponent + 23

    dot, exponent_bits, fraction_bits = _layout(exponent)
    fraction = magnitude / (2.0**exponent) - 1.0
    fraction_value = int(math.floor(fraction * (2**fraction_bits) + 0.5))
    if fraction_value == 2**fraction_bits:
        exponent += 1
        dot, exponent_bits, fraction_bits = _layout(exponent)
        fraction_value = 0
        if fraction_bits < 0:
            return sign + (239 if sign else 111)

    if dot == 1:
        return sign + (dot << 3) + fraction_value

    exponent_value = abs(exponent) - (2 ** (exponent_bits - 1))
    exponent_sign = 1 if exponent < 0 else 0
    exponent_field = (exponent_sign << (exponent_bits - 1)) | exponent_value
    return sign + (dot << 3) + (exponent_field << fraction_bits) + fraction_value


def hif8_to_float(value):
    value = int(value)
    if value == 0:
        return 0.0
    if value == 128:
        return float("nan")
    if value == 111:
        return float("inf")
    if value == 239:
        return float("-inf")

    sign = -1.0 if value >= 128 else 1.0
    dot = (value & 0x78) >> 3
    if dot >= 12:
        exponent_bits = (value & 0x1E) >> 1
        exponent = -exponent_bits if exponent_bits >= 8 else exponent_bits + 8
        mantissa = 1.0 + (value & 1) * 0.5
    elif dot >= 8:
        exponent_bits = (value & 0x1C) >> 2
        exponent = -exponent_bits if exponent_bits >= 4 else exponent_bits + 4
        mantissa = 1.0 + (value & 3) * 0.25
    elif dot >= 4:
        exponent_bits = (value & 0x18) >> 3
        exponent = -exponent_bits if exponent_bits >= 2 else exponent_bits + 2
        mantissa = 1.0 + (value & 7) * 0.125
    elif dot >= 2:
        exponent = -1 if (value & 8) else 1
        mantissa = 1.0 + (value & 7) * 0.125
    elif dot == 1:
        exponent = 0
        mantissa = 1.0 + (value & 7) * 0.125
    else:
        exponent = (value & 7) - 23
        mantissa = 1.0
    return sign * (2.0**exponent) * mantissa


def encode(values):
    values = np.asarray(values, dtype=np.float32)
    return np.fromiter((float_to_hif8(value) for value in values.flat), dtype=np.uint8).reshape(values.shape)


def decode(values):
    values = np.asarray(values, dtype=np.uint8)
    return np.fromiter((hif8_to_float(value) for value in values.flat), dtype=np.float32).reshape(values.shape)
