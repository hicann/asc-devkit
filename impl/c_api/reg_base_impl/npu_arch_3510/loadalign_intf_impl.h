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
    "impl/c_api/reg_base_impl/npu_arch_3510/loadalign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_LOADALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_LOADALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src)
{
    if ASC_IS_AIV {
        vlds(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), 0, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src)
{
    if ASC_IS_AIV {
        vlds(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vld(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vld(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), offset, UNPK4_B8);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(reinterpret_cast<vector_uint8_t&>(dst), reinterpret_cast<__ubuf__ uint8_t*>(src), offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t*& src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ uint8_t*& src_tmp = reinterpret_cast<__ubuf__ uint8_t*&>(src);
        vlds(reinterpret_cast<vector_uint8_t&>(dst), src_tmp, offset, UNPK4_B8, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t*& src_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(src);
        vlds(reinterpret_cast<vector_fp4x2_e1m2_t&>(dst), src_tmp, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        __ubuf__ uint8_t*& src_tmp = reinterpret_cast<__ubuf__ uint8_t*&>(src);
        vlds(reinterpret_cast<vector_uint8_t&>(dst), src_tmp, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_half& dst, __ubuf__ half*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_float& dst, __ubuf__ float*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_half& dst, __ubuf__ half*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B16, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_float& dst, __ubuf__ float*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, E2B_B32, POST_UPDATE);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
