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
 * \file bitwise_template.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/bitwise_template/bitwise_template.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/bitwise_and.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H__
#endif
#ifndef IMPL_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H
#define IMPL_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"

namespace AscendC {
template <auto func, typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void BitwiseTemplateImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, uint16_t repeatTime, uint32_t count, uint32_t oneRepElm,
    uint32_t offset)
{
    RegT dstVreg;
    RegT src0Vreg;
    RegT src1Vreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(src0Vreg, src0 + i * oneRepElm);
        Reg::LoadAlign(src1Vreg, src1 + i * oneRepElm);
        func(dstVreg, src0Vreg, src1Vreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(src0Vreg, src0 + i * oneRepElm + offset);
        Reg::LoadAlign(src1Vreg, src1 + i * oneRepElm + offset);
        func(dstVreg, src0Vreg, src1Vreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm + offset, dstVreg, mask);
    }
    mask = Reg::UpdateMask<T, Trait>(count);
    Reg::LoadAlign(src0Vreg, src0 + repeatTime * 2 * oneRepElm);
    Reg::LoadAlign(src1Vreg, src1 + repeatTime * 2 * oneRepElm);
    func(dstVreg, src0Vreg, src1Vreg, mask);
    Reg::StoreAlign(dst + repeatTime * 2 * oneRepElm, dstVreg, mask);
}

template <auto func, typename T>
__aicore__ inline void BitwiseTemplateImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "only support uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t data type on current device!");

    __ubuf__ T* dstTensor = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* src0Tensor = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* src1Tensor = (__ubuf__ T*)src1.GetPhyAddr();

    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * 2);
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm) / 2);
        uint32_t offset = static_cast<uint32_t>(repeatTime * oneRepElm);
        BitwiseTemplateImplVF<func, T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTraitNumTwo>(
            dstTensor, src0Tensor, src1Tensor, repeatTime, count, oneRepElm, offset);
    } else {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm) / 2);
        uint32_t offset = static_cast<uint32_t>(repeatTime * oneRepElm);
        BitwiseTemplateImplVF<func, T, Reg::RegTensor<T>>(
            dstTensor, src0Tensor, src1Tensor, repeatTime, count, oneRepElm, offset);
    }
}
} // namespace AscendC
#endif
#endif // IMPL_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_TEMPLATE_BITWISE_TEMPLATE_H__
#endif
