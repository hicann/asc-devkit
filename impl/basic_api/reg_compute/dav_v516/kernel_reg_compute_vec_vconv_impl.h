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
 * \file kernel_reg_compute_vec_vconv_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_vconv_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
constexpr int16_t SHIFT_ONE_BIT = 1;
constexpr int16_t SHIFT_EIGHT_BIT = 8;
constexpr int16_t SHIFT_SEVEN_BIT = 7;
constexpr uint16_t BF16_INF = 0x7f80;
constexpr uint16_t BF16_NAN = 0x7fc0;

template <typename T, typename U, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<int64_t, int32_t>, Tuple<float, half>,
        Tuple<float, bfloat16_t>, Tuple<half, hifloat8_t>, Tuple<half, uint8_t>, Tuple<half, int8_t>,
        Tuple<float, int16_t>, Tuple<bfloat16_t, fp8_e8m0_t>>();
    constexpr bool ppCondition = SupportType<
        Tuple<T, U>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>, Tuple<float, hifloat8_t>,
        Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<half, int4x2_t>, Tuple<bfloat16_t, int4x2_t>,
        Tuple<int16_t, int4x2_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (partCondition) {
        // vcvt_ii u82u16/s82s16/u162u32/s162u32/s162s32/s322s64
        // vcvt_ff f162f32/bf162f32/hif82f16/f8e8m02bf16
        // vcvt_if u82f16/s82f16/s162f32
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<bfloat16_t, fp8_e8m0_t>>()) {
            // f8e8m02bf16
            constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            vcvt((RegTensor<uint16_t>&)srcReg, (RegTensor<uint8_t>&)srcReg, mask, partModeValue, modeValue);
            vshls((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, SHIFT_SEVEN_BIT, mask, modeValue);
            // when f8e8m0 is nan, output bf16 to nan
            MaskReg nanMask;
            constexpr auto mgModeValue = GetMaskMergeMode<MaskMergeMode::MERGING>();
            vcmps_eq(nanMask, (RegTensor<uint16_t>&)dstReg, BF16_INF, mask);
            vdup((RegTensor<uint16_t>&)dstReg, BF16_NAN, nanMask, mgModeValue);
        } else {
            vcvt(dstReg, srcReg, mask, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<half, int4x2_t>>()) {
            // vcvt_if s42f16
            vcvt_s42f16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<bfloat16_t, int4x2_t>>()) {
            // vcvt_if s42bf16
            vcvt_s42bf16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<int16_t, int4x2_t>>()) {
            // vcvt_if s42s16
            vcvt_s42s16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else {
            // vcvt_ii u82u32/s82s32/
            // vcvt_ff hif82f32/e4m32f32/e5m22f32/fp4e2m12bf16/fp4e1m22bf16
            vcvt(dstReg, srcReg, mask, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<int32_t, int64_t>>();
    constexpr bool ppCondition =
        SupportType<Tuple<T, U>, Tuple<uint8_t, uint32_t>, Tuple<uint8_t, int32_t>, Tuple<int4x2_t, int16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    if constexpr (partCondition) {
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        // vcvt_ii u162u8/s162u8/u322u16/u322s16/s322u16/s322s16/s642s32
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, satModeValue, partModeValue, modeValue);
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<int4x2_t, int16_t>>()) {
            // vcvt_ii s162s4x2
            vcvt_s162s4(dstReg, srcReg, mask, satModeValue, ppModeValue, modeValue);
        } else {
            // vcvt_ii u322u8/s322u8
            vcvt(dstReg, srcReg, mask, satModeValue, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, satMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, satMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>, Tuple<int32_t, bfloat16_t>,
        Tuple<int64_t, float>, Tuple<half, float>, Tuple<bfloat16_t, float>, Tuple<hifloat8_t, half>,
        Tuple<fp8_e8m0_t, bfloat16_t>>();
    constexpr bool conditionNoneToRint =
        SupportType<Tuple<T, U>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<uint8_t, half>>();
    constexpr bool conditionNoNone = SupportType<Tuple<T, U>, Tuple<bfloat16_t, float>>();
    constexpr bool ppCondition = SupportType<
        Tuple<T, U>, Tuple<hifloat8_t, float>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>,
        Tuple<int4x2_t, half>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f322s16/f162u8/f162s8/bf162s32/f322s64
        // vcvt_ff f322f16/f322bf16/f162hif8/bf162f8e8m0/
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        if constexpr (conditionNoneToRint && (roundMode == RoundMode::CAST_NONE)) {
            constexpr auto roundModeNoneToRint = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeNoneToRint, satModeValue, partModeValue, modeValue);
        } else if constexpr (conditionNoNone) {
            static_assert(
                !(SupportEnum<roundMode, RoundMode::CAST_NONE>()),
                "roundMode can't be CAST_NONE when float to bfloat16_t!");
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, partModeValue, modeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<fp8_e8m0_t, bfloat16_t>>()) {
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            // bf162f8e8m0 will ignore sat mode and use sat only.
            constexpr auto satModeOnly =
                std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::SAT)>();
            vshls((RegTensor<uint16_t>&)srcReg, (RegTensor<uint16_t>&)srcReg, SHIFT_ONE_BIT, mask, modeValue);
            vshrs((RegTensor<uint16_t>&)srcReg, (RegTensor<uint16_t>&)srcReg, SHIFT_EIGHT_BIT, mask, modeValue);
            vcvt(
                (RegTensor<uint8_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, mask, satModeOnly, partModeValue, modeValue);
        } else {
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        // vcvt_fi f162s4x2
        if constexpr (SupportType<Tuple<T, U>, Tuple<int4x2_t, half>>()) {
            if (roundMode == RoundMode::CAST_NONE) {
                constexpr auto roundModeNoneToRint =
                    std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
                vcvt_f162s4(dstReg, srcReg, mask, roundModeNoneToRint, satModeValue, ppModeValue, modeValue);
            } else {
                vcvt_f162s4(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue, modeValue);
            }
        } else {
            // vcvt_ff f322hif8/f322e5m2/f322e4m3
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, satMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<Tuple<T, U>, Tuple<int32_t, float>, Tuple<int16_t, half>>();
    constexpr bool ppCondition = SupportType<Tuple<T, U>, Tuple<half, bfloat16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f322s32/f162s16
        vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, modeValue);
    } else if constexpr (ppCondition) {
        // bf162f16
        vcvt(dstReg, srcReg, mask, satModeValue, roundModeValue, modeValue);
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, satMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<Tuple<T, U>, Tuple<int32_t, half>, Tuple<float, int64_t>>();
    constexpr bool ppCondition =
        SupportType<Tuple<T, U>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<fp4x2_e1m2_t, bfloat16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f162s32
        // vcvt_if s642f32
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, roundModeValue, partModeValue, modeValue);
    } else if constexpr (ppCondition) {
        // vcvt_ff bf162fp4e2m1/bf162fp4e1m2
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        vcvt(dstReg, srcReg, mask, roundModeValue, ppModeValue, modeValue);
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// vcvt_if s162f16/s322f32
// vcvt_ff f162bf16
template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool conditionNoneToRint = SupportType<Tuple<T, U>, Tuple<half, int16_t>, Tuple<float, int32_t>>();
    if constexpr (conditionNoneToRint && (roundMode == RoundMode::CAST_NONE)) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
        vcvt(dstReg, srcReg, mask, roundModeValue, modeValue);
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
        vcvt(dstReg, srcReg, mask, roundModeValue, modeValue);
    }
}

template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<T, U>, Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<bfloat16_t, half>>(),
        "current cast data type is not supported on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// truncate f162f16/f322f32/bf162bf16
template <typename T, RoundMode roundMode, MaskMergeMode mode, typename U>
__simd_callee__ inline void TruncateImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current trunc api only supported Mode ZEROING on current device!");
    static_assert(
        SupportType<ActualT, half, float, bfloat16_t>(), "current trunc data type is not supported on current device!");
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

// s322s64 RegTraitNumOne -> RegTraitNumTwo
template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    RegTensor<int32_t> tmpReg0;
    RegTensor<int32_t> tmpReg1;
    RegTensor<int32_t> zeroReg;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    MaskReg lowMask, highMask;
    Duplicate(zeroReg, 0, maskFull);
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, srcReg, zeroReg);
    CastImpl<ActualT, ActualU, RegLayout::ZERO, mode>((RegTensor<int64_t>&)tmpReg0, tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, RegLayout::ZERO, mode>((RegTensor<int64_t>&)tmpReg1, tmpReg1, highMask);
    DeInterleave((RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)dstReg.reg[1], tmpReg0, tmpReg1);
}

template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int64_t, int32_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<V, RegTraitNumOne>(), "RegTensor srcReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// s64s32 RegTraitNumTwo -> RegTraitNumOne
template <typename T, typename U, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    RegTensor<int32_t> tmpReg0;
    RegTensor<int32_t> tmpReg1;
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, (RegTensor<int32_t>&)srcReg.reg[0], (RegTensor<int32_t>&)srcReg.reg[1]);
    CastImpl<ActualT, ActualU, satMode, RegLayout::ZERO, mode>(tmpReg0, (RegTensor<int64_t>&)tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, satMode, RegLayout::ZERO, mode>(tmpReg1, (RegTensor<int64_t>&)tmpReg1, highMask);
    DeInterleave(dstReg, tmpReg0, tmpReg0, tmpReg1);
}

template <typename T, typename U, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, int64_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "RegTensor dstReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, satMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, satMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// f322s64 RegTraitNumOne -> RegTraitNumTwo
template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    RegTensor<float> tmpReg0;
    RegTensor<float> tmpReg1;
    RegTensor<int32_t> zeroReg;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    Duplicate(zeroReg, 0, maskFull);
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, srcReg, (RegTensor<float>&)zeroReg);
    CastImpl<ActualT, ActualU, roundMode, satMode, RegLayout::ZERO, mode>(
        (RegTensor<int64_t>&)tmpReg0, tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, roundMode, satMode, RegLayout::ZERO, mode>(
        (RegTensor<int64_t>&)tmpReg1, tmpReg1, highMask);
    DeInterleave((RegTensor<float>&)dstReg.reg[0], (RegTensor<float>&)dstReg.reg[1], tmpReg0, tmpReg1);
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int64_t, float>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<V, RegTraitNumOne>(), "RegTensor srcReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, roundMode, satMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// s642f32 RegTraitNumTwo -> RegTraitNumOne
template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    const auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(RegLayout::ZERO)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    RegTensor<float> tmpReg0;
    RegTensor<float> tmpReg1;
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, (RegTensor<float>&)srcReg.reg[0], (RegTensor<float>&)srcReg.reg[1]);
    CastImpl<ActualT, ActualU, roundMode, RegLayout::ZERO, mode>(tmpReg0, (RegTensor<int64_t>&)tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, roundMode, RegLayout::ZERO, mode>(tmpReg1, (RegTensor<int64_t>&)tmpReg1, highMask);
    DeInterleave(dstReg, tmpReg0, tmpReg0, tmpReg1);
}

