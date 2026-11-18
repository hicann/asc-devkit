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
    "impl/c_api/reg_base_impl/npu_arch_9201/reg_permute_sel_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_PERMUTE_SEL_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_PERMUTE_SEL_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_intlv(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv_half_low_b8(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b8(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low_b16(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b16(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low_b32(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b32(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_high_b8(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b8(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high_b16(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b16(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high_b32(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlvv2_b32(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_half& dst, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_float& dst, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(
            reinterpret_cast<vector_uint8_t&>(dst), *reinterpret_cast<vector_uint8_t*>(&src0),
            *reinterpret_cast<vector_uint8_t*>(&src1), LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_low(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_half& dst, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_float& dst, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(
            reinterpret_cast<vector_uint8_t&>(dst), *reinterpret_cast<vector_uint8_t*>(&src0),
            *reinterpret_cast<vector_uint8_t*>(&src1), HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_intlv_half_high(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline vector_bool asc_intlv_half_low_b8(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_low_b8(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_intlv_half_low_b16(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_low_b16(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_intlv_half_low_b32(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_low_b32(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_intlv_half_high_b8(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_high_b8(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_intlv_half_high_b16(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_high_b16(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_intlv_half_high_b32(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_intlv_half_high_b32(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_intlv_half_low(vector_uint8_t src0, vector_uint8_t src1)
{
    vector_uint8_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_intlv_half_low(vector_int8_t src0, vector_int8_t src1)
{
    vector_int8_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_intlv_half_low(vector_uint16_t src0, vector_uint16_t src1)
{
    vector_uint16_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_intlv_half_low(vector_int16_t src0, vector_int16_t src1)
{
    vector_int16_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_intlv_half_low(vector_uint32_t src0, vector_uint32_t src1)
{
    vector_uint32_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_intlv_half_low(vector_int32_t src0, vector_int32_t src1)
{
    vector_int32_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_half asc_intlv_half_low(vector_half src0, vector_half src1)
{
    vector_half dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_float asc_intlv_half_low(vector_float src0, vector_float src1)
{
    vector_float dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_intlv_half_low(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    vector_fp8_e4m3fn_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_intlv_half_low(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    vector_fp8_e6m2_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_intlv_half_low(vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    vector_hifloat8_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_intlv_half_low(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    vector_fp8_e5m2_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_intlv_half_low(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    vector_fp8_e8m0_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_intlv_half_low(vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    vector_bfloat16_t dst;
    asc_intlv_half_low(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_intlv_half_high(vector_uint8_t src0, vector_uint8_t src1)
{
    vector_uint8_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_intlv_half_high(vector_int8_t src0, vector_int8_t src1)
{
    vector_int8_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_intlv_half_high(vector_uint16_t src0, vector_uint16_t src1)
{
    vector_uint16_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_intlv_half_high(vector_int16_t src0, vector_int16_t src1)
{
    vector_int16_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_intlv_half_high(vector_uint32_t src0, vector_uint32_t src1)
{
    vector_uint32_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_intlv_half_high(vector_int32_t src0, vector_int32_t src1)
{
    vector_int32_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_half asc_intlv_half_high(vector_half src0, vector_half src1)
{
    vector_half dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_float asc_intlv_half_high(vector_float src0, vector_float src1)
{
    vector_float dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_intlv_half_high(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    vector_fp8_e4m3fn_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_intlv_half_high(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    vector_fp8_e6m2_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_intlv_half_high(vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    vector_hifloat8_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_intlv_half_high(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    vector_fp8_e5m2_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_intlv_half_high(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    vector_fp8_e8m0_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_intlv_half_high(vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    vector_bfloat16_t dst;
    asc_intlv_half_high(dst, src0, src1);
    return dst;
}

__simd_callee__ inline void asc_deintlv(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv_even_b8(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b8(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even_b16(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b16(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even_b32(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b32(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_odd_b8(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b8(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd_b16(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b16(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd_b32(vector_bool& dst, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlvv2_b32(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_half& dst, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_float& dst, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(
            reinterpret_cast<vector_uint8_t&>(dst), *reinterpret_cast<vector_uint8_t*>(&src0),
            *reinterpret_cast<vector_uint8_t*>(&src1), LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_even(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, LOWER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_half& dst, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_float& dst, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(
            reinterpret_cast<vector_uint8_t&>(dst), *reinterpret_cast<vector_uint8_t*>(&src0),
            *reinterpret_cast<vector_uint8_t*>(&src1), HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline void asc_deintlv_odd(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vdintlvv2(dst, src0, src1, HIGHER);
    }
}

__simd_callee__ inline vector_bool asc_deintlv_even_b8(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_even_b8(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_deintlv_even_b16(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_even_b16(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_deintlv_even_b32(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_even_b32(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_deintlv_odd_b8(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_odd_b8(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_deintlv_odd_b16(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_odd_b16(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bool asc_deintlv_odd_b32(vector_bool src0, vector_bool src1)
{
    vector_bool dst;
    asc_deintlv_odd_b32(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_deintlv_even(vector_uint8_t src0, vector_uint8_t src1)
{
    vector_uint8_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_deintlv_even(vector_int8_t src0, vector_int8_t src1)
{
    vector_int8_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_deintlv_even(vector_uint16_t src0, vector_uint16_t src1)
{
    vector_uint16_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_deintlv_even(vector_int16_t src0, vector_int16_t src1)
{
    vector_int16_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_deintlv_even(vector_uint32_t src0, vector_uint32_t src1)
{
    vector_uint32_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_deintlv_even(vector_int32_t src0, vector_int32_t src1)
{
    vector_int32_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_half asc_deintlv_even(vector_half src0, vector_half src1)
{
    vector_half dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_float asc_deintlv_even(vector_float src0, vector_float src1)
{
    vector_float dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_deintlv_even(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    vector_fp8_e4m3fn_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_deintlv_even(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    vector_fp8_e6m2_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_deintlv_even(vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    vector_hifloat8_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_deintlv_even(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    vector_fp8_e5m2_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_deintlv_even(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    vector_fp8_e8m0_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_deintlv_even(vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    vector_bfloat16_t dst;
    asc_deintlv_even(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_deintlv_odd(vector_uint8_t src0, vector_uint8_t src1)
{
    vector_uint8_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_deintlv_odd(vector_int8_t src0, vector_int8_t src1)
{
    vector_int8_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_deintlv_odd(vector_uint16_t src0, vector_uint16_t src1)
{
    vector_uint16_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_deintlv_odd(vector_int16_t src0, vector_int16_t src1)
{
    vector_int16_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_deintlv_odd(vector_uint32_t src0, vector_uint32_t src1)
{
    vector_uint32_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_deintlv_odd(vector_int32_t src0, vector_int32_t src1)
{
    vector_int32_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_half asc_deintlv_odd(vector_half src0, vector_half src1)
{
    vector_half dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_float asc_deintlv_odd(vector_float src0, vector_float src1)
{
    vector_float dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_deintlv_odd(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    vector_fp8_e4m3fn_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_deintlv_odd(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    vector_fp8_e6m2_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_deintlv_odd(vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    vector_hifloat8_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_deintlv_odd(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    vector_fp8_e5m2_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_deintlv_odd(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    vector_fp8_e8m0_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_deintlv_odd(vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    vector_bfloat16_t dst;
    asc_deintlv_odd(dst, src0, src1);
    return dst;
}

__simd_callee__ inline void asc_pack_to_low(vector_uint32_t& dst, vector_uint64_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_uint32_t& dst, vector_int64_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint32_t& dst, vector_uint64_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint32_t& dst, vector_int64_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_uint16_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_int16_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_half& dst, vector_half src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_uint32_t& dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_int32_t& dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_float& dst, vector_float src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_uint64_t& dst, vector_uint64_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline void asc_pack_datablock(vector_int64_t& dst, vector_int64_t src)
{
    if ASC_IS_AIV {
        vpackv2(dst, src);
    }
}

__simd_callee__ inline vector_uint32_t asc_pack_to_low(vector_uint64_t src)
{
    vector_uint32_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_pack_to_low(vector_int64_t src)
{
    vector_uint32_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_pack_to_high(vector_uint64_t src)
{
    vector_uint32_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_pack_to_high(vector_int64_t src)
{
    vector_uint32_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_pack_datablock(vector_uint16_t src)
{
    vector_uint16_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_pack_datablock(vector_int16_t src)
{
    vector_int16_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_half asc_pack_datablock(vector_half src)
{
    vector_half dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_pack_datablock(vector_uint32_t src)
{
    vector_uint32_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_pack_datablock(vector_int32_t src)
{
    vector_int32_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_float asc_pack_datablock(vector_float src)
{
    vector_float dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint64_t asc_pack_datablock(vector_uint64_t src)
{
    vector_uint64_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_pack_datablock(vector_int64_t src)
{
    vector_int64_t dst;
    asc_pack_datablock(dst, src);
    return dst;
}
#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
