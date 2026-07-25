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
 * \file atanh_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/atanh/atanh_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/atanh.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATANH_ATANH_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ATANH_ATANH_C310_IMPL_H
#define IMPL_MATH_ATANH_ATANH_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/common.h"
#include "../../common/check.h"

namespace AscendC {
namespace Internal {
constexpr float ATANH_ONE = 1;
constexpr float ATANH_NEG_ONE = -1;
constexpr float ATANH_MULS_CONSTANT = 0.5;
/*
    atanh = 0.5 * ln((1 + x) / (1 - x))
*/
template <typename T>
__simd_vf__ inline void AtanhImplVF(__ubuf__ T* dst, __ubuf__ T* src, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> tmpReg2;
    Reg::RegTensor<float> dupReg;
    Reg::MaskReg mask;
    constexpr int32_t oneRepElm = static_cast<int32_t>(GetVecLen() / sizeof(float));
    Reg::Duplicate<float>(dupReg, ATANH_ONE);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(calCount);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, src + i * oneRepElm);
            Reg::Cast<float, half, castTraitB16ToB32>((Reg::RegTensor<float>&)srcVreg, srcVreg, mask);
        } else {
            Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        }
        Reg::Adds(tmpReg1, (Reg::RegTensor<float>&)srcVreg, ATANH_ONE, mask);
        Reg::Sub(tmpReg2, dupReg, (Reg::RegTensor<float>&)srcVreg, mask);
        Reg::Div(tmpReg1, tmpReg1, tmpReg2, mask);
        Reg::Ln(tmpReg1, tmpReg1, mask);
        Reg::Muls((Reg::RegTensor<float>&)dstVreg, tmpReg1, ATANH_MULS_CONSTANT, mask);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<half, float, castTraitB32ToB16>(dstVreg, (Reg::RegTensor<float>&)dstVreg, mask);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(dst + i * oneRepElm, dstVreg, mask);
        } else {
            Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        }
    }
}
} // namespace Internal

template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "Atanh only support half/float data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Atanh");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Atanh");
    constexpr int32_t oneRepElm = static_cast<int32_t>(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, oneRepElm));
    Internal::AtanhImplVF<T>(
        (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount, repeatTimes);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    AtanhImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanhImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    AtanhImpl<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AtanhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    AtanhImpl<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

} // namespace AscendC
#endif // IMPL_MATH_ATANH_ATANH_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATANH_ATANH_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ATANH_ATANH_C310_IMPL_H__
#endif
