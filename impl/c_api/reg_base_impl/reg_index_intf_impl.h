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
    "impl/c_api/reg_base_impl/reg_index_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_INDEX_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_INDEX_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, INC_ORDER);
    }
}

__simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, INC_ORDER);
    }
}

__simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, INC_ORDER);
    }
}

__simd_callee__ inline void asc_arange(vector_half& dst, half value)
{
    if ASC_IS_AIV {
        vci(dst, value, INC_ORDER);
    }
}

__simd_callee__ inline void asc_arange(vector_float& dst, float value)
{
    if ASC_IS_AIV {
        vci(dst, value, INC_ORDER);
    }
}

__simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, DEC_ORDER);
    }
}

__simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, DEC_ORDER);
    }
}

__simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value)
{
    if ASC_IS_AIV {
        vci(dst, value, DEC_ORDER);
    }
}

__simd_callee__ inline void asc_arange_descend(vector_half& dst, half value)
{
    if ASC_IS_AIV {
        vci(dst, value, DEC_ORDER);
    }
}

__simd_callee__ inline void asc_arange_descend(vector_float& dst, float value)
{
    if ASC_IS_AIV {
        vci(dst, value, DEC_ORDER);
    }
}

__simd_callee__ inline vector_int8_t asc_arange(int8_t value)
{
    vector_int8_t dst;
    asc_arange(dst, value);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_arange(int16_t value)
{
    vector_int16_t dst;
    asc_arange(dst, value);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_arange(int32_t value)
{
    vector_int32_t dst;
    asc_arange(dst, value);
    return dst;
}

__simd_callee__ inline vector_half asc_arange(half value)
{
    vector_half dst;
    asc_arange(dst, value);
    return dst;
}

__simd_callee__ inline vector_float asc_arange(float value)
{
    vector_float dst;
    asc_arange(dst, value);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_arange_descend(int8_t value)
{
    vector_int8_t dst;
    asc_arange_descend(dst, value);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_arange_descend(int16_t value)
{
    vector_int16_t dst;
    asc_arange_descend(dst, value);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_arange_descend(int32_t value)
{
    vector_int32_t dst;
    asc_arange_descend(dst, value);
    return dst;
}

__simd_callee__ inline vector_half asc_arange_descend(half value)
{
    vector_half dst;
    asc_arange_descend(dst, value);
    return dst;
}

__simd_callee__ inline vector_float asc_arange_descend(float value)
{
    vector_float dst;
    asc_arange_descend(dst, value);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