template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<float, int64_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "RegTensor dstReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, roundMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, const CastTrait& trait, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    if constexpr (trait.mrgMode == MaskMergeMode::MERGING) {
        static_assert(
            SupportEnum<trait.layoutMode, RegLayout::UNKNOWN, RegLayout::ZERO>(),
            "current Cast api only supported MERGING Mode for RegLayout::UNKNOWN/ZERO on current device!");
        static_assert(
            !(SupportType<ActualT, fp4x2_e2m1_t, fp4x2_e1m2_t, int4x2_t>() ||
              SupportType<ActualU, fp4x2_e2m1_t, fp4x2_e1m2_t, int4x2_t>()),
            "current Cast api do not support MERGING Mode for fp4x2_e2m1_t/fp4x2_e1m2_t/int4x2_t on current device!");
    }
    constexpr bool layoutMerge = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<int64_t, int32_t>, Tuple<float, half>,
        Tuple<float, bfloat16_t>, Tuple<half, hifloat8_t>, Tuple<half, uint8_t>, Tuple<half, int8_t>,
        Tuple<float, int16_t>, Tuple<bfloat16_t, fp8_e8m0_t>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>,
        Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<half, int4x2_t>, Tuple<bfloat16_t, int4x2_t>,
        Tuple<int16_t, int4x2_t>>();
    constexpr bool satLayMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, uint32_t>,
        Tuple<uint8_t, int32_t>, Tuple<int32_t, int64_t>, Tuple<int4x2_t, int16_t>>();
    constexpr bool rndSatLayoutMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>,
        Tuple<int32_t, bfloat16_t>, Tuple<int64_t, float>, Tuple<half, float>, Tuple<bfloat16_t, float>,
        Tuple<hifloat8_t, half>, Tuple<hifloat8_t, float>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>,
        Tuple<int4x2_t, half>, Tuple<fp8_e8m0_t, bfloat16_t>>();
    constexpr bool rndSatMergeCast =
        SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, float>, Tuple<int16_t, half>, Tuple<half, bfloat16_t>>();
    constexpr bool rndLayoutMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<int32_t, half>, Tuple<float, int64_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>,
        Tuple<fp4x2_e1m2_t, bfloat16_t>>();
    constexpr bool rndMergeCast =
        SupportType<Tuple<ActualT, ActualU>, Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<bfloat16_t, half>>();
    constexpr bool allNotSupport =
        !(layoutMerge && satLayMergeCast && rndSatLayoutMergeCast && rndSatMergeCast && rndLayoutMergeCast &&
          rndMergeCast);
    if constexpr (layoutMerge) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (satLayMergeCast) {
        if constexpr (CheckRegTrait<V, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.satMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.satMode, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatLayoutMergeCast) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.satMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.layoutMode, trait.mrgMode>(
                dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatMergeCast) {
        CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.mrgMode>(dstReg, srcReg, mask);
    } else if constexpr (rndLayoutMergeCast) {
        if constexpr (CheckRegTrait<V, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
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
