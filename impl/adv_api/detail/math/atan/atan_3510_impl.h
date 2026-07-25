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
 * \file atan_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/atan/atan_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/atan.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATAN_ATAN_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ATAN_ATAN_C310_IMPL_H
#define IMPL_MATH_ATAN_ATAN_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../../../../include/adv_api/math/atan_utils.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {
constexpr float ATAN_FP16_MAX = 32768;                 // 2^15
constexpr float ATAN_FP16_MIN = 3.0517578125e-05;      // 2^-15
constexpr float ATAN_FP32_MAX = 4611686018427387904;   // 2^62
constexpr float ATAN_FP32_MIN = 2.168404344971009e-19; // 2^-62
constexpr uint16_t TAYLOR_COUNT_FOUR = 4;              // x belongs to (0, tan(pi/8))
constexpr uint16_t TAYLOR_COUNT_SIX = 6;               // x belongs to (tan(pi/8), tan(pi/4))
constexpr float MIN_INPUT_VALUE = -10000;
constexpr float MAX_INPUT_VALUE = 10000;
// Calculates the Sign of given values.
// Algorithm:
//         FP16: sign(x) = 2**(15) * x /(2**(-15) + 2**(15) *|x|)
//         FP32: sign(x) = 2**(62) * x /(2**(-62) + 2**(62) *|x|)
template <typename T>
__simd_callee__ inline void Sign(
    Reg::RegTensor<T>& dstReg, Reg::RegTensor<T>& srcReg, Reg::RegTensor<T>& denominator, Reg::MaskReg preg)
{
    constexpr float kFpMax = sizeof(T) == sizeof(float) ? ATAN_FP32_MAX : ATAN_FP16_MAX;
    constexpr float kFpMin = sizeof(T) == sizeof(float) ? ATAN_FP32_MIN : ATAN_FP16_MIN;
    Reg::Muls(dstReg, srcReg, static_cast<T>(kFpMax), preg);
    Reg::Abs(denominator, dstReg, preg);
    Reg::Adds(denominator, denominator, static_cast<T>(kFpMin), preg);
    Reg::Div(dstReg, dstReg, denominator, preg);
}

// arctan(x) = x - x^3/3 + x^5/5 + ... + (-1)^k*x^(k*2+1)/( k*2+1)
// 1/(k*2+1)
__simd_callee__ inline void TaylorExpand(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& squareReg,
    const uint16_t expandLevel, Reg::MaskReg preg)
{
    // arctan(x) = x - x^3/3 + x^5/5 + ... + (-1)^k*x^(k*2+1)/( k*2+1)
    // 1/(k*2+1)
    constexpr float factorList[7] = {1,
                                     -0.3333333333333333,
                                     0.2,
                                     -0.14285714285714285,
                                     0.1111111111111111,
                                     -0.09090909090909091,
                                     0.07692307692307693};
    uint16_t COUNT_SIX = expandLevel == TAYLOR_COUNT_SIX ? 1 : 0;
    // The initial value of dstReg is assigned as the coefficient of the last item of expansion.
    Reg::Mul(squareReg, srcReg, srcReg, preg);
    Reg::Mul(dstReg, srcReg, srcReg, preg);
    Reg::Muls(dstReg, dstReg, factorList[expandLevel], preg);
    for (uint16_t i = 0; i < COUNT_SIX; ++i) {
        // dst*x^2+ the previous expand factor
        Reg::Adds(dstReg, dstReg, factorList[6], preg);
        Reg::Mul(dstReg, dstReg, squareReg, preg);
        Reg::Adds(dstReg, dstReg, factorList[5], preg);
    }
    // dst*x^2+ the previous expand factor
    Reg::Adds(dstReg, dstReg, factorList[4], preg);
    Reg::Mul(dstReg, dstReg, squareReg, preg);
    Reg::Adds(dstReg, dstReg, factorList[3], preg);
    Reg::Mul(dstReg, dstReg, squareReg, preg);
    Reg::Adds(dstReg, dstReg, factorList[2], preg);
    Reg::Mul(dstReg, dstReg, squareReg, preg);
    Reg::Adds(dstReg, dstReg, factorList[1], preg);
    Reg::Mul(dstReg, dstReg, squareReg, preg);
    Reg::Adds(dstReg, dstReg, factorList[0], preg);
    Reg::Mul(dstReg, dstReg, srcReg, preg);
}

