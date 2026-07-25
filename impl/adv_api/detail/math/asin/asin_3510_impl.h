/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asin_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/asin/asin_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/asin.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASIN_ASIN_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ASIN_ASIN_C310_IMPL_H
#define IMPL_MATH_ASIN_ASIN_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../math_constant_util.h"
#include "../../common/check.h"

namespace AscendC {
namespace Internal {
constexpr Reg::CastTrait ASIN_CAST_TRAIT_NONE = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_NONE};

constexpr Reg::CastTrait ASIN_CAST_TRAIT_FLOOR = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_FLOOR};

constexpr Reg::CastTrait ASIN_CAST_TRAIT_RINT = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

// Calculate Taylor Expansion according to (((k_nx^2 + k_n) * x^2 + k_(n-1)) * x^2 +k_(n-2) …�?*x^2 +k_0)*x.
template <typename T, typename RegT>
__simd_callee__ inline void AsinTaylorComputeInner(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::Muls(dstReg, dstReg, static_cast<T>(kCOEF[ASIN_TAYLOR_EXPAND_COUNT]), mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[6]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[5]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[4]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[3]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[2]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[1]), mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
    Reg::Adds(dstReg, dstReg, static_cast<T>(kCOEF[0]), mask);
}

template <typename T, typename RegT>
__simd_callee__ inline void AsinTaylorCompute(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<T> tmpReg;
    Reg::Mul(dstReg, srcReg, srcReg, mask);
    Reg::Mul(tmpReg, srcReg, srcReg, mask);
    AsinTaylorComputeInner<T>(dstReg, tmpReg, mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
}

// Calculate Taylor Expansion of Asin based on its square value, and set the source to be sqrt(x).
template <typename T, typename RegT>
__simd_callee__ inline void AsinTaylorComputeBySquareValue(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::Muls(dstReg, srcReg, static_cast<T>(NUM_ONE), mask);
    AsinTaylorComputeInner<T>(dstReg, srcReg, mask);
    // Update src to be sqrt(x).
    Reg::Sqrt(srcReg, srcReg, mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
}

template <typename T, typename RegT>
__simd_callee__ inline void CalRes2(RegT& resReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<T> tmpReg;
    Reg::Mul(tmpReg, srcReg, srcReg, mask);
    Reg::Muls(tmpReg, tmpReg, NEG_ONE, mask);
    Reg::Adds(tmpReg, tmpReg, NUM_ONE, mask);
    Reg::Sqrt(tmpReg, tmpReg, mask);
    AsinTaylorCompute<T>(resReg, tmpReg, mask);
    Reg::Muls(resReg, resReg, NEG_ONE, mask);
    Reg::Adds(resReg, resReg, HALF_PI, mask);
}

template <typename T, typename RegT>
__simd_callee__ inline void ProcessBranch(RegT& resReg1, RegT& resReg2, RegT& tmpReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<int32_t> s32Reg;
    Reg::Mins(tmpReg, tmpReg, BOUNDARY, mask);
    Reg::Adds(tmpReg, tmpReg, -BOUNDARY, mask);
    Reg::Cast<int32_t, T, ASIN_CAST_TRAIT_FLOOR>(s32Reg, tmpReg, mask);
    Reg::Cast<T, int32_t, ASIN_CAST_TRAIT_RINT>(tmpReg, s32Reg, mask);
    Reg::Muls(tmpReg, tmpReg, NEG_ONE, mask);
    Reg::Mul(resReg1, resReg1, tmpReg, mask);
    Reg::Muls(tmpReg, tmpReg, NEG_ONE, mask);
    Reg::Adds(tmpReg, tmpReg, NUM_ONE, mask);
    Reg::Mul(resReg2, resReg2, tmpReg, mask);
    Reg::Add(resReg1, resReg1, resReg2, mask);
}

// Calculate the sign of given values.
// Algorithm:
// FP16: sign(x) = 2^(15) * x /(2^(-15) + 2^(15) *|x|)
// FP32: sign(x) = 2^(62) * x /(2^(-62) + 2^(62) *|x|)
template <typename T, typename RegT>
__simd_callee__ inline void GetSign(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<T> denominatorReg;
    constexpr float FP16_MAX = 32768;                 // 2^15
    constexpr float FP16_MIN = 3.0517578125e-05;      // 2^-15
    constexpr float FP32_MAX = 4611686018427387904;   // 2^62
    constexpr float FP32_MIN = 2.168404344971009e-19; // 2^-62
    constexpr float kFpMax = sizeof(T) == sizeof(float) ? FP32_MAX : FP16_MAX;
    constexpr float kFpMin = sizeof(T) == sizeof(float) ? FP32_MIN : FP16_MIN;
    Reg::Muls(dstReg, srcReg, static_cast<T>(kFpMax), mask);
    Reg::Abs(denominatorReg, dstReg, mask);
    Reg::Adds(denominatorReg, denominatorReg, static_cast<T>(kFpMin), mask);
    Reg::Div(dstReg, dstReg, denominatorReg, mask);
}

// Compute asin values based on input types.
// asin(x) = arcsin(sqrt(1-x^2)) - PI*0.5 when x belongs to (-1, -2^(-0.5))
// asin(x) = the 15th order taylor expansion when x belongs to (-2^(-0.5), 2^(-0.5))
// asin(x) = PI*0.5 - arcsin(sqrt(1-x^2)) when x belongs to (2^(-0.5), 1)
template <typename T, bool convertToAcos = false>
__simd_vf__ inline void AsinComputeVFF32(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calSize, uint16_t repeatTimes, uint16_t stride)
{
    Reg::MaskReg mask;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dstReg;
    Reg::RegTensor<T> resReg1;
    Reg::RegTensor<T> resReg2;
    Reg::RegTensor<T> signReg;
    Reg::RegTensor<T> tmpReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(calSize);
        Reg::LoadAlign(srcReg, srcUb + i * stride);
        // Calculate res2 = PI*0.5 - taylor_compute(sqrt(1 - x^2)) -> resReg2.
        CalRes2<T>(resReg2, srcReg, mask);
        // Calculate res1 = taylor_compute(abs(x)) -> dst, abs(x) -> resReg1.
        Reg::Mul(tmpReg, srcReg, srcReg, mask);
        AsinTaylorComputeBySquareValue<T>(resReg1, tmpReg, mask);
        // As NPU are not good at scalar process like CPU for if-else statement, the solution here used for handling
        // above 3 scenarios is to calculate 0/1 choices combining the results on both options. e.g. Step1: Calculate
        // both option results of x, no matter which range it's at. result1(x), result2(x) Step2: Calculate 0/1 choices
        // of both option results of x, no matter which range it's at. choice1(x), choice2(x) Step3: Combine choice
        // result and options results, since at least one choice should be zero. Result = choice1(x) * result1(x) +
        // choice2(x) * result2(x) choice1 = -Floor(min(abs(x), BOUNDARY) - BOUNDARY). choice2 = 1 - choice1 res = res1
        // * choice1 + res2 * choice2
        ProcessBranch<T>(resReg1, resReg2, tmpReg, mask);
        GetSign<T>(signReg, srcReg, mask);
        Reg::Mul(dstReg, resReg1, signReg, mask);
        if constexpr (convertToAcos) {
            // Compute acos values according to formula: arccos(x) = PI*0.5 - arcsin(x).
            Reg::Adds(dstReg, dstReg, static_cast<T>(-HALF_PI), mask);
            Reg::Muls(dstReg, dstReg, static_cast<T>(NEG_ONE), mask);
        }
        Reg::StoreAlign(dstUb + i * stride, dstReg, mask);
    }
}

template <typename T, bool convertToAcos = false>
__simd_vf__ inline void AsinComputeVFF16(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calSize, uint16_t repeatTimes, uint16_t stride)
{
    Reg::MaskReg mask;
    Reg::RegTensor<half> srcReg;
    Reg::RegTensor<half> dstReg;
    Reg::RegTensor<half> halfReg1;
    Reg::RegTensor<half> halfReg2;
    Reg::RegTensor<half> tmpReg;
    Reg::RegTensor<float> floatReg1;
    Reg::RegTensor<float> floatReg2;
    Reg::RegTensor<int8_t> s8Reg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(calSize);
        // Cast src from half to float type for getting more precise results, but only computes by finishing
        // taylor expansion computation as it's the majority reason of precision loss.
        Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * stride);
        Reg::Cast<float, half, ASIN_CAST_TRAIT_NONE>(floatReg2, srcReg, mask);
        // Calculate res2 = PI*0.5 - taylor_compute(sqrt(1 - x^2)).
        Reg::Mul(floatReg2, floatReg2, floatReg2, mask);
        Reg::Muls(floatReg2, floatReg2, NEG_ONE, mask);
        Reg::Adds(floatReg2, floatReg2, NUM_ONE, mask);
        AsinTaylorComputeBySquareValue<half>(floatReg1, floatReg2, mask);
        Reg::Muls(floatReg1, floatReg1, NEG_ONE, mask);
        Reg::Adds(floatReg1, floatReg1, HALF_PI, mask);

        // Calculate res1 = taylor_compute(abs(x)).
        Reg::Abs(halfReg2, srcReg, mask);
        AsinTaylorCompute<half>(dstReg, halfReg2, mask);

        // As NPU are not good at scalar process like CPU for if-else statement, the solution here used for handling
        // above 3 scenarios is to calculate 0/1 choices combining the results on both options. e.g. Step1: Calculate
        // both option results of x, no matter which range it's at. result1(x), result2(x) Step2: Calculate 0/1 choices
        // of both option results of x, no matter which range it's at. choice1(x), choice2(x) Step3: Combine choice
        // result and optional result, since at least one choice should be zero. Result = choice1(x) * result1(x) +
        // choice2(x) * result2(x) choice1 = -Floor(min(abs(x), BOUNDARY) - BOUNDARY). choice2 = 1 - choice1 res = res1
        // * choice1 + res2 * choice2
        Reg::Mins(halfReg2, halfReg2, static_cast<half>(BOUNDARY), mask);
        Reg::Adds(halfReg2, halfReg2, static_cast<half>(-BOUNDARY), mask);
        Reg::Cast<int8_t, half, ASIN_CAST_TRAIT_FLOOR>(s8Reg, halfReg2, mask);
        Reg::Cast<half, int8_t, ASIN_CAST_TRAIT_NONE>(halfReg2, s8Reg, mask);
        Reg::Muls(halfReg2, halfReg2, static_cast<half>(NEG_ONE), mask);
        Reg::Mul(dstReg, dstReg, halfReg2, mask);
        Reg::Muls(halfReg2, halfReg2, static_cast<half>(NEG_ONE), mask);
        Reg::Adds(halfReg2, halfReg2, static_cast<half>(NUM_ONE), mask);
        Reg::Cast<float, half, ASIN_CAST_TRAIT_NONE>(floatReg2, halfReg2, mask);
        Reg::Mul(floatReg1, floatReg1, floatReg2, mask);
        Reg::Cast<float, half, ASIN_CAST_TRAIT_NONE>(floatReg2, dstReg, mask);
        Reg::Add(floatReg1, floatReg2, floatReg1, mask);
        GetSign<half>(halfReg1, srcReg, mask);
        Reg::Cast<float, half, ASIN_CAST_TRAIT_NONE>(floatReg2, halfReg1, mask);
        Reg::Mul(floatReg1, floatReg1, floatReg2, mask);
        if constexpr (convertToAcos) {
            // Compute acos values according to formula: arccos(x) = PI*0.5 - arcsin(x).
            Reg::Adds(floatReg1, floatReg1, -HALF_PI, mask);
            Reg::Muls(floatReg1, floatReg1, NEG_ONE, mask);
        }
        Reg::Cast<half, float, ASIN_CAST_TRAIT_RINT>(dstReg, floatReg1, mask);
        Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * stride, dstReg, mask);
    }
}
} // namespace Internal

template <typename T, bool convertToAcos = false>
__aicore__ inline void AsinCompute(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint32_t calSize)
{
    __ubuf__ T* dstUb = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)src.GetPhyAddr();

    // half dtype will be converted to float to improve precision;
    constexpr uint16_t stride = GetVecLen() / sizeof(float);
    uint16_t repeatTimes = CeilDivision(calSize, stride);
    if constexpr (IsSameType<T, half>::value) {
        Internal::AsinComputeVFF16<T, convertToAcos>(dstUb, srcUb, calSize, repeatTimes, stride);
    } else if (IsSameType<T, float>::value) {
        Internal::AsinComputeVFF32<T, convertToAcos>(dstUb, srcUb, calSize, repeatTimes, stride);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AsinImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "Asin only support half/float data type on current device!");

    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Asin");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Asin");
    AsinCompute(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AsinImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    AsinImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
}
} // namespace AscendC

#endif // IMPL_MATH_ASIN_ASIN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASIN_ASIN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASIN_ASIN_C310_IMPL_H__
#endif
