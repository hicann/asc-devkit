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
 * \file kernel_reg_compute_copy_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_copy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_COPY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_COPY_IMPL_H

#include "kernel_reg_compute_common_impl.h"
namespace AscendC {
namespace Reg {
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void CopyImpl(RegT& dstReg, RegT& srcReg, MaskReg mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::MERGING>(), "current Move api only supported MaskMergeMode MERGING!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vmov(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, typename RegT>
__simd_callee__ inline void CopyImpl(RegT& dstReg, RegT& srcReg)
{
    using ActualT = typename RegT::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    vmov(dstReg, srcReg);
}
} // namespace Reg
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COPY_IMPL__
#endif