// (x-y)/(1+xy)
__simd_callee__ inline void AtanTransform(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg,
    const float transFactor, Reg::MaskReg preg)
{
    // x*y
    Reg::Muls(dstReg, srcReg, transFactor, preg);
    // x*y + 1
    Reg::Adds(dstReg, dstReg, 1.0f, preg);
    // x=x-y
    Reg::Adds(tmpReg, srcReg, -transFactor, preg);
    // (x-y)/(1+xy)
    Reg::Div(dstReg, tmpReg, dstReg, preg);
    Reg::Abs(dstReg, dstReg, preg);
}

__simd_callee__ inline void AtanCompute(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& castReg, Reg::MaskReg preg)
{
    constexpr float piByFour = 0.78539816339744830961566084581988;
    constexpr float piByEight = 0.39269908169872415480783042290994;
    constexpr float tanPiByEight = 0.4142135623730950;

    Reg::RegTensor<float> clipReg;
    Reg::RegTensor<float> absReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> squareReg;
    // when x's value is too large the first calculator of TaylorExpand will be overflow. when epsilon is 0.0001,
    // the approximate value of `tan(pi/2 - 0.0001)` is 10000
    // Clip x to [MIN_INPUT_VALUE, MAX_INPUT_VALUE] in float
    Reg::Mins(clipReg, castReg, MAX_INPUT_VALUE, preg);
    Reg::Maxs(clipReg, clipReg, MIN_INPUT_VALUE, preg);
    Reg::Abs(absReg, clipReg, preg);
    // 1. x in (0, tan(pi/8))
    TaylorExpand(dstReg, absReg, squareReg, TAYLOR_COUNT_FOUR, preg);
    // 2. x in (tan(pi/8), tan(pi/4)), atan(x) = pi/8 + atan((x-tan(pi/8)) / (1 + x*tan(pi/8)))
    // normalize x to (0, tan(pi/8))
    AtanTransform(tmpReg, absReg, tmpReg2, tanPiByEight, preg); // tan(pi/8)
    TaylorExpand(tmpReg2, tmpReg, squareReg, TAYLOR_COUNT_FOUR, preg);
    Reg::Adds(tmpReg2, tmpReg2, piByEight, preg);
    Reg::Min(dstReg, dstReg, tmpReg2, preg);
    // x in (tan(pi/4), +�?, atan(x) = pi/4 + atan((x-1)/(x+1))
    // calculate |(x-1)/(x+1)|, normalize x to (0, tan(pi/4))
    // find the minimum value between atan(|(x-1)/(x+1)|) calculate in (0, tan(pi/8)) and (tan(pi/8), tan(pi/4))
    Reg::Adds(tmpReg2, absReg, 1.0f, preg);
    Reg::Adds(tmpReg, absReg, -1.0f, preg);
    Reg::Div(tmpReg, tmpReg, tmpReg2, preg);
    Reg::Abs(tmpReg, tmpReg, preg); // take the absolute value
    // 3. atan(|(x-1)/(x+1)|)
    TaylorExpand(tmpReg2, tmpReg, squareReg, TAYLOR_COUNT_FOUR, preg);
    // pi/4 + atan(|(x-1)/(x+1)|)
    Reg::Adds(tmpReg2, tmpReg2, piByFour, preg);
    Reg::Min(dstReg, dstReg, tmpReg2, preg);
    // 4.reuse the transform result in step 3, and calculate (x-tan(pi/8)) / (1 + x*tan(pi/8))
    AtanTransform(tmpReg2, tmpReg, squareReg, tanPiByEight, preg);
    TaylorExpand(tmpReg, tmpReg2, squareReg, TAYLOR_COUNT_SIX, preg);
    // pi/8 + pi/4 + atan((x-tan(pi/8)) / (1 + x*tan(pi/8)))
    Reg::Adds(tmpReg, tmpReg, piByEight, preg);
    Reg::Adds(tmpReg, tmpReg, piByFour, preg);
    Reg::Min(dstReg, dstReg, tmpReg, preg);
    Sign(tmpReg, clipReg, tmpReg2, preg);
    // dst = sign(x) * dst.
    Reg::Mul(dstReg, dstReg, tmpReg, preg);
}

