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
 * \file kernel_reg_compute_maskreg_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_reg_compute_maskreg_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_H__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_H
#define ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_H

#include "kernel_reg_compute_struct_intf.h"
#include "kernel_reg_compute_utils.h"
#include "kernel_reg_compute_common_intf.h"

namespace AscendC {
namespace Reg {
#if !defined(__NPU_ARCH__) ||                                                                                \
    (defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
                               (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)) ||                          \
     defined(__ASC_NPU_HOST__))
template <typename T, const RegTrait& regTrait = RegTraitNumOne>
__simd_callee__ inline MaskReg UpdateMask(uint32_t& scalarValue);

template <typename T, MaskPattern mode = MaskPattern::ALL, const RegTrait& regTrait = RegTraitNumOne>
__simd_callee__ inline MaskReg CreateMask();

__simd_callee__ inline void Not(MaskReg& dst, MaskReg& src, MaskReg& mask);

__simd_callee__ inline void And(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);

template <typename T = DefaultType, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensor(MaskReg& dst, U& srcReg);

__simd_callee__ inline void Or(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);

__simd_callee__ inline void Xor(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);

__simd_callee__ inline void Move(MaskReg& dst, MaskReg& src, MaskReg& mask);

__simd_callee__ inline void Move(MaskReg& dst, MaskReg& src);

template <typename T>
__simd_callee__ inline void Interleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1);

template <typename T>
__simd_callee__ inline void DeInterleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1);

__simd_callee__ inline void Select(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void Pack(MaskReg& dst, MaskReg& src);

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void UnPack(MaskReg& dst, MaskReg& src);

template <typename T>
__simd_callee__ inline MaskReg MoveMask();
#endif
} // namespace Reg
} // namespace AscendC

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
                              (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)) ||                          \
    defined(__ASC_NPU_HOST__)
#include "../../../impl/basic_api/reg_compute/kernel_reg_compute_maskreg_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_REG_COMPUTE_MASKREG_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_INTF_H__
#endif
