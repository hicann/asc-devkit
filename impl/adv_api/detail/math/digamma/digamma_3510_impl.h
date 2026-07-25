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
 * \file digamma_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/digamma/digamma_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/digamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H
#define IMPL_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "digamma_common_basic_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/digamma/digamma_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace DigammaInternal {
constexpr float MIN_NEG_FLOAT = -8388608.0;
constexpr float DIGAMMA_PI = 3.141592653589793238f;
constexpr float DIGAMMA_NEG_PI = -3.141592653589793238f;
constexpr uint32_t DIGAMMA_FLOAT_NOREUSE_CALC_PROC = 7;
constexpr uint32_t DIGAMMA_FLOAT_REUSE_CALC_PROC = 6;
constexpr uint32_t DIGAMMA_HALF_CALC_PROC = 8;
constexpr size_t DIGAMMA_MAX_LOOP = 5;

constexpr float posCalcConst[] = {2.10927960927960927961e-2, 7.57575757575757575758e-3, 4.16666666666666666667e-3,
                                  3.96825396825396825397e-3, 8.33333333333333333333e-3, 8.33333333333333333333e-2};
constexpr float tmp1CalcConst[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
constexpr float tmp1HalfCalcConst[] = {1.0, 2.0};
constexpr float picotCalcConst[] = {
    0.00326538085938f, 0.0242919921875f, 0.053466796875f, 0.133377909660f, 0.333332300186f};

static constexpr Reg::CastTrait FLOAT_TO_INT_CAST_TRAIT = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};
static constexpr Reg::CastTrait INT_TO_FLOAT_CAST_TRAIT = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

template <CMPMODE cmpMode>
__simd_callee__ inline void DigammaGenCompareMask(
    Reg::MaskReg& maskDst, Reg::RegTensor<float>& srcReg, const float scalar, Reg::MaskReg& mask)
{
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpScalarReg;
    Reg::Duplicate(tmpScalarReg, scalar, fullMask);
    Reg::Compare<float, cmpMode>(maskDst, srcReg, tmpScalarReg, mask);
}

__simd_callee__ inline void DigammaSelect(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg& mask)
{
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpScalarReg;
    Reg::Duplicate(tmpScalarReg, 0.0f, fullMask);
    Reg::Select(tmpReg, srcReg, tmpScalarReg, mask);
    Reg::Add(dstReg, tmpReg, dstReg, fullMask);
}

