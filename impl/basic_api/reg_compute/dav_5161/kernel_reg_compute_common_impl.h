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
 * \file kernel_reg_compute_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_utils.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"

namespace AscendC {
namespace Reg {
template <typename T, StoreDist dist>
__aicore__ inline constexpr StoreDist GetStoreDist()
{
    if constexpr (dist == StoreDist::DIST_NORM) {
        static_assert(SupportBytes<T, 1, 2, 4>(), "StoreDist DIST_NORM only support type b8/b16/b32 on current device");
        if constexpr (sizeof(T) == 1) {
            return StoreDist::DIST_NORM_B8;
        } else if constexpr (sizeof(T) == 2) {
            return StoreDist::DIST_NORM_B16;
        } else if constexpr (sizeof(T) == 4) {
            return StoreDist::DIST_NORM_B32;
        }
    }
    return dist;
}

template <typename T, const RegTrait& otherTrait = RegTraitNumOne>
constexpr __aicore__ inline bool CheckRegTrait()
{
    constexpr RegTrait regTrait = T::trait;
    return regTrait.REG_NUM == otherTrait.REG_NUM;
}

#ifndef __ASC_NPU_HOST__
template <RoundMode mode>
__aicore__ inline constexpr ::ROUND GetRound()
{
// To avoid naming conflicts of ROUND member variables in cpu debug,
// the names of the returned member variables are changed to be different from those of
// the ROUND enumeration class of the compiler.
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (mode == RoundMode::CAST_RINT) {
        return ::ROUND::CAST_RINT;
    } else if constexpr (mode == RoundMode::CAST_ROUND) {
        return ::ROUND::CAST_ROUND;
    } else if constexpr (mode == RoundMode::CAST_FLOOR) {
        return ::ROUND::CAST_FLOOR;
    } else if constexpr (mode == RoundMode::CAST_CEIL) {
        return ::ROUND::CAST_CEIL;
    } else if constexpr (mode == RoundMode::CAST_TRUNC) {
        return ::ROUND::CAST_TRUNC;
    } else if constexpr (mode == RoundMode::CAST_ODD) {
        return ::ROUND::CAST_ODD;
    } else {
        return ::ROUND::CAST_HYBRID;
    }
#else
    if constexpr (mode == RoundMode::CAST_RINT) {
        return ::ROUND::R;
    } else if constexpr (mode == RoundMode::CAST_ROUND) {
        return ::ROUND::A;
    } else if constexpr (mode == RoundMode::CAST_FLOOR) {
        return ::ROUND::F;
    } else if constexpr (mode == RoundMode::CAST_CEIL) {
        return ::ROUND::C;
    } else if constexpr (mode == RoundMode::CAST_TRUNC) {
        return ::ROUND::Z;
    } else if constexpr (mode == RoundMode::CAST_ODD) {
        return ::ROUND::O;
    } else {
        return ::ROUND::H;
    }
#endif
}
#endif

#ifndef __ASC_NPU_HOST__
template <MaskMergeMode mode>
__aicore__ inline constexpr auto GetMaskMergeMode()
{
// To avoid naming conflicts of mode struct in cpu debug.
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return std::integral_constant<::CpuMode, static_cast<::CpuMode>(mode)>();
#else
    return std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
#endif
}
#endif // #ifndef __ASC_NPU_HOST__

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::MERGING, typename RegT>
__simd_callee__ inline void CopyMerging(RegT& dstReg, RegT& srcReg, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vmov((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg, mask, modeValue);
    } else if constexpr (sizeof(ActualT) == 1) {
        vmov((RegTensor<uint8_t>&)dstReg, (RegTensor<uint8_t>&)srcReg, mask, modeValue);
    } else if constexpr (sizeof(ActualT) == 2) {
        vmov((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, mask, modeValue);
    } else {
        vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, mask, modeValue);
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__
#endif
