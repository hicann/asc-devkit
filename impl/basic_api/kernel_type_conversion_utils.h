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
 * \file kernel_type_conversion_utils.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_type_conversion_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TYPE_CONVERSION_UTILS_H__
#endif
#ifndef ASCENDC_TYPE_CONVERSION_UTILS_H
#define ASCENDC_TYPE_CONVERSION_UTILS_H
#include "utils/kernel_utils_macros.h"
#include "utils/kernel_utils_ceil_oom_que.h"
#include "utils/kernel_utils_constants.h"
#include "utils/kernel_utils_mode.h"
#include "utils/kernel_utils_struct_confusion_pad.h"
#include "utils/kernel_utils_struct_dma_params.h"
#include "utils/kernel_utils_struct_norm_sort.h"
#include "utils/kernel_utils_struct_param.h"

namespace AscendC {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || \
                              (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 5101) || \
                              (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
constexpr uint32_t BF16_TO_FP32_MAN_LEN = 16;
constexpr uint32_t FP32_EXP_PART_MASK = 0x7F800000u;
constexpr uint32_t FP32_MAN_PART_MASK = 0x007FFFFFu;
__aicore__ inline constexpr bool IsFp32Inf(const uint32_t x)
{
    return (((x & FP32_EXP_PART_MASK) == FP32_EXP_PART_MASK) && ((x & FP32_MAN_PART_MASK) == 0));
}

__aicore__ inline constexpr bool IsFp32Nan(const uint32_t x)
{
    return (((x & FP32_EXP_PART_MASK) == FP32_EXP_PART_MASK) && ((x & FP32_MAN_PART_MASK) != 0));
}

__aicore__ inline constexpr bool IsBf16NeedRound(uint64_t man)
{
    constexpr uint16_t truncLen = 16;
    constexpr uint64_t mask0 = 0x1ul << truncLen;
    constexpr uint64_t mask1 = 0x1ul << (truncLen - 1);
    constexpr uint64_t mask2 = mask1 - 1;

    bool lastBit = ((man & mask0) > 0);      // Last bit after conversion
    bool truncHighBit = ((man & mask1) > 0); // Highest bit in the truncated part
    bool truncLeft = ((man & mask2) > 0);    // Truncated left part (except for the highest bit)

    return (truncHighBit && (truncLeft || lastBit));
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
namespace FPTranslation {
// HiFloat8 -> Fp32
#define HIF8_SIGN_INDEX (7)
#define HIF8_BIT6_INDEX (6)
#define HIF8_BIT5_INDEX (5)
#define HIF8_BIT4_INDEX (4)
#define HIF8_BIT3_INDEX (3)
#define HIF8_EXTRACT_SIGN(x) (((x) >> HIF8_SIGN_INDEX) & 0x1)
#define HIF8_EXTRACT_BIT6(x) (((x) >> HIF8_BIT6_INDEX) & 0x1)
#define HIF8_EXTRACT_BIT5(x) (((x) >> HIF8_BIT5_INDEX) & 0x1)
#define HIF8_EXTRACT_BIT4(x) (((x) >> HIF8_BIT4_INDEX) & 0x1)
#define HIF8_EXTRACT_BIT3(x) (((x) >> HIF8_BIT3_INDEX) & 0x1)

constexpr int8_t HIF8_NAN = 0x80;
constexpr int8_t HIF8_POS_INF = 0x6F;
constexpr int8_t HIF8_NEG_INF = 0xEF;
constexpr int8_t HIF8_BIT_LEN = 8;
constexpr uint32_t FP32_NAN = 0x7FFFFFFF;
constexpr uint32_t FP32_EXP_BIAS = 127;

#define HIF8_MAX (0x6E)
#define HIF8_NEG_MAX (0xEE)

#define FP32_MAX_MAN (0x7FFFFF)
#define FP32_POS_INF (0x7F800000)
#define FP32_NEG_INF (0xff800000)
constexpr uint32_t FP32_SIGN_INDEX = 31;
constexpr uint32_t FP32_MAN_LEN = 23;
constexpr uint32_t BIT_WIDTH = 22;

__aicore__ inline uint32_t Fp32Constructor(uint32_t s, uint32_t e, uint32_t m)
{
    return (((s) << FP32_SIGN_INDEX) | ((e) << FP32_MAN_LEN) | ((m)&FP32_MAX_MAN));
}

__aicore__ inline uint32_t Hif8ToFloatCommon(const int8_t input)
{
    uint32_t ret = 0;
    uint8_t inputSign = HIF8_EXTRACT_SIGN(input);
    uint8_t bit6 = HIF8_EXTRACT_BIT6(input);
    uint8_t bit5 = HIF8_EXTRACT_BIT5(input);
    uint8_t bit4 = HIF8_EXTRACT_BIT4(input);
    uint8_t bit3 = HIF8_EXTRACT_BIT3(input);
    uint8_t dBitWidth = 0;
    uint8_t expBitWidth = 0;
    uint8_t manBitWidth = 0;

    // special case
    if (input == 0x0) {
        return 0;
    }
    if (input == HIF8_NAN) {
        return FP32_NAN;
    }
    if (input == HIF8_POS_INF) {
        return FP32_POS_INF;
    }
    if (input == HIF8_NEG_INF) {
        return FP32_NEG_INF;
    }

    // denormal value m - 23
    if (((bit6 == 0) && (bit5 == 0) && (bit4 == 0) && (bit3 == 0))) {
        return Fp32Constructor(inputSign, (input & 0x7) - FP32_MAN_LEN + FP32_EXP_BIAS, 0);
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
    expMsb = (input >> (manBitWidth + expBitWidth - 1)) & 0x1;
    if (expBitWidth != 0) {
        exp = ((input >> manBitWidth) & (expBitMask)) | (1 << (expBitWidth - 1)); // (1xxx)
        if (expMsb != 0) {                                                        // -ve
            exp = -1 * exp;
        }
    }
    exp = exp + FP32_EXP_BIAS;

    // cal Man value
    uint32_t man = 0;
    uint8_t manBitMask = 0;
    for (uint8_t i = 0; i < manBitWidth; i++) {
        manBitMask |= 0x1 << i;
    }
    man = input & manBitMask;
    man = man << (BIT_WIDTH - manBitWidth + 1);

    ret = Fp32Constructor(inputSign, (exp & 0xff), man);
    return ret;
}

__aicore__ inline uint32_t Hif8ToFp32(const uint8_t fpVal) { return Hif8ToFloatCommon(fpVal); }

// FP8 (E5M2) -> Fp32
#define FP8_SIGN_INDEX (7)
#define FP8_T_NAN (0x7F)
#define FP8_MAX_MAN (0x7)

constexpr int16_t FP8E5M2_EXP_MASK = 0x7C;
constexpr int16_t FP8E5M2_MAN_MASK = 0x3;
constexpr uint32_t FP8E5M2_MAN_LEN = 2;
constexpr uint32_t FP8E5M2_EXP_BIAS = 15;

#define FP8E5M2_MAN_HIDE_BIT (0x4)
#define FP8E5M2_T_MAX (0x7B)
#define FP8E5M2_MAX_EXP (0x1F)
#define FP8E5M2_MAX_MAN (0x3)
#define FP8E5M2_INF (0X7C)
#define FP8E5M2_ABS_MAKS (0X7F)

__aicore__ inline uint8_t Fp8e5m2Constructor(uint16_t s, uint16_t e, uint16_t m)
{
    return (((s) << FP8_SIGN_INDEX) | ((e) << FP8E5M2_MAN_LEN) | ((m)&FP8E5M2_MAX_MAN));
}

__aicore__ inline bool Fp8e5m2IsNan(const uint16_t& x)
{
    return ((((x)&FP8E5M2_EXP_MASK) == FP8E5M2_EXP_MASK) && (((x)&FP8E5M2_MAN_MASK) != 0));
}

__aicore__ inline bool Fp8e5m2IsInf(const uint16_t& x)
{
    return ((x == static_cast<uint8_t>(0x7C)) || (x == static_cast<uint8_t>(0xFC))) ? true : false;
}

__aicore__ inline int8_t Fp8e5m2ExtracSign(int8_t x) { return (((x) >> FP8_SIGN_INDEX) & 0x1); }

__aicore__ inline int8_t Fp8e5m2ExtracExp(int8_t x) { return (((x) >> FP8E5M2_MAN_LEN) & 0x1F); }

__aicore__ inline int8_t Fp8e5m2ExtracMan(uint8_t x)
{
    return ((((x) >> 0) & 0x3) | (((((x) >> FP8E5M2_MAN_LEN) & 0x1F) > 0 ? 1 : 0) * 0x4));
}

__aicore__ inline void ExtractFp8e5m2(const int8_t val, uint8_t& s, int8_t& e, uint8_t& m)
{
    // 1.Extract
    s = Fp8e5m2ExtracSign(val);
    e = static_cast<int16_t>(Fp8e5m2ExtracExp(val));
    m = Fp8e5m2ExtracMan(val);

    // Denormal---useless
    if (e == static_cast<int16_t>(-FP8E5M2_EXP_BIAS)) {
        e = static_cast<int8_t>(-FP8E5M2_EXP_BIAS + 1);
    }
}

__aicore__ inline uint32_t Fp8e5m2ToFp32(const uint8_t fpVal)
{
    uint32_t ret = 0;

    if (fpVal == 0x0) {
        return 0x0;
    } else if (fpVal == static_cast<uint8_t>(0x80)) {
        return 0x80000000;
    }
    // no INF value for fp8
    if (Fp8e5m2IsNan(fpVal)) {
        return FP32_NAN;
    }
    // for E5M2 need process inf
    if (Fp8e5m2IsInf(fpVal)) {
        return ((Fp8e5m2ExtracSign(fpVal) << FP32_SIGN_INDEX) | FP32_POS_INF);
    }

    uint8_t fp8Sign = 0;
    uint8_t fp8Man = 0;
    int8_t fp8Exp = 0;
    ExtractFp8e5m2(fpVal, fp8Sign, fp8Exp, fp8Man);
    bool isDenormal = false;
    if (fp8Exp == 0) {
        isDenormal = true;
    }

    while ((fp8Man != 0) && ((fp8Man & FP8E5M2_MAN_HIDE_BIT) == 0)) {
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
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(fp8Exp + 1)) - FP8E5M2_EXP_BIAS) + FP32_EXP_BIAS;
        } else {
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(fp8Exp)) - FP8E5M2_EXP_BIAS) + FP32_EXP_BIAS;
        }
        mRet = fp8Man << (FP32_MAN_LEN - FP8E5M2_MAN_LEN);
    }
    // No overflow or underflow
    uint32_t fVal = Fp32Constructor(sRet, eRet, mRet);
    ret = fVal;
    return ret;
}

