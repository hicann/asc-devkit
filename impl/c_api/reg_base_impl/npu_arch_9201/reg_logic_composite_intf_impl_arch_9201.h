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
    "impl/c_api/reg_base_impl/npu_arch_9201/reg_logic_composite_intf_impl_arch_9201.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_LOGIC_COMPOSITE_INTF_IMPL_ARCH_9201_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_LOGIC_COMPOSITE_INTF_IMPL_ARCH_9201_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_shiftleft(
    vector_uint64_t& dst, vector_uint64_t src0, vector_uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src0, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(
    vector_int64_t& dst, vector_int64_t src0, vector_uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src0, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint64_t asc_shiftleft(vector_uint64_t src0, vector_uint32_t shift, vector_bool mask)
{
    vector_uint64_t dst;
    asc_shiftleft(dst, src0, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_shiftleft(vector_int64_t src0, vector_uint32_t shift, vector_bool mask)
{
    vector_int64_t dst;
    asc_shiftleft(dst, src0, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftright(
    vector_uint64_t& dst, vector_uint64_t src0, vector_uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src0, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(
    vector_int64_t& dst, vector_int64_t src0, vector_uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src0, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint64_t asc_shiftright(vector_uint64_t src0, vector_uint32_t shift, vector_bool mask)
{
    vector_uint64_t dst;
    asc_shiftright(dst, src0, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_shiftright(vector_int64_t src0, vector_uint32_t shift, vector_bool mask)
{
    vector_int64_t dst;
    asc_shiftright(dst, src0, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint64_t& dst, vector_uint64_t src, uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int64_t& dst, vector_int64_t src, uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint64_t asc_shiftleft_scalar(vector_uint64_t src, uint32_t shift, vector_bool mask)
{
    vector_uint64_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_shiftleft_scalar(vector_int64_t src, uint32_t shift, vector_bool mask)
{
    vector_int64_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint64_t& dst, vector_uint64_t src, uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int64_t& dst, vector_int64_t src, uint32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint64_t asc_shiftright_scalar(vector_uint64_t src, uint32_t shift, vector_bool mask)
{
    vector_uint64_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_shiftright_scalar(vector_int64_t src, uint32_t shift, vector_bool mask)
{
    vector_int64_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
