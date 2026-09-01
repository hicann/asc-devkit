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
 * \file simt_stub.h
 * \brief
 */
#ifndef ASCENDC_SIMT_STUB_H
#define ASCENDC_SIMT_STUB_H

#if defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
#include <cmath>
#include <cfenv>
#include "kernel_simt_cpu.h"
#include "simt_stub_fun.h"
#include "kernel_fp16.h"
#include "kernel_bf16.h"
#include "kernel_vectorized.h"

#define __launch_bounds__(x)

namespace {
constexpr int16_t HALF_MAX_EXP = 31;
const float HALF_SUBNORMAL_THRESHOLD = std::pow(2, -14);
} // namespace

static half __hmax_nan(half x, half y) { return half(0); }

static half __hmin_nan(half x, half y) { return half(0); }

template <typename T>
T __shfl(T var, int32_t src_lane, int32_t width)
{
    return var;
}

template <typename T>
T __shfl_up(T var, int32_t delta, int32_t width)
{
    return var;
}

template <typename T>
T __shfl_down(T var, int32_t delta, int32_t width)
{
    return var;
}

template <typename T>
T __shfl_xor(T var, int32_t lane_mask, int32_t width)
{
    return var;
}

static inline unsigned int __brev(unsigned int x)
{
    unsigned int result = 0;
    for (int i = 0; i < 32; i++) {
        result |= ((x >> i) & 1) << (31 - i);
    }
    return result;
}

static inline int __popc(unsigned int x)
{
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

template <typename T, typename U>
constexpr uint32_t GetRoundBitNum()
{
    if constexpr (std::is_same<T, half>::value && std::is_same<U, float>::value) {
        return bfloat16::FP32_MAN_LEN - static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_LEN);
    } else if constexpr (std::is_same<T, bfloat16_t>::value && std::is_same<U, float>::value) {
        return bfloat16::FP32_MAN_LEN - bfloat16::BF16_MAN_LEN;
    }
    return 0;
}

template <typename T>
constexpr uint32_t GetMantissaLen()
{
    if constexpr (std::is_same<T, half>::value) {
        return static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_LEN);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        return bfloat16::BF16_MAN_LEN;
    }
    return 0;
}