template <typename T, bool isReuseSource = false>
__simd_vf__ inline void AtanTaylorVFImpl(__ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t calCount)
{
    uint16_t repeatTimes = CeilDivision(calCount, B32_DATA_NUM_PER_REPEAT);

    uint32_t sreg = calCount;
    Reg::MaskReg preg;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> dstReg;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(sreg);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
            Reg::Cast<float, T, castTraitB16ToB32>(castReg, srcReg, preg);
        } else {
            Reg::LoadAlign(castReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
        }
        AtanCompute(dstReg, castReg, preg);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, castTraitB32ToB16>(srcReg, dstReg, preg);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * B32_DATA_NUM_PER_REPEAT, srcReg, preg);
        } else {
            Reg::StoreAlign(dstUb + i * B32_DATA_NUM_PER_REPEAT, dstReg, preg);
        }
    }
}

//  when x < 0, Atan(x) = atan(-x)
//  when x belongs to (0, tan(pi/8)), Atan(x) = atan(x)
//  when x belongs to (tan(pi/8), tan(pi/4)), Atan(x) = pi/8 + atan((x- tan(pi/8)) / (1+ x*tan(pi/8)))
//  when x belongs to (tan(pi/4), +�?, Atan(x) = pi/4 + atan((x-1)/(x+1))
template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanTaylorImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    AtanTaylorVFImpl<T, isReuseSource>(dstUb, srcUb, calCount);
}

