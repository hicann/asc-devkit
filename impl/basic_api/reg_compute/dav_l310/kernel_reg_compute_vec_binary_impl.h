/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_reg_compute_vec_binary_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../basic_api/dav_l310/kernel_operator_common_impl.h"

namespace AscendC {
namespace Reg {
namespace Internal {
__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .precisionMode = false, .algo = DivAlgo::INTRINSIC};
}

__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(const DivSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .precisionMode = sprMode->precisionMode, .algo = sprMode->algo};
}
} // namespace Internal
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void AddImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void SubImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsub(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void MulImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmul(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void DivImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const DivSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const DivSpecificMode* ");
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(mode);
    static_assert(!sprMode.precisionMode, "precision mode for Reg Div is not supported on the current device!");
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void MaxImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmax(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void MinImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmin(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <
    typename T = DefaultType, typename SHIFT_T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING,
    typename RegT, typename RegShiftT>
__simd_callee__ inline void ShiftLeftImpl(RegT& dstReg, RegT& srcReg0, RegShiftT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    using ActualShiftT = typename RegShiftT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        Std::is_same_v<SHIFT_T, DefaultType> || Std::is_same_v<SHIFT_T, ActualShiftT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<ActualShiftT, int8_t, int16_t, int32_t>(),
        "current src1 data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshl(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <
    typename T = DefaultType, typename SHIFT_T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING,
    typename RegT, typename RegShiftT>
__simd_callee__ inline void ShiftRightImpl(RegT& dstReg, RegT& srcReg0, RegShiftT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    using ActualShiftT = typename RegShiftT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        Std::is_same_v<SHIFT_T, DefaultType> || Std::is_same_v<SHIFT_T, ActualShiftT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<ActualShiftT, int8_t, int16_t, int32_t>(),
        "current src1 data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vshr(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void AndImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vand(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void OrImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vor(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void XorImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vxor(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void PreluImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vprelu(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, typename RegT>
__simd_callee__ inline void MullImpl(RegT& dstReg0, RegT& dstReg1, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Mull api is not supported on current device!"); });
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void MulAddDstImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current MulAddDst api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmula(dstReg, srcReg0, srcReg1, mask, modeValue);
}

} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif
