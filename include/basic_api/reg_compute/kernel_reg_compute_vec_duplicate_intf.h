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
 * \file kernel_reg_compute_vec_duplicate_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_reg_compute_vec_duplicate_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_INTF_H__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_INTERFACE_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_INTERFACE_H

#include "kernel_reg_compute_common_intf.h"
#include "kernel_reg_compute_utils.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void Duplicate(S& dstReg, U scalarValue);

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
__simd_callee__ inline void Duplicate(S& dstReg, U scalarValue, MaskReg& mask);

template <
    typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING,
    typename S>
__simd_callee__ inline void Duplicate(S& dstReg, S& srcReg, MaskReg& mask);
} // namespace Reg
} // namespace AscendC

#include "kernel_reg_compute_vec_data_reorder_intf.h"
#if defined(__NPU_ARCH__) &&                                                                                      \
        ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)) || \
    defined(__ASC_NPU_HOST__)
#include "../../../impl/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_INTF_H__
#endif
