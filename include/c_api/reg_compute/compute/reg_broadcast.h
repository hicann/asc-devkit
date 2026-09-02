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
#error "compute/reg_broadcast.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value, vector_bool mask);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value, vector_bool mask);

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value, vector_bool mask);

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value, vector_bool mask);

__simd_callee__ inline vector_half asc_duplicate_scalar(half value, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value, vector_bool mask);

__simd_callee__ inline vector_float asc_duplicate_scalar(float value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint8_t& dst, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int8_t& dst, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint16_t& dst, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int16_t& dst, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_half& dst, half value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint32_t& dst, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int32_t& dst, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar_merge(vector_float& dst, float value, vector_bool mask);

__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value);

__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value);

__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value);

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value);

__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value);

__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value);

__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value);

__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value);

__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value);

__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value);

__simd_callee__ inline vector_half asc_duplicate_scalar(half value);

__simd_callee__ inline vector_float asc_duplicate_scalar(float value);

__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value);

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value);

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value);

__simd_callee__ inline void asc_duplicate(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_duplicate(vector_uint8_t src, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_duplicate(vector_int8_t src, vector_bool mask);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate(vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate(vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate(vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_duplicate(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_duplicate(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_duplicate(vector_half src, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_duplicate(vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_duplicate(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_duplicate(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_float asc_duplicate(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_merge(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst, vector_float src, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#endif
