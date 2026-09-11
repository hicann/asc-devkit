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
    "impl/c_api/reg_base_impl/loadunalign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_LOADUNALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_LOADUNALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vldas(dst, src_tmp);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, reinterpret_cast<__ubuf__ uint8_t*>(src));
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src)
{
    if ASC_IS_AIV {
        vldas(dst, src);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vlda(dst, src_tmp, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, reinterpret_cast<__ubuf__ uint8_t*>(src), offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vlda(dst, src, offset);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_half& dst, vector_load_unalign& src0, __ubuf__ half* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_float& dst, vector_load_unalign& src0, __ubuf__ float* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t* src1)
{
    if ASC_IS_AIV {
        vldus(reinterpret_cast<vector_uint8_t&>(dst), src0, reinterpret_cast<__ubuf__ uint8_t*>(src1));
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t* src1)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1);
    }
}

__simd_callee__ inline void asc_loadunalign(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t* src1)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src1_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src1);
        vldus(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src0, src1_tmp);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_half& dst, vector_load_unalign& src0, __ubuf__ half*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_float& dst, vector_load_unalign& src0, __ubuf__ float*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(
            reinterpret_cast<vector_uint8_t&>(dst), src0, reinterpret_cast<__ubuf__ uint8_t*&>(src1), count,
            POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(
            reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src0, reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(src1),
            count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t*& src1, uint32_t count)
{
    if ASC_IS_AIV {
        vldus(dst, src0, src1, count, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_half& dst, vector_load_unalign& src0, __ubuf__ half* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_float& dst, vector_load_unalign& src0, __ubuf__ float* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(reinterpret_cast<vector_uint8_t&>(dst), src0, offset, reinterpret_cast<__ubuf__ uint8_t*>(src1), inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(
            reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src0, offset,
            reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src1), inc);
    }
}

__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t* src1, addr_reg& offset, uint32_t inc)
{
    if ASC_IS_AIV {
        vldu(dst, src0, offset, src1, inc);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
