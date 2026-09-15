/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_bf16_debug.cpp
 * \brief
 */

#if defined(ASCENDC_CPU_DEBUG)
#include "stub_def.h"

constexpr uint32_t b_half_inf = 0x7F80;
constexpr uint32_t b_half_neg_inf = 0xFF80;
constexpr uint32_t half_max_nan = 0x7FFF;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
bool __isnan(bfloat16_t x)
{
    uint16_t* intX = (uint16_t*)&x;
    return (*intX > b_half_inf && *intX <= half_max_nan) || (*intX > b_half_neg_inf);
}

bool __isinf(bfloat16_t x)
{
    uint16_t* intX = (uint16_t*)&x;
    return (*intX == b_half_inf || *intX == b_half_neg_inf);
}

bfloat16_t __fma(bfloat16_t x, bfloat16_t y, bfloat16_t z)
{
    if (__isnan(z) || __isnan(x) || __isnan(y)) {
        return NAN;
    }
    return x * y + z;
}

bfloat16_t __max(bfloat16_t x, bfloat16_t y)
{
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

bfloat16_t __min(bfloat16_t x, bfloat16_t y)
{
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

bfloat16_t FindNear2ndPow(bfloat16_t x)
{
    bfloat16_t b_half_one;
    b_half_one = 1;
    bfloat16_t b_half_two;
    b_half_two = 2; // 2:bfloat16 val two
    if (b_half_two >= x) {
        return b_half_one;
    } else {
        return (bfloat16_t)pow(2, static_cast<uint32_t>(log2(x))); // 2:base of the pow function
    }
}

bfloat16_t __floorf(bfloat16_t x)
{
    if (__isinf(x)) {
        return x;
    }
    uint16_t* uintX = (uint16_t*)(&x);
    uint16_t bf16Sign15Bit = 0xf;
    uint16_t bf16Frac7Bit = 0x7;
    int8_t sign = ((((*uintX) >> bf16Sign15Bit) & 0x1) == 0) ? 1 : -1;
    uint16_t exp = ((*uintX) >> bf16Frac7Bit) & 0xff;
    bfloat16_t frac = (*uintX) & 0x7f;
    bfloat16_t two = 2.0;
    uint16_t bfl16Exp126Bit = 0x7e;
    uint16_t bfl16Exp127Bit = 0x7f;
    if (exp == 0xff && !(frac < 0 || frac > 0)) {
        return (bfloat16_t)(sign * INFINITY);
    } else if (exp == 0xff && (frac > 0 || frac < 0)) {
        return (bfloat16_t)(sign * NAN);
    }
    bfloat16_t base;
    bfloat16_t minVal;
    if (exp == 0) {
        base = sign * (pow(two, (exp - bfl16Exp126Bit)) * (frac / (pow(two, bf16Frac7Bit))));
        if (exp - bfl16Exp126Bit > 0) {
            minVal = sign * (pow(two, (exp - bfl16Exp126Bit)));
        } else {
            minVal = 0;
        }
    } else {
        base = sign * (pow(two, (exp - bfl16Exp127Bit)) * (1 + (frac / (pow(two, bf16Frac7Bit)))));
        if (exp - bfl16Exp127Bit > 0) {
            minVal = sign * (pow(two, (exp - bfl16Exp127Bit)));
        } else {
            minVal = 0;
        }
    }
    if (base > minVal) {
        while (base - minVal >= 1) {
            minVal += FindNear2ndPow(base - minVal);
        }
        return minVal;
    } else if (base < minVal) {
        while (minVal - base > 0) {
            minVal -= FindNear2ndPow(minVal - base);
        }
        return minVal;
    }
    return base;
}

bfloat16_t __rintf(bfloat16_t x)
{
    if (__isinf(x)) {
        return x;
    }
    bfloat16_t floorX = __floorf(x);
    if (!(x < floorX || x > floorX)) {
        return x;
    }
    bfloat16_t b_half_one;
    b_half_one = 1;
    bfloat16_t ceilX = floorX + b_half_one;
    bfloat16_t midVal = 0.5;
    if (x - floorX > midVal) {
        return ceilX;
    } else if (x - floorX < midVal) {
        return floorX;
    } else {
        // Same Distance Round to Even
        uint16_t two = 2;
        if (((int32_t)ceilX) % two == 0) {
            return ceilX;
        } else {
            return floorX;
        }
    }
}

bfloat16_t __ceilf(bfloat16_t x)
{
    bfloat16_t floorX = __floorf(x);
    if (!(floorX < x || floorX > x)) {
        return floorX;
    }
    bfloat16_t b_half_one;
    b_half_one = 1;
    return floorX + b_half_one;
}
#endif
#endif
