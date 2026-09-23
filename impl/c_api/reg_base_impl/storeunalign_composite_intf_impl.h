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
    "impl/c_api/reg_base_impl/storeunalign_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_STOREUNALIGN_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_STOREUNALIGN_COMPOSITE_INTF_IMPL_H

#include "c_api/utils/sys_var.h"
#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(int8_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(uint8_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(int16_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(uint16_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(int32_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(uint32_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ half* dst, vector_half src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(half);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(float);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(int64_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(bfloat16_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(fp8_e4m3fn_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(hifloat8_t);
        vstus(
            ureg, count, *reinterpret_cast<vector_uint8_t*>(&src), reinterpret_cast<__ubuf__ uint8_t*&>(dst),
            POST_UPDATE);
        vstas(ureg, reinterpret_cast<__ubuf__ uint8_t*>(dst), 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(fp8_e5m2_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(fp8_e8m0_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(fp4x2_e2m1_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(fp4x2_e1m2_t);
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        constexpr uint32_t count = asc_get_vf_len() / sizeof(int4b_t);
        __ubuf__ float4_e1m2x2_t*& dst_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(dst);
        vstus(ureg, count, *reinterpret_cast<vector_fp4x2_e1m2_t*>(&src), dst_tmp, POST_UPDATE);
        vstas(ureg, dst_tmp, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ half* dst, vector_half src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(
            ureg, count, *reinterpret_cast<vector_uint8_t*>(&src), reinterpret_cast<__ubuf__ uint8_t*&>(dst),
            POST_UPDATE);
        vstas(ureg, reinterpret_cast<__ubuf__ uint8_t*>(dst), 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        vstus(ureg, count, src, dst, POST_UPDATE);
        vstas(ureg, dst, 0);
    }
}

__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src, uint32_t count)
{
    if ASC_IS_AIV {
        vector_store_unalign ureg;
        __ubuf__ float4_e1m2x2_t*& dst_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(dst);
        vstus(ureg, count, *reinterpret_cast<vector_fp4x2_e1m2_t*>(&src), dst_tmp, POST_UPDATE);
        vstas(ureg, dst_tmp, 0);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