template <typename T, bool isReuseSource = false>
__simd_vf__ inline void AtanPolynomialVFImpl(__ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t calCount)
{
    constexpr float a1 = -0.333329409;
    constexpr float a2 = 0.199887753;
    constexpr float a3 = -0.141718030;
    constexpr float a4 = 0.105184801;
    constexpr float a5 = -0.0725297481;
    constexpr float a6 = 0.0398497507;
    constexpr float a7 = -0.0143969795;
    constexpr float a8 = 0.00245002890;
    constexpr float b1 = 1.68325555;
    constexpr float b2 = 0.933189452;
    constexpr float floatOne = 1.0;
    constexpr int32_t signBit = -2147483648;
    constexpr uint16_t vlSize = static_cast<uint16_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, vlSize));

    uint32_t sreg = calCount;
    Reg::MaskReg preg0;
    Reg::MaskReg preg1;
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::RegTensor<T> vreg0;
    Reg::RegTensor<T> vreg1;
    Reg::RegTensor<T> vreg2;
    Reg::RegTensor<T> vreg3;
    Reg::RegTensor<T> vreg4;
    Reg::RegTensor<T> vreg5;
    Reg::RegTensor<T> vreg6;
    Reg::RegTensor<T> vreg7;
    Reg::RegTensor<T> vreg8;
    Reg::RegTensor<T> vreg9;
    Reg::RegTensor<T> vreg10;
    Reg::RegTensor<T> vreg11;
    Reg::RegTensor<T> vreg12;
    Reg::RegTensor<T> vreg13;
    Reg::RegTensor<T> vreg14;
    Reg::RegTensor<int32_t> vreg15;

    Reg::Duplicate(vreg1, a1, fullMask);
    Reg::Duplicate(vreg2, a2, fullMask);
    Reg::Duplicate(vreg3, a3, fullMask);
    Reg::Duplicate(vreg4, a4, fullMask);
    Reg::Duplicate(vreg5, a5, fullMask);
    Reg::Duplicate(vreg6, a6, fullMask);
    Reg::Duplicate(vreg8, a8, fullMask);
    Reg::Duplicate(vreg10, floatOne, fullMask);

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg0 = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vreg0, srcUb + i * vlSize);
        // x_con = (if x_abs > 1.0 ? 1.0 / x_abs : x_abs)
        Reg::Abs(vreg9, vreg0, preg0);
        Reg::Div(vreg11, vreg10, vreg9, preg0);
        Reg::CompareScalar<T, CMPMODE::GE>(preg1, vreg9, floatOne, preg0);
        Reg::Select(vreg11, vreg11, vreg9, preg1);

        // y = taylor_expansion(x_con)
        Reg::Mul(vreg12, vreg11, vreg11, preg0); // s_x = x_con*x_con
        Reg::Duplicate(vreg7, a7, preg0);
        Reg::MulAddDst(vreg7, vreg8, vreg12, preg0);      // y = a8*s_x + a7
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg6, preg0); // y = y*s_x + a6
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg5, preg0); // y = y*s_x + a5
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg4, preg0); // y = y*s_x + a4
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg3, preg0); // y = y*s_x + a3
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg2, preg0); // y = y*s_x + a2
        Reg::FusedMulDstAdd(vreg7, vreg12, vreg1, preg0); // y = y*s_x + a1

        Reg::Mul(vreg12, vreg7, vreg12, preg0);             // tmp = (y*s_x)
        Reg::FusedMulDstAdd(vreg12, vreg11, vreg11, preg0); // y = (y*s_x) * x_con + x_con

        Reg::Duplicate(vreg11, b1, preg0);
        Reg::Duplicate(vreg14, b2, preg0);
        Reg::Neg(vreg13, vreg12, preg0);               // -1.0*y
        Reg::MulAddDst(vreg13, vreg11, vreg14, preg0); // y_if = b1*b2 + (-1.0 * y) //vmula
        Reg::Select(vreg13, vreg13, vreg12, preg1);    // y = (if x_abs > 1.0 ? y_if : y)

        // x_s32 = f32_to_s32(float(x))
        // x_s32_temp = x_s32 & 0x80000000
        Reg::Duplicate(vreg15, signBit, preg0);
        Reg::And(vreg15, (Reg::RegTensor<int32_t>&)vreg0, vreg15, preg0);
        // y_s32 = f32_to_s32(float(y))
        // y_temp = y_s32 | x_s32_temp
        Reg::Or(vreg15, (Reg::RegTensor<int32_t>&)vreg13, vreg15, preg0);
        // y = s32_to_f32(y_temp)
        Reg::StoreAlign(dstUb + i * vlSize, (Reg::RegTensor<T>&)vreg15, preg0);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanPolynomialImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();

    AtanPolynomialVFImpl<T, isReuseSource>(dstUb, srcUb, calCount);
}
} // namespace Internal

template <typename T, bool isReuseSource, const AtanConfig& config>
__aicore__ inline void AtanImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Atan");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Atan");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than srcTensor length %u", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((calCount <= dstTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than dstTensor length %u", calCount,
            dstTensor.GetSize());
    });

    if constexpr (config.algo == defaultAtanConfig.algo) {
        static_assert(
            SupportType<T, half, float>(),
            "Atan with TAYLOR_EXPANSION algorithm only support half/float data type on current device!");
        Internal::AtanTaylorImpl(dstTensor, srcTensor, calCount);
    } else {
        static_assert(
            SupportType<T, float>(),
            "Atan with POLYNOMIAL_APPROXIMATION algorithm only support float data type on current device!!");
        Internal::AtanPolynomialImpl(dstTensor, srcTensor, calCount);
    }
}

template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
__aicore__ inline void AtanImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Atan");

    AtanImpl<T, isReuseSource, config>(dstTensor, srcTensor, calCount);
}
} // namespace AscendC

#endif // IMPL_MATH_ATAN_ATAN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATAN_ATAN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATAN_ATAN_C310_IMPL_H__
#endif
