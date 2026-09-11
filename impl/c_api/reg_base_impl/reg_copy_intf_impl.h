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
    "impl/c_api/reg_base_impl/reg_copy_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_COPY_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_COPY_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_copy(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vmov(dst, src, mask, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src, vector_bool mask)
{
    if ASC_IS_AIV {
        pmov(dst, src, mask);
    }
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        pmov(dst, src);
    }
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint16_t src, int16_t part)
{
    if ASC_IS_AIV {
        movvp(dst, src, part);
    }
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint32_t src, int16_t part)
{
    if ASC_IS_AIV {
        movvp(dst, src, part);
    }
}

__simd_callee__ inline vector_int8_t asc_mask(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_mask(vector_uint8_t src, vector_bool mask)
{
    vector_uint8_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_mask(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_mask(vector_uint16_t src, vector_bool mask)
{
    vector_uint16_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_mask(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_mask(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_mask(vector_uint32_t src, vector_bool mask)
{
    vector_uint32_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_mask(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_mask(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_mask(vector_bool src, vector_bool mask)
{
    vector_bool dst;
    asc_copy(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_mask(vector_bool src)
{
    vector_bool dst;
    asc_copy(dst, src);
    return dst;
}

__simd_callee__ inline vector_bool asc_extract_mask(vector_uint16_t src, int16_t part)
{
    vector_bool dst;
    asc_copy(dst, src, part);
    return dst;
}

__simd_callee__ inline vector_bool asc_extract_mask(vector_uint32_t src, int16_t part)
{
    vector_bool dst;
    asc_copy(dst, src, part);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
