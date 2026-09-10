/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_type_conversion_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_TYPE_CONVERSION_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_5102_ASC_TYPE_CONVERSION_UTILS_H

#include "impl/utils/sys_macros.h"
namespace __asc_aicore {
template <typename T>
constexpr __aicore__ inline uint64_t get_scalar_bitcode_value(T scalarValue)
{
    union ScalarBitcode {
        __aicore__ ScalarBitcode() {}
        T input;
        uint64_t output;
    } data;

    data.input = scalarValue;
    return data.output;
}

template <typename T, typename U>
constexpr __aicore__ inline U get_scalar_bitcode_value(T scalarValue)
{
    union ScalarBitcode {
        __aicore__ ScalarBitcode() {}
        T input;
        U output;
    } data;

    data.input = scalarValue;
    return static_cast<U>(data.output);
}

namespace __fp_conv {
// HiFloat8 -> Fp32
constexpr uint32_t CONST_FP32_NAN = 0x7FFFFFFF;
constexpr uint32_t CONST_FP32_EXP_BIAS = 127;
constexpr uint32_t CONST_FP32_POS_INF = 0x7F800000;
constexpr uint32_t CONST_FP32_NEG_INF = 0xFF800000;
constexpr uint32_t CONST_FP32_MAN_LEN = 23;

__aicore__ inline uint32_t fp32_constructor(uint32_t s, uint32_t e, uint32_t m)
{
    constexpr uint32_t fp32_max_man = 0x7FFFFF;
    return (((s) << 31) | ((e) << 23) | ((m)&fp32_max_man));
}

__aicore__ inline uint32_t hif8_to_fp32(const uint8_t fpVal)
{
    constexpr uint8_t hif8Nan = 0x80;
    constexpr uint8_t hif8PosInf = 0x6F;
    constexpr uint8_t hif8NegInf = 0xEF;
    constexpr uint32_t bitWidth = 22;
    uint32_t ret = 0;
    uint8_t inputSign = (((fpVal) >> 7) & 0x1);
    uint8_t bit6 = (((fpVal) >> 6) & 0x1);
    uint8_t bit5 = (((fpVal) >> 5) & 0x1);
    uint8_t bit4 = (((fpVal) >> 4) & 0x1);
    uint8_t bit3 = (((fpVal) >> 3) & 0x1);
    uint8_t dBitWidth = 0;
    uint8_t expBitWidth = 0;
    uint8_t manBitWidth = 0;

    // special case
    if (fpVal == 0x0) {
        return 0;
    }
    if (fpVal == hif8Nan) {
        return CONST_FP32_NAN;
    }
    if (fpVal == hif8PosInf) {
        return CONST_FP32_POS_INF;
    }
    if (fpVal == hif8NegInf) {
        return CONST_FP32_NEG_INF;
    }

    // denormal value m - 23
    if (((bit6 == 0) && (bit5 == 0) && (bit4 == 0) && (bit3 == 0))) {
        return fp32_constructor(inputSign, (fpVal & 0x7) - CONST_FP32_MAN_LEN + CONST_FP32_EXP_BIAS, 0);
    }

    // DBitWidth, ManBitWidth, ExpBitWidth
    if (bit6 == 0) {
        if (bit5 == 0) {
            if (bit4 == 0) { // D = b000
                expBitWidth = 0;
                dBitWidth = 0x4;
            } else { // D = b001
                expBitWidth = 0x1;
                dBitWidth = 0x3;
            }
        } else { // D = b01
            expBitWidth = 0x2;
            dBitWidth = 0x2;
        }
    } else {
        if (bit5 == 0) { // D == b10
            expBitWidth = 0x3;
        } else { // D == b11
            expBitWidth = 0x4;
        }
        dBitWidth = 0x2;
    }
    manBitWidth = 0x8 - dBitWidth - expBitWidth - 1;

    uint8_t expBitMask = 0x0;
    // extract exp bits
    for (uint8_t i = 0; i < expBitWidth; i++) {
        expBitMask |= 0x1 << i; // e.g. expBitWidth = 4 -> 1111
    }

    // cal Exp value
    int8_t exp = 0;
    int8_t expMsb = 0;
    expMsb = (fpVal >> (manBitWidth + expBitWidth - 1)) & 0x1;
    if (expBitWidth != 0) {
        exp = ((fpVal >> manBitWidth) & (expBitMask)) | (1 << (expBitWidth - 1)); // (1xxx)
        if (expMsb != 0) {                                                        // -ve
            exp = -1 * exp;
        }
    }
    exp = exp + CONST_FP32_EXP_BIAS;

    // cal Man value
    uint32_t man = 0;
    uint8_t manBitMask = 0;
    for (uint8_t i = 0; i < manBitWidth; i++) {
        manBitMask |= 0x1 << i;
    }
    man = fpVal & manBitMask;
    man = man << (bitWidth - manBitWidth + 1);

    ret = fp32_constructor(inputSign, (exp & 0xff), man);
    return ret;
}

// FP8 (E5M2) -> Fp32
__aicore__ inline bool fp8e5m2_is_nan(const uint16_t& x)
{
    constexpr int16_t fp8e5m2ExpMask = 0x7C;
    constexpr int16_t fp8e5m2ManMask = 0x3;
    return ((((x)&fp8e5m2ExpMask) == fp8e5m2ExpMask) && (((x)&fp8e5m2ManMask) != 0));
}

__aicore__ inline bool fp8e5m2_is_inf(const uint16_t& x)
{
    return ((x == static_cast<uint8_t>(0x7C)) || (x == static_cast<uint8_t>(0xFC))) ? true : false;
}

__aicore__ inline int8_t fp8e5m2_extract_sign(int8_t x) { return (((x) >> 7) & 0x1); }

__aicore__ inline int8_t fp8e5m2_extract_exp(int8_t x) { return (((x) >> 2) & 0x1F); }

__aicore__ inline int8_t fp8e5m2_extract_man(uint8_t x)
{
    return ((((x) >> 0) & 0x3) | (((((x) >> 2) & 0x1F) > 0 ? 1 : 0) * 0x4));
}

__aicore__ inline void extract_fp8e5m2(const int8_t val, uint8_t& s, int8_t& e, uint8_t& m)
{
    constexpr uint32_t fp8e5m2ExpBias = 15;
    // 1.Extract
    s = fp8e5m2_extract_sign(val);
    e = static_cast<int16_t>(fp8e5m2_extract_exp(val));
    m = fp8e5m2_extract_man(val);

    // Denormal---useless
    if (e == static_cast<int16_t>(-fp8e5m2ExpBias)) {
        e = static_cast<int8_t>(-fp8e5m2ExpBias + 1);
    }
}

__aicore__ inline uint32_t fp8e5m2_to_fp32(const uint8_t fpVal)
{
    constexpr uint32_t fp8e5m2ExpBias = 15;
    constexpr uint32_t fp8e5m2ManLen = 2;
    constexpr int8_t fp8e5m2ManHideBit = 0x4;
    uint32_t ret = 0;
    if (fpVal == 0x0) {
        return 0x0;
    } else if (fpVal == static_cast<uint8_t>(0x80)) {
        return 0x80000000;
    }
    // no INF value for fp8
    if (fp8e5m2_is_nan(fpVal)) {
        return CONST_FP32_NAN;
    }
    // for E5M2 need process inf
    if (fp8e5m2_is_inf(fpVal)) {
        return ((fp8e5m2_extract_sign(fpVal) << 31) | CONST_FP32_POS_INF);
    }

    uint8_t fp8Sign = 0;
    uint8_t fp8Man = 0;
    int8_t fp8Exp = 0;
    extract_fp8e5m2(fpVal, fp8Sign, fp8Exp, fp8Man);
    bool isDenormal = false;
    if (fp8Exp == 0) {
        isDenormal = true;
    }

    while ((fp8Man != 0) && ((fp8Man & fp8e5m2ManHideBit) == 0)) {
        fp8Man <<= 1;
        fp8Exp--;
    }
    uint32_t eRet = 0;
    uint32_t mRet = 0;
    uint32_t sRet = fp8Sign;
    if (fp8Man == 0) {
        eRet = 0;
        mRet = 0;
    } else {
        if (isDenormal) {
            // denormal
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(fp8Exp + 1)) - fp8e5m2ExpBias) + CONST_FP32_EXP_BIAS;
        } else {
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(fp8Exp)) - fp8e5m2ExpBias) + CONST_FP32_EXP_BIAS;
        }
        mRet = fp8Man << (CONST_FP32_MAN_LEN - fp8e5m2ManLen);
    }
    // No overflow or underflow
    uint32_t fVal = fp32_constructor(sRet, eRet, mRet);
    ret = fVal;
    return ret;
}