template <typename T, typename U, ROUND rnd>
void HandleRound(uint32_t sign, int16_t& exp, uint32_t& mantissa, uint32_t round_part)
{
    constexpr uint32_t round_bit_num = GetRoundBitNum<T, U>();
    constexpr uint32_t round_carry = 1U << round_bit_num;
    constexpr uint32_t round_bit_map = round_carry - 1;
    constexpr uint32_t round_first_bit = 1U << (round_bit_num - 1);
    constexpr uint32_t round_left_bit = round_first_bit - 1;

    if constexpr (rnd == ROUND::CAST_RINT) {
        if ((round_part & round_first_bit) != 0) {
            if ((round_part & round_left_bit) != 0) {
                mantissa += 1;
            } else if ((mantissa & 1) == 1) {
                mantissa += 1;
            }
        }
    } else if constexpr (rnd == ROUND::CAST_FLOOR) {
        if ((sign == 1) && (round_part != 0)) {
            mantissa += 1;
        }
    } else if constexpr (rnd == ROUND::CAST_CEIL) {
        if ((sign == 0) && (round_part != 0)) {
            mantissa += 1;
        }
    } else if constexpr (rnd == ROUND::CAST_ROUND) {
        if ((round_part & round_first_bit) != 0) {
            mantissa += 1;
        }
    } else if constexpr (rnd == ROUND::CAST_ODD) {
        if ((round_part != 0) && ((mantissa & 1) == 0)) {
            mantissa += 1;
        }
    }

    if ((mantissa & (1U << GetMantissaLen<T>())) != 0) {
        exp += 1;
    }
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation sat = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
float CastIntegralToFloat(SRC_TYPE src)
{
    if constexpr (
        rnd == ROUND::CAST_RINT || rnd == ROUND::CAST_FLOOR || rnd == ROUND::CAST_CEIL || rnd == ROUND::CAST_TRUNC) {
        fenv_t env;
        std::fegetenv(&env);
        constexpr int32_t round = [] {
            if constexpr (rnd == ROUND::CAST_RINT) {
                return FE_TONEAREST;
            } else if constexpr (rnd == ROUND::CAST_FLOOR) {
                return FE_DOWNWARD;
            } else if constexpr (rnd == ROUND::CAST_CEIL) {
                return FE_UPWARD;
            } else if constexpr (rnd == ROUND::CAST_TRUNC) {
                return FE_TOWARDZERO;
            }
        }();
        std::fesetround(round);
        float res = static_cast<float>(src);
        std::fesetenv(&env);
        return res;
    }

    float f = static_cast<float>(src);
    SRC_TYPE tmp = static_cast<SRC_TYPE>(f);
    if (src == tmp) {
        return f;
    }

    float f_up = 0;
    float f_down = 0;
    if (src < tmp) {
        f_up = f;
        f_down = std::nextafter(f, -INFINITY);
    } else {
        f_up = std::nextafter(f, INFINITY);
        f_down = f;
    }

    if constexpr (rnd == ROUND::CAST_ROUND) {
        SRC_TYPE src_up = static_cast<SRC_TYPE>(f_up);
        SRC_TYPE src_down = static_cast<SRC_TYPE>(f_down);
        if (src_up - src == src - src_down) {
            return src > 0 ? f_up : f_down;
        } else if (src_up - src < src - src_down) {
            return f_up;
        } else {
            return f_down;
        }
    } else if constexpr (rnd == ROUND::CAST_ODD) {
        uint32_t f_bits = *reinterpret_cast<uint32_t*>(&f_up);
        return (f_bits & 1) == 0 ? f_up : f_down;
    }

    return static_cast<float>(src);
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation sat = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
float __cvt_float(SRC_TYPE src)
{
    static_assert(
        std::is_same_v<SRC_TYPE, int32_t> || std::is_same_v<SRC_TYPE, uint32_t> || std::is_same_v<SRC_TYPE, uint64_t> ||
            std::is_same_v<SRC_TYPE, int64_t> || std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, float> ||
            std::is_same_v<SRC_TYPE, bfloat16_t>,
        "src type can only be int32_t/uint32_t/int64_t/uint64_t and half/float/bfloat_t");
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");

    if constexpr (std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, bfloat16_t>) {
        if (__isnan(src)) {
            return NAN;
        }
        if (__isinf(src)) {
            return copysignf(INFINITY, src);
        }
        return src.ToFloat();
    } else if constexpr (std::is_same_v<SRC_TYPE, float>) {
        if constexpr (rnd == ROUND::CAST_RINT) {
            return rintf(src);
        } else if constexpr (rnd == ROUND::CAST_ROUND) {
            return roundf(src);
        } else if constexpr (rnd == ROUND::CAST_FLOOR) {
            return floorf(src);
        } else if constexpr (rnd == ROUND::CAST_CEIL) {
            return ceilf(src);
        } else if constexpr (rnd == ROUND::CAST_TRUNC) {
            return truncf(src);
        } else {
            return src;
        }
    }
    if constexpr (std::is_integral<SRC_TYPE>::value) {
        return CastIntegralToFloat<rnd, sat>(src);
    }
    return 0.0f;
}

template <RoundingSaturation rst, typename T, typename U>
bool HandleOverflow(uint16_t sign, int32_t exp, U& res)
{
    if constexpr (std::is_same_v<T, half>) {
        if (exp < 0) { // underflow
            res = sign << static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_INDEX);
        } else if (exp >= HALF_MAX_EXP) { // overflow
            if constexpr (rst == RoundingSaturation::RS_DISABLE_VALUE) {
                res = (sign << static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_INDEX)) |
                      static_cast<uint16_t>(Fp16BasicParam::K_FP16_EXP_MASK);
            } else {
                res = (sign << static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_INDEX)) |
                      static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAX);
            }
        } else {
            return false;
        }
        return true;
    } else if constexpr (std::is_same_v<T, bfloat16_t>) {
        if (exp == static_cast<int32_t>(Fp32BasicParam::K_FP32_MAX_EXP)) {
            if (rst == RoundingSaturation::RS_DISABLE_VALUE) {
                res = (sign << bfloat16::BF16_SIGN_INDEX) | bfloat16::BF16_INFINITY;
            } else {
                res = (sign << bfloat16::BF16_SIGN_INDEX) | bfloat16::BF16_ABS_MAX;
            }
            return true;
        }
        return false;
    }
    return false;
}

