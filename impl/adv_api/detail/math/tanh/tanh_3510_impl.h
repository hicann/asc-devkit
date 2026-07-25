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
 * \file tanh_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/tanh/tanh_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/tanh.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_TANH_TANH_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_TANH_TANH_C310_IMPL_H
#define IMPL_MATH_TANH_TANH_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../common/check.h"

namespace AscendC {
namespace TanhInternal {
constexpr float FP32_ZERO_015 = 0.0157296831;
constexpr float FP32_ZERO_NEG_052 = -0.0523029624;
constexpr float FP32_ZERO_133 = 0.133152977;
constexpr float FP32_ZERO_NEG_333 = -0.333327681;
constexpr float FP32_TWENTY = 20.0;
constexpr float FP32_TWO = 2.0;
constexpr float FP32_ZERO_55 = 0.55;
constexpr float FP32_MIN_EXP = -8.8;
constexpr float FP32_MAX_EXP = 8.8;

constexpr Reg::CastTrait tanhCastTraitF162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait tanhCastTraitF322F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
} // namespace TanhInternal

template <typename T>
__simd_vf__ inline void TanhIntrinsicImpl(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t calCount, const uint16_t repeatTimes)
{
    uint32_t sreg = calCount;
    Reg::MaskReg preg;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> dstReg;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(sreg);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
            Reg::Cast<float, T, TanhInternal::tanhCastTraitF162F32>(castReg, srcReg, preg);
        } else {
            Reg::LoadAlign(castReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
        }
        Reg::Mins(castReg, castReg, TanhInternal::FP32_MAX_EXP, preg);
        Reg::Maxs(castReg, castReg, TanhInternal::FP32_MIN_EXP, preg);
        Reg::Muls(tmpReg, castReg, TanhInternal::FP32_TWO, preg);
        Reg::Exp(castReg, tmpReg, preg);

        Reg::Adds(dstReg, castReg, -1.0f, preg);
        Reg::Adds(tmpReg, castReg, 1.0f, preg);
        Reg::Div(dstReg, dstReg, tmpReg, preg);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, TanhInternal::tanhCastTraitF322F16>(srcReg, dstReg, preg);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * B32_DATA_NUM_PER_REPEAT, srcReg, preg);
        } else {
            Reg::StoreAlign(dstUb + i * B32_DATA_NUM_PER_REPEAT, dstReg, preg);
        }
    }
}

template <typename T>
__simd_vf__ inline void TanhCompensationImpl(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t calCount, const uint16_t repeatTimes)
{
    uint32_t sreg = calCount;
    Reg::MaskReg preg, cmpMaskReg;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> vregInput, vregInputAbs;
    Reg::RegTensor<float> vregInputSqr, vregInputMid;
    Reg::RegTensor<float> vregOutput;
    Reg::RegTensor<float> vregScalar1, vregScalar2;

    Reg::Duplicate(vregScalar1, TanhInternal::FP32_ZERO_133);
    Reg::Duplicate(vregScalar2, TanhInternal::FP32_ZERO_NEG_333);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(sreg);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
            Reg::Cast<float, T, TanhInternal::tanhCastTraitF162F32>(vregInput, srcReg, preg);
        } else {
            Reg::LoadAlign(vregInput, srcUb + i * B32_DATA_NUM_PER_REPEAT);
        }
        Reg::Mul(vregInputSqr, vregInput, vregInput, preg);
        Reg::Muls(vregOutput, vregInputSqr, TanhInternal::FP32_ZERO_015, preg);
        Reg::Adds(vregOutput, vregOutput, TanhInternal::FP32_ZERO_NEG_052, preg);
        Reg::FusedMulDstAdd(vregOutput, vregInputSqr, vregScalar1, preg);
        Reg::FusedMulDstAdd(vregOutput, vregInputSqr, vregScalar2, preg);
        Reg::Mul(vregOutput, vregOutput, vregInputSqr, preg);
        Reg::FusedMulDstAdd(vregOutput, vregInput, vregInput, preg);

        Reg::Abs(vregInputAbs, vregInput, preg);
        Reg::Mins(vregInput, vregInput, TanhInternal::FP32_TWENTY, preg);
        Reg::Muls(vregInput, vregInput, TanhInternal::FP32_TWO, preg);
        Reg::Exp(vregInput, vregInput, preg);
        Reg::Adds(vregInputMid, vregInput, -1.0f, preg);
        Reg::Adds(vregInputSqr, vregInput, 1.0f, preg);
        Reg::Div(vregInputMid, vregInputMid, vregInputSqr, preg);

        Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg, vregInputAbs, TanhInternal::FP32_ZERO_55, preg);
        Reg::Select(vregOutput, vregOutput, vregInputMid, cmpMaskReg);

        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, TanhInternal::tanhCastTraitF322F16>(srcReg, vregOutput, preg);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * B32_DATA_NUM_PER_REPEAT, srcReg, preg);
        } else {
            Reg::StoreAlign(dstUb + i * B32_DATA_NUM_PER_REPEAT, vregOutput, preg);
        }
    }
}

/*
 * Formula is y= (e^(2x)-1)/(e^(2x)+1)
 */
template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
__aicore__ inline void TanhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");

    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Tanh");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Tanh");

    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    uint16_t repeatTimes = CeilDivision(calCount, B32_DATA_NUM_PER_REPEAT);
    if constexpr (config.algo == TanhAlgo::INTRINSIC) {
        TanhIntrinsicImpl<T>(dstUb, srcUb, calCount, repeatTimes);
    } else {
        TanhCompensationImpl<T>(dstUb, srcUb, calCount, repeatTimes);
    }
}

template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
__aicore__ inline void TanhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    TanhImpl<T, isReuseSource, config>(dstTensor, srcTensor, calCount);
}
} // namespace AscendC

#endif // IMPL_MATH_TANH_TANH_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_TANH_TANH_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_TANH_TANH_C310_IMPL_H__
#endif