// FP8 (E4M3) -> Fp32
constexpr uint32_t CONST_FP8E4M3_EXP_BIAS = 7;
constexpr uint32_t CONST_FP8E4M3_MAN_LEN = 3;

__aicore__ inline bool fp8e4m3_is_nan(const int8_t& x)
{
    return (((x == static_cast<int8_t>(0x7F)) || (x == static_cast<int8_t>(0xFF))) ? true : false);
}

__aicore__ inline uint16_t fp8e4m3_extract_sign(uint8_t x) { return (((x) >> 7) & 0x1); }

__aicore__ inline uint16_t fp8e4m3_extract_exp(uint8_t x) { return (((x) >> CONST_FP8E4M3_MAN_LEN) & 0xF); }

__aicore__ inline uint16_t fp8e4m3_extract_man(uint8_t x)
{
    return ((((x) >> 0) & 0x7) | (((((x) >> CONST_FP8E4M3_MAN_LEN) & 0xF) > 0 ? 1 : 0) * 0x8));
}

__aicore__ inline void extract_fp8(const int8_t val, uint8_t& s, int8_t& e, uint8_t& m)
{
    // 1.Extract
    s = fp8e4m3_extract_sign(val);
    e = static_cast<int16_t>(fp8e4m3_extract_exp(val));
    m = fp8e4m3_extract_man(val);

    // Denormal
    if (e == static_cast<int16_t>(-CONST_FP8E4M3_EXP_BIAS)) {
        e = static_cast<int8_t>(-CONST_FP8E4M3_EXP_BIAS + 1);
    }
}

