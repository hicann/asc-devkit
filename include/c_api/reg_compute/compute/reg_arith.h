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
#error "compute/reg_arith.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ARITH_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ARITH_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline vector_uint8_t asc_sub(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_sub(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_sub(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_sub(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_half asc_sub(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_sub(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_sub(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_sub(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_float asc_sub(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_sub(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask);

__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask);

__simd_callee__ inline vector_int8_t asc_min(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_min(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_min(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_min(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_min(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_min(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_half asc_min(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_min(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_float asc_min(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_min(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_neg(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_neg(vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_neg(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_neg(vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_neg(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_neg(vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_neg(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline vector_half asc_neg(vector_half src, vector_bool mask);

__simd_callee__ inline void asc_neg(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_float asc_neg(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_abs(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_abs(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_abs(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_abs(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_abs(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_abs(vector_int8_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_abs(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_abs(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_abs(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_abs(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_mull(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_mull(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_half asc_sqrt(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_sqrt(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_sqrt(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_sqrt(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_mul(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_mul(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_half asc_mul(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_mul(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_mul(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_mul(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_float asc_mul(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_mul(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_mul(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_mul_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_add(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_add(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_add(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_add(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_half asc_add(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_float asc_add(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_add(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_add(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_add(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_mul_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_mul_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_half asc_mul_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_mul_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_mul_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_float asc_mul_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_add(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask);

__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_max(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_max(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_max(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_max(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_max(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_max(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_float asc_max(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_half asc_max(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_max(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_max(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_max(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_min_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_half asc_exp(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_exp(vector_float src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_min_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_min_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_min_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_min_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_min_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_min_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_float asc_min_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_half asc_min_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_min_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_exp(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_exp(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_add_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_add_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_add_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_add_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_add_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_half asc_add_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_add_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_add_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_add_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_float asc_add_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_ln(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_ln(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_half asc_div(vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline vector_float asc_div(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_div(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_div(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_div(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_div(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_log(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_log(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_div(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_div(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_div(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_div(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_div(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_div(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_max_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_max_scalar(vector_int8_t src, int8_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_max_scalar(vector_int16_t src, int16_t value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_max_scalar(vector_int32_t src, int32_t value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_max_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_max_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_max_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_half asc_max_scalar(vector_half src, half value, vector_bool mask);

__simd_callee__ inline vector_float asc_max_scalar(vector_float src, float value, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_max_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H
#endif
