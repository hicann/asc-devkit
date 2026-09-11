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
    "impl/c_api/reg_base_impl/reg_broadcast_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_BROADCAST_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_BROADCAST_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value)
{
    if ASC_IS_AIV {
        vbr(dst, value);
    }
}

__simd_callee__ inline void asc_duplicate(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value, vector_bool mask)
{
    vector_uint8_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value, vector_bool mask)
{
    vector_int8_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value, vector_bool mask)
{
    vector_fp8_e4m3fn_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value, vector_bool mask)
{
    vector_fp8_e5m2_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value, vector_bool mask)
{
    vector_fp8_e8m0_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value, vector_bool mask)
{
    vector_uint16_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value, vector_bool mask)
{
    vector_int16_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_duplicate_scalar(half value, vector_bool mask)
{
    vector_half dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value, vector_bool mask)
{
    vector_uint32_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value, vector_bool mask)
{
    vector_int32_t dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_duplicate_scalar(float value, vector_bool mask)
{
    vector_float dst;
    asc_duplicate_scalar(dst, value, mask);
    return dst;
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint8_t& dst, uint8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int8_t& dst, int8_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint16_t& dst, uint16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int16_t& dst, int16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_half& dst, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint32_t& dst, uint32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int32_t& dst, int32_t value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_float& dst, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, value, mask, MODE_MERGING);
    }
}

__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value)
{
    vector_uint8_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value)
{
    vector_int8_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value)
{
    vector_uint16_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value)
{
    vector_int16_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value)
{
    vector_uint32_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value)
{
    vector_int32_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_half asc_duplicate_scalar(half value)
{
    vector_half dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_float asc_duplicate_scalar(float value)
{
    vector_float dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value)
{
    vector_bfloat16_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value)
{
    vector_fp8_e4m3fn_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value)
{
    vector_fp8_e5m2_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value)
{
    vector_fp8_e8m0_t dst;
    asc_duplicate_scalar(dst, value);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_duplicate(vector_uint8_t src, vector_bool mask)
{
    vector_uint8_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_duplicate(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate(vector_fp8_e4m3fn_t src, vector_bool mask)
{
    vector_fp8_e4m3fn_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate(vector_fp8_e5m2_t src, vector_bool mask)
{
    vector_fp8_e5m2_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate(vector_fp8_e8m0_t src, vector_bool mask)
{
    vector_fp8_e8m0_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_duplicate(vector_uint16_t src, vector_bool mask)
{
    vector_uint16_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_duplicate(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_duplicate(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_duplicate(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_duplicate(vector_uint32_t src, vector_bool mask)
{
    vector_uint32_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_duplicate(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_duplicate(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_duplicate(dst, src, mask);
    return dst;
}

__simd_callee__ inline void asc_duplicate_highest(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_highest(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_merge(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_LOWEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vdup(dst, src, mask, POS_HIGHEST, MODE_MERGING);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
