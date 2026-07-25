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
 * \file round_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/round/round_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/round.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ROUND_ROUND_C310_IMPL_H
#define IMPL_MATH_ROUND_ROUND_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/check.h"

namespace AscendC {
template <typename T, bool isReuseSource = false>
__simd_vf__ inline void RoundCompute(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calCount, const uint16_t repeatTimes)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dstReg;
    Reg::MaskReg maskReg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(calCount);
        Reg::LoadAlign<T>(srcReg, srcUb + i * repeatElm);
        Reg::Truncate<T, RoundMode::CAST_RINT>(dstReg, srcReg, maskReg);
        Reg::StoreAlign<T>(dstUb + i * repeatElm, dstReg, maskReg);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void RoundImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");

    RoundImpl(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void RoundImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, float, half>(), "Round only support half/float data type on current device");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Round");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Round");

    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, repeatElm));

    RoundCompute<T, isReuseSource>(dstUb, srcUb, calCount, repeatTimes);
}
} // namespace AscendC
#endif // IMPL_MATH_ROUND_ROUND_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_C310_IMPL_H__
#endif
