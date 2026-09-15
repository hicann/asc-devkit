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
 * \file if_finite_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/isfinite/is_finite_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/is_finite.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISFINITE_IS_FINITE_COMMON_IMPL_H__
#endif
#ifndef LIB_MATH_IS_FINITE_IMPL_H
#define LIB_MATH_IS_FINITE_IMPL_H
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
// Implementation Process
// 1. Use vcmp_ne for comparison. If the value is nan, the out should be false.
// 2. Use vcmps_eq for comparison. If the value is ±inf, the out should be false.
// 3. Use MaskOr to get final mask
// 4. Based on the output data type, use the corresponding 1 and 0 of maskreg select.

namespace AscendC {
template <typename T, typename U>
__simd_vf__ inline void IsFiniteVFImpl(__ubuf__ U* dstUb, __ubuf__ T* srcUb, uint32_t calCount)
{
    constexpr float ONE = 1.0;
    constexpr uint16_t BF16_ONE = 0x3f80;
    constexpr float ZERO = 0.0;
    constexpr uint32_t INF = 0x7f800000;
    constexpr uint32_t NEG_INF = 0xff800000;
    constexpr uint32_t HALF_INF = 0x7c00;
    constexpr uint32_t HALF_NEG_INF = 0xfc00;
    constexpr uint32_t B_HALF_INF = 0x7f80;
    constexpr uint32_t B_HALF_NEG_INF = 0xff80;

    Reg::RegTensor<T, Reg::RegTraitNumOne> vSrcReg0;
    Reg::RegTensor<U, Reg::RegTraitNumOne> vDstReg0, vReg0, vReg1;

    if constexpr (IsSameType<U, bfloat16_t>::value) {
        Reg::Duplicate((Reg::RegTensor<uint16_t, Reg::RegTraitNumOne>&)vReg0, ZERO);
        Reg::Duplicate((Reg::RegTensor<uint16_t, Reg::RegTraitNumOne>&)vReg1, BF16_ONE);
    } else if constexpr (IsSameType<U, bool>::value) {
        Reg::Duplicate((Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg0, ZERO);
        Reg::Duplicate((Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg1, ONE);
    } else {
        Reg::Duplicate(vReg0, ZERO);
        Reg::Duplicate(vReg1, ONE);
    }

    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg preg;
    Reg::MaskReg cmpMaskReg;
    Reg::MaskReg cmpMaskNAN, cmpMaskPINF, cmpMaskNINF, cmpMaskINF;

    uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<T, Reg::RegTraitNumOne>(sreg);
        Reg::LoadAlign(vSrcReg0, srcUb + i * sregLower);
        Reg::Compare<T, CMPMODE::NE>(cmpMaskNAN, vSrcReg0, vSrcReg0, preg);
        if constexpr (IsSameType<T, float>::value) {
            Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpMaskPINF, (Reg::RegTensor<uint32_t>&)vSrcReg0, INF, preg);
            Reg::CompareScalar<uint32_t, CMPMODE::EQ>(cmpMaskNINF, (Reg::RegTensor<uint32_t>&)vSrcReg0, NEG_INF, preg);
        } else if constexpr (IsSameType<T, half>::value) {
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(cmpMaskPINF, (Reg::RegTensor<uint16_t>&)vSrcReg0, HALF_INF, preg);
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(
                cmpMaskNINF, (Reg::RegTensor<uint16_t>&)vSrcReg0, HALF_NEG_INF, preg);
        } else if constexpr (IsSameType<T, bfloat16_t>::value) {
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(
                cmpMaskPINF, (Reg::RegTensor<uint16_t>&)vSrcReg0, B_HALF_INF, preg);
            Reg::CompareScalar<uint16_t, CMPMODE::EQ>(
                cmpMaskNINF, (Reg::RegTensor<uint16_t>&)vSrcReg0, B_HALF_NEG_INF, preg);
        }

        Reg::MaskOr(cmpMaskINF, cmpMaskPINF, cmpMaskNINF, preg);
        Reg::MaskOr(cmpMaskReg, cmpMaskNAN, cmpMaskINF, preg);
        if constexpr (IsSameType<U, bool>::value) {
            if constexpr (IsSameType<T, float>::value) {
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::Select(
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vDstReg0,
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg0,
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg1, cmpMaskReg);
                Reg::MaskPack(preg, preg);
                Reg::MaskPack(preg, preg);
            } else {
                Reg::MaskPack(cmpMaskReg, cmpMaskReg);
                Reg::Select(
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vDstReg0,
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg0,
                    (Reg::RegTensor<int8_t, Reg::RegTraitNumOne>&)vReg1, cmpMaskReg);
                Reg::MaskPack(preg, preg);
            }
        } else {
            Reg::Select(vDstReg0, vReg0, vReg1, cmpMaskReg);
        }
        Reg::StoreAlign(dstUb + i * sregLower, vDstReg0, preg);
    }
}

template <typename T, typename U>
__aicore__ inline void IsFiniteImpl(const LocalTensor<U>& dst, const LocalTensor<T>& src, uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    ASCENDC_ASSERT(calCount <= src.GetSize(), {
        KERNEL_LOG(KERNEL_ERROR, "CalCount should <= %d", src.GetSize());
        return;
    });

    static_assert(
        (SupportType<
            Tuple<U, T>, Tuple<half, half>, Tuple<bool, half>, Tuple<float, float>, Tuple<bool, float>,
            Tuple<bfloat16_t, bfloat16_t>, Tuple<bool, bfloat16_t>>()),
        "Failed to check dtype in IsInf, current api support dtype combination is U : half/bool, "
        "T : half; U : float/bool, T : float, U: bfloat16_t/bool, T: bfloat16_t");

    __ubuf__ T* srcUb = (__ubuf__ T*)src.GetPhyAddr();
    __ubuf__ U* dstUb = (__ubuf__ U*)dst.GetPhyAddr();

    IsFiniteVFImpl<T, U>(dstUb, srcUb, calCount);
}

} // namespace AscendC
#endif
#endif // IMPL_MATH_ISFINITE_ISFINITE_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISFINITE_IS_FINITE_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ISFINITE_IS_FINITE_COMMON_IMPL_H__
#endif
