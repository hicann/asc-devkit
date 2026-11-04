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
 * \file kernel_reg_compute_vec_unary_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_unary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_SINGLE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_SINGLE_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/utils/std/type_traits.h"

namespace AscendC {
namespace Reg {
namespace Internal {
__aicore__ inline constexpr SqrtSpecificMode GetSqrtSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .precisionMode = false, .algo = SqrtAlgo::INTRINSIC};
}

__aicore__ inline constexpr SqrtSpecificMode GetSqrtSpecificMode(const SqrtSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .precisionMode = sprMode->precisionMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr ExpSpecificMode GetExpSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = ExpAlgo::INTRINSIC};
}

__aicore__ inline constexpr ExpSpecificMode GetExpSpecificMode(const ExpSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr LnSpecificMode GetLnSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = LnAlgo::INTRINSIC};
}

__aicore__ inline constexpr LnSpecificMode GetLnSpecificMode(const LnSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr LogSpecificMode GetLogSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = LogAlgo::INTRINSIC};
}

__aicore__ inline constexpr LogSpecificMode GetLogSpecificMode(const LogSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr Log2SpecificMode GetLog2SpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = Log2Algo::INTRINSIC};
}

__aicore__ inline constexpr Log2SpecificMode GetLog2SpecificMode(const Log2SpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr Log10SpecificMode GetLog10SpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = Log10Algo::INTRINSIC};
}

__aicore__ inline constexpr Log10SpecificMode GetLog10SpecificMode(const Log10SpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}
} // namespace Internal
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AbsImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vabs(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReluImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int32_t, half, float>(), "current data type is not supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrelu(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ExpPrecision(U& dstReg, U& srcReg, MaskReg& maskSubnormal)
{
    U regTwo;
    U tmpReg0;
    using ActualT = typename U::ActualT;
    constexpr ExpSpecificMode sprMode = Internal::GetExpSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vdup(regTwo, 2, maskSubnormal, modeValue);
    vdiv(tmpReg0, srcReg, regTwo, maskSubnormal, modeValue);
    vexp(tmpReg0, tmpReg0, maskSubnormal, modeValue);
    vmul(dstReg, tmpReg0, tmpReg0, maskSubnormal, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ExpImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const ExpSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const ExpSpecificMode* ");
    constexpr ExpSpecificMode sprMode = Internal::GetExpSpecificMode(mode);
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Exp api only supported Mode ZEROING on current device!");
    static_assert(sprMode.algo == ExpAlgo::INTRINSIC, "only ExpAlgo::INTRINSIC is supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vexp(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SqrtImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const SqrtSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const SqrtSpecificMode* ");
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    constexpr SqrtSpecificMode sprMode = Internal::GetSqrtSpecificMode(mode);
    static_assert(sprMode.algo == SqrtAlgo::INTRINSIC, "only SqrtAlgo::INTRINSIC is supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vsqrt(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, typename U, const LogSpecificMode* mode>
__simd_callee__ inline void LnCompute(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr LogSpecificMode sprMode = Internal::GetLogSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    static_assert(sprMode.algo == LogAlgo::INTRINSIC, "only LogAlgo::INTRINSIC is supported on current device!");
    vln(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void LogImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const LogSpecificMode*>::value ||
            IsSameType<decltype(mode), const LnSpecificMode*>::value,
        "mode type must be MaskMergeMode or const LogSpecificMode* or const LnSpecificMode* ");
    if constexpr (IsSameType<decltype(mode), const LogSpecificMode*>::value) {
        constexpr LogSpecificMode sprMode = Internal::GetLogSpecificMode(mode);
        static_assert(
            SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
            "current Log api only supports Mode ZEROING on current device!");
        LnCompute<T, U, mode>(dstReg, srcReg, mask);
    } else if constexpr (IsSameType<decltype(mode), const LnSpecificMode*>::value) {
        constexpr LnSpecificMode sprMode = Internal::GetLnSpecificMode(mode);
        static_assert(
            SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
            "current Ln api only supports Mode ZEROING on current device!");
        if constexpr (sprMode.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE) {
            static constexpr AscendC::Reg::LogSpecificMode logMode = {
                MaskMergeMode::ZEROING, LogAlgo::PRECISION_1ULP_FTZ_FALSE};
            LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
        } else {
            static constexpr AscendC::Reg::LogSpecificMode logMode = {MaskMergeMode::ZEROING, LogAlgo::INTRINSIC};
            LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vln(dstReg, srcReg, mask, modeValue);
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING>
__simd_callee__ inline void LogXImpl(
    RegTensor<half>& dstReg, RegTensor<half>& srcReg, MaskReg& mask, const float lnXReciprocal)
{
    vector_f16 f16RegLow;
    vector_f16 f16RegHigh;
    vector_f32 f32RegLow;
    vector_f32 f32RegHigh;
    vector_bool MaskLow;
    vector_bool MaskHigh;
    vector_f16 tmpReg;
    constexpr auto patAll = std::integral_constant<::Pat, static_cast<::Pat>(Reg::MaskPattern::ALL)>();
    vector_bool maskAll = pset_b32(patAll);
    constexpr auto patVal = std::integral_constant<::Pat, static_cast<::Pat>(Reg::MaskPattern::H)>();
    vector_bool selMask = pset_b16(patVal);
    constexpr auto partModeEvenVal = std::integral_constant<::Part, static_cast<::Part>(RegLayout::ZERO)>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::NO_SAT)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    punpack(MaskLow, mask, LOWER);
    punpack(MaskHigh, mask, HIGHER);

    vintlv(f16RegLow, f16RegHigh, srcReg, srcReg);

    vcvt(f32RegLow, f16RegLow, maskAll, partModeEvenVal, modeValue);
    vcvt(f32RegHigh, f16RegHigh, maskAll, partModeEvenVal, modeValue);

    vln(f32RegLow, f32RegLow, maskAll, modeValue);
    vln(f32RegHigh, f32RegHigh, maskAll, modeValue);

    vmuls(f32RegLow, f32RegLow, lnXReciprocal, maskAll, modeValue);
    vmuls(f32RegHigh, f32RegHigh, lnXReciprocal, maskAll, modeValue);

    vcvt(f16RegLow, f32RegLow, MaskLow, roundModeValue, satModeValue, partModeEvenVal, modeValue);
    vcvt(f16RegHigh, f32RegHigh, MaskHigh, roundModeValue, satModeValue, partModeEvenVal, modeValue);

    vdintlv(f16RegLow, tmpReg, f16RegLow, tmpReg);
    vdintlv(f16RegHigh, tmpReg, tmpReg, f16RegHigh);
    vsel(dstReg, f16RegLow, f16RegHigh, selMask);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log2Impl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const Log2SpecificMode*>::value,
        "mode type must be either MaskMergeMode or const Log2SpecificMode* ");
    constexpr Log2SpecificMode sprMode = Internal::GetLog2SpecificMode(mode);
    static_assert(sprMode.algo == Log2Algo::INTRINSIC, "only Log2Algo::INTRINSIC is supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr float ln2Reciprocal = 1.4426950408889634; // 1.0/Ln2;
    if constexpr (SupportType<ActualT, half>()) {
        LogXImpl<mode>(dstReg, srcReg, mask, ln2Reciprocal);
    } else {
        vln(dstReg, srcReg, mask, modeValue);
        vmuls(dstReg, dstReg, ln2Reciprocal, mask, modeValue);
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log10Impl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const Log10SpecificMode*>::value,
        "mode type must be either MaskMergeMode or const Log10SpecificMode* ");
    constexpr Log10SpecificMode sprMode = Internal::GetLog10SpecificMode(mode);
    static_assert(sprMode.algo == Log10Algo::INTRINSIC, "only Log10Algo::INTRINSIC is supported on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr float ln10Reciprocal = 0.43429448190325176; // 1.0/Ln10;
    if constexpr (SupportType<ActualT, half>()) {
        LogXImpl<mode>(dstReg, srcReg, mask, ln10Reciprocal);
    } else {
        vln(dstReg, srcReg, mask, modeValue);
        vmuls(dstReg, dstReg, ln10Reciprocal, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void NegImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vneg(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void NotImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vnot(dstReg, srcReg, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_SINGLE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__
#endif
