/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#pragma message( \
    "impl/c_api/instr_impl/npu_arch_3510/cube_compute_impl/asc_set_l0c_copy_nz_para_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\\"#include \\\"c_api/asc_simd.h\\\"\\\" and use public functions or variables defined in interface headers files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_IMPL_ASC_SET_L0C_COPY_NZ_PARA_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_IMPL_ASC_SET_L0C_COPY_NZ_PARA_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

constexpr uint8_t ASC_L0C_COPY_NZ_PARA_SRC_SHIFT = 16;
constexpr uint8_t ASC_L0C_COPY_NZ_PARA_DST_SHIFT = 32;

__aicore__ inline void asc_set_l0c_copy_nz_para_impl(
    uint16_t matrix_num, uint16_t src_nz_matrix_stride, uint32_t dst_matrix_stride)
{
    if ASC_IS_AIC {
        uint64_t config = (static_cast<uint64_t>(dst_matrix_stride) << ASC_L0C_COPY_NZ_PARA_DST_SHIFT) |
                          (static_cast<uint64_t>(src_nz_matrix_stride) << ASC_L0C_COPY_NZ_PARA_SRC_SHIFT) | matrix_num;
        set_loop3_para(config);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
