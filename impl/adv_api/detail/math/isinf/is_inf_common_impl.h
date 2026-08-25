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
 * \file if_inf_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/isinf/is_inf_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/is_inf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISINF_IS_INF_COMMON_IMPL_H__
#endif
#ifndef LIB_MATH_IS_INF_IMPL_H
#define LIB_MATH_IS_INF_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/is_inf_utils.h"

#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/isinf/is_inf_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T, typename U>
__simd_vf__ inline void IsInfImplVF(__ubuf__ T* dst, __ubuf__ U* src, uint32_t count, uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U));
    constexpr uint32_t floatInf = F32_INF;
    constexpr uint32_t floatNInf = F32_NEG_INF;
    constexpr uint32_t halfInf = 0x7c00;
    constexpr uint32_t halfNInf = 0xfc00;

    Reg::RegTensor<U> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<T> vReg0;
    Reg::RegTensor<T> vReg1;
    Reg::MaskReg mask;
    Reg::MaskReg cmpMaskReg;
    Reg::MaskReg cmpInfMask;
    Reg::MaskReg cmpNInfMask;
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
        if constexpr (Std::is_same_v<U, float>) {
            Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpInfMask, (Reg::RegTensor<uint32_t>&)srcVreg, floatInf, mask);
            Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpNInfMask, (Reg::RegTensor<uint32_t>&)srcVreg, floatNInf, mask);
        } else if constexpr (Std::is_same_v<U, half>) {
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(cmpInfMask, (Reg::RegTensor<uint16_t>&)srcVreg, halfInf, mask);
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(cmpNInfMask, (Reg::RegTensor<uint16_t>&)srcVreg, halfNInf, mask);
        }
        Reg::MaskOr(cmpMaskReg, cmpInfMask, cmpNInfMask, mask);
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

template <const IsInfConfig& config, typename T, typename U>
__aicore__ inline void IsInfImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(
        (SupportType<Tuple<T, U>, Tuple<half, half>, Tuple<bool, half>, Tuple<float, float>, Tuple<bool, float>>()),
        "Failed to check dtype in IsInf, current api "
        "support dtype combination is T : half/bool, U : half; T : float/bool, U : float");
    CHECK_FUNC_HIGHLEVEL_API(IsInf, (T, U, config.isReuseSource), (dst, src, sharedTmpBuffer, count));
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    IsInfImplVF<T, U>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), count, repeatTimes);
}

template <const IsInfConfig& config, typename T, typename U>
__aicore__ inline void IsInfImpl(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    IsInfImpl<config, T, U>(dst, src, sharedTmpBuffer, count);
}
} // namespace AscendC
#endif
#endif // IMPL_MATH_ISINF_ISINF_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISINF_IS_INF_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISINF_IS_INF_COMMON_IMPL_H__
#endif