// FP8 (E4M3) -> Fp32
#define FP8_SIGN_INDEX (7)
#define FP8_T_MAX (0x7E)
#define FP8_T_NEG_MAX (0x8E)
#define FP8_T_NAN (0x7F)
constexpr uint32_t FP8E4M3_EXP_BIAS = 7;
constexpr uint32_t FP8E4M3_EXP_LEN = 4;
constexpr uint32_t FP8E4M3_MAN_LEN = 3;
#define FP8_MAX_EXP (0xF)
#define FP8_MAX_MAN (0x7)
#define FP8_MAN_HIDE_BIT (0x8)

__aicore__ inline bool Fp8e4m3IsNan(const int8_t& x)
{
    return (((x == static_cast<int8_t>(0x7F)) || (x == static_cast<int8_t>(0xFF))) ? true : false);
}

__aicore__ inline uint8_t Fp8e4m3Constructor(uint16_t s, uint16_t e, uint16_t m)
{
    return (((s) << FP8_SIGN_INDEX) | ((e) << FP8E4M3_MAN_LEN) | ((m)&FP8_MAX_MAN));
}

__aicore__ inline uint16_t Fp8e4m3ExtracSign(uint8_t x) { return (((x) >> FP8_SIGN_INDEX) & 0x1); }

__aicore__ inline uint16_t Fp8e4m3ExtracExp(uint8_t x) { return (((x) >> FP8E4M3_MAN_LEN) & 0xF); }

