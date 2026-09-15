/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_gather_mask_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_gather_mask_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_gather_mask_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_GATHER_MASK_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_GATHER_MASK_INTERFACE_IMPL_H

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510 || defined(__ASC_NPU_HOST__)
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_gather_mask_impl.h"
#endif

namespace AscendC {
namespace Reg {
template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSpr()
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510 || defined(__ASC_NPU_HOST__)
    return GetSprImpl<spr>();
#else
    return 0;
#endif
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510 || defined(__ASC_NPU_HOST__)
template <typename T = DefaultType, GatherMaskMode store = GatherMaskMode::NO_STORE_REG, typename U>
__simd_callee__ inline void GatherMask(U& dstReg, U& srcReg, MaskReg& mask)
{
    GatherMaskImpl<T, store, U>(dstReg, srcReg, mask);
}
template <typename T, GatherMaskMode store, typename U>
__simd_callee__ inline void Squeeze(U& dstReg, U& srcReg, MaskReg& mask)
{
    GatherMaskImpl<T, store, U>(dstReg, srcReg, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void PrefixSum(U& dstReg, MaskReg& mask)
{
    PrefixSumImpl<T, U>(dstReg, mask);
}
template <typename T, typename U>
__simd_callee__ inline void Unsqueeze(U& dstReg, MaskReg& mask)
{
    PrefixSumImpl<T, U>(dstReg, mask);
}

template <SpecialPurposeReg spr>
__simd_callee__ inline void ClearSpr()
{
    ClearSprImpl<spr>();
}

template <typename T, typename U, typename S, typename V>
__simd_callee__ inline void Gather(S& dstReg, S& srcReg, V& indexReg)
{
    GatherImpl<T, U, S, V>(dstReg, srcReg, indexReg);
}
#endif
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_KERNEL_REG_COMPUTE_COPY_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_GATHER_MASK_INTF_IMPL__
#endif
