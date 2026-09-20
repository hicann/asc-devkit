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
 * \file kernel_reg_compute_vec_binary_scalar_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_vec_binary_scalar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/utils/std/type_traits.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"

namespace AscendC {
namespace Reg {

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void AddsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Adds api only supported Mode ZEROING on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vadds(dstReg, srcReg, scalarValue, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MulsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<U, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Muls api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmuls(dstReg, srcReg, scalarValue, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MaxsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Maxs api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmaxs(dstReg, srcReg, scalarValue, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MinsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Mins api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmins(dstReg, srcReg, scalarValue, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void LeakyReluImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(SupportType<U, half, float>(), "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current LeakyRelu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vlrelu(dstReg, srcReg, scalarValue, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__
#endif
