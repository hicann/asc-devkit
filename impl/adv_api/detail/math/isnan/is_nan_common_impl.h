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
 * \file if_nan_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/isnan/is_nan_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/is_nan.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISNAN_IS_NAN_COMMON_IMPL_H__
#endif
#ifndef LIB_MATH_IS_NAN_IMPL_H
#define LIB_MATH_IS_NAN_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/is_nan_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/isnan/is_nan_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T, typename U>
__simd_vf__ inline void IsNanImplVF(__ubuf__ T* dst, __ubuf__ U* src, uint32_t count, uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U));
    Reg::RegTensor<U> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<T> vReg0;
    Reg::RegTensor<T> vReg1;
    Reg::MaskReg mask;
    Reg::MaskReg cmpMaskReg;
    if constexpr (Std::is_same_v<T, bool>) {
        Reg::Duplicate((Reg::RegTensor<uint8_t>&)vReg0, 0u);
        Reg::Duplicate((Reg::RegTensor<uint8_t>&)vReg1, 1u);
    } else {
        Reg::Duplicate(vReg0, 0.0f);
        Reg::Duplicate(vReg1, 1.0f);
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<U>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
        Reg::Compare<U, CMPMODE::EQ>(cmpMaskReg, srcVreg, srcVreg, mask);
        Reg::Not(cmpMaskReg, cmpMaskReg, mask);
#else
        Reg::Compare<U, CMPMODE::NE>(cmpMaskReg, srcVreg, srcVreg, mask);
#endif
        if constexpr (Std::is_same_v<T, bool>) {
            if constexpr (Std::is_same_v<U, float>) {
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::Select(dstVreg, vReg1, vReg0, cmpMaskReg);
                Reg::MaskPack(mask, mask);
                Reg::MaskPack(mask, mask);
            } else if constexpr (Std::is_same_v<U, half>) {
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::Select(dstVreg, vReg1, vReg0, cmpMaskReg);
                Reg::MaskPack(mask, mask);
            }
        } else {
            Reg::Select(dstVreg, vReg1, vReg0, cmpMaskReg);
        }
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <const IsNanConfig& config, typename T, typename U>
__aicore__ inline void IsNanImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(
        (SupportType<Tuple<T, U>, Tuple<half, half>, Tuple<bool, half>, Tuple<float, float>, Tuple<bool, float>>()),
        "Failed to check dtype in IsNan, current api "
        "support dtype combination is T : half/bool, U : half; T : float/bool, U : float");
    CHECK_FUNC_HIGHLEVEL_API(IsNan, (T, U, config.isReuseSource), (dst, src, sharedTmpBuffer, count));
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    IsNanImplVF<T, U>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), count, repeatTimes);
}

template <const IsNanConfig& config, typename T, typename U>
__aicore__ inline void IsNanImpl(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    IsNanImpl<config, T, U>(dst, src, sharedTmpBuffer, count);
}
} // namespace AscendC
#endif
#endif // IMPL_MATH_ISNAN_ISNAN_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISNAN_IS_NAN_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISNAN_IS_NAN_COMMON_IMPL_H__
#endif
