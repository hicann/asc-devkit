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
 * \file kernel_reg_compute_vec_cmpsel_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_fused_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_fused_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"

namespace AscendC {
namespace Reg {
template <typename T0, typename T1, typename T2, RegLayout layout, typename T3, typename T4>
__simd_callee__ inline void FusedMulsCastImpl(T3& dstReg, T4& srcReg, T2 scalarValue, MaskReg& mask)
{
    using ActualT = typename T3::ActualT;
    using ActualU = typename T4::ActualT;
    static_assert(Std::is_same_v<T0, DefaultType> || Std::is_same_v<T0, ActualT>, "T0 type is not correct!");
    static_assert(Std::is_same_v<T1, DefaultType> || Std::is_same_v<T1, ActualU>, "T1 type is not correct!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU, T2>, Tuple<half, float, float>>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<layout, RegLayout::ZERO, RegLayout::ONE>(),
        "current FusedMulsCast api only supported RegLayout ZERO, ONE on current device!");

    constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layout)>();
    vmulscvt(dstReg, srcReg, scalarValue, mask, partModeValue);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void FusedAbsSubImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, half, float, int64_t>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current FusedAbsSub api only supported Mode ZEROING on current device!");
    if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            Sub(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            Abs(traitTwoDstReg, traitTwoDstReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            Sub(dstReg, srcReg0, srcReg1, mask);
            Abs(dstReg, dstReg, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vabsdif(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T, typename U, RegLayout layout, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void FusedExpSubImpl(S& dstReg, V& srcReg0, V& srcReg1, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<half, half>, Tuple<float, float>>(),
        "current data type is not supported on current device!");
#else
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<float, float>, Tuple<float, half>>(),
        "current data type is not supported on current device!");
#endif
    static_assert(
        SupportEnum<layout, RegLayout::ZERO, RegLayout::ONE>(),
        "current FusedExpSub api only supported RegLayout ZERO, ONE on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current FusedExpSub api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layout)>();
    vexpdif(dstReg, srcReg0, srcReg1, mask, partModeValue);
}

template <typename T, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void FusedMulDstAddImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    static_assert(SupportEnum<mode, MaskMergeMode::ZEROING>(), "FusedMulDstAdd only support Mode ZEROING");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__
#endif
