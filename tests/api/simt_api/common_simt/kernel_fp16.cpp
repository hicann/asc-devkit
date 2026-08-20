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
 * \file kernel_fp16.cpp
 * \brief
 */
#include "kernel_fp16.h"

namespace {
constexpr uint16_t K_MAN_BIT_LENGTH = 11;
constexpr int16_t HFEXP_NUM = 31;
constexpr uint32_t ERET_NUM = 255;
} // namespace

// namespace float16 {
/**
 * @ingroup half global filed
 * @brief   round mode of last valid digital
 */
const enum TagFp16RoundMode ROUND_MODE = TagFp16RoundMode::K_ROUND_TO_NEAREST;

void ExtractFp16(const uint16_t& val, uint16_t& s, int16_t& e, uint16_t& m)
{
    // 1.Extract
    s = FP16_EXTRAC_SIGN(val);
    e = FP16_EXTRAC_EXP(val);
    m = FP16_EXTRAC_MAN(val);
    // Denormal
    if (e == 0) {
        e = 1;
    }
}

/**
 * @ingroup half static method
 * @param [in] man       truncated mantissa
 * @param [in] shiftOut left shift bits based on ten bits
 * @brief   judge whether to add one to the result while converting half to
 * other datatype
 * @return  Return true if add one, otherwise false
 */
static bool IsRoundOne(uint64_t man, uint16_t truncLen)
{
    uint64_t mask0 = 0x4;
    uint64_t mask1 = 0x2;
    uint64_t mask2;
    uint16_t shiftOut = static_cast<uint16_t>(truncLen - static_cast<uint16_t>(DimIndex::K_DIM2));
    mask0 = mask0 << shiftOut;
    mask1 = mask1 << shiftOut;
    mask2 = mask1 - 1;

    bool lastBit = ((man & mask0) > 0);
    bool truncHigh = false;
    bool truncLeft = false;
    if (ROUND_MODE == TagFp16RoundMode::K_ROUND_TO_NEAREST) {
        truncHigh = ((man & mask1) > 0);
        truncLeft = ((man & mask2) > 0);
    }
    return (truncHigh && (truncLeft || lastBit));
}

/**
 * @ingroup half public method
 * @param [in] exp       exponent of half value
 * @param [in] man       exponent of half value
 * @brief   normalize half value
 * @return
 */
static void Fp16Normalize(int16_t& exp, uint16_t& man)
{
    // set to invalid data
    if (exp >= static_cast<int16_t>(Fp16BasicParam::K_FP16_MAX_EXP)) {
        exp = static_cast<int16_t>(Fp16BasicParam::K_FP16_MAX_EXP);
        man = static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAX_MAN);
    } else if ((exp == 0) && (man == static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_HIDE_BIT))) {
        exp++;
        man = 0;
    }
}

/**
 * @ingroup half math conversion static method
 * @param [in] fpVal uint16_t value of half object
 * @brief   Convert half to float/fp32
 * @return  Return float/fp32 value of fpVal which is the value of half object
 */
static float Fp16ToFloat(const uint16_t& fpVal)
{
    uint16_t hfSign;
    uint16_t hfMan;
    int16_t hfExp;
    ExtractFp16(fpVal, hfSign, hfExp, hfMan);

    while ((hfMan != 0) && ((hfMan & static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_HIDE_BIT)) == 0)) {
        hfMan <<= 1;
        hfExp--;
    }

    uint32_t eRet;
    uint32_t mRet;
    uint32_t sRet = hfSign;

    if (hfExp == HFEXP_NUM) {
        eRet = ERET_NUM;
        mRet = hfMan
               << (static_cast<uint32_t>(Fp32BasicParam::K_FP32_MAN_LEN) -
                   static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_LEN));
        uint32_t fVal = FP32_CONSTRUCTOR(sRet, eRet, mRet);
        auto pRetV = reinterpret_cast<float*>(&fVal);

        return *pRetV;
    }

    if (hfMan == 0) {
        eRet = 0;
        mRet = 0;
    } else {
        eRet = (static_cast<uint32_t>(hfExp) - static_cast<uint32_t>(Fp16BasicParam::K_FP16_EXP_BIAS)) +
               static_cast<uint32_t>(Fp32BasicParam::K_FP32_EXP_BIAS);
        mRet = static_cast<uint32_t>(hfMan & static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_MASK));
        mRet = mRet
               << (static_cast<uint32_t>(Fp32BasicParam::K_FP32_MAN_LEN) -
                   static_cast<uint32_t>(Fp16BasicParam::K_FP16_MAN_LEN));
    }
    uint32_t fVal = FP32_CONSTRUCTOR(sRet, eRet, mRet);
    auto pRetV = reinterpret_cast<float*>(&fVal);

    return *pRetV;
}

