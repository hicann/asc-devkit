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
 * \file kernel_bf16.cpp
 * \brief
 */
#include "kernel_bf16.h"
#include "kernel_fp32.h"
#include "kernel_utils.h"

namespace bfloat16 {
uint16_t Bf16T::FloatToBf16(const float& fVal) const
{
    float fpVal = fVal;
    uint32_t ui32Val = AscendC::GetScalarBitcodeValue<float, uint32_t>(fpVal); // 1:8:23bit sign:exp:man
    uint16_t sRet = Fp32ExtracSign(ui32Val);
    if (Fp32IsInf(ui32Val)) {
        return (BF16_EXP_MASK | (sRet << BF16_SIGN_INDEX));
    }

    if (Fp32IsNan(ui32Val)) {
        return BF16_ABS_MAX;
    }
    uint32_t expFp = Fp32ExtracExp(ui32Val);
    uint32_t manFp = (ui32Val & FP32_MAN_MASK); // 23 bit mantissa dont't need to care about denormal
    manFp = (manFp | FP32_MAN_HIDE_BIT);

    uint32_t mLenDelta = FP32_MAN_LEN - BF16_MAN_LEN;
    uint16_t eRet = static_cast<uint16_t>(expFp);
    uint16_t mRet = static_cast<uint16_t>(manFp >> mLenDelta);

    bool needRound = IsRoundOne(sRet, manFp, static_cast<uint16_t>(mLenDelta));
    if (needRound) {
        if ((eRet == (BF16_MAX_EXP - 1)) && ((mRet & BF16_MAN_MASK) == BF16_MAX_MAN)) {
            return (BF16_EXP_MASK | (sRet << BF16_SIGN_INDEX));
        }
        ++mRet;
    }

    if (mRet > (BF16_MAN_HIDE_BIT | BF16_MAN_MASK)) {
        ++eRet;
    }

    return Bf16Constructor(sRet, eRet, mRet);
}

static float Bf16ToFloat(const uint16_t& fpVal)
{
    float ret = 0.0f;
    uint32_t uret = 0;
    ConvertU32ToFp32 convertU32ToFp32;

    if (Bf16IsInf(fpVal)) {
        uret = FP32_POS_INF | (static_cast<uint32_t>(Bf16ExtracSign(fpVal)) << FP32_SIGN_INDEX);
        convertU32ToFp32.i = uret;
        ret = convertU32ToFp32.f;
        return ret;
    }

    if (Bf16IsNan(fpVal)) {
        uret = FP32_NAN;
        convertU32ToFp32.i = uret;
        ret = convertU32ToFp32.f;
        return ret;
    }

    uint32_t sRet = static_cast<uint32_t>((fpVal >> BF16_SIGN_INDEX) & 0x1);
    uint32_t eRet = static_cast<uint32_t>((fpVal >> BF16_MAN_LEN) & 0xff);

    eRet = static_cast<uint32_t>(
        (static_cast<int32_t>(eRet) - static_cast<int32_t>(BF16_EXP_BIAS)) + static_cast<int32_t>(FP32_EXP_BIAS));
    uint32_t mRet = static_cast<uint32_t>(fpVal & 0x7f);
    mRet = mRet << (FP32_MAN_LEN - BF16_MAN_LEN);

    uint32_t fVal = ((sRet & 0x1) << FP32_SIGN_INDEX);
    if ((fpVal & 0x7fff) != 0) {
        fVal |= ((eRet & 0xff) << FP32_MAN_LEN);
        fVal |= (mRet & 0x7fffff);
    }
    convertU32ToFp32.i = fVal;
    ret = convertU32ToFp32.f;
    return ret;
}

Bf16T& Bf16T::operator=(const Bf16T& fp)
{
    if (&fp == this) {
        return *this;
    }
    val = fp.val;
    return *this;
}

Bf16T& Bf16T::operator=(const float& fVal)
{
    val = FloatToBf16(fVal);
    return *this;
}

uint16_t Bf16T::Bf16Compute(uint16_t fp1, uint16_t fp2, uint16_t mode) const
{
    float fr = 0.0f;
    bool nanStatus = false;
    bool infStatus = false;
    if (Bf16IsNan(fp1) || Bf16IsNan(fp2) ||
        ((Bf16IsInf(fp1) && Bf16IsInf(fp2)) && (((fp1 ^ fp2) >> BF16_SIGN_INDEX) != 0))) {
        nanStatus = true;
    }
    if (Bf16IsInf(fp1) || Bf16IsInf(fp2)) {
        infStatus = true;
    }

    if (nanStatus) {
        return BF16_NAN; // nan
    } else if (infStatus) {
        return Bf16IsInf(fp1) ? static_cast<uint16_t>(BF16_INFINITY | (Bf16ExtracSign(fp1) << BF16_SIGN_INDEX)) :
                                static_cast<uint16_t>(BF16_INFINITY | (Bf16ExtracSign(fp2) << BF16_SIGN_INDEX));
    }

    float f1 = Bf16ToFloat(fp1);
    float f2 = Bf16ToFloat(fp2);
    switch (mode) {
        case 0:
            fr = f1 + f2;
            break;
        case 1:
            fr = f1 - f2;
            break;
    }

    uint16_t retBf16 = FloatToBf16(fr);
    return retBf16;
}

uint16_t Bf16T::Bf16Add(uint16_t fp1, uint16_t fp2) const
{
    uint16_t add = 0;
    return Bf16Compute(fp1, fp2, add);
}

uint16_t Bf16T::Bf16Sub(uint16_t fp1, uint16_t fp2) const
{
    uint16_t sub = 1;
    return Bf16Compute(fp1, fp2, sub);
}

// operate
Bf16T Bf16T::operator-() const
{
    Bf16T ret;
    ret.val = val ^ static_cast<uint16_t>(1u << BF16_SIGN_INDEX);
    return ret;
}

Bf16T Bf16T::operator+(const Bf16T fp) const
{
    uint16_t retVal = Bf16Add(val, fp.val);
    Bf16T ret;
    ret.val = retVal;
    return ret;
}

Bf16T Bf16T::operator-(const Bf16T fp) const
{
    uint16_t retVal = Bf16Sub(val, fp.val);
    Bf16T ret;
    ret.val = retVal;
    return ret;
}

Bf16T Bf16T::operator+=(const Bf16T fp)
{
    val = Bf16Add(val, fp.val);
    return *this;
}

Bf16T Bf16T::operator-=(const Bf16T fp)
{
    val = Bf16Sub(val, fp.val);
    return *this;
}

Bf16T::operator float() const { return Bf16ToFloat(val); }

float Bf16T::ToFloat() const { return Bf16ToFloat(val); }
} // namespace bfloat16
