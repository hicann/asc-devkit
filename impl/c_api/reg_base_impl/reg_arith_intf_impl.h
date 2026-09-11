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
    "impl/c_api/reg_base_impl/reg_arith_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_ARITH_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_ARITH_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_sub(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsub(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsubc(carry, dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsubc(carry, dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_min(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmin(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_neg(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vneg(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_neg(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vneg(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_neg(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vneg(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_neg(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vneg(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_neg(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vneg(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vabs(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vabs(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vabs(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vabs(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vabs(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vaddc(carry, dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vaddc(carry, dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_max(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmax(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_min_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sqrt(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqrt(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_sqrt(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqrt(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmul(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mul_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_exp(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vexp(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_exp(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vexp(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_add_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_div(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vdiv(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_max_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_add(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_add(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_add(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_add(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_add(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_add(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_add(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_add(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_add(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_add(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask)
{
    if ASC_IS_AIV {
        vaddcs(carry, dst, src0, src1, carry_src, mask);
    }
}

__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask)
{
    if ASC_IS_AIV {
        vaddcs(carry, dst, src0, src1, carry_src, mask);
    }
}

__simd_callee__ inline vector_uint8_t asc_sub(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_sub(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_sub(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_sub(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_sub(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_sub(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_sub(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_sub(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_sub(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask)
{
    if ASC_IS_AIV {
        vsubcs(carry, dst, src0, src1, carry_src, mask);
    }
}

__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask)
{
    if ASC_IS_AIV {
        vsubcs(carry, dst, src0, src1, carry_src, mask);
    }
}

__simd_callee__ inline vector_int8_t asc_min(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_min(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_min(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_min(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_min(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_min(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_min(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_min(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_min(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_min(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_neg(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_neg(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_neg(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_neg(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_neg(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_neg(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_neg(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_neg(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_neg(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_neg(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_abs(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_abs(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_abs(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_abs(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_abs(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_abs(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_abs(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_abs(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_abs(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_abs(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_max(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_max(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_max(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_max(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_max(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_max(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_max(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_max(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_max(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_max(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_min_scalar(vector_int32_t src, int32_t value, vector_bool mask)
{
    vector_int32_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_min_scalar(vector_uint32_t src, uint32_t value, vector_bool mask)
{
    vector_uint32_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_min_scalar(vector_int16_t src, int16_t value, vector_bool mask)
{
    vector_int16_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_min_scalar(vector_uint16_t src, uint16_t value, vector_bool mask)
{
    vector_uint16_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_min_scalar(vector_int8_t src, int8_t value, vector_bool mask)
{
    vector_int8_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_min_scalar(vector_uint8_t src, uint8_t value, vector_bool mask)
{
    vector_uint8_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_min_scalar(vector_float src, float value, vector_bool mask)
{
    vector_float dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_min_scalar(vector_half src, half value, vector_bool mask)
{
    vector_half dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_min_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_min_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_sqrt(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_sqrt(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_sqrt(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_sqrt(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_mul(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_mul(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_mul(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_mul(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_mul(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_mul(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_mul(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_mul(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_mul_scalar(vector_int16_t src, int16_t value, vector_bool mask)
{
    vector_int16_t dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_mul_scalar(vector_uint16_t src, uint16_t value, vector_bool mask)
{
    vector_uint16_t dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_mul_scalar(vector_half src, half value, vector_bool mask)
{
    vector_half dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_mul_scalar(vector_int32_t src, int32_t value, vector_bool mask)
{
    vector_int32_t dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_mul_scalar(vector_uint32_t src, uint32_t value, vector_bool mask)
{
    vector_uint32_t dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_mul_scalar(vector_float src, float value, vector_bool mask)
{
    vector_float dst;
    asc_mul_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_exp(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_exp(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_exp(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_exp(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_add_scalar(vector_int8_t src, int8_t value, vector_bool mask)
{
    vector_int8_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_add_scalar(vector_uint8_t src, uint8_t value, vector_bool mask)
{
    vector_uint8_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_add_scalar(vector_int16_t src, int16_t value, vector_bool mask)
{
    vector_int16_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_add_scalar(vector_uint16_t src, uint16_t value, vector_bool mask)
{
    vector_uint16_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_add_scalar(vector_half src, half value, vector_bool mask)
{
    vector_half dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_add_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_add_scalar(vector_int32_t src, int32_t value, vector_bool mask)
{
    vector_int32_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_add_scalar(vector_uint32_t src, uint32_t value, vector_bool mask)
{
    vector_uint32_t dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_add_scalar(vector_float src, float value, vector_bool mask)
{
    vector_float dst;
    asc_add_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline void asc_ln(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vln(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_ln(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vln(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_log(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vln(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_log(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vln(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_div(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_div(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_div(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_div(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_div(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_div(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_div(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_max_scalar(vector_int8_t src, int8_t value, vector_bool mask)
{
    vector_int8_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_max_scalar(vector_int16_t src, int16_t value, vector_bool mask)
{
    vector_int16_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_max_scalar(vector_int32_t src, int32_t value, vector_bool mask)
{
    vector_int32_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_max_scalar(vector_uint8_t src, uint8_t value, vector_bool mask)
{
    vector_uint8_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_max_scalar(vector_uint16_t src, uint16_t value, vector_bool mask)
{
    vector_uint16_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_max_scalar(vector_uint32_t src, uint32_t value, vector_bool mask)
{
    vector_uint32_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_max_scalar(vector_half src, half value, vector_bool mask)
{
    vector_half dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_max_scalar(vector_float src, float value, vector_bool mask)
{
    vector_float dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_max_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_max_scalar(dst, src, value, mask);
    return dst;
}

__simd_callee__ inline void asc_mull(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmull(dst0, dst1, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_mull(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmull(dst0, dst1, src0, src1, mask);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
