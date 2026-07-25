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
 * \file exp_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/exp/exp_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/exp.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_EXP_EXP_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_EXP_EXP_C310_IMPL_H
#define IMPL_MATH_EXP_EXP_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"

namespace AscendC {
namespace ExpAPI {
constexpr Reg::CastTrait castTraitF162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitS162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitF322F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
template <typename T, uint8_t taylorExpandLevel>
__simd_vf__ inline void ExpCompute(
    __ubuf__ T* dst, __ubuf__ T* src, uint32_t calCount, uint16_t repeatTimes, __ubuf__ float* taylorExpandTmpBuffer)
{
    constexpr float dupConstant = 2.0f;
    constexpr uint32_t floatInf = F32_INF;
    constexpr uint32_t floatNInf = F32_NEG_INF;
    Reg::MaskReg mask, cmpInfMask, cmpNInfMask;
    Reg::RegTensor<T> dstVreg, tempSrcVreg;
    Reg::RegTensor<float> srcVreg, tempDstVreg;
    Reg::RegTensor<float> intVreg, expIntVreg;
    Reg::RegTensor<float> decimalVreg, expDecimalVreg;
    Reg::RegTensor<float> powVreg, denominatorVreg;
    Reg::RegTensor<float> factorialReg, tmpReg;
    Reg::RegTensor<int16_t> iterVreg;
    Reg::RegTensor<float> vReg0, vReg1;

    Reg::Duplicate(vReg0, 0.0f);
    Reg::Duplicate((Reg::RegTensor<uint32_t>&)vReg1, floatInf);
    mask = Reg::CreateMask<float>();
    Reg::Duplicate<float>(tmpReg, 1.0f);
    Reg::Arange<float>(factorialReg, dupConstant);
    Reg::Div(factorialReg, tmpReg, factorialReg, mask);
    Reg::StoreAlign(taylorExpandTmpBuffer, factorialReg, mask);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(calCount);
        if constexpr (IsSameType<T, half>::value) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(tempSrcVreg, src + i * oneRepSize);
            Reg::Cast<float, T, castTraitF162F32>(srcVreg, tempSrcVreg, mask);
        } else {
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
        }
        Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpInfMask, (Reg::RegTensor<uint32_t>&)srcVreg, floatInf, mask);
        Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpNInfMask, (Reg::RegTensor<uint32_t>&)srcVreg, floatNInf, mask);
        // intX = floor(x)
        Reg::Truncate<float, RoundMode::CAST_FLOOR>(intVreg, srcVreg, mask);
        // decimalX = x - intX
        Reg::Sub(decimalVreg, srcVreg, intVreg, mask);
        // expIntX = exp(intX)
        Reg::Exp(expIntVreg, intVreg, mask);
        // expDecimalX = sum((decimalX ^ n) / n!) n is taylorExpandLevel
        Reg::Adds(expDecimalVreg, decimalVreg, 1.0f, mask);
        if constexpr (taylorExpandLevel > 1) {
            powVreg = decimalVreg;
            constexpr uint16_t vloopEnd = taylorExpandLevel - 1;
            for (uint16_t j = 0; j < vloopEnd; ++j) {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(denominatorVreg, taylorExpandTmpBuffer + j);
                Reg::Mul(powVreg, powVreg, decimalVreg, mask);
                Reg::Mul(powVreg, powVreg, denominatorVreg, mask);
                Reg::Add(expDecimalVreg, expDecimalVreg, powVreg, mask);
            }
        }
        // exp(x) = expIntX * expDecimalX
        Reg::Mul(tempDstVreg, expIntVreg, expDecimalVreg, mask);
        Reg::Select(tempDstVreg, vReg0, tempDstVreg, cmpNInfMask);
        Reg::Select(tempDstVreg, vReg1, tempDstVreg, cmpInfMask);
        if constexpr (IsSameType<T, half>::value) {
            Reg::Cast<T, float, castTraitF322F16>(dstVreg, tempDstVreg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * oneRepSize, dstVreg, mask);
        } else {
            Reg::StoreAlign(dst + i * oneRepSize, tempDstVreg, mask);
        }
    }
}

template <typename T, uint8_t taylorExpandLevel, bool isReuseSource>
__aicore__ inline void ExpImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(dstLocal, "dstLocal", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcLocal, "srcLocal", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", dstLocal, "dstLocal", "Exp");
    CheckCalCount(calCount, "calCount", srcLocal, "srcLocal", "Exp");
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");

    if constexpr (taylorExpandLevel == 0) {
        Exp<T>(dstLocal, srcLocal, calCount);
        return;
    }
    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);
    __ubuf__ float* sharedTmpBufferAddr = (__ubuf__ float*)sharedTmpBuffer.GetPhyAddr();
    ExpAPI::ExpCompute<T, taylorExpandLevel>(dst, src, calCount, repeatTimes, sharedTmpBufferAddr);
}

template <typename T, uint8_t taylorExpandLevel, bool isReuseSource>
__aicore__ inline void ExpImpl(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ExpImpl<T, taylorExpandLevel, isReuseSource>(dstLocal, srcLocal, sharedTmpBuffer, calCount);
}

} // namespace ExpAPI
} // namespace AscendC
#endif // IMPL_MATH_EXP_EXP_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_EXP_EXP_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_EXP_EXP_C310_IMPL_H__
#endif