__aicore__ inline uint32_t fp8e4m3_to_fp32(const int8_t fpVal)
{
    constexpr uint8_t fp8ManHideBit = 0x8;
    uint32_t ret = 0;
    if (fpVal == 0x0) {
        return 0x0;
    }
    if (fpVal == static_cast<int8_t>(0x80)) {
        return 0x80000000;
    }
    // no INF value for fp8
    if (fp8e4m3_is_nan(fpVal)) {
        return CONST_FP32_NAN;
    }

    uint8_t hf8Sign;
    uint8_t hf8Man;
    int8_t hf8Exp;
    extract_fp8(fpVal, hf8Sign, hf8Exp, hf8Man);
    bool isDenormal = false;
    if (hf8Exp == 0) {
        isDenormal = true;
    }

    while ((hf8Man != 0) && ((hf8Man & fp8ManHideBit) == 0)) {
        hf8Man <<= 1;
        hf8Exp--;
    }
    uint32_t eRet;
    uint32_t mRet;
    uint32_t sRet = hf8Sign;
    if (hf8Man == 0) {
        eRet = 0;
        mRet = 0;
    } else {
        if (isDenormal) {
            // denormal
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(hf8Exp + 1)) - CONST_FP8E4M3_EXP_BIAS) +
                   CONST_FP32_EXP_BIAS;
        } else {
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(hf8Exp)) - CONST_FP8E4M3_EXP_BIAS) + CONST_FP32_EXP_BIAS;
        }
        mRet = hf8Man << (CONST_FP32_MAN_LEN - CONST_FP8E4M3_MAN_LEN);
    }
    // No overflow or underflow
    uint32_t fVal = fp32_constructor(sRet, eRet, mRet);
    ret = fVal;
    return ret;
}