__simd_callee__ inline void DigammaPositiveTmp0(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg)
{
    Reg::MaskReg mask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> tmpScalarReg;
    Reg::Adds(tmpReg1, srcReg, 10.0f, mask);
    Reg::Ln(dstReg, tmpReg1, mask);
    Reg::Duplicate(tmpScalarReg, 1.0f, mask);
    Reg::Div(tmpReg1, tmpScalarReg, tmpReg1, mask);
    Reg::Muls(tmpReg2, tmpReg1, 0.5f, mask);
    Reg::Sub(dstReg, dstReg, tmpReg2, mask);
    Reg::Mul(tmpReg1, tmpReg1, tmpReg1, mask);
    Reg::Duplicate(tmpReg2, 8.33333333333333333333e-2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[0U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[1U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[2U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[3U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[4U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[5U], mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, mask);
    Reg::Sub(dstReg, dstReg, tmpReg2, mask);
}

__simd_callee__ inline void DigammaPositiveTmp1(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg)
{
    Reg::MaskReg mask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> tmpScalarReg;
    Reg::Duplicate(tmpScalarReg, 1.0f, mask);
    Reg::Div(dstReg, tmpScalarReg, srcReg, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[0U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[1U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[2U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[3U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[4U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[5U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[6U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[7U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[8U], mask);
    Reg::Div(tmpReg2, tmpScalarReg, tmpReg2, mask);
    Reg::Add(dstReg, dstReg, tmpReg2, mask);
}

__simd_callee__ inline void DigammaPositive(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<float> tmpRegForPos;
    DigammaPositiveTmp0(dstReg, srcReg);
    DigammaPositiveTmp1(tmpRegForPos, srcReg);
    Reg::Sub(dstReg, dstReg, tmpRegForPos, mask);
}

__simd_callee__ inline void DigammaNegPicotPix(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg)
{
    Reg::MaskReg mask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg mask1;
    Reg::MaskReg mask2;
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> tmpReg3;
    Reg::RegTensor<float> tmpScalarReg;
    Reg::RegTensor<int32_t> tmpReg2s32;
    Reg::Add(tmpReg1, srcReg, srcReg, mask);
    Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>(tmpReg2s32, tmpReg1, mask);
    Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpReg2, tmpReg2s32, mask);
    Reg::Sub(tmpReg1, tmpReg1, tmpReg2, mask);
    Reg::Muls(tmpReg1, tmpReg1, 1.5707963267948966f, mask);
    Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>(tmpReg2s32, tmpReg2, mask);
    Reg::Duplicate((Reg::RegTensor<int32_t>&)tmpReg3, 1, mask);
    Reg::And<uint16_t>(
        (Reg::RegTensor<uint16_t>&)tmpReg2s32, (Reg::RegTensor<uint16_t>&)tmpReg2s32,
        (Reg::RegTensor<uint16_t>&)tmpReg3, mask);
    Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpReg2, tmpReg2s32, mask);
    DigammaGenCompareMask<CMPMODE::LT>(mask1, tmpReg2, 0.5f, mask);
    DigammaGenCompareMask<CMPMODE::GE>(mask2, tmpReg2, 0.5f, mask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg1, mask);
    Reg::Duplicate(dstReg, 0.0093383789065f, mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(dstReg, dstReg, picotCalcConst[0U], mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(dstReg, dstReg, picotCalcConst[1U], mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(dstReg, dstReg, picotCalcConst[2U], mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(dstReg, dstReg, picotCalcConst[3U], mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Adds(dstReg, dstReg, picotCalcConst[4U], mask);
    Reg::Mul(dstReg, dstReg, tmpReg2, mask);
    Reg::Mul(dstReg, dstReg, tmpReg1, mask);
    Reg::Add(tmpReg1, dstReg, tmpReg1, mask);
    Reg::Duplicate(dstReg, 0.0f, mask);
    DigammaSelect(dstReg, tmpReg1, tmpReg3, mask2);
    Reg::Duplicate(tmpScalarReg, -1.0f, mask);
    Reg::Div(tmpReg1, tmpScalarReg, tmpReg1, mask);
    DigammaSelect(dstReg, tmpReg1, tmpReg3, mask1);
    Reg::Muls(dstReg, dstReg, DIGAMMA_PI, mask);
}

__simd_callee__ inline void DigammaNegative(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpReg3;
    Reg::RegTensor<float> tmpReg4;
    Reg::Muls(tmpReg3, srcReg, -1.0f, fullMask);
    Reg::Adds(tmpReg3, tmpReg3, 1.0f, fullMask);
    DigammaPositive(dstReg, tmpReg3, mask);
    DigammaNegPicotPix(tmpReg4, srcReg);
    Reg::Add(dstReg, dstReg, tmpReg4, mask);
}

__simd_callee__ inline void DigammaGenNegIntMask(
    Reg::MaskReg& maskdst, Reg::RegTensor<float>& srcReg, const float scalar, Reg::RegTensor<float>& tmpCal1,
    Reg::MaskReg& mask)
{
    Reg::MaskReg tmpmask;
    Reg::MaskReg mask1;
    Reg::MaskReg mask2;
    Reg::RegTensor<int32_t> tmpReg2s32;
    DigammaGenCompareMask<CMPMODE::LT>(mask1, srcReg, 0.0f, mask);
    DigammaGenCompareMask<CMPMODE::GT>(mask2, srcReg, MIN_NEG_FLOAT, mask);
    Reg::MaskAnd(mask1, mask1, mask2, mask);
    Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>(tmpReg2s32, srcReg, mask);
    Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpCal1, tmpReg2s32, mask);
    Reg::Compare<float, CMPMODE::EQ>(mask2, srcReg, tmpCal1, mask);
    Reg::MaskAnd(maskdst, mask1, mask2, mask);
}

__simd_callee__ inline void DigammaGenNanMask(
    Reg::MaskReg& mask0, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask1, Reg::MaskReg& mask2, Reg::MaskReg& mask)
{
    DigammaGenCompareMask<CMPMODE::LT>(mask1, srcReg, 0.0f, mask);
    DigammaGenCompareMask<CMPMODE::GE>(mask2, srcReg, 0.0f, mask);
    Reg::MaskNot(mask1, mask1, mask);
    Reg::MaskNot(mask2, mask2, mask);
    Reg::MaskAnd(mask0, mask1, mask2, mask);
}

__simd_callee__ inline void DigammaComputeImpl(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    Reg::MaskReg mask0;
    Reg::MaskReg mask1;
    Reg::MaskReg mask2;
    Reg::RegTensor<float> resultReg;
    Reg::RegTensor<float> tmpCal1;
    Reg::RegTensor<float> tmpCal2;
    Reg::RegTensor<float> tmpCal3;
    Reg::RegTensor<float> tmpCal4;
    Reg::RegTensor<float> tmpCal5;
    Reg::RegTensor<float> tmpScalar;

    NotNumUnion notNum;
    notNum.i = F32_NAN;
    Reg::Duplicate(dstReg, 0.0f, mask);
    Reg::Duplicate(resultReg, notNum.f, mask);
    DigammaGenCompareMask<CMPMODE::LE>(mask0, srcReg, MIN_NEG_FLOAT, mask);
    DigammaSelect(dstReg, resultReg, tmpCal3, mask0);
    DigammaGenNegIntMask(mask1, srcReg, MIN_NEG_FLOAT, tmpCal1, mask);
    DigammaSelect(dstReg, resultReg, tmpCal3, mask1);

    DigammaGenNanMask(mask0, srcReg, mask1, mask2, mask);
    DigammaSelect(dstReg, resultReg, tmpCal3, mask0);
    DigammaGenCompareMask<CMPMODE::GE>(mask0, srcReg, 0.0f, mask);
    DigammaPositive(resultReg, srcReg, mask);
    DigammaSelect(dstReg, resultReg, tmpCal3, mask0);
    DigammaGenCompareMask<CMPMODE::LT>(mask0, srcReg, 0.0f, mask);
    DigammaNegative(resultReg, srcReg, mask);
    DigammaSelect(dstReg, resultReg, tmpCal3, mask0);
}

template <typename T = float, bool isReuseSource = false>
__simd_vf__ inline void DigammaImpl(__ubuf__ float* dstUb, __ubuf__ float* srcUb, uint32_t calCount)
{
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> srcReg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(srcReg, srcUb + i * sregLower);
        DigammaComputeImpl(dstReg, srcReg, fullMask);
        Reg::StoreAlign(dstUb + i * sregLower, dstReg, mask);
    }
}
} // namespace DigammaInternal

template <typename T, bool isReuseSource = false>
__aicore__ inline void DigammaCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmp, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Digamma, (T, isReuseSource), (dst, src, tmp, calCount));
    CheckTensorPosition(dst, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(src, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(tmp, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", src, "srcTensor", "Digamma");
    CheckCalCount(calCount, "calCount", dst, "dstTensor", "Digamma");

    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    if constexpr (Std::is_same<T, float>::value) {
        __ubuf__ T* dstUb = (__ubuf__ T*)dst.GetPhyAddr();
        __ubuf__ T* srcUb = (__ubuf__ T*)src.GetPhyAddr();
        DigammaInternal::DigammaImpl<T, isReuseSource>(dstUb, srcUb, calCount);
    } else if constexpr (Std::is_same<T, half>::value) {
        if constexpr (isReuseSource) {
            static_assert(SupportType<T, float>(), "isReuseSource is only supported for float on current device!");
        }
        constexpr uint32_t oneBlockElm = static_cast<uint32_t>(ONE_BLK_SIZE / sizeof(T));
        uint16_t countAlign = static_cast<uint16_t>(CeilDivision(calCount, oneBlockElm)) * oneBlockElm;
        LocalTensor<float> tmpBuffer = tmp.ReinterpretCast<float>();
        LocalTensor<float> srcF32 = tmpBuffer[0];
        LocalTensor<float> dstF32 = tmpBuffer[countAlign];
        AscendC::Cast(srcF32, src, AscendC::RoundMode::CAST_NONE, calCount);
        __ubuf__ float* srcUb = (__ubuf__ float*)srcF32.GetPhyAddr();
        __ubuf__ float* dstUb = (__ubuf__ float*)dstF32.GetPhyAddr();
        DigammaInternal::DigammaImpl<float, isReuseSource>(dstUb, srcUb, calCount);
        AscendC::Cast(dst, dstF32, AscendC::RoundMode::CAST_NONE, calCount);
    }
}
} // namespace AscendC
#endif // IMPL_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_C310_IMPL_H__
#endif