template <ROUND rnd, RoundingSaturation rst>
half __cvt_half(float x)
{
    float conform_flag = 0;
    if ((x > 0) && x < HALF_SUBNORMAL_THRESHOLD) {
        x += HALF_SUBNORMAL_THRESHOLD;
        conform_flag = -1;
    }
    if ((x < 0) && (x > (-1 * HALF_SUBNORMAL_THRESHOLD))) {
        x -= HALF_SUBNORMAL_THRESHOLD;
        conform_flag = 1;
    }

    uint32_t f_bits = *reinterpret_cast<uint32_t*>(&x);
    uint16_t sign = bfloat16::Fp32ExtracSign(f_bits);
    uint32_t exp = bfloat16::Fp32ExtracExp(f_bits);
    uint32_t mantissa = f_bits & FP32_MAN_MASK;
    uint16_t res = 0;
    // handle INF / NaN
    if (exp == static_cast<uint32_t>(Fp32BasicParam::K_FP32_MAX_EXP)) {
        res = (sign << static_cast<uint32_t>(Fp16BasicParam::K_FP16_SIGN_INDEX)) |
              (mantissa ? static_cast<uint16_t>(Fp16BasicParam::K_FP16_ABS_MAX) :
                          static_cast<uint16_t>(Fp16BasicParam::K_FP16_EXP_MASK));
        return *reinterpret_cast<half*>(&res);
    }

    int16_t half_exp =
        static_cast<int16_t>(exp) - (bfloat16::FP32_EXP_BIAS - static_cast<uint16_t>(Fp16BasicParam::K_FP16_EXP_BIAS));
    if (HandleOverflow<rst, half>(sign, half_exp, res)) {
        return *reinterpret_cast<half*>(&res);
    }

    uint32_t round_bit_num = bfloat16::FP32_MAN_LEN - static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_LEN);
    uint32_t half_mantissa = mantissa >> round_bit_num;
    uint32_t round_part = mantissa & ((1 << round_bit_num) - 1);

    HandleRound<half, float, rnd>(sign, half_exp, half_mantissa, round_part);
    res = (sign << static_cast<uint16_t>(Fp16BasicParam::K_FP16_EXP_BIAS)) |
          (half_exp << static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_LEN)) |
          (half_mantissa & static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_MASK));
    if (HandleOverflow<rst, half>(sign, half_exp, res)) {
        return *reinterpret_cast<half*>(&res);
    }
    half tmp = *reinterpret_cast<half*>(&res);
    tmp += half(conform_flag * HALF_SUBNORMAL_THRESHOLD);
    return tmp;
}

