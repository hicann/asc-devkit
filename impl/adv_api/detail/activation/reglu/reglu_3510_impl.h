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
 * \file reglu_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/reglu/reglu_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/reglu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_C310_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_REGLU_REGLU_IMPL_C310_H
#define IMPL_ACTIVATION_REGLU_REGLU_IMPL_C310_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {
template <typename T>
__simd_vf__ inline void ReGluImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t count, const uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcOrigin0;
    Reg::RegTensor<T> srcOrigin1;
    Reg::RegTensor<float> srcVreg0;
    Reg::RegTensor<float> srcVreg1;
    Reg::RegTensor<float> tmpReg0;
    Reg::RegTensor<float> dstVreg;
    Reg::MaskReg mask;
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(count);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin0, src0 + i * oneRepElm);
            Reg::Cast<float, T, castTraitB16ToB32>(srcVreg0, srcOrigin0, mask);
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin1, src1 + i * oneRepElm);
            Reg::Cast<float, T, castTraitB16ToB32>(srcVreg1, srcOrigin1, mask);
        } else {
            Reg::LoadAlign(srcVreg0, src0 + i * oneRepElm);
            Reg::LoadAlign(srcVreg1, src1 + i * oneRepElm);
        }
        Reg::Maxs(tmpReg0, srcVreg1, 0.0f, mask);
        Reg::Mul(dstVreg, srcVreg0, tmpReg0, mask);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, castTraitB32ToB16>((Reg::RegTensor<T>&)dstVreg, dstVreg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * oneRepElm, (Reg::RegTensor<T>&)dstVreg, mask);
        } else {
            Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        }
    }
}
} // namespace Internal

template <typename T, bool isReuseSource = false>
__aicore__ inline void ReGluImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(
        SupportType<T, half, float, bfloat16_t>(),
        "ReGlu only support half/float/bfloat16_t data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor0, "srcTensor0", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor1, "srcTensor1", "VECIN/VECOUT/VECCALC");
    CheckCalCount(count, "count", dstTensor, "dstTensor", "ReGlu");
    CheckCalCount(count, "count", srcTensor0, "srcTensor0", "ReGlu");
    CheckCalCount(count, "count", srcTensor1, "srcTensor1", "ReGlu");
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    Internal::ReGluImplVF<T>(
        (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor0.GetPhyAddr(), (__ubuf__ T*)srcTensor1.GetPhyAddr(),
        count, repeatTimes);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void ReGluImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    ReGluImpl<T, isReuseSource>(dstTensor, srcTensor0, srcTensor1, count);
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_REGLU_REGLU_IMPL_C310_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_C310_IMPL_H__
#endif
