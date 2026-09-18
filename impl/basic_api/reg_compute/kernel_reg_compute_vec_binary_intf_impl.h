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
 * \file kernel_reg_compute_vec_binary_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_vec_binary_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_INTERFACE_IMPL_H

#include "../../../include/basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"

#if __NPU_ARCH__ == 3003
#include "../../basic_api/reg_compute/dav_l300/kernel_reg_compute_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3113
#include "../../basic_api/reg_compute/dav_l311/kernel_reg_compute_vec_binary_impl.h"
#elif __NPU_ARCH__ == 5102
#include "../../basic_api/reg_compute/dav_m510/kernel_reg_compute_vec_binary_impl.h"
#elif __NPU_ARCH__ == 5162
#include "../../basic_api/reg_compute/dav_v516/kernel_reg_compute_vec_binary_impl.h"
#else
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_vec_binary_impl.h"
#endif

namespace AscendC {
namespace Reg {
template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    AddImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Sub(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    SubImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Mul(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MulImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, auto mode, typename U>
__simd_callee__ inline void Div(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    DivImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Max(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MaxImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Min(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MinImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void ShiftLeft(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
{
    ShiftLeftImpl<T, U, mode, S, V>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void ShiftRight(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
{
    ShiftRightImpl<T, U, mode, S, V>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void And(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    AndImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Or(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    OrImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Xor(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    XorImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void Prelu(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    PreluImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T, typename U>
__simd_callee__ inline void Mull(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MullImpl<T, U>(dstReg0, dstReg1, srcReg0, srcReg1, mask);
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void MulAddDst(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MulAddDstImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Mula(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    MulAddDstImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddCarryOut(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    AddCarryOutImpl<T, U>(carry, dstReg, srcReg0, srcReg1, mask);
}
template <typename T, typename U>
__simd_callee__ inline void Add(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    AddCarryOutImpl<T, U>(carry, dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubCarryOut(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    SubCarryOutImpl<T, U>(carry, dstReg, srcReg0, srcReg1, mask);
}
template <typename T, typename U>
__simd_callee__ inline void Sub(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    SubCarryOutImpl<T, U>(carry, dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddCarryOuts(
    MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    AddCarryOutsImpl<T, U>(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}
template <typename T, typename U>
__simd_callee__ inline void AddC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    AddCarryOutsImpl<T, U>(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubCarryOuts(
    MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    SubCarryOutsImpl<T, U>(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}
template <typename T, typename U>
__simd_callee__ inline void SubC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    SubCarryOutsImpl<T, U>(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_KERNEL_REG_COMPUTE_VEC_BINARY_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_INTF_IMPL__
#endif