template <ROUND rnd, RoundingSaturation rst>
bfloat16_t __cvt_bfloat16_t(float x)
{
    uint32_t f_bits = *reinterpret_cast<uint32_t*>(&x);
    uint16_t sign = bfloat16::Fp32ExtracSign(f_bits);
    uint32_t exp = bfloat16::Fp32ExtracExp(f_bits);
    uint32_t mantissa = f_bits & FP32_MAN_MASK;
    uint16_t res = 0;

    // handle INF / NaN
    if (exp == static_cast<int32_t>(Fp32BasicParam::K_FP32_MAX_EXP)) {
        res = mantissa == 0 ? ((sign << bfloat16::BF16_SIGN_INDEX) | bfloat16::BF16_EXP_MASK) : bfloat16::BF16_NAN;
        return *reinterpret_cast<bfloat16_t*>(&res);
    }
    if (exp == 0 && mantissa == 0) {
        res = sign << bfloat16::BF16_SIGN_INDEX;
        return *reinterpret_cast<bfloat16_t*>(&res);
    }

    int16_t bf16_exp = static_cast<int16_t>(exp);
    uint32_t round_bit_num = bfloat16::FP32_MAN_LEN - bfloat16::BF16_MAN_LEN;
    uint32_t bf16_mantissa = mantissa >> round_bit_num;
    uint32_t round_part = mantissa & ((1 << round_bit_num) - 1);

    HandleRound<bfloat16_t, float, rnd>(sign, bf16_exp, bf16_mantissa, round_part);
    res = (sign << bfloat16::BF16_SIGN_INDEX) | (bf16_exp << bfloat16::BF16_MAN_LEN) |
          (bf16_mantissa & bfloat16::BF16_MAN_MASK);
    (void)HandleOverflow<rst, bfloat16_t>(sign, bf16_exp, res);

    return *reinterpret_cast<bfloat16_t*>(&res);
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
half __cvt_half(SRC_TYPE src)
{
    if (__isnan(src)) {
        uint16_t half_nan = static_cast<uint16_t>(Fp16BasicParam::K_FP16_ABS_MAX);
        if constexpr (rst == RoundingSaturation::RS_DISABLE_VALUE) {
            return *reinterpret_cast<half*>(&half_nan);
        } else {
            return half(0);
        }
    }

    if constexpr (std::is_same_v<SRC_TYPE, bfloat16_t>) {
        float temp = __cvt_float<rnd, rst>(src);
        return __cvt_half<rnd, rst>(temp);
    }

    if constexpr (std::is_same_v<SRC_TYPE, half>) {
        float tmp = __cvt_float<rnd, rst>(src);
        tmp = __cvt_float<rnd, rst>(tmp);
        return __cvt_half<rnd, rst>(tmp);
    }
    return half(0);
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
bfloat16_t __cvt_bfloat16_t(SRC_TYPE src)
{
    if (__isnan(src)) {
        uint16_t bf16_nan = bfloat16::BF16_NAN;
        if constexpr (rst == RoundingSaturation::RS_DISABLE_VALUE) {
            return *reinterpret_cast<bfloat16_t*>(&bf16_nan);
        } else {
            return bfloat16_t(0);
        }
    }
    if constexpr (std::is_same_v<SRC_TYPE, half>) {
        float temp = __cvt_float<rnd, rst>(src);
        return __cvt_bfloat16_t<rnd, rst>(temp);
    }
    if constexpr (std::is_same_v<SRC_TYPE, bfloat16_t>) {
        float temp = __cvt_float<rnd, rst>(src);
        temp = __cvt_float<rnd, rst>(temp);
        return __cvt_bfloat16_t<rnd, rst>(temp);
    }
    return bfloat16_t(0);
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
int32_t __cvt_int32_t(SRC_TYPE x)
{
    static_assert(
        std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, float> || std::is_same_v<SRC_TYPE, bfloat16_t>,
        "src type can only be half/float/bfloat_t");

    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");

    static_assert(rst == RoundingSaturation::RS_ENABLE_VALUE, "sat type can only be: RS_ENABLE");
    if (__isnan(x)) {
        return 0;
    }
    if (__isinf(x)) {
        if (x > SRC_TYPE{0}) {
            return INT32_MAX;
        } else {
            return INT32_MIN;
        }
    }
    if constexpr (std::is_same_v<SRC_TYPE, float>) {
        return static_cast<int32_t>(__cvt_float<rnd>(x));
    } else if constexpr (std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, bfloat16_t>) {
        float f = __cvt_float<rnd>(x);
        return static_cast<int32_t>(__cvt_float<rnd>(f));
    }
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
uint32_t __cvt_uint32_t(SRC_TYPE x)
{
    static_assert(
        std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, float> || std::is_same_v<SRC_TYPE, bfloat16_t>,
        "src type can only be half/float/bfloat_t");

    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");

    static_assert(rst == RoundingSaturation::RS_ENABLE_VALUE, "sat type can only be: RS_ENABLE");
    if (__isnan(x) || x < SRC_TYPE{0}) {
        return 0;
    }
    if (__isinf(x) && x > SRC_TYPE{0}) {
        return UINT32_MAX;
    }
    if constexpr (std::is_same_v<SRC_TYPE, float>) {
        return static_cast<uint32_t>(__cvt_float<rnd>(x));
    } else if constexpr (std::is_same_v<SRC_TYPE, half> || std::is_same_v<SRC_TYPE, bfloat16_t>) {
        float f = __cvt_float<rnd>(x);
        return static_cast<uint32_t>(__cvt_float<rnd>(f));
    }
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
int64_t __cvt_int64_t(SRC_TYPE x)
{
    static_assert(std::is_same_v<SRC_TYPE, float>, "src type can only be float");
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");
    static_assert(rst == RoundingSaturation::RS_ENABLE_VALUE, "sat type can only be: RS_ENABLE");
    if (__isnan(x)) {
        return 0;
    }
    if (__isinf(x)) {
        if (x > SRC_TYPE{0}) {
            return INT64_MAX;
        } else {
            return INT64_MIN;
        }
    }
    if constexpr (std::is_same_v<SRC_TYPE, float>) {
        return static_cast<int64_t>(__cvt_float<rnd>(x));
    }
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
uint64_t __cvt_uint64_t(SRC_TYPE x)
{
    static_assert(std::is_same_v<SRC_TYPE, float>, "src type can only be float");
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");
    static_assert(rst == RoundingSaturation::RS_ENABLE_VALUE, "sat type can only be: RS_ENABLE");
    if constexpr (std::is_same_v<SRC_TYPE, float>) {
        if (__isnan(x) || x < SRC_TYPE{0}) {
            return 0;
        }
        if (__isinf(x) && x > SRC_TYPE{0}) {
            return UINT64_MAX;
        }
        return static_cast<uint64_t>(__cvt_float<rnd>(x));
    }
}

template <ROUND rnd, RoundingSaturation rst>
half __cvt_half(int32_t x)
{
    float temp = __cvt_float<rnd, rst>(x);
    return __cvt_half<rnd, rst>(temp);
}

template <ROUND rnd, RoundingSaturation rst>
half __cvt_half(uint32_t x)
{
    float temp = __cvt_float<rnd, rst>(x);
    return __cvt_half<rnd, rst>(temp);
}

template <ROUND rnd, RoundingSaturation rst>
bfloat16_t __cvt_bfloat16_t(int32_t x)
{
    float temp = __cvt_float<rnd, rst>(x);
    return __cvt_bfloat16_t<rnd, rst>(temp);
}

template <ROUND rnd, RoundingSaturation rst>
bfloat16_t __cvt_bfloat16_t(uint32_t x)
{
    float temp = __cvt_float<rnd, rst>(x);
    return __cvt_bfloat16_t<rnd, rst>(temp);
}

template <ROUND rnd = ROUND::CAST_ROUND, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE>
float2 __cvt_float2(hifloat8x2_t src)
{
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");
    float2 res{src.x.ToFloat(), src.y.ToFloat()};
    return res;
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
float2 __cvt_float2(SRC_TYPE src)
{
    static_assert(
        std::is_same_v<SRC_TYPE, half2> || std::is_same_v<SRC_TYPE, bfloat16x2_t> ||
            std::is_same_v<SRC_TYPE, float8_e4m3x2_t> || std::is_same_v<SRC_TYPE, float8_e5m2x2_t>,
        "src type can only be half2/bfloat16x2_t/float8_e4m3x2_t/float8_e5m2x2_t");
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");
    float2 res = {src.x.ToFloat(), src.y.ToFloat()};
    if constexpr (std::is_same_v<SRC_TYPE, half2> || std::is_same_v<SRC_TYPE, bfloat16x2_t>) {
        res = {__cvt_float(src.x), __cvt_float(src.y)};
    }
    return res;
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
bfloat16x2_t __cvt_bfloat16x2_t(SRC_TYPE src)
{
    static_assert(std::is_same_v<SRC_TYPE, float2>, "stc type can only be float2");
    static_assert(
        (rnd == ROUND::CAST_RINT) || (rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_FLOOR) ||
            (rnd == ROUND::CAST_CEIL) || (rnd == ROUND::CAST_TRUNC),
        "rnd type can only be: ROUND_R, ROUND_A, ROUND_F, ROUND_C,ROUND_Z");
    bfloat16x2_t tmp;
    tmp.x = __cvt_bfloat16_t<rnd, rst>(src.x);
    tmp.y = __cvt_bfloat16_t<rnd, rst>(src.y);
    return tmp;
}

template <
    ROUND rnd = ROUND::CAST_RINT, RoundingSaturation rst = RoundingSaturation::RS_DISABLE_VALUE, typename SRC_TYPE>
half2 __cvt_half2(SRC_TYPE src)
{
    half2 res;
    if constexpr (std::is_same_v<SRC_TYPE, hifloat8x2_t>) {
        res = {half(src.x.ToFloat()), half(src.y.ToFloat())};
    } else {
        res = {__cvt_half<rnd, rst>(src.x), __cvt_half<rnd, rst>(src.y)};
    }
    return res;
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
hifloat8x2_t __cvt_hifloat8x2_t(SRC_TYPE src)
{
    static_assert(
        std::is_same_v<SRC_TYPE, float2> || std::is_same_v<SRC_TYPE, half2>, "stc type can only be float2/half2");
    static_assert((rnd == ROUND::CAST_ROUND) || (rnd == ROUND::CAST_HYBRID), "rnd type can only be: ROUND_A, ROUND_H");
    hifloat8x2_t res{0.0, 0.0};
    if constexpr (std::is_same_v<SRC_TYPE, float2>) {
        res = {hifloat8_t(src.x), hifloat8_t(src.y)};
    } else if constexpr (std::is_same_v<SRC_TYPE, half2>) {
        float2 tmp{src.x.ToFloat(), src.y.ToFloat()};
        res = {hifloat8_t(tmp.x), hifloat8_t(tmp.y)};
    }
    return res;
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e4m3x2_t __cvt_float8_e4m3x2_t(SRC_TYPE src)
{
    static_assert(std::is_same_v<SRC_TYPE, float2>, "stc type can only be float2");
    static_assert(rnd == ROUND::CAST_RINT, "rnd type can only be: ROUND_R");
    float8_e4m3x2_t res{fp8_e4m3fn_t(src.x), fp8_e4m3fn_t(src.y)};
    return res;
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e5m2x2_t __cvt_float8_e5m2x2_t(SRC_TYPE src)
{
    static_assert(std::is_same_v<SRC_TYPE, float2>, "stc type can only be float2");
    static_assert(rnd == ROUND::CAST_RINT, "rnd type can only be: ROUND_R");
    float8_e5m2x2_t res{fp8_e5m2_t(src.x), fp8_e5m2_t(src.y)};
    return res;
}

inline half2 __float22half2_rz(float2 const x)
{
    half2 res;
    res.x = __cvt_half<ROUND::CAST_TRUNC, RoundingSaturation::RS_DISABLE_VALUE>(x.x);
    res.y = __cvt_half<ROUND::CAST_TRUNC, RoundingSaturation::RS_DISABLE_VALUE>(x.y);
    return res;
}

static float2 __half22float2(half2 const x)
{
    float2 res;
    res.x = __cvt_float<ROUND::CAST_RINT, RoundingSaturation::RS_DISABLE_VALUE>(x.x);
    res.y = __cvt_float<ROUND::CAST_RINT, RoundingSaturation::RS_DISABLE_VALUE>(x.y);
    return res;
}

namespace cce {
template <auto funcPtr, typename... Args>
void async_invoke(const dim3& dim, Args&&... args)
{
    AscendC::Simt::ThreadBlock& threadBlock = AscendC::Simt::ThreadBlock::GetBlockInstance();
    const uint32_t threadNum = 1024;
    threadBlock.Init(threadNum);
    auto func = [&args...]() { funcPtr(args...); };
    for (uint32_t i = 0; i < threadNum; i++) {
        threadBlock.Schedule(func, i);
    }
    threadBlock.FinishJobs();
}
} // namespace cce

enum L1CacheType : uint32_t { NON_CACHEABLE = 0, CACHEABLE = 1 };
enum class LD_L2CacheType : uint32_t { L2_CACHE_HINT_NORMAL_FV = 0 };
enum class ST_L2CacheType : uint32_t { L2_CACHE_HINT_NORMAL_FV = 0 };

template <
    LD_L2CacheType L2Cache = LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV,
    L1CacheType L1CacheType = L1CacheType::NON_CACHEABLE, typename T>
T __ldg(__gm__ T* address)
{
    return *address;
}
template <
    ST_L2CacheType L2Cache = ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV,
    L1CacheType L1CacheType = L1CacheType::NON_CACHEABLE, typename T>
void __stg(__gm__ T* address, T val)
{
    *address = val;
}

constexpr int32_t warpSize = 32;

static int32_t laneid() { return 0; }

static int32_t lanemask_lt() { return 0xFFFFFFFF; }

#endif
#endif

#endif
