/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_gather_mask_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_gather_mask_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_gather_mask_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_GATHER_MASK_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_GATHER_MASK_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, GatherMaskMode store = GatherMaskMode::NO_STORE_REG, typename U>
__simd_callee__ inline void GatherMaskImpl(U& dstReg, U srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, float>(),
        "current data type is not supported on current device!");
    constexpr auto modeValue = std::integral_constant<::StoreMode, static_cast<::StoreMode>(store)>();
    vsqz(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void PrefixSumImpl(U& dstReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t>(),
        "current data type is not supported on current device!");
    vusqz(dstReg, mask);
}

template <SpecialPurposeReg spr = SpecialPurposeReg::AR>
__aicore__ inline int64_t GetSprImpl()
{
    static_assert(
        SupportEnum<spr, SpecialPurposeReg::AR>(),
        "current GetSpr api only support SpecialPurposeReg AR on current device!");
    return get_ar();
}

template <SpecialPurposeReg spr = SpecialPurposeReg::AR>
__simd_callee__ inline void ClearSprImpl()
{
    constexpr uint8_t SPR_AR_VALUE = 74;

    static_assert(
        SupportEnum<spr, SpecialPurposeReg::AR>(),
        "current ClearSpr api only support SpecialPurposeReg AR on current device!");
    constexpr auto sprValue = std::integral_constant<::Spr, static_cast<::Spr>(SPR_AR_VALUE)>();
    sprclr(sprValue);
}

template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void GatherImpl(S& dstReg, S& srcReg, V& indexReg)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>,
        "Gather T data type is not correct on current device!");
    static_assert(
        Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>,
        "Gather U data type is not correct  on current device!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "Gather does not support current data type on current device!");
    static_assert(
        SupportType<ActualU, uint8_t, uint16_t, uint32_t>(),
        "Gather does not support current data type on current device!");
    static_assert(
        (sizeof(ActualT) == 1 && sizeof(ActualU) == 1) || (sizeof(ActualT) == 2 && sizeof(ActualU) == 2) ||
            (sizeof(ActualT) == 4 && sizeof(ActualU) == 4),
        "Gather does not support current data type combination on current device!");
    if constexpr (sizeof(ActualT) == 1) {
        vselr((RegTensor<uint8_t>&)dstReg, (RegTensor<uint8_t>&)srcReg, (RegTensor<uint8_t>&)indexReg);
    } else if constexpr (sizeof(ActualT) == 2) {
        vselr((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, (RegTensor<uint16_t>&)indexReg);
    } else if constexpr (sizeof(ActualT) == 4) {
        vselr((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, (RegTensor<uint32_t>&)indexReg);
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_GATHER_MASK_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_IMPL__
#endif
