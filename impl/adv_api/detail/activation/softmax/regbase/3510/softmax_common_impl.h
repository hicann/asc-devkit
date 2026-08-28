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
 * \file softmax_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/3510/softmax_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_COMMON_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_COMMON_IMPL_H

#include "../../../../common/common.h"

namespace AscendC {
__simd_callee__ inline void SoftmaxExpSub(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg0, Reg::RegTensor<float>& srcReg1, Reg::MaskReg& mask)
{
#if !defined(__ASC_FTZ__)
    Reg::RegTensor<float> tmpReg;
    Reg::Sub(tmpReg, srcReg0, srcReg1, mask);
    Reg::Exp(dstReg, tmpReg, mask);
#else
    Reg::FusedExpSub(dstReg, srcReg0, srcReg1, mask);
#endif
}

template <typename T>
__simd_callee__ inline void LoadIfNeedCast(Reg::RegTensor<float>& dstReg, __ubuf__ T* srcUb, Reg::MaskReg& preg)
{
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> tmpReg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(tmpReg, srcUb);
        Reg::Cast<float, T, Internal::castTraitB16ToB32>(dstReg, tmpReg, preg);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_NORM>(dstReg, srcUb);
    }
}

template <typename T>
__simd_callee__ inline void LoadIfNeedCastM1(Reg::RegTensor<float>& dstReg, __ubuf__ T* srcUb, Reg::MaskReg& preg)
{
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> castVreg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(castVreg, srcUb);
        Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)castVreg, (Reg::RegTensor<uint16_t>&)castVreg);
        Reg::Cast<float, T, Internal::castTraitB16ToB32>(dstReg, castVreg, preg);
    } else {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(dstReg, srcUb);
    }
}

template <typename T>
__simd_callee__ inline void StoreIfNeedCastM1(__ubuf__ T* dstUb, Reg::RegTensor<float>& srcReg, Reg::MaskReg& preg)
{
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> castVreg;
        Reg::Cast<T, float, Internal::castTraitB32ToB16>(castVreg, srcReg, preg);
        Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)castVreg, (Reg::RegTensor<uint32_t>&)castVreg);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B16>(dstUb, castVreg, preg);
    } else {
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(dstUb, srcReg, preg);
    }
}

template <typename T>
__simd_callee__ inline void StoreIfNeedCast(__ubuf__ T* dstUb, Reg::RegTensor<float>& srcReg, Reg::MaskReg& preg)
{
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> tmpReg;
        Reg::Cast<T, float, Internal::castTraitB32ToB16>(tmpReg, srcReg, preg);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb, tmpReg, preg);
    } else {
        Reg::StoreAlign<T, Reg::StoreDist::DIST_NORM>(dstUb, srcReg, preg);
    }
}

template <typename T>
__simd_callee__ inline void LoadE2B(Reg::RegTensor<T>& dstReg, __ubuf__ T* srcUb)
{
    if constexpr (sizeof(T) == 2) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B16>(dstReg, srcUb);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B32>(dstReg, srcUb);
    }
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_IMPL_H__
#endif
