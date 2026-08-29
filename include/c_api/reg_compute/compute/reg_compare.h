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
#error "compute/reg_compare.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ge_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_le(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_le(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_le_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_eq_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_lt_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_gt_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bool asc_ne_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
