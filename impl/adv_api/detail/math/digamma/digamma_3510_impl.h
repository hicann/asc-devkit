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

static constexpr Reg::DivSpecificMode divMode = {Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};

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

__simd_callee__ inline void DigammaPositive(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> tmpScalarReg;

    // Inline DigammaPositiveTmp0: compute the asymptotic expansion at srcReg + 10.
    Reg::Adds(tmpReg1, srcReg, 10.0f, fullMask);
    Reg::Ln(dstReg, tmpReg1, fullMask);
    Reg::Duplicate(tmpScalarReg, 1.0f, fullMask);
    Reg::Div<float, &divMode>(tmpReg1, tmpScalarReg, tmpReg1, fullMask);
    Reg::Muls(tmpReg2, tmpReg1, 0.5f, fullMask);
    Reg::Sub(dstReg, dstReg, tmpReg2, fullMask);
    Reg::Mul(tmpReg1, tmpReg1, tmpReg1, fullMask);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::Duplicate(tmpReg2, 8.33333333333333333333e-2f, fullMask);
#else
    Reg::Duplicate(tmpReg2, 8.33333333333333333333e-2, fullMask);
#endif
    Reg::Duplicate(tmpScalarReg, posCalcConst[0U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[1U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[2U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[3U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[4U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Duplicate(tmpScalarReg, posCalcConst[5U], fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Mul(tmpReg2, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(dstReg, dstReg, tmpReg2, fullMask);

    // Inline DigammaPositiveTmp1: accumulate the recurrence correction from srcReg to srcReg + 10.
    Reg::Duplicate(tmpScalarReg, 1.0f, fullMask);
    Reg::Div(tmpReg1, tmpScalarReg, srcReg, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[0U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[1U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[2U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[3U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[4U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[5U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[6U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[7U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Adds(tmpReg2, srcReg, tmp1CalcConst[8U], fullMask);
    Reg::Div<float, &divMode>(tmpReg2, tmpScalarReg, tmpReg2, fullMask);
    Reg::Add(tmpReg1, tmpReg1, tmpReg2, fullMask);
    Reg::Sub(dstReg, dstReg, tmpReg1, mask);
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

        Reg::MaskReg mask0;
        Reg::MaskReg mask1;
        Reg::MaskReg mask2;
        Reg::RegTensor<float> resultReg;
        Reg::RegTensor<float> tmpCal3;

        // Inline DigammaComputeImpl: initialize the result and handle values below the supported range.
        NotNumUnion notNum;
        notNum.i = F32_NAN;
        Reg::Duplicate(dstReg, 0.0f, fullMask);
        Reg::Duplicate(resultReg, notNum.f, fullMask);
        DigammaGenCompareMask<CMPMODE::LE>(mask0, srcReg, MIN_NEG_FLOAT, fullMask);
        DigammaSelect(dstReg, resultReg, tmpCal3, mask0);

        // Inline DigammaGenNegIntMask: mark negative integer poles in the supported input range.
        {
            Reg::RegTensor<int32_t> tmpReg2s32;
            DigammaGenCompareMask<CMPMODE::LT>(mask1, srcReg, 0.0f, fullMask);
            DigammaGenCompareMask<CMPMODE::GT>(mask2, srcReg, MIN_NEG_FLOAT, fullMask);
            Reg::MaskAnd(mask1, mask1, mask2, fullMask);
            Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>(tmpReg2s32, srcReg, fullMask);
            Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpCal3, tmpReg2s32, fullMask);
            Reg::Compare<float, CMPMODE::EQ>(mask2, srcReg, tmpCal3, fullMask);
            Reg::MaskAnd(mask1, mask1, mask2, fullMask);
        }
        DigammaSelect(dstReg, resultReg, tmpCal3, mask1);

        // Inline DigammaGenNanMask: identify NaN values without introducing another callee frame.
        DigammaGenCompareMask<CMPMODE::LT>(mask1, srcReg, 0.0f, fullMask);
        DigammaGenCompareMask<CMPMODE::GE>(mask2, srcReg, 0.0f, fullMask);
        Reg::MaskNot(mask1, mask1, fullMask);
        Reg::MaskNot(mask2, mask2, fullMask);
        Reg::MaskAnd(mask0, mask1, mask2, fullMask);
        DigammaSelect(dstReg, resultReg, tmpCal3, mask0);

        DigammaGenCompareMask<CMPMODE::GE>(mask0, srcReg, 0.0f, fullMask);
        DigammaPositive(resultReg, srcReg, fullMask);
        DigammaSelect(dstReg, resultReg, tmpCal3, mask0);

        // Spill the accumulated non-negative and special-value results to dstUb. The negative path does not depend on
        // dstReg, so its physical register can be released until the final result merge.
        Reg::StoreAlign(dstUb + i * sregLower, dstReg, mask);

        DigammaGenCompareMask<CMPMODE::LT>(mask0, srcReg, 0.0f, fullMask);
        // Inline DigammaNegative and its one-shot positive/reflection subcomputations.
        {
            Reg::RegTensor<float> negArgReg;
            Reg::RegTensor<float> tmpReg1;
            Reg::RegTensor<float> tmpReg2;
            Reg::RegTensor<float> tmpReg4;

            Reg::Muls(negArgReg, srcReg, -1.0f, fullMask);
            Reg::Adds(negArgReg, negArgReg, 1.0f, fullMask);

            DigammaPositive(resultReg, negArgReg, fullMask);

            // Inline DigammaNegPicotPix: reuse dead RegTensor values for the integer cast and select scratch.
            Reg::Add(tmpReg1, srcReg, srcReg, fullMask);
            Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>((Reg::RegTensor<int32_t>&)tmpReg4, tmpReg1, fullMask);
            Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpReg2, (Reg::RegTensor<int32_t>&)tmpReg4, fullMask);
            Reg::Sub(tmpReg1, tmpReg1, tmpReg2, fullMask);
            Reg::Muls(tmpReg1, tmpReg1, 1.5707963267948966f, fullMask);
            Reg::Cast<int32_t, float, FLOAT_TO_INT_CAST_TRAIT>((Reg::RegTensor<int32_t>&)tmpReg4, tmpReg2, fullMask);
            Reg::Duplicate((Reg::RegTensor<int32_t>&)negArgReg, 1, fullMask);
            Reg::And<uint16_t>(
                (Reg::RegTensor<uint16_t>&)tmpReg4, (Reg::RegTensor<uint16_t>&)tmpReg4,
                (Reg::RegTensor<uint16_t>&)negArgReg, fullMask);
            Reg::Cast<float, int32_t, INT_TO_FLOAT_CAST_TRAIT>(tmpReg2, (Reg::RegTensor<int32_t>&)tmpReg4, fullMask);
            DigammaGenCompareMask<CMPMODE::LT>(mask1, tmpReg2, 0.5f, fullMask);
            DigammaGenCompareMask<CMPMODE::GE>(mask2, tmpReg2, 0.5f, fullMask);
            Reg::Mul(tmpReg2, tmpReg1, tmpReg1, fullMask);
            Reg::Duplicate(tmpReg4, 0.0093383789065f, fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Adds(tmpReg4, tmpReg4, picotCalcConst[0U], fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Adds(tmpReg4, tmpReg4, picotCalcConst[1U], fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Adds(tmpReg4, tmpReg4, picotCalcConst[2U], fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Adds(tmpReg4, tmpReg4, picotCalcConst[3U], fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Adds(tmpReg4, tmpReg4, picotCalcConst[4U], fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg2, fullMask);
            Reg::Mul(tmpReg4, tmpReg4, tmpReg1, fullMask);
            Reg::Add(tmpReg1, tmpReg4, tmpReg1, fullMask);
            Reg::Duplicate(tmpReg4, 0.0f, fullMask);
            DigammaSelect(tmpReg4, tmpReg1, negArgReg, mask2);
            Reg::Duplicate(tmpReg2, -1.0f, fullMask);
            Reg::Div<float, &divMode>(tmpReg1, tmpReg2, tmpReg1, fullMask);
            DigammaSelect(tmpReg4, tmpReg1, negArgReg, mask1);
            Reg::Muls(tmpReg4, tmpReg4, DIGAMMA_PI, fullMask);
            Reg::Add(resultReg, resultReg, tmpReg4, fullMask);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        Reg::LoadAlign(dstReg, dstUb + i * sregLower);
        DigammaSelect(dstReg, resultReg, tmpCal3, mask0);
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
