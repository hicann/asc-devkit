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
#error "reg_copy.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_REG_COPY_H
#define INCLUDE_C_API_REG_COMPUTE_REG_COPY_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_copy(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src, vector_bool mask);

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src);

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint16_t src, int16_t part);

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint32_t src, int16_t part);

// ==========asc_mask return-value overloads(u8/s8/half/u16/s16/float/u32/s32/bf16/bool)==========
__simd_callee__ inline vector_int8_t asc_mask(vector_int8_t src, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_mask(vector_uint8_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_mask(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_mask(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_mask(vector_half src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_mask(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_mask(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_float asc_mask(vector_float src, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_mask(vector_bfloat16_t src, vector_bool mask);

__simd_callee__ inline vector_bool asc_mask(vector_bool src, vector_bool mask);

__simd_callee__ inline vector_bool asc_mask(vector_bool src);

// ==========asc_extract_mask return-value overloads(u16/u32)==========
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint16_t src, int16_t part);

__simd_callee__ inline vector_bool asc_extract_mask(vector_uint32_t src, int16_t part);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