__aicore__ inline uint16_t Fp8e4m3ExtracMan(uint8_t x)
{
    return ((((x) >> 0) & 0x7) | (((((x) >> FP8E4M3_MAN_LEN) & 0xF) > 0 ? 1 : 0) * 0x8));
}

__aicore__ inline void ExtractFP8(const int8_t val, uint8_t& s, int8_t& e, uint8_t& m)
{
    // 1.Extract
    s = Fp8e4m3ExtracSign(val);
    e = static_cast<int16_t>(Fp8e4m3ExtracExp(val));
    m = Fp8e4m3ExtracMan(val);

    // Denormal
    if (e == static_cast<int16_t>(-FP8E4M3_EXP_BIAS)) {
        e = static_cast<int8_t>(-FP8E4M3_EXP_BIAS + 1);
    }
}

__aicore__ inline uint32_t Fp8e4m3ToFp32(const int8_t fpVal)
{
    uint32_t ret = 0;
    if (fpVal == 0x0) {
        return 0x0;
    }
    if (fpVal == static_cast<int8_t>(0x80)) {
        return 0x80000000;
    }
    // no INF value for fp8
    if (Fp8e4m3IsNan(fpVal)) {
        return FP32_NAN;
    }

    uint8_t hf8Sign;
    uint8_t hf8Man;
    int8_t hf8Exp;
    ExtractFP8(fpVal, hf8Sign, hf8Exp, hf8Man);
    bool isDenormal = false;
    if (hf8Exp == 0) {
        isDenormal = true;
    }

    while ((hf8Man != 0) && ((hf8Man & FP8_MAN_HIDE_BIT) == 0)) {
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
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(hf8Exp + 1)) - FP8E4M3_EXP_BIAS) + FP32_EXP_BIAS;
        } else {
            eRet = (static_cast<uint64_t>(static_cast<int64_t>(hf8Exp)) - FP8E4M3_EXP_BIAS) + FP32_EXP_BIAS;
        }
        mRet = hf8Man << (FP32_MAN_LEN - FP8E4M3_MAN_LEN);
    }
    // No overflow or underflow
    uint32_t fVal = Fp32Constructor(sRet, eRet, mRet);
    ret = fVal;
    return ret;
}

