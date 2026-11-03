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
    "impl/c_api/reg_base_impl/npu_arch_9201/ub_gather_intf_impl_arch_9201.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_UB_GATHER_INTF_IMPL_ARCH_9201_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_UB_GATHER_INTF_IMPL_ARCH_9201_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_gather(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint16_t index, vector_bool mask)
{
    if ASC_IS_AIV {
        __ubuf__ float8_e5m2_t* src_tmp = reinterpret_cast<__ubuf__ float8_e5m2_t*>(src);
        vgather2(reinterpret_cast<vector_fp8_e5m2_t&>(dst), src_tmp, index, mask);
    }
}

__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint32_t index, vector_bool mask)
{
    if ASC_IS_AIV {
        __ubuf__ float8_e5m2_t* src_tmp = reinterpret_cast<__ubuf__ float8_e5m2_t*>(src);
        vgatherb(reinterpret_cast<vector_fp8_e5m2_t&>(dst), src_tmp, index, mask);
    }
}

__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint32_t index)
{
    if ASC_IS_AIV {
        __ubuf__ float8_e5m2_t* src_tmp = reinterpret_cast<__ubuf__ float8_e5m2_t*>(src);
        vgatherb(reinterpret_cast<vector_fp8_e5m2_t&>(dst), src_tmp, index);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_gather(__ubuf__ fp8_e6m2_t* src, vector_uint16_t index, vector_bool mask)
{
    vector_fp8_e6m2_t dst;
    asc_gather(dst, src, index, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_gather_datablock(
    __ubuf__ fp8_e6m2_t* src, vector_uint32_t index, vector_bool mask)
{
    vector_fp8_e6m2_t dst;
    asc_gather_datablock(dst, src, index, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e6m2_t asc_gather_datablock(__ubuf__ fp8_e6m2_t* src, vector_uint32_t index)
{
    vector_fp8_e6m2_t dst;
    asc_gather_datablock(dst, src, index);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