// Fp4e2m1 -> Bf16
__aicore__ inline bfloat16_t fp4e2m1_to_bfloat(const uint8_t fpVal)
{
    constexpr uint16_t fp4e2m1ToBf16[16] = {0x0,    0x3F00, 0x3F80, 0x3FC0, 0x4000, 0x4040, 0x4080, 0x40C0,
                                            0x8000, 0xBF00, 0xBF80, 0xBFC0, 0xC000, 0xC040, 0xC080, 0xC0C0};
    uint8_t fp4Val = fpVal & 0xf;
    uint16_t ret = fp4e2m1ToBf16[fp4Val];
    return get_scalar_bitcode_value<uint16_t, bfloat16_t>(ret);
}

// Fp4e1m2 -> Bf16
__aicore__ inline bfloat16_t fp4e1m2_to_bfloat(const uint8_t fpVal)
{
    constexpr uint16_t fp4e1m2ToBf16[16] = {0x0,    0x3E80, 0x3F00, 0x3F40, 0x3F80, 0x3FA0, 0x3FC0, 0x3FE0,
                                            0x8000, 0xBE80, 0xBF00, 0xBF40, 0xBF80, 0xBFA0, 0xBFC0, 0xBFE0};
    uint8_t fp4Val = fpVal & 0xf;
    uint16_t ret = fp4e1m2ToBf16[fp4Val];
    return get_scalar_bitcode_value<uint16_t, bfloat16_t>(ret);
}

__aicore__ inline float bf16_to_fp32(const bfloat16_t& bVal)
{
    bfloat16_t bNum = bVal;
    uint32_t uiNum = (get_scalar_bitcode_value<bfloat16_t, uint32_t>(bNum)) << 16;
    float fNum = get_scalar_bitcode_value<uint32_t, float>(uiNum);
    return fNum;
}
} // namespace __fp_conv

template <typename T>
__aicore__ constexpr inline float cast_type(const T& bVal)
{
    uint8_t uiNum = 0;
    uint32_t result = 0;
    float fNum = 0;

    if constexpr (std::is_same<T, bfloat16_t>::value) {
        fNum = __fp_conv::bf16_to_fp32(bVal);
    } else if constexpr (std::is_same<T, hifloat8_t>::value) {
        uiNum = get_scalar_bitcode_value<hifloat8_t, uint8_t>(bVal);
        result = __fp_conv::hif8_to_fp32(uiNum);
        return get_scalar_bitcode_value<uint32_t, float>(result);
    } else if constexpr (std::is_same<T, float8_e5m2_t>::value) {
        uiNum = get_scalar_bitcode_value<float8_e5m2_t, uint8_t>(bVal);
        result = __fp_conv::fp8e5m2_to_fp32(uiNum);
        return get_scalar_bitcode_value<uint32_t, float>(result);
    } else if constexpr (std::is_same<T, float8_e4m3_t>::value) {
        uiNum = get_scalar_bitcode_value<float8_e4m3_t, uint8_t>(bVal);
        result = __fp_conv::fp8e4m3_to_fp32(uiNum);
        return get_scalar_bitcode_value<uint32_t, float>(result);
    } else if constexpr (std::is_same<T, float4_e1m2x2_t>::value) {
        uiNum = get_scalar_bitcode_value<float4_e1m2x2_t, uint8_t>(bVal);
        bfloat16_t bf16Val = __fp_conv::fp4e1m2_to_bfloat(uiNum);
        return __fp_conv::bf16_to_fp32(bf16Val);
    } else {
        uiNum = get_scalar_bitcode_value<T, uint8_t>(bVal);
        bfloat16_t bf16Val = __fp_conv::fp4e2m1_to_bfloat(uiNum);
        return __fp_conv::bf16_to_fp32(bf16Val);
    }
    return fNum;
}

template <typename T>
__aicore__ constexpr inline float to_float(const T& bVal)
{
    return cast_type<T>(bVal);
}
} // namespace __asc_aicore
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_TYPE_CONVERSION_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_TYPE_CONVERSION_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_TYPE_CONVERSION_UTILS_H
