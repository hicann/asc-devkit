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
 * \file kernel_simt_math_impl.h
 * \brief
 */
#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_MATH_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_MATH_IMPL_H

#if defined(ASCENDC_CPU_DEBUG)
#include <cmath>

#include "../../../basic_api/kernel_utils.h"
#include "stub_def.h"
#endif
#include "impl/simt_api/cpp/dav_3510/kernel_simt_constant.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_common_impl.h"

namespace AscendC {
namespace Simt {

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AbsImpl(T x)
{
    return abs(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t AbsImpl(int64_t x) { return llabs(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float AbsImpl(float x) { return fabs(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half AbsImpl(half x)
{
    half res = fabs(static_cast<float>(x));
    return res;
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AbsImpl(T x)
{
    if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, float> || std::is_same_v<T, int64_t>) {
        return abs(x);
    } else if constexpr (std::is_same_v<T, half>) {
        uint16_t bits = *reinterpret_cast<uint16_t*>(&x);
        bits &= 0x7FFF;
        return *reinterpret_cast<half*>(&bits);
    }
}
#endif

#ifdef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T UMulHi(T dividend, T magic)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t, uint64_t>, "Input type T only supports uint32_t, uint64_t.");

    if constexpr (std::is_same<T, uint32_t>::value) {
        return (static_cast<uint64_t>(dividend) * static_cast<uint64_t>(magic)) >> ConstantsInternal::FOUR_BYTE_LEN;
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        uint64_t dividendHigh = dividend >> ConstantsInternal::FOUR_BYTE_LEN;
        uint64_t dividendLow = dividend & ConstantsInternal::FULL_MASK_B32;
        uint64_t magicHigh = magic >> ConstantsInternal::FOUR_BYTE_LEN;
        uint64_t magicLow = magic & ConstantsInternal::FULL_MASK_B32;

        uint64_t dividendLowMagicLow = dividendLow * magicLow;
        uint64_t dividendLowMagicLowHigh = dividendLowMagicLow >> ConstantsInternal::FOUR_BYTE_LEN;

        uint64_t dividendHighMagicLow = dividendHigh * magicLow;
        uint64_t dividendHighMagicLowHigh = dividendHighMagicLow >> ConstantsInternal::FOUR_BYTE_LEN;
        uint64_t dividendHighMagicLowLow = dividendHighMagicLow & ConstantsInternal::FULL_MASK_B32;

        uint64_t dividendLowMagicHigh = dividendLow * magicHigh;
        uint64_t dividendLowMagicHighHigh = dividendLowMagicHigh >> ConstantsInternal::FOUR_BYTE_LEN;
        uint64_t dividendLowMagicHighLow = dividendLowMagicHigh & ConstantsInternal::FULL_MASK_B32;

        uint64_t dividendHighMagicHigh = dividendHigh * magicHigh;

        uint64_t bitFrom32To63 = dividendLowMagicLowHigh + dividendHighMagicLowLow + dividendLowMagicHighLow;

        return dividendHighMagicHigh + dividendHighMagicLowHigh + dividendLowMagicHighHigh +
               (bitFrom32To63 >> ConstantsInternal::FOUR_BYTE_LEN);
    }
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T UintDivImpl(T dividend, T magic, T shift)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t, uint64_t>, "Input type T only supports uint32_t, uint64_t.");
#ifdef ASCENDC_CPU_DEBUG
    if constexpr (std::is_same<T, uint32_t>::value) {
        ASCENDC_ASSERT(dividend <= ConstantsInternal::U32_MAX_VAL, {
            KERNEL_LOG(KERNEL_ERROR, "dividend must not be greater than UINT32_MAX");
        });
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        ASCENDC_ASSERT(dividend <= ConstantsInternal::U64_MAX_VAL, {
            KERNEL_LOG(KERNEL_ERROR, "dividend must not be greater than UINT_64_MAX");
        });
    }

    T q = UMulHi(dividend, magic);
#else
    T q = 0;
    if constexpr (std::is_same<T, uint32_t>::value) {
        q = __umulhi(dividend, magic);
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        q = __umul64hi(dividend, magic);
    }
#endif

    T sum = dividend + q;
    return sum >> shift;
}

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FmaImpl(T x, T y, T z)
{
    if (IsNanImpl(z) || IsNanImpl(x) || IsNanImpl(y)) {
        return NAN;
    }
    return ((double)x * (double)y) + (double)z;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half FmaImpl(half x, half y, half z)
{
    if (IsNanImpl(z) || IsNanImpl(x) || IsNanImpl(y)) {
        return NAN;
    }
    return (static_cast<float>(x) * static_cast<float>(y)) + static_cast<float>(z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float FmaImpl(float x, float y, float z)
{
    if (IsNanImpl(z) || IsNanImpl(x) || IsNanImpl(y)) {
        return NAN;
    }
    return std::fmaf(x, y, z);
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T FmaImpl(T x, T y, T z)
{
    return x * y + z;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float FmaImpl(float x, float y, float z) { return __fma(x, y, z); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half FmaImpl(half x, half y, half z) { return __fma(x, y, z); }
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MaxImpl(T x, T y)
{
    return std::max(x, y);
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MaxImpl(T x, T y)
{
    if constexpr (
        std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t> || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
        return max(x, y);
    } else if constexpr (std::is_same_v<T, float>) {
        if (IsNan(x)) {
            return y;
        } else if (IsNan(y)) {
            return x;
        }
        return __fmaxf(x, y);
    } else if constexpr (std::is_same_v<T, half>) {
        if (IsNan(x)) {
            return y;
        } else if (IsNan(y)) {
            return x;
        }
        return __hmax_nan(x, y);
    }
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MinImpl(T x, T y)
{
    return std::min(x, y);
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MinImpl(T x, T y)
{
    if constexpr (
        std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t> || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
        return min(x, y);
    } else if constexpr (std::is_same_v<T, float>) {
        if (IsNan(x)) {
            return y;
        } else if (IsNan(y)) {
            return x;
        }
        return __fminf(x, y);
    } else if constexpr (std::is_same_v<T, half>) {
        if (IsNan(x)) {
            return y;
        } else if (IsNan(y)) {
            return x;
        }
        return __hmin_nan(x, y);
    }
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float DimImpl(float x, float y)
{
    if (IsNanImpl(x)) {
        return x;
    } else if (IsNanImpl(y)) {
        return y;
    }
    return (x > y) ? (x - y) : 0;
}

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RemQuoImpl(float x, float y, int* quo)
{
    *quo = 0;
    int32_t negE = -8;
    int32_t maxS32 = 0xffffffff;
    int32_t one = 1;
    int32_t low3bit = 0x7;
    int32_t Max3Bit = 7;
    float remainder = remquo(x, y, quo);
    if (*quo < -Max3Bit || *quo > Max3Bit) {
        if ((x <= 0 && y <= 0) || (x >= 0 && y >= 0)) {
            *quo = *quo & low3bit;
        } else {
            *quo = *quo ^ maxS32;
            *quo = *quo | negE;
            *quo = *quo + one;
        }
    }
    return remainder;
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SetNegX(float absX) { return -absX; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SubSetResPos(float absX, float absY)
{
    return (absX < absY) ? absX - absY : absY - absX;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void SetQuo(int32_t* quo, int32_t nSign)
{
    int32_t negE = -8;
    int32_t maxS32 = 0xffffffff;
    int32_t one = 1;
    int32_t low3bit = 0x7;

    if (nSign < 0) {
        *quo = *quo ^ maxS32;
        *quo = *quo | negE;
        *quo = *quo + one;
    } else {
        *quo = *quo & low3bit;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float XLeY(
    float absX, float tmpVal, float absY, bool isXPos, uint32_t signFlag, float res, int32_t* quo, int32_t nSign)
{
    float doubleX = absX + absX;
    float sign = (isXPos) ? 1.0 : -1.0;

    if (doubleX > absY) {
        *quo += 1;
        SetQuo(quo, nSign);
        return sign * SubSetResPos(absX, absY);
    }

    if ((doubleX != absY) | (signFlag == 0)) {
        SetQuo(quo, nSign);
        if (isXPos) {
            return res;
        } else {
            return SetNegX(absX);
        }
    }
    *quo += 1;
    SetQuo(quo, nSign);
    return sign * SubSetResPos(absX, absY);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RemQuoImpl(float x, float y, int* quo)
{
    bool isXPos = x >= 0;
    float absX = AbsImpl(x);
    float absY = AbsImpl(y);
    bool isXInf = absX > ConstantsInternal::SIMT_FP32_INF || IsNanImpl(x);
    bool isYInf = absY > ConstantsInternal::SIMT_FP32_INF || IsNanImpl(y);
    *quo = 0;
    int32_t nSign = ((x <= 0 && y <= 0) || (x >= 0 && y >= 0)) ? 1 : -1;
    float res = x + y;
    if (isXInf | isYInf) {
        return res;
    }

    res = ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    if ((absX == ConstantsInternal::SIMT_FP32_INF) || (absY == 0)) {
        return res;
    }

    float tmpVal = 0.0;
    uint32_t signFlag = 0;
    if (absX < absY) {
        res = x;
        return XLeY(absX, tmpVal, absY, isXPos, signFlag, res, quo, nSign);
    }

    uint32_t* uAbsY = (uint32_t*)(&absY);
    uint32_t uY = (*uAbsY) & ConstantsInternal::MAN_BIT_FLOAT;
    uint32_t* uAbsX = (uint32_t*)(&absX);
    uint32_t uX = (*uAbsX) & ConstantsInternal::EXP_BIT_FLOAT;
    float xYVal = 0.0;
    uint32_t* uf26 = (uint32_t*)(&xYVal);
    *uf26 = uY | uX;
    bool isGtAbsX = xYVal > absX && !IsNanImpl(xYVal);
    res = 0.0;
    float nXYVal = (isGtAbsX) ? (xYVal * 0.5f) : xYVal;
    if (absX == nXYVal && !IsNanImpl(nXYVal)) {
        return res;
    }

    tmpVal = 0.0;
    res = absX;
    *quo = 0;
    if (nXYVal < absY || IsNanImpl(nXYVal)) {
        return XLeY(absX, tmpVal, absY, isXPos, signFlag, res, quo, nSign);
    }

    bool isXLtXy = absX < nXYVal;
    signFlag = 0;
    bool isXyGeY = true;
    float negTwo = -2.0;
    float posTwo = 2.0;
    int32_t n = 0;
    while (isXyGeY) {
        n = n + n;
        if (isXLtXy) {
            nXYVal = nXYVal * 0.5f;
            isXyGeY = nXYVal >= absY;
            if (isXyGeY) {
                isXLtXy = absX < nXYVal;
                signFlag = 0;
                continue;
            }
            break;
        }
        tmpVal = (posTwo * absX) + (nXYVal * negTwo);
        absX = absX - nXYVal;
        signFlag = 1;
        n += 1;
        nXYVal = nXYVal * 0.5f;
        isXyGeY = nXYVal >= absY;
        if (isXyGeY) {
            isXLtXy = absX < nXYVal;
            signFlag = 0;
        }
    }
    res = absX;
    *quo = n;

    return XLeY(absX, tmpVal, absY, isXPos, signFlag, res, quo, nSign);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ModImpl(float x, float y) { return fmodf(x, y); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SetResModNeg(float modRes)
{
    uint32_t* uModRes = (uint32_t*)(&modRes);
    *uModRes = (*uModRes) | ConstantsInternal::NEG_SIGN_BIT;
    return modRes;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ModImpl(float x, float y)
{
    bool isXPos = x > 0;
    float absX = AbsImpl(x);
    float absY = AbsImpl(y);
    bool isXNan = IsNanImpl(x);
    bool isYNan = IsNanImpl(y);

    bool isInfNotNan = IsInfImpl(absX) && !isXNan;
    bool isZeroNotNan = (absY == 0) && !isYNan;
    if (isInfNotNan | isZeroNotNan) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    if (isYNan || isXNan || absX < absY) {
        bool gtInfOrNan = (absY > ConstantsInternal::SIMT_FP32_INF) || isXNan || isYNan;
        float xyVal = (gtInfOrNan) ? (x + y) : x;
        bool ltZeroOrNan = (absX <= 0) || isXNan;
        return (ltZeroOrNan) ? (xyVal + x) : xyVal;
    }

    uint32_t* uAbsY = (uint32_t*)&absY;
    uint32_t yManBits = (*uAbsY) & ConstantsInternal::MAN_BIT_FLOAT;
    uint32_t* uAbsX = (uint32_t*)(&absX);
    uint32_t xExpBits = (*uAbsX) & ConstantsInternal::EXP_BIT_FLOAT;
    uint32_t xyBits = yManBits | xExpBits;

    float xyVal = 0;
    uint32_t* uxyVal = (uint32_t*)&xyVal;
    *uxyVal = xyBits;
    bool isGtX = (xyVal > absX) && !IsNanImpl(xyVal) && !isXNan;
    float halfXyVal = xyVal * 0.5f;
    xyVal = (isGtX) ? halfXyVal : xyVal;
    float modRes = absX;

    if (xyVal < absY || IsNanImpl(xyVal) || isYNan) {
        if (!isXPos) {
            return SetResModNeg(modRes);
        }
        return modRes;
    }
    float subTmp;
    bool xyValGeY = true;
    bool cmpTmp;
    while (xyValGeY) {
        subTmp = modRes - xyVal;
        cmpTmp = modRes < xyVal || IsNanImpl(modRes) || IsNanImpl(xyVal);
        modRes = (cmpTmp) ? modRes : subTmp;
        xyVal = xyVal * 0.5f;
        xyValGeY = (xyVal >= absY) || IsNanImpl(xyVal) || isYNan;
    }
    if (!isXPos) {
        return SetResModNeg(modRes);
    }
    return modRes;
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RemainderImpl(float x, float y) { return remainder(x, y); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float RemainderImpl(float x, float y)
{
    int32_t quo = -1;
    return RemQuoImpl(x, y, &quo);
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CopySignImpl(float x, float y)
{
    return (y >= 0) ? AbsImpl(x) : -AbsImpl(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float NearByIntImpl(float x)
{
    if (IsInfImpl(x) || IsNanImpl(x)) {
        return x;
    }
    return RintImpl(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float NextAfterImpl(float x, float y)
{
    if (IsNanImpl(x) || IsNanImpl(y)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    uint32_t* f = (uint32_t*)&x;
    if (x > 0) {
        if (x < y) { // when x < src, x bit +1
            (*f)++;
        } else if (x > y) { // when x > src, x bit -1
            (*f)--;
        }
    } else if (x < 0) {
        if (x > y) {
            (*f)++;
        } else if (x < y) {
            (*f)--;
        }
    } else if (x == 0) {
        if (y > 0) {
            *f = 1;
        } else if (y < 0) {
            *f = 0x80000001;
        }
    }
    return x;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ScaLbnImpl(float x, int n)
{
    if (IsInfImpl(x) || IsNanImpl(x)) {
        return x;
    } else if (x == 0) {
        return x;
    }

    float two = 2.0;
    float fp32ExponentMidVal = 127;
#if defined(ASCENDC_CPU_DEBUG)
    if (n < 0) {
        n = -n;
        if (n > fp32ExponentMidVal) {
            int mulValExp = n - fp32ExponentMidVal;
            n = fp32ExponentMidVal;
            x = x / powf(two, static_cast<float>(mulValExp));
        }
        return x / powf(two, n);
    }
    if (n > fp32ExponentMidVal) {
        int mulValExp = n - fp32ExponentMidVal;
        n = fp32ExponentMidVal;
        x = x * powf(two, static_cast<float>(mulValExp));
    }
    return x * powf(two, static_cast<float>(n));
#else
    if (n < 0) {
        n = -n;
        if (n > fp32ExponentMidVal) {
            int mulValExp = n - fp32ExponentMidVal;
            n = fp32ExponentMidVal;
            x = x / __powf(two, static_cast<float>(mulValExp));
        }
        return x / __powf(two, n);
    }
    if (n > fp32ExponentMidVal) {
        int mulValExp = n - fp32ExponentMidVal;
        n = fp32ExponentMidVal;
        x = x * __powf(two, static_cast<float>(mulValExp));
    }
    return x * __powf(two, static_cast<float>(n));
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ScaLbnImpl(float x, long int n)
{
    return ScaLbnImpl(x, static_cast<int>(n));
}

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t BrevImpl(uint32_t x)
{
    uint32_t reversedX = 0;
    for (int i = 0; i < ConstantsInternal::FOUR_BYTE_LEN; ++i) {
        reversedX <<= ConstantsInternal::ONE_UINT32;
        reversedX |= (x & 1);
        x >>= 1;
    }
    return reversedX;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t BrevImpl(uint64_t x)
{
    uint64_t reversedX = 0;
    for (int i = 0; i < ConstantsInternal::EIGHT_BYTE_LEN; ++i) {
        reversedX <<= ConstantsInternal::ONE_UINT64;
        reversedX |= (x & 1);
        x >>= 1;
    }
    return reversedX;
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t BrevImpl(uint64_t x)
{
    return __brev(static_cast<unsigned long long>(x));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t BrevImpl(uint32_t x) { return __brev(x); }
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzIntrinsics(uint8_t bitLen, T x, T one)
{
    int32_t count = 0;
    for (int i = 0; i < bitLen; i++) {
        T tmp = one << (bitLen - 1 - i);
        if (((tmp & x) >> (bitLen - 1 - i)) == 0) {
            count += 1;
        } else {
            break;
        }
    }
    return count;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzImpl(T x)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, int64_t, uint32_t, uint64_t>,
        "Input type of Clz function only supports int32_t, uint32_t, int64_t, uint64_t.");
    if constexpr (SupportTypeSimtInternel<T, uint32_t>) {
        return ClzIntrinsics(ConstantsInternal::FOUR_BYTE_LEN, x, ConstantsInternal::ONE_UINT32);
    } else if constexpr (SupportTypeSimtInternel<T, uint64_t>) {
        return ClzIntrinsics(ConstantsInternal::EIGHT_BYTE_LEN, x, ConstantsInternal::ONE_UINT64);
    } else if constexpr (SupportTypeSimtInternel<T, int32_t>) {
        return ClzIntrinsics(ConstantsInternal::FOUR_BYTE_LEN, x, ConstantsInternal::ONE_INT32);
    } else if constexpr (SupportTypeSimtInternel<T, int64_t>) {
        return ClzIntrinsics(ConstantsInternal::EIGHT_BYTE_LEN, x, ConstantsInternal::ONE_INT64);
    }
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzIntrinsics(uint32_t x) { return __clz(static_cast<int32_t>(x)); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzIntrinsics(int32_t x) { return __clz(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzIntrinsics(uint64_t x) { return __clz(static_cast<long long>(x)); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzIntrinsics(int64_t x) { return __clz(static_cast<long long>(x)); }

// count the leading zero bits
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t ClzImpl(T x)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, int64_t, uint32_t, uint64_t>,
        "Input type of Clz function only supports int32_t, uint32_t, int64_t, uint64_t.");
    return ClzIntrinsics(x);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t PopcIntrinsics(uint8_t bitLen, T x, T one)
{
    int32_t count = 0;
    for (int i = 0; i < bitLen; i++) {
        if (((x & (one << i)) >> i) == 1) {
            count += 1;
        }
    }
    return count;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t PopcImpl(uint32_t x)
{
    return PopcIntrinsics(ConstantsInternal::FOUR_BYTE_LEN, x, ConstantsInternal::ONE_UINT32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t PopcImpl(uint64_t x)
{
    return PopcIntrinsics(ConstantsInternal::EIGHT_BYTE_LEN, x, ConstantsInternal::ONE_UINT64);
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t PopcImpl(uint32_t x) { return __popc(static_cast<unsigned int>(x)); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t PopcImpl(uint64_t x)
{
    return __popc(static_cast<unsigned long long>(x));
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t BytePermImpl(uint32_t x, uint32_t y, uint32_t s)
{
    uint64_t tmp64 = (static_cast<uint64_t>(y) << ConstantsInternal::FOUR_BYTE_LEN) | x;
    uint8_t selector0 = (s & 0x7);
    uint8_t selector1 = (s >> 4) & 0x7;
    uint8_t selector2 = (s >> 8) & 0x7;
    uint8_t selector3 = (s >> 12) & 0x7;
    uint8_t byte0 = (tmp64 >> (selector0 * ConstantsInternal::ONE_BYTE_LEN)) & 0xFF;
    uint8_t byte1 = (tmp64 >> (selector1 * ConstantsInternal::ONE_BYTE_LEN)) & 0xFF;
    uint8_t byte2 = (tmp64 >> (selector2 * ConstantsInternal::ONE_BYTE_LEN)) & 0xFF;
    uint8_t byte3 = (tmp64 >> (selector3 * ConstantsInternal::ONE_BYTE_LEN)) & 0xFF;
    return byte0 | (byte1 << ConstantsInternal::ONE_BYTE_LEN) | (byte2 << ConstantsInternal::TWO_BYTE_LEN) |
           (byte3 << ConstantsInternal::THREE_BYTE_LEN);
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t BytePermImpl(uint32_t x, uint32_t y, uint32_t s)
{
    return __byte_perm(x, y, s);
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t FfsImpl(int32_t x)
{
    if (x == 0) {
        return 0;
    }
    int lsb = x & (~x + 1);
    return __builtin_ctz(lsb) + 1;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t FfsImpl(int64_t x)
{
    if (x == 0) {
        return 0;
    }
    int lsb = x & (~x + 1);
    return __builtin_ctz(lsb) + 1;
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t FfsImpl(int32_t x) { return __ffs(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t FfsImpl(int64_t x) { return __ffs(static_cast<long long>(x)); }
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MulHiImpl(T x, T y)
{
    if constexpr (std::is_same_v<T, uint32_t>) {
        uint64_t src0 = static_cast<uint64_t>(x);
        uint64_t src1 = static_cast<uint64_t>(y);
        uint64_t dst = src0 * src1;
        dst = dst >> ConstantsInternal::FOUR_BYTE_LEN;
        return static_cast<uint32_t>(dst);
    } else {
        int64_t src0 = static_cast<int64_t>(x);
        int64_t src1 = static_cast<int64_t>(y);
        int64_t dst = src0 * src1;
        dst = dst >> ConstantsInternal::FOUR_BYTE_LEN;
        return static_cast<int32_t>(dst);
    }
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t MulHiImpl(uint32_t x, uint32_t y) { return __umulhi(x, y); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t MulHiImpl(int32_t x, int32_t y) { return __mulhi(x, y); }
#endif
} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_MATH_IMPL_H
