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
 * \file asinh_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/asinh/asinh_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/asinh.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASINH_ASINH_C310_IMPL_H__
#endif
#ifndef DETAIL_MATH_ASINH_ASINH_C310_IMPL_H
#define DETAIL_MATH_ASINH_ASINH_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/check.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {
constexpr float ASINH_ONE = 1;
constexpr float ASINH_NEG_ONE = -1;
constexpr float ASINH_ZERO = 0;
/*
    when x < 0 : asinh = -ln(-x + sqrt(x ^ 2 - 1))
    when x > 0 : asinh = ln(x + sqrt(x ^ 2 - 1))
*/
template <typename T>
__simd_vf__ inline void AsinhImplVF(__ubuf__ T* dst, __ubuf__ T* src, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> tmpReg2;
    Reg::MaskReg mask;
    Reg::MaskReg signMaskReg;
    constexpr int32_t oneRepElm = static_cast<int32_t>(GetVecLen() / sizeof(float));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(calCount);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, src + i * oneRepElm);
            Reg::Cast<float, half, castTraitB16ToB32>((Reg::RegTensor<float>&)srcVreg, srcVreg, mask);
        } else {
            Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        }
        Reg::CompareScalar<float, CMPMODE::LT>(signMaskReg, (Reg::RegTensor<float>&)srcVreg, ASINH_ZERO, mask);
        Reg::Abs(tmpReg1, (Reg::RegTensor<float>&)srcVreg, mask);
        Reg::Mul(tmpReg2, (Reg::RegTensor<float>&)srcVreg, (Reg::RegTensor<float>&)srcVreg, mask);
        Reg::Adds(tmpReg2, tmpReg2, ASINH_ONE, mask);
        Reg::Sqrt(tmpReg2, tmpReg2, mask);
        Reg::Add(tmpReg1, tmpReg1, tmpReg2, mask);
        Reg::Ln(tmpReg1, tmpReg1, mask);
        Reg::Muls((Reg::RegTensor<float>&)dstVreg, tmpReg1, ASINH_NEG_ONE, signMaskReg);
        Reg::Or(
            (Reg::RegTensor<uint32_t>&)dstVreg, (Reg::RegTensor<uint32_t>&)dstVreg, (Reg::RegTensor<uint32_t>&)tmpReg1,
            mask);
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
__aicore__ inline void AsinhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "Asinh only support half/float data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Asinh");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Asinh");
    constexpr int32_t oneRepElm = static_cast<int32_t>(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, oneRepElm));
    Internal::AsinhImplVF<T>(
        (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount, repeatTimes);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AsinhImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    AsinhImpl<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void AsinhImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    AsinhImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
}

} // namespace AscendC
#endif // DETAIL_MATH_ASINH_ASINH_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASINH_ASINH_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ASINH_ASINH_C310_IMPL_H__
#endif
