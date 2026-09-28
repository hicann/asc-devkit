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
    "impl/c_api/reg_base_impl/npu_arch_9201/cube_compute_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_CUBE_COMPUTE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_CUBE_COMPUTE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source,
            c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source,
            c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_s8s4(
            c_matrix, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv,
            c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source,
            c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source,
            c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_f16s4(
            c_matrix, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv,
            c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_bf16s4(
            c_matrix, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv,
            c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad((__cc__ float*)xd, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, true,
            false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad((__cc__ float*)xd, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, true,
            false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad_s8s4(
            (__cc__ int32_t*)xd, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode),
            disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int8_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad((__cc__ float*)xd, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, true,
            false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int8_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad((__cc__ float*)xd, a_matrix, b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode), disable_gemv, true,
            false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad_f16s4(
            (__cc__ float*)xd, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode),
            disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv)
{
    if ASC_IS_AIC {
        uint64_t xd = ((uint64_t)c_matrix) & 0xffffffffULL | ((bias & 0xffffffffULL) << 32);
        mad_bf16s4(
            (__cc__ float*)xd, a_matrix, (__cb__ void*)b_matrix, m, k, n, static_cast<uint8_t>(unit_flag_mode),
            disable_gemv, true, false);
    }
}

//=================================asc_mmad_mx======================================

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ hif4x2_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ hif4x2_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, 0, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, c_matrix_source, c_matrix_init_val);
    }
}

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, bias, a_matrix, scale_a_matrix, b_matrix, scale_b_matrix, m, k, n,
            static_cast<uint8_t>(unit_flag_mode), disable_gemv, true, false);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
