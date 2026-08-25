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
 * \file logical_not_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/logical_not/logical_not_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/logical_not.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOGICAL_NOT_LOGICAL_NOT_COMMON_IMPL_H__
#endif
#ifndef LIB_MATH_LOGICAL_NOT_IMPL_H
#define LIB_MATH_LOGICAL_NOT_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/logical_not_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/logical_not/logical_not_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T, typename U, typename RegT, typename RegU, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void LogicalNotVF(
    __ubuf__ T* dst, __ubuf__ U* src, uint16_t repeatTime, uint32_t count, uint32_t oneRepElm)
{
    RegT dstVreg;
    RegT brcZeroReg;
    RegT brcOneReg;
    RegU srcVreg;
    Reg::MaskReg mask;
    Reg::MaskReg cmpMask;

    Reg::Duplicate(brcOneReg, 1u);
    Reg::Duplicate(brcZeroReg, 0u);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<U, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::CompareScalar<U, CMPMODE::EQ>(cmpMask, srcVreg, static_cast<U>(0), mask);
        if constexpr (sizeof(U) == 2) {
            Reg::MaskPack(cmpMask, cmpMask);
            Reg::MaskPack(mask, mask);
        } else if constexpr (sizeof(U) == 4 || sizeof(U) == 8) {
            Reg::MaskPack(cmpMask, cmpMask);
            Reg::MaskPack(cmpMask, cmpMask);
            Reg::MaskPack(mask, mask);
            Reg::MaskPack(mask, mask);
        }
        Reg::Select(dstVreg, brcOneReg, brcZeroReg, cmpMask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <const LogicalNotConfig& config = DEFAULT_LOGICAL_NOT_CONFIG, typename T, typename U>
__aicore__ inline void LogicalNotImpl(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, bool>(), "LogicalNot only support bool data type on current device!");
    static_assert(
        SupportType<
            U, bool, uint8_t, int8_t, half, bfloat16_t, uint16_t, int16_t, float, uint32_t, int32_t, uint64_t,
            int64_t>(),
        "LogicalNot only support bool/uint8_t/int8_t/half/bfloat16_t/"
        "uint16_t/int16_t/float/uint32_t/int32_t/uint64_t/int64_t data type on current device!");
    CHECK_FUNC_HIGHLEVEL_API(LogicalNot, (T, U, config.isReuseSource), (dst, src, count));
    using RegT = Reg::RegTensor<T>;
    constexpr uint32_t LOGICAL_NOT_B64_REPEAT_STRIDE = 2;
    if constexpr (sizeof(U) == 8) {
        using RegU = Reg::RegTensor<U, Reg::RegTraitNumTwo>;
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U) * LOGICAL_NOT_B64_REPEAT_STRIDE);
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
        LogicalNotVF<T, U, RegT, RegU, Reg::RegTraitNumTwo>(
            (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), repeatTime, count, oneRepElm);
    } else {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(U));
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
        if constexpr (Std::is_same_v<U, bool>) {
            using RegU = Reg::RegTensor<uint8_t>;
            LogicalNotVF<T, uint8_t, RegT, RegU>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ uint8_t*)src.GetPhyAddr(), repeatTime, count, oneRepElm);
        } else {
            using RegU = Reg::RegTensor<U>;
            LogicalNotVF<T, U, RegT, RegU>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), repeatTime, count, oneRepElm);
        }
    }
}

} // namespace AscendC
#endif
#endif // IMPL_MATH_LOGICAL_NOT_LOGICAL_NOT_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOGICAL_NOT_LOGICAL_NOT_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOGICAL_NOT_LOGICAL_NOT_COMMON_IMPL_H__
#endif
