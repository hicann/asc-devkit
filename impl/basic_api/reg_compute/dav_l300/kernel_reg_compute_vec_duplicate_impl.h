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
 * \file kernel_reg_compute_vec_duplicate_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l300/kernel_reg_compute_vec_duplicate_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_duplicate_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../basic_api/dav_l300/kernel_operator_common_impl.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, typename T1, typename RegT>
__simd_callee__ inline void DuplicateImpl(RegT& dstReg, T1 scalarValue)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, bool, int8_t, uint8_t, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "unsupported datatype on current device!");
    vbr(dstReg, (ActualT)scalarValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename T1, typename RegT>
__simd_callee__ inline void DuplicateImpl(RegT& dstReg, T1 scalarValue, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, bool, int8_t, uint8_t, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "unsupported datatype on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdup(dstReg, (ActualT)scalarValue, mask, modeValue);
}

template <
    typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING,
    typename RegT>
__simd_callee__ inline void DuplicateImpl(RegT& dstReg, RegT& srcReg, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, bool, int8_t, uint8_t, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "unsupported datatype on current device!");

    constexpr auto posValue = std::integral_constant<::Pos, static_cast<::Pos>(pos)>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();

    vdup(dstReg, srcReg, mask, posValue, modeValue);
}

template <typename T = DefaultType, typename RegT>
__simd_callee__ inline void InterleaveImpl(RegT& dstReg0, RegT& dstReg1, RegT& srcReg0, RegT& srcReg1)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "Interleave only support type b8/b16/b32 on current device!");
    vintlv(dstReg0, dstReg1, srcReg0, srcReg1);
}

template <typename T = DefaultType, typename RegT>
__simd_callee__ inline void DeInterleaveImpl(RegT& dstReg0, RegT& dstReg1, RegT& srcReg0, RegT& srcReg1)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "DeInterleave only support type b8/b16/b32 on current device!");
    vdintlv(dstReg0, dstReg1, srcReg0, srcReg1);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__
#endif
