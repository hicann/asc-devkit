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
    "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_set_l3d_rpt_b_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"c_api/asc_simd.h\"\" and use public functions or variables defined in interface headers files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L3D_RPT_B_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L3D_RPT_B_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

constexpr uint8_t ASC_L3D_RPT_B_REPEAT_TIMES_SHIFT = 16;
constexpr uint8_t ASC_L3D_RPT_B_DIRECTION_SHIFT = 24;
constexpr uint8_t ASC_L3D_RPT_B_DST_STRIDE_K_SHIFT = 32;
constexpr uint8_t ASC_L3D_RPT_B_DST_START_POS_M_SHIFT = 48;

__aicore__ inline void asc_set_l3d_rpt_b_impl(uint64_t config)
{
    if ASC_IS_AIC {
        set_l3d_rpt_b(config);
    }
}

__aicore__ inline void asc_set_l3d_rpt_b_impl(
    uint16_t repeat_stride, uint8_t repeat_times, asc_l13d_repeat_direction repeat_direction, uint16_t dst_stride_k,
    uint16_t dst_start_pos_m)
{
    if ASC_IS_AIC {
        uint64_t config = (static_cast<uint64_t>(dst_start_pos_m) << ASC_L3D_RPT_B_DST_START_POS_M_SHIFT) |
                          (static_cast<uint64_t>(dst_stride_k) << ASC_L3D_RPT_B_DST_STRIDE_K_SHIFT) |
                          (static_cast<uint64_t>(repeat_direction) << ASC_L3D_RPT_B_DIRECTION_SHIFT) |
                          (static_cast<uint64_t>(repeat_times) << ASC_L3D_RPT_B_REPEAT_TIMES_SHIFT) |
                          static_cast<uint64_t>(repeat_stride);
        set_l3d_rpt_b(config);
    }
}
#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
