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
 * \file kernel_reg_compute_vec_binary_scalar_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_vec_binary_scalar_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTERFACE_IMPL_H

#if __NPU_ARCH__ == 3003
#include "../../basic_api/reg_compute/dav_l300/kernel_reg_compute_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3113
#include "../../basic_api/reg_compute/dav_l311/kernel_reg_compute_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 5102
#include "../../basic_api/reg_compute/dav_m510/kernel_reg_compute_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 5162
#include "../../basic_api/reg_compute/dav_v516/kernel_reg_compute_vec_binary_scalar_impl.h"
#else
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_vec_binary_scalar_impl.h"
#endif

namespace AscendC {
namespace Reg {
template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void Adds(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    AddsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void Muls(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    MulsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void Maxs(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    MaxsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void Mins(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    MinsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void ShiftLefts(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    ShiftLeftsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void ShiftRights(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    ShiftRightsImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S>
__simd_callee__ inline void LeakyRelu(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    LeakyReluImpl<T, U, mode, S>(dstReg, srcReg, scalarValue, mask);
}
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_INTF_IMPL__
#endif
