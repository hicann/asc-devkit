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
 * \file silu_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/silu/silu_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/silu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_C310_IMPL_H__
#endif

#ifndef IMPL_MATH_SILU_SILU_C310_IMPL_H
#define IMPL_MATH_SILU_SILU_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"

namespace AscendC {
namespace Internal {
constexpr Reg::CastTrait siluFloat2HalfCastTrait0 = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::MERGING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait siluFloat2HalfCastTrait1 = {
    Reg::RegLayout::ONE, Reg::SatMode::SAT, Reg::MaskMergeMode::MERGING, RoundMode::CAST_RINT};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
template <typename T, bool highPrecision = false>
__simd_vf__ inline void SiluComputeVF(__ubuf__ T* dst, __ubuf__ T* src, uint32_t count, const uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> tmpReg0;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<T> tmpReg1;
    Reg::RegTensor<float> tmpCastReg0;
    Reg::RegTensor<float> tmpCastReg1;
    Reg::MaskReg mask;
    if constexpr (highPrecision && IsSameType<T, half>::value) {
        mask = Reg::CreateMask<T>();
        Reg::Duplicate(tmpReg1, 0, mask);
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::Muls(tmpReg0, srcVreg, -1.0f, mask);
        if constexpr (highPrecision && IsSameType<T, half>::value) {
            Reg::ExpSub<float, T, Reg::RegLayout::ZERO>(tmpCastReg0, tmpReg0, tmpReg1, mask);
            Reg::ExpSub<float, T, Reg::RegLayout::ONE>(tmpCastReg1, tmpReg0, tmpReg1, mask);
            Reg::Cast<half, float, siluFloat2HalfCastTrait0>(tmpReg0, tmpCastReg0, mask);
            Reg::Cast<half, float, siluFloat2HalfCastTrait1>(tmpReg0, tmpCastReg1, mask);
        } else {
            Reg::Exp(tmpReg0, tmpReg0, mask);
        }
        Reg::Adds(tmpReg0, tmpReg0, 1.0f, mask);
        Reg::Div(dstVreg, srcVreg, tmpReg0, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}
#else
template <typename T>
__simd_vf__ inline void SiluComputeVF(__ubuf__ T* dst, __ubuf__ T* src, uint32_t count, const uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> tmpReg0;
    Reg::RegTensor<T> dstVreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::Muls(dstVreg, srcVreg, -1.0f, mask);
        Reg::Exp(tmpReg0, dstVreg, mask);
        Reg::Adds(tmpReg0, tmpReg0, 1.0f, mask);
        Reg::Div(dstVreg, srcVreg, tmpReg0, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}
#endif
} // namespace Internal

template <typename T, bool isReuseSource = false, bool highPrecision = false>
__aicore__ inline void SiluCompute(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "Silu only support half/float data type on current device!");
    CheckTensorPosition(dstLocal, "dstLocal", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcLocal, "srcLocal", "VECIN/VECOUT/VECCALC");
    CheckCalCount(count, "count", dstLocal, "dstLocal", "Silu");
    CheckCalCount(count, "count", srcLocal, "srcLocal", "Silu");

    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
    Internal::SiluComputeVF<T, highPrecision>(
#else
    Internal::SiluComputeVF<T>(
#endif
        (__ubuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), count, repeatTimes);
}
} // namespace AscendC
#endif // IMPL_MATH_SILU_SILU_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_C310_IMPL_H__
#endif
