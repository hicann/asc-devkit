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
 * \file kernel_reg_compute_vec_cmpsel_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l300/kernel_reg_compute_vec_cmpsel_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_cmpsel_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H

#include "kernel_reg_compute_common_impl.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename RegT>
__simd_callee__ inline void CompareImpl(MaskReg& dstMask, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    if constexpr (mode == CMPMODE::EQ) {
        vcmp_eq(dstMask, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::NE) {
        vcmp_ne(dstMask, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::GT) {
        vcmp_gt(dstMask, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::GE) {
        vcmp_ge(dstMask, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::LT) {
        vcmp_lt(dstMask, srcReg0, srcReg1, mask);
    } else if constexpr (mode == CMPMODE::LE) {
        vcmp_le(dstMask, srcReg0, srcReg1, mask);
    }
}

template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename RegT, typename ScalarT>
__simd_callee__ inline void CompareScalarImpl(MaskReg& dstMask, RegT& srcReg, ScalarT scalarValue, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    if constexpr (sizeof(ActualT) < 8) {
        if constexpr (mode == CMPMODE::EQ) {
            vcmps_eq(dstMask, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::NE) {
            vcmps_ne(dstMask, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::GT) {
            vcmps_gt(dstMask, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::GE) {
            vcmps_ge(dstMask, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::LT) {
            vcmps_lt(dstMask, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::LE) {
            vcmps_le(dstMask, srcReg, scalarValue, mask);
        }
    } else if constexpr (sizeof(ActualT) == 8) {
        RegT dupReg;
        Duplicate(dupReg, scalarValue, mask);
        Compare<T, mode, RegT>(dstMask, srcReg, dupReg, mask);
    }
}

template <typename T = DefaultType, typename RegT>
__simd_callee__ inline void SelectImpl(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    vsel(dstReg, srcReg0, srcReg1, mask);
}

} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__
#endif