static uint16_t Fp16AddCalVal(const uint16_t& sRet, int16_t eRet, uint16_t mRet, uint32_t mTrunc, uint16_t shiftOut)
{
    uint16_t mMin = static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_HIDE_BIT) << shiftOut;
    uint16_t mMax = mMin << 1;
    // Denormal
    while ((mRet < mMin) && (eRet > 0)) { // the value of mRet should not be smaller than 2^23
        mRet = mRet << 1;
        mRet += (static_cast<uint32_t>(Fp32BasicParam::K_FP32_SIGN_MASK) & mTrunc) >>
                static_cast<uint16_t>(Fp32BasicParam::K_FP32_SIGN_INDEX);
        mTrunc = mTrunc << 1;
        eRet = eRet - 1;
    }
    while (mRet >= mMax) { // the value of mRet should be smaller than 2^24
        mTrunc = mTrunc >> 1;
        mTrunc = mTrunc | (static_cast<uint32_t>(Fp32BasicParam::K_FP32_SIGN_MASK) * (mRet & 1));
        mRet = mRet >> 1;
        eRet = eRet + 1;
    }

    bool bLastBit = ((mRet & 1) > 0);
    bool bTruncHigh = (ROUND_MODE == TagFp16RoundMode::K_ROUND_TO_NEAREST) &&
                      ((mTrunc & static_cast<uint32_t>(Fp32BasicParam::K_FP32_SIGN_MASK)) > 0);
    bool bTruncLeft = (ROUND_MODE == TagFp16RoundMode::K_ROUND_TO_NEAREST) &&
                      ((mTrunc & static_cast<uint32_t>(Fp32BasicParam::K_FP32_ABS_MAX)) > 0);
    mRet = ManRoundToNearest(bLastBit, bTruncHigh, bTruncLeft, mRet, shiftOut);
    while (mRet >= mMax) {
        mRet = mRet >> 1;
        eRet = eRet + 1;
    }

    if ((eRet == 0) && (mRet <= mMax)) {
        mRet = mRet >> 1;
    }
    Fp16Normalize(eRet, mRet);
    uint16_t ret = FP16_CONSTRUCTOR(sRet, static_cast<uint16_t>(eRet), mRet);
    return ret;
}

/**
 * @ingroup half math operator
 * @param [in] v1 left operator value of half object
 * @param [in] v2 right operator value of half object
 * @brief   Performing half addition
 * @return  Return half result of adding this and fp
 */
static uint16_t Fp16Add(uint16_t v1, uint16_t v2)
{
    uint16_t sa;
    uint16_t sb;
    int16_t ea;
    int16_t eb;
    uint32_t ma;
    uint32_t mb;
    uint16_t maTmp;
    uint16_t mbTmp;
    uint16_t shiftOut = 0;
    // 1.Extract
    ExtractFp16(v1, sa, ea, maTmp);
    ExtractFp16(v2, sb, eb, mbTmp);
    ma = maTmp;
    mb = mbTmp;

    uint16_t sum;
    uint16_t sRet;
    if (sa != sb) {
        ReverseMan(sa > 0, ma);
        ReverseMan(sb > 0, mb);
        sum = static_cast<uint16_t>(GetManSum(ea, ma, eb, mb));
        sRet = (sum & static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_MASK)) >>
               static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_INDEX);
        ReverseMan(sRet > 0, ma);
        ReverseMan(sRet > 0, mb);
    } else {
        sum = static_cast<uint16_t>(GetManSum(ea, ma, eb, mb));
        sRet = sa;
    }

    if (sum == 0) {
        shiftOut = 3; // shift to left 3 bits
        ma = ma << shiftOut;
        mb = mb << shiftOut;
    }

    uint32_t mTrunc = 0;
    int16_t eRet = std::max(ea, eb);
    uint32_t eTmp = static_cast<uint32_t>(std::abs(ea - eb));
    if (ea > eb) {
        mTrunc = (mb << (static_cast<uint32_t>(BitShift::K_BIT_SHIFT32) - eTmp));
        mb = RightShift(mb, eTmp);
    } else if (ea < eb) {
        mTrunc = (ma << (static_cast<uint32_t>(BitShift::K_BIT_SHIFT32) - eTmp));
        ma = RightShift(ma, eTmp);
    }
    // calculate mantissav
    auto mRet = static_cast<uint16_t>(ma + mb);
    return Fp16AddCalVal(sRet, eRet, mRet, mTrunc, shiftOut);
}

