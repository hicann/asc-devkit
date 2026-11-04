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
 * \file kernel_reg_compute_vec_vconv_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_vconv_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
template <typename T, typename U, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<float, half>, Tuple<half, uint8_t>,
        Tuple<half, int8_t>, Tuple<float, int16_t>>();
    constexpr bool ppCondition =
        SupportType<Tuple<T, U>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>, Tuple<half, int4x2_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (partCondition) {
        // vcvt_ii u82u16/s82s16/u162u32/s162u32/s162s32
        // vcvt_ff f162f32
        // vcvt_if u82f16/s82f16/s162f32
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, partModeValue, modeValue);
    } else if constexpr (ppCondition) {
        // vcvt_ii u82u32/s82s32/s42f16
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<half, int4x2_t>>()) {
            vcvt_s42f16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else {
            vcvt(dstReg, srcReg, mask, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>>();
    constexpr bool ppCondition = SupportType<Tuple<T, U>, Tuple<uint8_t, uint32_t>, Tuple<uint8_t, int32_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    if constexpr (partCondition) {
        // vcvt_ii u162u8/s162u8/u322u16/u322s16/s322u16/s322s16
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, satModeValue, partModeValue, modeValue);
    } else if constexpr (ppCondition) {
        // vcvt_ii u322u8/s322u8
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, satModeValue, ppModeValue, modeValue);
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported!");
    }
}

// vcvt_fi f322s16/f162u8/f162s8
// vcvt_ff f322f16
template <typename T, typename U, RoundMode roundMode, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>, Tuple<half, float>,
            Tuple<int4x2_t, half>>(),
        "current cast data type is not supported!");
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "current cast api RegLayout Mode is not supported!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (SupportType<Tuple<T, U>, Tuple<int4x2_t, half>>()) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        vcvt_f162s4(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue, modeValue);
    } else {
        vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, partModeValue, modeValue);
    }
}

// vcvt_fi f322s32/f162s16
template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<int32_t, float>, Tuple<int16_t, half>>(),
        "current cast data type is not supported!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, modeValue);
}

// vcvt_fi f162s32
template <typename T, typename U, RoundMode roundMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(SupportType<Tuple<T, U>, Tuple<int32_t, half>>(), "current cast data type is not supported!");
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "current cast api RegLayout Mode is not supported!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    vcvt(dstReg, srcReg, mask, roundModeValue, partModeValue, modeValue);
}

// vcvt_if s162f16/s322f32
template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<half, int16_t>, Tuple<float, int32_t>>(),
        "current cast data type is not supported!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    vcvt(dstReg, srcReg, mask, roundModeValue, modeValue);
}

// truncate f162f16/f322f32
template <typename T, RoundMode roundMode, MaskMergeMode mode, typename U>
__simd_callee__ inline void TruncateImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(SupportType<ActualT, half, float>(), "current trunc data type is not supported!");
    if constexpr (roundMode == RoundMode::CAST_NONE) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
        vtrc(dstReg, srcReg, roundModeValue, mask, modeValue);
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
        vtrc(dstReg, srcReg, roundModeValue, mask, modeValue);
    }
}

template <typename T, typename U, const CastTrait& trait, typename RegT, typename RegU>
__simd_callee__ inline void CastImpl(RegT& dstReg, RegU& srcReg, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    using ActualU = typename RegU::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    constexpr bool layoutMerge = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<float, half>, Tuple<half, uint8_t>,
        Tuple<half, int8_t>, Tuple<float, int16_t>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>,
        Tuple<half, int4x2_t>>();

    constexpr bool satLayMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, uint32_t>,
        Tuple<uint8_t, int32_t>>();
    constexpr bool rndSatLayoutMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>, Tuple<half, float>,
        Tuple<int4x2_t, half>>();
    constexpr bool rndSatMergeCast =
        SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, float>, Tuple<int16_t, half>>();
    constexpr bool rndLayoutMergeCast = SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, half>>();
    constexpr bool rndMergeCast = SupportType<Tuple<ActualT, ActualU>, Tuple<half, int16_t>, Tuple<float, int32_t>>();
    constexpr bool allNotSupport =
        !(layoutMerge && satLayMergeCast && rndSatLayoutMergeCast && rndSatMergeCast && rndLayoutMergeCast &&
          rndMergeCast);
    if constexpr (layoutMerge) {
        if constexpr (CheckRegTrait<RegT, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.mrgMode, RegT, RegU>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (satLayMergeCast) {
        if constexpr (CheckRegTrait<RegU, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.satMode, trait.mrgMode, RegT, RegU>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.satMode, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatLayoutMergeCast) {
        if constexpr (CheckRegTrait<RegT, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.satMode, trait.mrgMode, RegT, RegU>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.layoutMode, trait.mrgMode>(
                dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatMergeCast) {
        CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.mrgMode>(dstReg, srcReg, mask);
    } else if constexpr (rndLayoutMergeCast) {
        if constexpr (CheckRegTrait<RegU, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.mrgMode, RegT, RegU>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.roundMode, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (rndMergeCast) {
        CastImpl<ActualT, ActualU, trait.roundMode, trait.mrgMode>(dstReg, srcReg, mask);
    } else {
        static_assert(allNotSupport, "current cast data type is not supported on current device!");
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__
#endif
