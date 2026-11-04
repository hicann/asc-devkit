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
 * \file kernel_reg_compute_vec_cmpsel_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_vec_fused_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_fused_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H

#include "kernel_reg_compute_common_impl.h"

namespace AscendC {
namespace Reg {

template <typename T, typename U, typename ScalarT, RegLayout layout, typename RegT, typename RegU>
__simd_callee__ inline void FusedMulsCastImpl(RegT& dstReg, RegU& srcReg, ScalarT scalarValue, MaskReg& mask)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "MulsCast is not supported on current device!"); });
}

template <typename T, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void FusedMulDstAddImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmadd(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, MaskMergeMode mode, typename RegT>
__simd_callee__ inline void FusedAbsSubImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");

    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vabsdif(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T, typename U, RegLayout layout, MaskMergeMode mode, typename RegT, typename RegU>
__simd_callee__ inline void FusedExpSubImpl(RegT& dstReg, RegU& srcReg0, RegU& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vsub(srcReg0, srcReg0, srcReg1, mask, modeValue);
    vexp(dstReg, srcReg0, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_FUSED_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_FUSED_IMPL__
#endif
