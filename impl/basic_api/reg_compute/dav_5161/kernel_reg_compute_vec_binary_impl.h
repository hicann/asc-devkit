/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_vec_binary_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
namespace Internal {
__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .precisionMode = false, .algo = DivAlgo::INTRINSIC};
}

__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(const DivSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .precisionMode = sprMode->precisionMode, .algo = sprMode->algo};
}
} // namespace Internal

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Add api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        AddOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        AddOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Sub api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsub(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Mul api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmul(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivPrecisionImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    // Improves Reg with high precision mode by using error-complemention approach with following formula.
    //
    // sign = vand(a, -0.0)
    // sign_one = vor(1.0, sign)

    // inf_nan_tmp = vor(x1, neg_zero)
    // inf_nan_temp_uint32 = 0xFF800000
    // inf_nan_cmp_mask = vcmp(inf_nan_temp_uint32, neg_inf_uint32, "ge", "bit")

    // x1 = div(a, b)
    // temp_b = -1.0*b

    // a = fma(a, temp_b, x1)  // a = a + temp_b*x1, high precision fma instr is required.
    // temp_mask = vcmp(a, a, "eq", "bit")
    // temp_a = vsel(temp_mask, a, one_temp)
    // e = div(temp_a, b)

    // x = x1 + e
    // x = vsel(inf_nan_cmp_mask, x1, x)
    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr uint32_t infNanBound = 0xff800000u;
    constexpr uint32_t signBitNum = 0x80000000u;
    constexpr int32_t precisionThreshold = -64;
    constexpr uint32_t exponentMask = 0x7F800000u;
    constexpr uint32_t mantissaMask = 0x007FFFFFu;
    constexpr int32_t exponentBias = 127;

    RegTensor<T> regNegZero;
    RegTensor<T> tmpDst;
    RegTensor<T> r, z, y;
    RegTensor<uint32_t> infNan;

    MaskReg cmpMaskReg;
    MaskReg infNanCmp;
    MaskReg zeroCmp;
    MaskReg preg;
    preg = pset_b8(PAT_ALL);
    vdup((vector_u32&)regNegZero, signBitNum, preg, modeValue);
    vdiv(z, srcReg0, srcReg1, mask, modeValue);
    vor(infNan, (vector_u32&)z, (vector_u32&)regNegZero, mask, modeValue);
    tmpDst = z;
    vcmps_eq(zeroCmp, z, 0.0f, mask);
    vcmps_ge(infNanCmp, infNan, infNanBound, mask);
    por(infNanCmp, infNanCmp, zeroCmp, mask);

    RegTensor<uint32_t> src0ExpBits, src0Reg;
    RegTensor<int32_t> src0Exp;
    RegTensor<uint32_t> scaleBits;
    RegTensor<T> scale;
    RegTensor<T> aScaled, bScaled;
    MaskReg needScaleMask;
    MaskReg noScaleMask;

    vdup(src0Reg, exponentMask, mask, modeValue);
    And(src0ExpBits, (RegTensor<uint32_t>&)srcReg0, src0Reg, mask);
    vshrs(src0ExpBits, src0ExpBits, (int16_t)23, mask, modeValue);
    vdup(src0Reg, exponentBias, mask, modeValue);
    vsub(src0Exp, (RegTensor<int32_t>&)src0ExpBits, (RegTensor<int32_t>&)src0Reg, mask, modeValue);

    Compares<int32_t, CMPMODE::LT>(needScaleMask, src0Exp, precisionThreshold, mask);
    Not(noScaleMask, needScaleMask, mask);

    RegTensor<int32_t> k;
    RegTensor<int32_t> thresholdVec;
    RegTensor<int32_t> zeroVec;
    vdup(thresholdVec, precisionThreshold, mask, modeValue);
    vdup(zeroVec, 0, mask, modeValue);
    vsub(k, thresholdVec, src0Exp, mask, modeValue);
    vmax(k, k, zeroVec, mask, modeValue);

    RegTensor<int32_t> newExp;
    vadds(newExp, k, exponentBias, needScaleMask, modeValue);
    vshls(scaleBits, (RegTensor<uint32_t>&)newExp, (int16_t)23, needScaleMask, modeValue);

    RegTensor<T> scaleOne;
    vdup(scaleOne, 1.0f, mask, modeValue);
    vsel(scale, (RegTensor<T>&)scaleBits, scaleOne, needScaleMask);

    vmul(aScaled, srcReg0, scale, mask, modeValue);
    vmul(bScaled, srcReg1, scale, mask, modeValue);

    vmuls(y, bScaled, -1.0f, mask, modeValue);
    r = aScaled;
    vmula(r, z, y, mask, modeValue);
    RegTensor<T> rPre, rNext, zPre, zNext;

    vadds((vector_s32&)zPre, (vector_s32&)z, -1, mask, modeValue);
    vadds((vector_s32&)zNext, (vector_s32&)z, 1, mask, modeValue);

    rPre = aScaled;
    rNext = aScaled;

    vmula(rPre, zPre, y, mask, modeValue);
    vmula(rNext, zNext, y, mask, modeValue);

    vabs(r, r, mask, modeValue);
    vabs(rPre, rPre, mask, modeValue);
    vabs(rNext, rNext, mask, modeValue);
    vcmp_lt(cmpMaskReg, r, rPre, mask);
    vsel(r, r, rPre, cmpMaskReg);
    vsel(z, z, zPre, cmpMaskReg);

    vcmp_lt(cmpMaskReg, rNext, r, mask);
    vsel(z, zNext, z, cmpMaskReg);
    vsel(dstReg, tmpDst, z, infNanCmp);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U, bool is0ULP>
__simd_callee__ inline void DivIEEE754FloatImpl(
    RegTensor<float>& dst, RegTensor<float>& src0, RegTensor<float>& src1, MaskReg& mask)
{
    constexpr uint32_t exponentExtractor = 0x807FFFFF;
    constexpr uint32_t signExtractor = 0x80000000;
    constexpr uint32_t exponentNormalizer = 0x3F800000;

    NotNumUnion subnormalThreshold;
    subnormalThreshold.i = 0x007FFFFF;

    NotNumUnion nan;
    nan.i = F32_NAN;
    NotNumUnion min_denormal;
    min_denormal.i = 0x1;

    NotNumUnion normalizeScaleEnlarge;
    normalizeScaleEnlarge.i = 0x4B000000; // 2^23
    NotNumUnion normalizeScaleReduce;
    normalizeScaleReduce.i = 0x34000000; // 2^-23

    Reg::RegTensor<float> maxSubnormal;
    Reg::RegTensor<uint32_t> tmp0;
    Reg::RegTensor<int32_t> tmp1;
    Reg::RegTensor<uint32_t> tmp2;

    Reg::RegTensor<float> src0Abs;
    Reg::RegTensor<float> src0Subnormal;
    Reg::RegTensor<float> src0Norm;
    Reg::RegTensor<float> src0All;
    Reg::RegTensor<float> src0AbsNorm;

    Reg::RegTensor<float> src1Abs;
    Reg::RegTensor<float> src1Subnormal;
    Reg::RegTensor<float> src1Norm;
    Reg::RegTensor<float> src1All;
    Reg::RegTensor<float> src1AbsNorm;

    MaskReg mask0;
    MaskReg maskSrc0Normal;
    MaskReg maskSrc0Subnormal;
    MaskReg maskSrc1Normal;
    MaskReg maskSrc1Subnormal;
    MaskReg maskTmp;
    MaskReg maskNan;      // divisor or dividend 0
    MaskReg maskInf;      // divisor or dividend inf
    MaskReg maskSrc0Zero; // dividend 0
    MaskReg maskSrc1Zero; // divisor 0
    MaskReg maskValid;
    MaskReg maskNorm;

    RegTensor<uint32_t> src0Exponent;
    RegTensor<uint32_t> src1Exponent;

    RegTensor<float> z1;
    RegTensor<float> z2;
    RegTensor<int32_t> scale;
    RegTensor<uint32_t> dstExponent;
    RegTensor<uint32_t> dstSign;

    // subnormal threshold
    Duplicate(maxSubnormal, subnormalThreshold.f, mask);

    // ===========================================================
    // acquiring valid numbers (no inf, no 0)
    // ===========================================================
    Abs(src0Abs, src0, mask);
    Abs(src1Abs, src1, mask);

    // get positions of inf values
    Duplicate(tmp0, F32_INF, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskInf, (RegTensor<uint32_t>&)src0Abs, tmp0, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskTmp, (RegTensor<uint32_t>&)src1Abs, tmp0, mask);
    Or(maskValid, maskInf, maskTmp, mask);
    // get positions of 0 divisor or dividend
    Duplicate(tmp0, 0, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskSrc0Zero, (RegTensor<uint32_t>&)src0Abs, tmp0, mask);
    // merge for positions of invalid numbers
    Or(maskValid, maskValid, maskSrc0Zero, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskSrc1Zero, (RegTensor<uint32_t>&)src1Abs, tmp0, mask);
    // negating for positions of valid numbers
    Or(maskValid, maskValid, maskSrc1Zero, mask);
    Not(maskValid, maskValid, mask);

    // normalize subnormal elements of src0
    // get positions of subnormal numbers in dividend
    Compare<float, CMPMODE::LT>(maskSrc0Subnormal, src0Abs, maxSubnormal, mask);
    // negating for normal positions
    Not(maskSrc0Normal, maskSrc0Subnormal, mask);
    // normalizatoin
    Muls(src0Subnormal, src0, normalizeScaleEnlarge.f, maskSrc0Subnormal);

    // normalize subnormal elements of src1
    Compare<float, CMPMODE::LT>(maskSrc1Subnormal, src1Abs, maxSubnormal, mask);
    Not(maskSrc1Normal, maskSrc1Subnormal, mask);
    Muls(src1Subnormal, src1, normalizeScaleEnlarge.f, maskSrc1Subnormal);

    // merge the normalized subnormal elements with normal elements
    Select(src0All, src0, src0Subnormal, maskSrc0Normal);
    Select(src1All, src1, src1Subnormal, maskSrc1Normal);

    // standardized the exponent bits of src0 and src1
    // zero out the exponent bits 00000000
    Duplicate(tmp0, exponentExtractor, mask);
    And((RegTensor<uint32_t>&)src0Norm, (RegTensor<uint32_t>&)src0All, tmp0, maskValid);
    And((RegTensor<uint32_t>&)src1Norm, (RegTensor<uint32_t>&)src1All, tmp0, maskValid);
    // set the exponent bits to 01111111
    Duplicate(tmp0, exponentNormalizer, mask);
    Add((RegTensor<uint32_t>&)src0Norm, (RegTensor<uint32_t>&)src0Norm, tmp0, maskValid);
    Add((RegTensor<uint32_t>&)src1Norm, (RegTensor<uint32_t>&)src1Norm, tmp0, maskValid);
    Select(src0Norm, src0Norm, src0All, maskValid);
    Select(src1Norm, src1Norm, src1All, maskValid);
    Abs(src0AbsNorm, src0Norm, maskValid);
    Abs(src1AbsNorm, src1Norm, maskValid);
    Compare<float, CMPMODE::LE>(maskNorm, src0AbsNorm, src1AbsNorm, maskValid);

    if constexpr (is0ULP) {
        DivPrecisionImpl<float, mode, U>(dst, src0Norm, src1Norm, mask);
    } else {
        constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(MaskMergeMode::ZEROING);
        constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
        vdiv(dst, src0Norm, src1Norm, mask, modeValue);
    }

    // subnormal dividend, normal divisor
    And(mask0, maskSrc0Subnormal, maskSrc1Normal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleReduce.f, mask0);
    Select(dst, z1, dst, mask0);

    // normal dividend, subnormal divisor
    And(mask0, maskSrc0Normal, maskSrc1Subnormal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleEnlarge.f, mask0);
    // merge the compensated result
    Select(dst, z1, dst, mask0);

    // preserve sign for error handling section below
    Duplicate(tmp0, signExtractor, mask);
    And((RegTensor<uint32_t>&)dstSign, (RegTensor<uint32_t>&)dst, tmp0, mask);

    // ===========================================================
    // exponent operation
    // ===========================================================
    // extract the exponent section 0 11..11 00..00
    Duplicate(tmp0, F32_INF, mask);
    And(src0Exponent, (RegTensor<uint32_t>&)src0All, tmp0, mask);
    And(src1Exponent, (RegTensor<uint32_t>&)src1All, tmp0, mask);
    And(dstExponent, (RegTensor<uint32_t>&)dst, tmp0, mask);

    // exponent subtraction (effectively fp number division)
    ShiftRights(src0Exponent, src0Exponent, (int16_t)23, mask);
    ShiftRights(src1Exponent, src1Exponent, (int16_t)23, mask);
    ShiftRights(dstExponent, dstExponent, (int16_t)23, mask);
    Sub(scale, (RegTensor<int32_t>&)src0Exponent, (RegTensor<int32_t>&)src1Exponent, mask);
    Adds(scale, scale, 127, mask);
    // ===========================================================
    // exception handling
    // ===========================================================
    // overflow (exponent over 255) underflow (exponent under 0) detection // FP32:1S + 8E + 23M
    Duplicate(tmp1, -23, mask);
    // True if underflow/overflow
    Compare<int32_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int32_t>&)tmp1, mask);
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, min_denormal.i, mask0);
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Duplicate(tmp2, static_cast<uint32_t>(0), mask0);
    Add((RegTensor<uint32_t>&)z2, (RegTensor<uint32_t>&)dstSign, tmp2, mask0);
    Select(z1, z2, z1, maskNorm);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);
    Compare<int32_t, CMPMODE::LT>(mask0, scale, (RegTensor<int32_t>&)tmp1, mask);
    // set overflown/underflown result to infinity
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, 0, mask); // set to 0
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 255, mask);
    Compare<int32_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int32_t>&)tmp0, mask);
    And(mask0, mask0, maskValid, mask);
    Reg::Duplicate(tmp1, 1, mask0);
    Reg::Sub(tmp1, scale, tmp1, mask0);
    Select(scale, tmp1, scale, mask0);
    Muls(z1, dst, 2, mask0);
    Select(dst, z1, dst, mask0);

    Compare<int32_t, CMPMODE::GT>(mask0, scale, (RegTensor<int32_t>&)tmp0, mask);
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, F32_INF, mask); // set to infinity
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 0, maskValid);
    Compare<int32_t, CMPMODE::GT>(mask0, scale, (RegTensor<int32_t>&)tmp0, maskValid);
    ShiftLefts(tmp1, scale, (int16_t)23, mask0);
    Mul(z1, dst, (RegTensor<float>&)tmp1, mask0);
    Select(dst, z1, dst, mask0);

    Not(mask0, mask0, maskValid);
    Duplicate(tmp0, 4194304, mask0); // set 0x0040 0000
    Abs(scale, scale, mask0);
    ShiftRight(scale, (RegTensor<int32_t>&)tmp0, scale, mask0);
    Mul(z1, dst, (RegTensor<float>&)scale, mask0);
    Select(dst, z1, dst, mask0);

    // get the position of nan
    Duplicate(tmp0, nan.i, mask);
    Compare<float, CMPMODE::NE>(maskNan, src0Abs, src0Abs, mask);
    Compare<float, CMPMODE::NE>(maskTmp, src1Abs, src1Abs, mask);
    Or(maskNan, maskNan, maskTmp, mask);
    // set output with nan input to nan
    Select(dst, (RegTensor<float>&)tmp0, dst, maskNan);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivIEEE754HalfImpl(
    RegTensor<half>& dst, RegTensor<half>& src0, RegTensor<half>& src1, MaskReg& mask)
{
    constexpr uint16_t exponentExtractor = 0x83FF;
    constexpr uint16_t signExtractor = 0x8000;
    constexpr uint16_t exponentNormalizer = 0x3C00;
    constexpr uint16_t F16_INF = 0x7C00;

    HalfUnion subnormalThreshold;
    subnormalThreshold.i = 0x03FF;

    HalfUnion nan;
    nan.i = 0x7E00;
    HalfUnion min_denormal;
    min_denormal.i = 0x1;

    HalfUnion normalizeScaleEnlarge;
    normalizeScaleEnlarge.i = 0x6400; // 2^10
    HalfUnion normalizeScaleReduce;
    normalizeScaleReduce.i = 0x1400; // 2^-10

    Reg::RegTensor<half> maxSubnormal;
    Reg::RegTensor<uint16_t> tmp0;
    Reg::RegTensor<int16_t> tmp1;
    Reg::RegTensor<uint16_t> tmp2;

    Reg::RegTensor<half> src0Abs;
    Reg::RegTensor<half> src0Subnormal;
    Reg::RegTensor<half> src0Norm;
    Reg::RegTensor<half> src0All;
    Reg::RegTensor<half> src0AbsNorm;

    Reg::RegTensor<half> src1Abs;
    Reg::RegTensor<half> src1Subnormal;
    Reg::RegTensor<half> src1Norm;
    Reg::RegTensor<half> src1All;
    Reg::RegTensor<half> src1AbsNorm;

    MaskReg mask0;
    MaskReg maskSrc0Normal;
    MaskReg maskSrc0Subnormal;
    MaskReg maskSrc1Normal;
    MaskReg maskSrc1Subnormal;
    MaskReg maskTmp;
    MaskReg maskNan;      // divisor or dividend 0
    MaskReg maskInf;      // divisor or dividend inf
    MaskReg maskSrc0Zero; // dividend 0
    MaskReg maskSrc1Zero; // divisor 0
    MaskReg maskValid;
    MaskReg maskNorm;

    RegTensor<uint16_t> src0Exponent;
    RegTensor<uint16_t> src1Exponent;

    RegTensor<half> z1;
    RegTensor<half> z2;
    RegTensor<int16_t> scale;
    RegTensor<uint16_t> dstExponent;
    RegTensor<uint16_t> dstSign;

    // subnormal threshold
    Duplicate(maxSubnormal, subnormalThreshold.f, mask);

    // ===========================================================
    // acquiring valid numbers (no inf, no 0)
    // ===========================================================
    Abs(src0Abs, src0, mask);
    Abs(src1Abs, src1, mask);

    // get positions of inf values
    Duplicate(tmp0, F16_INF, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskInf, (RegTensor<uint16_t>&)src0Abs, tmp0, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskTmp, (RegTensor<uint16_t>&)src1Abs, tmp0, mask);
    Or(maskValid, maskInf, maskTmp, mask);
    // get positions of 0 divisor or dividend
    Duplicate(tmp0, 0, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskSrc0Zero, (RegTensor<uint16_t>&)src0Abs, tmp0, mask);
    // merge for positions of invalid numbers
    Or(maskValid, maskValid, maskSrc0Zero, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskSrc1Zero, (RegTensor<uint16_t>&)src1Abs, tmp0, mask);
    // negating for positions of valid numbers
    Or(maskValid, maskValid, maskSrc1Zero, mask);
    Not(maskValid, maskValid, mask);

    // normalize subnormal elements of src0
    // get positions of subnormal numbers in dividend
    Compare<half, CMPMODE::LT>(maskSrc0Subnormal, src0Abs, maxSubnormal, mask);
    // negating for normal positions
    Not(maskSrc0Normal, maskSrc0Subnormal, mask);
    // normalizatoin
    Muls(src0Subnormal, src0, normalizeScaleEnlarge.f, maskSrc0Subnormal);

    // normalize subnormal elements of src1
    Compare<half, CMPMODE::LT>(maskSrc1Subnormal, src1Abs, maxSubnormal, mask);
    Not(maskSrc1Normal, maskSrc1Subnormal, mask);
    Muls(src1Subnormal, src1, normalizeScaleEnlarge.f, maskSrc1Subnormal);

    // merge the normalized subnormal elements with normal elements
    Select(src0All, src0, src0Subnormal, maskSrc0Normal);
    Select(src1All, src1, src1Subnormal, maskSrc1Normal);

    // standardized the exponent bits of src0 and src1
    // zero out the exponent bits 00000000
    Duplicate(tmp0, exponentExtractor, mask);
    And((RegTensor<uint16_t>&)src0Norm, (RegTensor<uint16_t>&)src0All, tmp0, maskValid);
    And((RegTensor<uint16_t>&)src1Norm, (RegTensor<uint16_t>&)src1All, tmp0, maskValid);
    // set the exponent bits to 01111111
    Duplicate(tmp0, exponentNormalizer, mask);
    Add((RegTensor<uint16_t>&)src0Norm, (RegTensor<uint16_t>&)src0Norm, tmp0, maskValid);
    Add((RegTensor<uint16_t>&)src1Norm, (RegTensor<uint16_t>&)src1Norm, tmp0, maskValid);
    Select(src0Norm, src0Norm, src0All, maskValid);
    Select(src1Norm, src1Norm, src1All, maskValid);
    Abs(src0AbsNorm, src0Norm, maskValid);
    Abs(src1AbsNorm, src1Norm, maskValid);
    Compare<half, CMPMODE::LE>(maskNorm, src0AbsNorm, src1AbsNorm, maskValid);

    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(MaskMergeMode::ZEROING);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vdiv(dst, src0Norm, src1Norm, mask, modeValue);

    // subnormal dividend, normal divisor
    And(mask0, maskSrc0Subnormal, maskSrc1Normal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleReduce.f, mask0);
    Select(dst, z1, dst, mask0);

    // normal dividend, subnormal divisor
    And(mask0, maskSrc0Normal, maskSrc1Subnormal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleEnlarge.f, mask0);
    // merge the compensated result
    Select(dst, z1, dst, mask0);

    // preserve sign for error handling section below
    Duplicate(tmp0, signExtractor, mask);
    And((RegTensor<uint16_t>&)dstSign, (RegTensor<uint16_t>&)dst, tmp0, mask);

    // ===========================================================
    // exponent operation
    // ===========================================================
    // extract the exponent section 0 11..11 00..00
    Duplicate(tmp0, F16_INF, mask);
    And(src0Exponent, (RegTensor<uint16_t>&)src0All, tmp0, mask);
    And(src1Exponent, (RegTensor<uint16_t>&)src1All, tmp0, mask);
    And(dstExponent, (RegTensor<uint16_t>&)dst, tmp0, mask);

    // exponent subtraction (effectively fp number division)
    ShiftRights(src0Exponent, src0Exponent, (int16_t)10, mask);
    ShiftRights(src1Exponent, src1Exponent, (int16_t)10, mask);
    ShiftRights(dstExponent, dstExponent, (int16_t)10, mask);
    Sub(scale, (RegTensor<int16_t>&)src0Exponent, (RegTensor<int16_t>&)src1Exponent, mask);
    Adds(scale, scale, 15, mask);
    // ===========================================================
    // exception handling
    // ===========================================================
    // overflow (exponent over 31) underflow (exponent under -9) detection // FP16:1S + 5E + 9M
    Duplicate(tmp1, -9, mask);
    Compare<int16_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int16_t>&)tmp1, mask);
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, min_denormal.i, mask0);
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Duplicate(tmp2, static_cast<uint16_t>(0), mask0);
    Add((RegTensor<uint16_t>&)z2, (RegTensor<uint16_t>&)dstSign, tmp2, mask0);
    Select(z1, z2, z1, maskNorm);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);
    // True if underflow/overflow
    Compare<int16_t, CMPMODE::LT>(mask0, scale, (RegTensor<int16_t>&)tmp1, mask);
    // set overflown/underflown result to infinity
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, 0, mask); // set to 0
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 31, mask);
    Compare<int16_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int16_t>&)tmp0, mask);
    And(mask0, mask0, maskValid, mask);
    Reg::Duplicate(tmp1, 1, mask0);
    Reg::Sub(tmp1, scale, tmp1, mask0);
    Select(scale, tmp1, scale, mask0);
    Muls(z1, dst, 2, mask0);
    Select(dst, z1, dst, mask0);

    Compare<int16_t, CMPMODE::GT>(mask0, scale, (RegTensor<int16_t>&)tmp0, mask);
    And(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, F16_INF, mask); // set to infinity
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    Not(mask0, mask0, mask);
    And(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 0, maskValid);
    Compare<int16_t, CMPMODE::GT>(mask0, scale, (RegTensor<int16_t>&)tmp0, maskValid);
    ShiftLefts(tmp1, scale, (int16_t)10, mask0);
    Mul(z1, dst, (RegTensor<half>&)tmp1, mask0);
    Select(dst, z1, dst, mask0);

    Not(mask0, mask0, maskValid);
    Duplicate(tmp0, 512, mask0); // set 0x0200
    Abs(scale, scale, mask0);
    ShiftRight(scale, (RegTensor<int16_t>&)tmp0, scale, mask0);
    Mul(z1, dst, (RegTensor<half>&)scale, mask0);
    Select(dst, z1, dst, mask0);

    // get the position of nan
    Duplicate(tmp0, nan.i, mask);
    Compare<half, CMPMODE::NE>(maskNan, src0Abs, src0Abs, mask);
    Compare<half, CMPMODE::NE>(maskTmp, src1Abs, src1Abs, mask);
    Or(maskNan, maskNan, maskTmp, mask);
    // set output with nan input to nan
    Select(dst, (RegTensor<half>&)tmp0, dst, maskNan);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const DivSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const DivSpecificMode* ");
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(mode);
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Div api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    if constexpr (sprMode.precisionMode) {
        static_assert(SupportType<ActualT, float>(), "only float data type is supported in precsion mode.");
    }
    if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE) {
        static_assert(SupportType<T, half, float>(), "Reg Div for PRECISION_1ULP_FTZ_FALSE only supports half/float.");
    } else if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
        static_assert(SupportType<T, float>(), "Reg Div for PRECISION_0ULP_FTZ_FALSE only supports float.");
    } else if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE) {
        static_assert(SupportType<T, float>(), "Reg Div for PRECISION_0ULP_FTZ_TRUE only supports float.");
    }
    if constexpr (SupportType<ActualT, half>()) {
        if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE) {
            DivIEEE754HalfImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
        } else {
            vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
        }
    } else if constexpr (SupportType<ActualT, float>()) {
        if constexpr (sprMode.precisionMode) {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            } else {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            }
        } else {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            } else if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, false>(dstReg, srcReg0, srcReg1, mask);
            } else if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE) {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            } else {
                vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
            }
        }
    } else if constexpr (SupportType<ActualT, uint32_t>() || SupportType<ActualT, int32_t>()) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
        RegTensor<float> dstReg0;
        RegTensor<float> dstReg1;
        vcvt(dstReg0, (RegTensor<int32_t>&)srcReg0, mask, roundModeValue, modeValue);
        vcvt(dstReg1, (RegTensor<int32_t>&)srcReg1, mask, roundModeValue, modeValue);
        vdiv((RegTensor<float>&)dstReg, dstReg0, dstReg1, mask, modeValue);
        constexpr auto satModeValue =
            std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::SAT)>();
        vcvt((RegTensor<int32_t>&)dstReg, (RegTensor<float>&)dstReg, mask, roundModeValue, satModeValue, modeValue);
    } else {
        vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MaxOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
        vmax(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
        U tmpReg;
        MaskReg selMask;
        Compare<ActualT, CMPMODE::GT>(selMask, srcReg0, srcReg1, mask);
        Select(dstReg, srcReg0, srcReg1, selMask);
        Duplicate(tmpReg, static_cast<ActualT>(0));
        Select(dstReg, dstReg, tmpReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MaxImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Max api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        MaxOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        MaxOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MinOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
        vmin(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
        U tmpReg;
        MaskReg selMask;
        Compare<ActualT, CMPMODE::LT>(selMask, srcReg0, srcReg1, mask);
        Select(dstReg, srcReg0, srcReg1, selMask);
        Duplicate(tmpReg, static_cast<ActualT>(0));
        Select(dstReg, dstReg, tmpReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MinImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Min api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        MinOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        MinOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void OrImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Or api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vor((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, mask, modeValue);
    } else {
        vor(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PreluImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current Prelu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vprelu(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulAddDstImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current MulAddDst api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmula(dstReg, srcReg0, srcReg1, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif
