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
 * \file erf_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/erf/erf_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/erf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ERF_ERF_C310_IMPL_H
#define IMPL_MATH_ERF_ERF_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../../../../include/adv_api/math/erf_utils.h"
#include "../../common/check.h"

namespace AscendC {
namespace ErfAPI {

constexpr Reg::CastTrait castTraitF162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitF322F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr uint32_t ERF_C0 = 0x3F8060FE;
constexpr uint32_t ERF_P1[] = {0x38EB4C3A, 0xBAAE005B, 0x3C09919F, 0xBD24D99A, 0x3E235519, 0x3F69B4F9, 0x3F210A14};
constexpr uint32_t ERF_P2[] = {0x38B1E96A, 0xBA574D20, 0x3BAAD5EA, 0xBCDC1BE7, 0x3DE718AF, 0xBEC093AC, 0x3E0375D3};

// Clip x to [-3.92, 3.92]
__simd_callee__ inline void ErfClip(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    constexpr float ERF_BOUNDARY_MAX = 3.92;
    Reg::Mins(dstReg, srcReg, ERF_BOUNDARY_MAX, mask);
    Reg::Maxs(dstReg, dstReg, -ERF_BOUNDARY_MAX, mask);
}

// P(x) = (((((0.053443748819x^2+0.75517016694e1)x^2+0.10162808918e3)x^2
//          +0.13938061484e4)x^2+0.50637915060e4)x^2+0.29639384698e5)x
__simd_callee__ inline void ErfComputeP(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    constexpr float SCALAR_P0 = 0.29639384698e5;
    constexpr float SCALAR_P1 = 0.50637915060e4;
    constexpr float SCALAR_P2 = 0.13938061484e4;
    constexpr float SCALAR_P3 = 0.10162808918e3;
    constexpr float SCALAR_P4 = 0.75517016694e1;
    constexpr float SCALAR_P5 = 0.053443748819;

    Reg::RegTensor<float> tmpReg;
    Reg::Mul(tmpReg, srcReg, srcReg, mask);
    Reg::Muls(dstReg, tmpReg, SCALAR_P5, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_P4, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_P3, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_P2, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_P1, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_P0, mask);
    Reg::Mul(dstReg, dstReg, srcReg, mask);
}

// Q(x) = ((((x^2+0.31212858877e2)x^2+0.39856963806e3)x^2+0.30231248150e4)x^2+0.13243365831e5)x^2+0.26267224157e5
__simd_callee__ inline void ErfComputeQ(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    constexpr float SCALAR_Q0 = 0.26267224157e5;
    constexpr float SCALAR_Q1 = 0.13243365831e5;
    constexpr float SCALAR_Q2 = 0.30231248150e4;
    constexpr float SCALAR_Q3 = 0.39856963806e3;
    constexpr float SCALAR_Q4 = 0.31212858877e2;

    Reg::RegTensor<float> tmpReg;
    Reg::Mul(tmpReg, srcReg, srcReg, mask);
    Reg::Adds(dstReg, tmpReg, SCALAR_Q4, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_Q3, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_Q2, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_Q1, mask);
    Reg::Mul(dstReg, dstReg, tmpReg, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_Q0, mask);
}

__simd_callee__ inline void ErfPadeCompute(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    // x = Clip(x), Erf(x) = P(x) / Q(x)
    Reg::RegTensor<float> tmpReg;
    ErfClip(dstReg, srcReg, mask);
    ErfComputeP(tmpReg, dstReg, mask);
    ErfComputeQ(dstReg, dstReg, mask);

    Reg::Div(dstReg, tmpReg, dstReg, mask);
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, Reg::RegTensor<float>& srcReg2,
    Reg::RegTensor<float>& srcReg3, Reg::MaskReg& mask)
{
    Reg::RegTensor<float> tmpReg = srcReg1;
    Reg::FusedMulDstAdd(tmpReg, srcReg2, srcReg3, mask);
    dstReg = tmpReg;
}

__simd_callee__ inline void ErfSpecialCaseCompute(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg& mask)
{
    /*
     * if (f5 < int32_as_float(0x3F8060FE)) {
     *    *y = f26;
     * } else {
     *    float f23 = exp(f26 * log(2.0));
     *    float f25 = int32_as_float(0x3F800000) - f23;
     *    unsigned int r3 = float_as_int32(f4) & 0x80000000;
     *    unsigned int r4 = r3 | float_as_int32(f25);
     *    *y = int32_as_float(r4);
     * }
     */
    constexpr uint32_t ERF_R0 = 0x3F8060FE;
    constexpr uint32_t ERF_R1 = 0x3F800000;
    constexpr uint32_t ERF_R2 = 0x80000000;
    constexpr float LOG2_VALUE = 2.0f;

    Reg::RegTensor<float> tmpF5Reg, tmpF32Reg, tmpF32Reg1;
    Reg::RegTensor<uint32_t> tmpU32Reg;
    Reg::MaskReg cmpMask;
    Reg::Abs(tmpF5Reg, srcReg, mask);
    Reg::Duplicate(tmpU32Reg, ERF_R0, mask);
    Reg::Compare<float, CMPMODE::LT>(cmpMask, tmpF5Reg, (Reg::RegTensor<float>&)tmpU32Reg, mask);

    Reg::Duplicate(tmpF32Reg, LOG2_VALUE, mask);
    Reg::Log(tmpF32Reg, tmpF32Reg, mask);
    Reg::Mul(tmpF32Reg, tmpReg, tmpF32Reg, mask);
    Reg::Exp(tmpF32Reg, tmpF32Reg, mask); // tmpF32Reg: f23
    Reg::Duplicate(tmpU32Reg, ERF_R1, mask);
    Reg::Sub(tmpF32Reg1, (Reg::RegTensor<float>&)tmpU32Reg, tmpF32Reg, mask); // tmpF32Reg1: f25

    Reg::Duplicate(tmpU32Reg, ERF_R2, mask);
    Reg::And(tmpU32Reg, (Reg::RegTensor<uint32_t>&)srcReg, tmpU32Reg, mask);
    Reg::Or(tmpU32Reg, tmpU32Reg, (Reg::RegTensor<uint32_t>&)tmpF32Reg1, mask); // tmpU32Reg: r4

    Reg::Select(dstReg, tmpReg, (Reg::RegTensor<float>&)tmpU32Reg, cmpMask);
}

__simd_callee__ inline void ErfSubsectionCompute(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<float> tmpF5Reg, tmpF32Reg, tmpF32Reg1, tmpF32Reg2;
    Reg::RegTensor<uint32_t> tmpU32Reg, tmpU32Reg1;
    Reg::MaskReg cmpMask;
    /*
     * float f4 = x;
     * float f5 = fabsf(x);
     * bool p1 = f5 < int32_as_float(0x3F8060FE);
     * bool p2 = f5 >= int32_as_float(0x3F8060FE);
     */
    Reg::Abs(tmpF5Reg, srcReg, mask);
    Reg::Duplicate(tmpU32Reg, ERF_C0, mask);
    Reg::Compare<float, CMPMODE::GE>(cmpMask, tmpF5Reg, (Reg::RegTensor<float>&)tmpU32Reg, mask);
    /*
     * float f6 = f4 * f4;
     * float f7 = p2 ? f5 : f6;
     * float f8 = p2 ? int32_as_float(0x38EB4C3A) : int32_as_float(0x28B1E96A);
     * float f9 = p2 ? int32_as_float(0xBAAE005B) : int32_as_float(0xBA574D20);
     * float f10 = fmaf(f8, f7, f9);
     */
    Reg::Mul(tmpF32Reg1, srcReg, srcReg, mask);
    Reg::Select(tmpF32Reg, tmpF5Reg, tmpF32Reg1, cmpMask); // tmpF32Reg: f7
    Reg::Duplicate(tmpU32Reg, ERF_P1[0], mask);
    Reg::Duplicate(tmpU32Reg1, ERF_P2[0], mask);
    Reg::Select(tmpF32Reg1, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask);
    Reg::Duplicate(tmpU32Reg, ERF_P1[1], mask);
    Reg::Duplicate(tmpU32Reg1, ERF_P2[1], mask);
    Reg::Select(tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask);
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask); // tmpF32Reg1: f10
    /*
     * float f11 = p2 ? int32_as_float(0x3C09919F) : int32_as_float(0x3BAAD5EA);
     * float f12 = fmaf(f10, f7, f11);
     */
    Reg::Duplicate(tmpU32Reg, ERF_P1[2], mask);  // int32_as_float(0x3C09919F)
    Reg::Duplicate(tmpU32Reg1, ERF_P2[2], mask); // int32_as_float(0x3BAAD5EA)
    Reg::Select(
        tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask); // tmpF32Reg2: f11
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask);                                       // tmpF32Reg1: f12
    /*
     * float f13 = p2 ? int32_as_float(0xBD24D99A) : int32_as_float(0xBCDC1BE7);
     * float f14 = fmaf(f12, f7, f13);
     */
    Reg::Duplicate(tmpU32Reg, ERF_P1[3], mask);  // int32_as_float(0xBD24D99A)
    Reg::Duplicate(tmpU32Reg1, ERF_P2[3], mask); // int32_as_float(0xBCDC1BE7)
    Reg::Select(
        tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask); // tmpF32Reg2: f13
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask);                                       // tmpF32Reg1: f14
    /*
     * float f15 = p2 ? int32_as_float(0x3E235519) : int32_as_float(0x3DE718AF);
     * float f16 = fmaf(f14, f7, f15);
     */
    Reg::Duplicate(tmpU32Reg, ERF_P1[4], mask);  // int32_as_float(0x3E235519)
    Reg::Duplicate(tmpU32Reg1, ERF_P2[4], mask); // int32_as_float(0x3DE718AF)
    Reg::Select(
        tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask); // tmpF32Reg2: f13
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask);                                       // tmpF32Reg1: f16
    /*
     * float f17 = p2 ? int32_as_float(0x3F69B4F9) : int32_as_float(0xBEC093AC);
     * float f18 = fmaf(f16, f7, f17);
     */
    Reg::Duplicate(tmpU32Reg, ERF_P1[5], mask);  // int32_as_float(0x3F69B4F9)
    Reg::Duplicate(tmpU32Reg1, ERF_P2[5], mask); // int32_as_float(0xBEC093AC)
    Reg::Select(
        tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask); // tmpF32Reg2: f13
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask);                                       // tmpF32Reg1: f18
    /*
     * float f19 = p2 ? int32_as_float(0x3F210A14) : int32_as_float(0x3E0375D3);
     * float f20 = fmaf(f18, f7, f19);
     */
    Reg::Duplicate(tmpU32Reg, ERF_P1[6], mask);  // int32_as_float(0x3F210A14)
    Reg::Duplicate(tmpU32Reg1, ERF_P2[6], mask); // int32_as_float(0x3E0375D3)
    Reg::Select(
        tmpF32Reg2, (Reg::RegTensor<float>&)tmpU32Reg, (Reg::RegTensor<float>&)tmpU32Reg1, cmpMask); // tmpF32Reg2: f19
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg, tmpF32Reg2, mask);                                       // tmpF32Reg1: f20
    /*
     * float f21 = -f5;
     * float f22 = p2 ? f21 : f4
     * float f26 = fmaf(f20, f22, f22);
     */
    Reg::Neg(tmpF32Reg, tmpF5Reg, mask);
    Reg::Select(tmpF32Reg2, tmpF32Reg, srcReg, cmpMask);
    FMaf(tmpF32Reg1, tmpF32Reg1, tmpF32Reg2, tmpF32Reg2, mask); // tmpF32Reg1: f26
    ErfSpecialCaseCompute(dstReg, srcReg, tmpF32Reg1, mask);
}

template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
__simd_vf__ inline void ErfCoreImpl(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::MaskReg mask;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> dstReg;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(calCount);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
            Reg::Cast<float, T, castTraitF162F32>(castReg, srcReg, mask);
        } else {
            Reg::LoadAlign(castReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
        }
        if constexpr (config.algo == ErfAlgo::PADE_APPROXIMATION) {
            ErfPadeCompute(dstReg, castReg, mask);
        } else {
            ErfSubsectionCompute(dstReg, castReg, mask);
        }
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, castTraitF322F16>(srcReg, dstReg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * B32_DATA_NUM_PER_REPEAT, srcReg, mask);
        } else {
            Reg::StoreAlign(dstUb + i * B32_DATA_NUM_PER_REPEAT, dstReg, mask);
        }
    }
}
} // namespace ErfAPI

template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
__aicore__ inline void ErfCheckParams(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Erf");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Erf");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Erf");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Erf");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Erf");
}

template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
__aicore__ inline void ErfImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    ErfCheckParams<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    uint16_t repeatTimes = CeilDivision(calCount, B32_DATA_NUM_PER_REPEAT);
    ErfAPI::ErfCoreImpl<T, isReuseSource, config>(dstUb, srcUb, calCount, repeatTimes);
}

template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
__aicore__ inline void ErfImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ErfImpl<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}

} // namespace AscendC

#endif // IMPL_MATH_ERF_ERF_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_C310_IMPL_H__
#endif
