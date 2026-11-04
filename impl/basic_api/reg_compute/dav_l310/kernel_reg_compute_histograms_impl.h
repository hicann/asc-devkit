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
 * \file kernel_reg_compute_histograms_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_histograms_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_histograms_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_HISTOGRAMS_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_HISTOGRAMS_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_HISTOGRAMS_IMPL_H

#include "kernel_reg_compute_common_impl.h"
namespace AscendC {
namespace Reg {
template <
    typename T = DefaultType, typename U = DefaultType, HistogramsBinType mode, HistogramsType type, typename RegT,
    typename RegU>
__simd_callee__ inline void HistogramsImpl(RegU& dstReg, RegT& srcReg, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    using ActualU = typename RegU::ActualT;
    static_assert((SupportType<ActualT, uint8_t>()), "current data type is not supported on current device!");
    static_assert((SupportType<ActualU, uint16_t>()), "current data type is not supported on current device!");
    auto constexpr HistogramsMode = std::integral_constant<::Bin, static_cast<::Bin>(mode)>();
    if constexpr (type == HistogramsType::FREQUENCY) {
        dhistv2(dstReg, srcReg, mask, HistogramsMode);
    } else if constexpr (type == HistogramsType::ACCUMULATE) {
        chistv2(dstReg, srcReg, mask, HistogramsMode);
    }
}
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_MODULE_REG_COMPUTE_HISTOGRAMS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_HISTOGRAMS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_HISTOGRAMS_IMPL__
#endif
