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
 * \file kernel_reg_compute_copy_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_copy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_COPY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_COPY_IMPL_H

#include "kernel_reg_compute_common_impl.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::MERGING, typename U>
__simd_callee__ inline void CopyImpl(U& dstReg, U& srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, half, float,
            bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::MERGING>(),
        "current Copy api only supported MaskMergeMode MERGING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vmov((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg, mask, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        vmov(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            constexpr auto lowerDist =
                std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>();
            MaskReg dstMask;
            MaskReg tmpMask;
            MaskReg dumpMask;
            ppack(tmpMask, mask, lowerDist);
            pintlv_b32(dstMask, dumpMask, tmpMask, tmpMask);
            vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, dstMask, modeValue);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            vmov((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg.reg[0], mask, modeValue);
            vmov((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1], mask, modeValue);
        }
    }
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void CopyImpl(U& dstReg, U& srcReg)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, half, float,
            bfloat16_t>(),
        "current data type is not supported on current device!");
    if constexpr (IsSameType<ActualT, bool>::value) {
        vmov((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg);
    } else if constexpr (sizeof(ActualT) != 8) {
        vmov(dstReg, srcReg);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            vmov((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg.reg[0]);
            vmov((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1]);
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_COPY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__
#endif