// operate
half half::operator-() const
{
    half ret;
    ret.val = val ^ static_cast<uint16_t>(Fp16BasicParam::K_FP16_SIGN_MASK);
    return ret;
}

half half::operator+(const half fp) const
{
    uint16_t retVal = Fp16Add(val, fp.val);
    half ret;
    ret.val = retVal;
    return ret;
}
half half::operator+=(const half fp)
{
    val = Fp16Add(val, fp.val);
    return *this;
}
// compare
bool half::operator==(const half& fp) const
{
    bool result = true;
    if (FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val)) {
        result = true;
    } else {
        result =
            ((val & static_cast<uint16_t>(NumBitMax::K_BIT_LEN16_MAX)) ==
             (fp.val & static_cast<uint16_t>(NumBitMax::K_BIT_LEN16_MAX))); // bit compare
    }
    return result;
}
bool half::operator!=(const half& fp) const
{
    bool result = true;
    if (FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val)) {
        result = false;
    } else {
        result =
            ((val & static_cast<uint16_t>(NumBitMax::K_BIT_LEN16_MAX)) !=
             (fp.val & static_cast<uint16_t>(NumBitMax::K_BIT_LEN16_MAX))); // bit compare
    }
    return result;
}
static bool CmpPosNums(const uint16_t& ea, const uint16_t& eb, const uint16_t& ma, const uint16_t& mb)
{
    bool result = true;
    if (ea > eb) { // ea - eb >= 1; Va always larger than Vb
        result = true;
    } else if (ea == eb) {
        result = ma > mb;
    } else {
        result = false;
    }
    return result;
}
static bool CmpNegNums(const uint16_t& ea, const uint16_t& eb, const uint16_t& ma, const uint16_t& mb)
{
    bool result = true;
    if (ea < eb) {
        result = true;
    } else if (ea == eb) {
        result = ma < mb;
    } else {
        result = false;
    }
    return result;
}
bool half::operator>(const half& fp) const
{
    uint16_t sa;
    uint16_t sb;
    uint16_t ea;
    uint16_t eb;
    uint16_t ma;
    uint16_t mb;
    bool result = true;

    // 1.Extract
    sa = FP16_EXTRAC_SIGN(val);
    sb = FP16_EXTRAC_SIGN(fp.val);
    ea = static_cast<uint16_t>(FP16_EXTRAC_EXP(val));
    eb = static_cast<uint16_t>(FP16_EXTRAC_EXP(fp.val));
    ma = FP16_EXTRAC_MAN(val);
    mb = FP16_EXTRAC_MAN(fp.val);

    // Compare
    if ((sa == 0) && (sb > 0)) { // +  -
        // -0=0
        result = !(FP16_IS_ZERO(val) && FP16_IS_ZERO(fp.val));
    } else if ((sa == 0) && (sb == 0)) { // + +
        result = CmpPosNums(ea, eb, ma, mb);
    } else if ((sa > 0) && (sb > 0)) { // - -    opposite to  + +
        result = CmpNegNums(ea, eb, ma, mb);
    } else {
        // -  +
        result = false;
    }

    return result;
}

bool half::operator>=(const half& fp) const
{
    bool result = true;
    if (((*this) > fp) || ((*this) == fp)) {
        result = true;
    } else {
        result = false;
    }

    return result;
}

bool half::operator<=(const half& fp) const
{
    bool result = true;
    if ((*this) > fp) {
        result = false;
    }
    return result;
}

bool half::operator<(const half& fp) const
{
    bool result = true;
    if ((*this) >= fp) {
        result = false;
    }
    return result;
}

