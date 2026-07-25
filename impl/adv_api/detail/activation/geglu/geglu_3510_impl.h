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
 * \file geglu_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/geglu/geglu_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/geglu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GEGLU_C310_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_GEGLU_GEGLU_IMPL_C310_H
#define IMPL_ACTIVATION_GEGLU_GEGLU_IMPL_C310_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {
constexpr float gegluConstantA = 22.36386;
constexpr float gegluConstantB = -0.071354814;
template <typename T>
__simd_vf__ inline void GeGLUImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t count, const uint16_t repeatTimes)
{
    Reg::RegTensor<half> srcOrigin0;
    Reg::RegTensor<half> srcOrigin1;
    Reg::RegTensor<float> srcVreg0;
    Reg::RegTensor<float> srcVreg1;
    Reg::RegTensor<float> tmpReg0;
    Reg::RegTensor<float> tmpReg1;
    Reg::RegTensor<float> dstVreg;
    Reg::MaskReg mask;
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(count);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin0, src0 + i * oneRepElm);
            Reg::Cast<float, half, castTraitB16ToB32>(srcVreg0, srcOrigin0, mask);
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin1, src1 + i * oneRepElm);
            Reg::Cast<float, half, castTraitB16ToB32>(srcVreg1, srcOrigin1, mask);
        } else {
            Reg::LoadAlign(srcVreg0, src0 + i * oneRepElm);
            Reg::LoadAlign(srcVreg1, src1 + i * oneRepElm);
        }
        Reg::Mul(tmpReg0, srcVreg1, srcVreg1, mask);
        Reg::Adds(tmpReg0, tmpReg0, gegluConstantA, mask);
        Reg::Mul(tmpReg0, tmpReg0, srcVreg1, mask);
        Reg::Muls(tmpReg0, tmpReg0, gegluConstantB, mask);
        Reg::Exp(tmpReg1, tmpReg0, mask);
        Reg::Adds(tmpReg1, tmpReg1, 1.0f, mask);
        Reg::Div(tmpReg1, srcVreg1, tmpReg1, mask);
        Reg::Mul(dstVreg, srcVreg0, tmpReg1, mask);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<half, float, castTraitB32ToB16>((Reg::RegTensor<half>&)dstVreg, dstVreg, mask);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(
                dst + i * oneRepElm, (Reg::RegTensor<half>&)dstVreg, mask);
        } else {
            Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        }
    }
}
} // namespace Internal

template <typename T, bool isReuseSource = false>
__aicore__ inline void GeGLUImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "GeGLU only support half/float data type on current device!");
    ASCENDC_ASSERT((srcTensor0.GetSize() == srcTensor1.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "Input params.GetSize must be equal with each other!");
    });
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor0, "srcTensor0", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor1, "srcTensor1", "VECIN/VECOUT/VECCALC");
    CheckCalCount(count, "count", dstTensor, "dstTensor", "GeGLU");
    CheckCalCount(count, "count", srcTensor0, "srcTensor0", "GeGLU");
    CheckCalCount(count, "count", srcTensor1, "srcTensor1", "GeGLU");
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    Internal::GeGLUImplVF<T>(
        (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor0.GetPhyAddr(), (__ubuf__ T*)srcTensor1.GetPhyAddr(),
        count, repeatTimes);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void GeGLUImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    GeGLUImpl<T, isReuseSource>(dstTensor, srcTensor0, srcTensor1, count);
}

} // namespace AscendC
#endif // IMPL_ACTIVATION_GEGLU_GEGLU_IMPL_C310_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GEGLU_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GEGLU_C310_IMPL_H__
#endif
