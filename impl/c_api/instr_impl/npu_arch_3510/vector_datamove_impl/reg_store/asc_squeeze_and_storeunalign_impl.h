/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_squeeze_and_store_unalign_impl.h
 * \brief
 */

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_IMPL_REG_STORE_ASC_SQUEEZE_AND_STOREUNALIGN_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_IMPL_REG_STORE_ASC_SQUEEZE_AND_STOREUNALIGN_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ int8_t* dst_addr, vector_int8_t dst, vector_store_unalign& src0, vector_int8_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ uint8_t* dst_addr, vector_uint8_t dst, vector_store_unalign& src0, vector_uint8_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ int16_t* dst_addr, vector_int16_t dst, vector_store_unalign& src0, vector_int16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ uint16_t* dst_addr, vector_uint16_t dst, vector_store_unalign& src0, vector_uint16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ int32_t* dst_addr, vector_int32_t dst, vector_store_unalign& src0, vector_int32_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ uint32_t* dst_addr, vector_uint32_t dst, vector_store_unalign& src0, vector_uint32_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ half* dst_addr, vector_half dst, vector_store_unalign& src0, vector_half src1, vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ float* dst_addr, vector_float dst, vector_store_unalign& src0, vector_float src1, vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ bfloat16_t* dst_addr, vector_bfloat16_t dst, vector_store_unalign& src0, vector_bfloat16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ fp8_e4m3fn_t* dst_addr, vector_fp8_e4m3fn_t dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ fp8_e5m2_t* dst_addr, vector_fp8_e5m2_t dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ fp8_e8m0_t* dst_addr, vector_fp8_e8m0_t dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ fp4x2_e2m1_t* dst_addr, vector_fp4x2_e2m1_t dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ fp4x2_e1m2_t* dst_addr, vector_fp4x2_e1m2_t dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_impl(
    __ubuf__ int4b_t* dst_addr, vector_int4x2_t dst, vector_store_unalign& src0, vector_int4x2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        __ubuf__ float4_e1m2x2_t*& dst_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(dst_addr);
        vstur(src0, *reinterpret_cast<vector_fp4x2_e1m2_t*>(&dst), dst_tmp, POST_UPDATE);
    }
}

#endif
