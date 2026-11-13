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
#warning \
    "impl/c_api/reg_base_impl/npu_arch_3510/cube_compute_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_COMPOSITE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

// Keep the original overloads and their intrinsic calls; only the type triples vary.
#define ASC_DEFINE_CUBE_MMAD_SYNC_3510(api, intrin, c_type, a_type, b_type)                               \
    ASC_DEPRECATED(9.2.0, "2027/09/07", api and asc_sync)                                                 \
    __aicore__ inline void api##_sync(                                                                    \
        __cc__ c_type* c_matrix, __ca__ a_type* a_matrix, __cb__ b_type* b_matrix, uint16_t left_height,  \
        uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, \
        bool c_matrix_init_val)                                                                           \
    {                                                                                                     \
        if ASC_IS_AIC {                                                                                   \
            intrin(                                                                                       \
                c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv,   \
                c_matrix_source, c_matrix_init_val);                                                      \
            asc_sync_post_process();                                                                      \
        }                                                                                                 \
    }

ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp4x2_e1m2_t, fp4x2_e1m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp4x2_e1m2_t, fp4x2_e2m1_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp4x2_e2m1_t, fp4x2_e1m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp4x2_e2m1_t, fp4x2_e2m1_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp8_e4m3fn_t, fp8_e4m3fn_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp8_e4m3fn_t, fp8_e5m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp8_e5m2_t, fp8_e4m3fn_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad_mx, mad_mx, float, fp8_e5m2_t, fp8_e5m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, bfloat16_t, bfloat16_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, fp8_e4m3fn_t, fp8_e4m3fn_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, fp8_e4m3fn_t, fp8_e5m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, fp8_e5m2_t, fp8_e4m3fn_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, fp8_e5m2_t, fp8_e5m2_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, half, half)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, float, float)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, int32_t, int8_t, int8_t)
ASC_DEFINE_CUBE_MMAD_SYNC_3510(asc_mmad, mad, float, hifloat8_t, hifloat8_t)

#undef ASC_DEFINE_CUBE_MMAD_SYNC_3510

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