// Fp4e2m1 -> Bf16
const uint16_t Fp4e2m1ToBf16[16] = {0x0,    0x3F00, 0x3F80, 0x3FC0, 0x4000, 0x4040, 0x4080, 0x40C0,
                                    0x8000, 0xBF00, 0xBF80, 0xBFC0, 0xC000, 0xC040, 0xC080, 0xC0C0};

__aicore__ inline bfloat16_t Fp4e2m1ToBfloat(const uint8_t fpVal)
{
    uint8_t fp4Val = fpVal & 0xf;
    uint16_t ret = Fp4e2m1ToBf16[fp4Val];
    return GetScalarBitcodeValue<uint16_t, bfloat16_t>(ret);
}

// Fp4e1m2 -> Bf16
const uint16_t Fp4e1m2ToBf16[16] = {0x0,    0x3E80, 0x3F00, 0x3F40, 0x3F80, 0x3FA0, 0x3FC0, 0x3FE0,
                                    0x8000, 0xBE80, 0xBF00, 0xBF40, 0xBF80, 0xBFA0, 0xBFC0, 0xBFE0};

__aicore__ inline bfloat16_t Fp4e1m2ToBfloat(const uint8_t fpVal)
{
    uint8_t fp4Val = fpVal & 0xf;
    uint16_t ret = Fp4e1m2ToBf16[fp4Val];
    return GetScalarBitcodeValue<uint16_t, bfloat16_t>(ret);
}

__aicore__ inline float Bf16ToFp32(const bfloat16_t& bVal)
{
    bfloat16_t bNum = bVal;
    uint32_t uiNum = (GetScalarBitcodeValue<bfloat16_t, uint32_t>(bNum)) << BF16_TO_FP32_MAN_LEN;
    float fNum = AscendC::GetScalarBitcodeValue<uint32_t, float>(uiNum);
    return fNum;
}
} // namespace FPTranslation
#endif
#endif
} // namespace AscendC
#endif // ASCENDC_TYPE_CONVERSION_UTILS_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TYPE_CONVERSION_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TYPE_CONVERSION_UTILS_H__
#endif
