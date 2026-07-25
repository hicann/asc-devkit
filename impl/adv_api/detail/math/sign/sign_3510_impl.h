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
 * \file sign_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/sign/sign_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/sign.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_SIGN_SIGN_C310_IMPL_H
#define IMPL_MATH_SIGN_SIGN_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"

namespace AscendC {
namespace SignInternal {
template <typename T, typename RegT, const Reg::RegTrait& trait = Reg::RegTraitNumOne>
__simd_vf__ inline void SignCoreCompute(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calCount, uint16_t repeatTime, uint32_t vlSize)
{
    Reg::MaskReg signMask;
    Reg::MaskReg cmpMask0;
    Reg::MaskReg cmpMask1;
    RegT brcZeroReg;
    RegT brcOneReg;
    RegT brcNegOneReg;
    RegT srcReg;
    RegT selReg0;
    RegT selReg1;
    Reg::Duplicate(brcZeroReg, 0);
    Reg::Duplicate(brcOneReg, 1);
    Reg::Duplicate(brcNegOneReg, -1);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        signMask = Reg::UpdateMask<T, trait>(calCount);
        Reg::LoadAlign(srcReg, srcUb + i * vlSize);
        Reg::CompareScalar<T, CMPMODE::LT>(cmpMask0, srcReg, 0, signMask);
        Reg::CompareScalar<T, CMPMODE::GT>(cmpMask1, srcReg, 0, signMask);
        Reg::Select(selReg0, brcNegOneReg, brcZeroReg, cmpMask0);
        Reg::Select(selReg1, brcOneReg, selReg0, cmpMask1);
        Reg::StoreAlign(dstUb + i * vlSize, selReg1, signMask);
    }
}
} // namespace SignInternal

template <typename T, bool isReuseSource = false>
__aicore__ inline void SignCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    SignCompute<T, isReuseSource>(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void SignCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    static_assert(
        SupportType<T, half, float, int64_t>(), "Sign only support half/float/int64_t data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Sign");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Sign");
    constexpr uint32_t SIGN_B64_REPEAT_STRIDE = 2;
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    if constexpr (sizeof(T) == 8) {
        using RegT = Reg::RegTensor<T, Reg::RegTraitNumTwo>;
        constexpr int32_t vlSize = static_cast<int32_t>(GetVecLen() / sizeof(T) * SIGN_B64_REPEAT_STRIDE);
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, vlSize));
        SignInternal::SignCoreCompute<T, RegT, Reg::RegTraitNumTwo>(dstUb, srcUb, calCount, repeatTime, vlSize);
    } else {
        using RegT = Reg::RegTensor<T>;
        constexpr int32_t vlSize = static_cast<int32_t>(GetVecLen() / sizeof(T));
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, vlSize));
        SignInternal::SignCoreCompute<T, RegT>(dstUb, srcUb, calCount, repeatTime, vlSize);
    }
}
} // namespace AscendC

#endif // IMPL_MATH_SIGN_SIGN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_C310_IMPL_H__
#endif
