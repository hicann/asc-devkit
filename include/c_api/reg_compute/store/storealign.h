/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "reg_compute/store/storealign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_STORE_STOREALIGN_H
#define INCLUDE_C_API_REG_COMPUTE_STORE_STOREALIGN_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_datamove_impl.h"
#endif

__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int4b_t* dst, vector_int4x2_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ int16_t* dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ int64_t* dst, vector_int64_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint64_t* dst, vector_uint64_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ float* dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ half* dst, vector_half src0, vector_half src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1);

__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1);

__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_uint8_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst, vector_int16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int4b_t* dst, vector_int4x2_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int16_t* dst, vector_int16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int64_t* dst, vector_int64_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint64_t* dst, vector_uint64_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ half* dst, vector_half src0, vector_half src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1, int32_t offset);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int8_t*& dst, vector_int8_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint8_t*& dst, vector_uint8_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ half*& dst, vector_half src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int8_t*& dst, vector_int8_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint8_t*& dst, vector_uint8_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ half*& dst, vector_half src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ float*& dst, vector_float src, int32_t offset);

__simd_callee__ inline void asc_storealign_1st_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int64_t*& dst, vector_int64_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint64_t*& dst, vector_uint64_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ half*& dst, vector_half src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_postupdate_v2 is deprecated. Please use "
             "asc_storealign_pack_quarter_postupdate instead.")]]
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_postupdate_v2 is deprecated. Please use "
             "asc_storealign_pack_quarter_postupdate instead.")]]
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_postupdate_v2 is deprecated. Please use "
             "asc_storealign_pack_quarter_postupdate instead.")]]
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint8_t* dst, vector_uint8_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int4b_t* dst, vector_int4x2_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src, addr_reg offset);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int64_t* dst, vector_int64_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint64_t* dst, vector_uint64_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, addr_reg offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, addr_reg offset, vector_bool mask);

[[deprecated("NOTICE: asc_storealign_pack_v2 is deprecated. Please use asc_storealign_pack_quarter instead.")]]
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ half* dst, vector_half src0, vector_half src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1, addr_reg offset);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int8_t* dst, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint8_t* dst, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int16_t* dst, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ int32_t* dst, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ half* dst, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(
    __ubuf__ float* dst, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int8_t*& dst, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint8_t*& dst, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e4m3fn_t*& dst, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ hifloat8_t*& dst, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e5m2_t*& dst, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e8m0_t*& dst, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ half*& dst, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ float*& dst, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src, addr_reg offset);

__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src);

__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset);

__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ float* dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H
#endif