half half::operator++()
{
    half one = 1.0;
    val = Fp16Add(val, one.val);
    return *this;
}

half half::operator++(int)
{
    half oldBf = *this;
    operator++();
    return oldBf;
}

bool half::operator&&(const half fp) const { return (val != 0) && (fp.val != 0); }

bool half::operator||(const half fp) const { return (val != 0) || (fp.val != 0); }

uint16_t half::FloatToFp16(const float& fVal) const
{
    uint16_t sRet;
    uint16_t mRet;
    int16_t eRet;
    uint32_t ef;
    uint32_t mf;
    const uint32_t ui32V = *(reinterpret_cast<const uint32_t*>(&fVal)); // 1:8:23bit sign:exp:man
    uint32_t mLenDelta;

    sRet = static_cast<uint16_t>(
        (ui32V & static_cast<uint32_t>(Fp32BasicParam::K_FP32_SIGN_MASK)) >>
        static_cast<uint16_t>(Fp32BasicParam::K_FP32_SIGN_INDEX)); // 4Byte->2Byte
    ef = (ui32V & static_cast<uint32_t>(Fp32BasicParam::K_FP32_EXP_MASK)) >>
         static_cast<uint16_t>(Fp32BasicParam::K_FP32_MAN_LEN); // 8 bit exponent
    mf =
        (ui32V &
         static_cast<uint32_t>(Fp32BasicParam::K_FP32_MAN_MASK)); // 23 bit mantissa dont't need to care about denormal
    mLenDelta =
        static_cast<uint16_t>(Fp32BasicParam::K_FP32_MAN_LEN) - static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_LEN);

    bool needRound = false;
    // Exponent overflow/NaN converts to signed inf/NaN
    if (ef > 0x8Fu) { // 0x8Fu:142=127+15
        eRet = static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAX_EXP) - 1;
        mRet = static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAX_MAN);
    } else if (ef <= 0x70u) { // 0x70u:112=127-15 Exponent underflow converts to denormalized half or signed zero
        eRet = 0;
        if (ef >= 0x67) { // 0x67:103=127-24 Denormal
            mf = (mf | static_cast<uint32_t>(Fp32BasicParam::K_FP32_MAN_HIDE_BIT));
            uint16_t shiftOut = static_cast<uint16_t>(Fp32BasicParam::K_FP32_MAN_LEN);
            uint64_t mTmp = (static_cast<uint64_t>(mf)) << (ef - 0x67);

            needRound = IsRoundOne(mTmp, shiftOut);
            mRet = static_cast<uint16_t>(mTmp >> shiftOut);
            if (needRound) {
                mRet++;
            }
        } else if ((ef == 0x66) && (mf > 0)) { // 0x66:102 Denormal 0<f_v<min(Denormal)
            mRet = 1;
        } else {
            mRet = 0;
        }
    } else { // Regular case with no overflow or underflow
        eRet = static_cast<int16_t>(ef - 0x70u);

        needRound = IsRoundOne(mf, static_cast<uint16_t>(mLenDelta));
        mRet = static_cast<uint16_t>(mf >> mLenDelta);
        if (needRound) {
            mRet++;
        }
        if ((mRet & static_cast<uint16_t>(Fp16BasicParam::K_FP16_MAN_HIDE_BIT)) != 0) {
            eRet++;
        }
    }

    Fp16Normalize(eRet, mRet);
    return FP16_CONSTRUCTOR(sRet, static_cast<uint16_t>(eRet), mRet);
}

uint16_t half::DoubleToFp16(const double& dVal) { return 0; }

uint16_t half::Int16ToFp16(const int16_t& iVal) const { return iVal; }

uint16_t half::UInt16ToFp16(const uint16_t& uiVal) { return uiVal; }
uint16_t half::Int32ToFp16(const int32_t& iVal) const { return iVal; }

uint16_t half::UInt32ToFp16(const uint32_t& uiVal) const { return uiVal; }

// evaluation
half& half::operator=(const half& fp)
{
    if (&fp == this) {
        return *this;
    }
    val = fp.val;
    return *this;
}

// convert
half::operator float() const { return Fp16ToFloat(val); }

float half::ToFloat() const { return Fp16ToFloat(val); }
// } // namespace float16
