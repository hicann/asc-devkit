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
 * \file cosh_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/cosh/cosh_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cosh.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COSH_COSH_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_COSH_COSH_C310_IMPL_H
#define IMPL_MATH_COSH_COSH_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/check.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {
// Computes cosh values based on input types.
// According formula: cosh(x) = (e^x + e^(-x))/2 = e^(x-ln2) + 0.25/(e^(x-ln2)).
template <typename T>
__simd_vf__ inline void CoshCompute(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calCount, uint16_t repeatTimes)
{
    constexpr float scalarNegLnTwo = -0.6931472;
    constexpr float scalarBrc = 0.25;
    constexpr uint32_t vlSize = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    Reg::MaskReg coshMask;
    Reg::RegTensor<float> brcReg;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> computeReg0;
    Reg::RegTensor<float> computeReg1;
    Reg::RegTensor<float> resReg;
    Reg::RegTensor<T> dstReg;
    Reg::Duplicate(brcReg, scalarBrc);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        coshMask = Reg::UpdateMask<float>(calCount);
        if constexpr (SupportBytes<T, 2>()) {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * vlSize);
            Reg::Cast<float, half, castTraitB16ToB32>(castReg, srcReg, coshMask);
        } else {
            Reg::LoadAlign(castReg, srcUb + i * vlSize);
        }
        Reg::Adds(castReg, castReg, scalarNegLnTwo, coshMask);
        Reg::Exp(computeReg0, castReg, coshMask);
        Reg::Div(computeReg1, brcReg, computeReg0, coshMask);
        Reg::Add(resReg, computeReg0, computeReg1, coshMask);
        if constexpr (SupportBytes<T, 2>()) {
            Reg::Cast<half, float, castTraitB32ToB16>(dstReg, resReg, coshMask);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * vlSize, dstReg, coshMask);
        } else {
            Reg::StoreAlign(dstUb + i * vlSize, resReg, coshMask);
        }
    }
}
} // namespace Internal

template <typename T, bool isReuseSource = false>
__aicore__ inline void CoshImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    CoshImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void CoshImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, half, float>(), "Cosh only support half/float data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Cosh");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Cosh");

    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    constexpr int32_t vlSize = static_cast<int32_t>(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, vlSize));
    Internal::CoshCompute<T>(dstUb, srcUb, calCount, repeatTimes);
}
} // namespace AscendC

#endif // IMPL_MATH_COSH_COSH_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COSH_COSH_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COSH_COSH_C310_IMPL_H__
#endif
