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
#error "compute/reg_fused.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_FUSED_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_FUSED_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_relu(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_relu(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_relu(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_abs_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_abs_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_axpy(vector_half& dst, vector_half src, half value, vector_bool mask);

__simd_callee__ inline void asc_axpy(vector_float& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_muls_v2(vector_half& dst, vector_float src, float value, vector_bool mask);

__simd_callee__ inline void asc_madd(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_madd(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_leakyrelu(vector_float& dst, vector_float src, float alpha, vector_bool mask);

__simd_callee__ inline void asc_leakyrelu(vector_half& dst, vector_half src, half alpha, vector_bool mask);

__simd_callee__ inline vector_float asc_leakyrelu(vector_float src, float alpha, vector_bool mask);

__simd_callee__ inline vector_half asc_leakyrelu(vector_half src, half alpha, vector_bool mask);

__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos);

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__simd_callee__ inline vector_float asc_exp_sub(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos);

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos);

#endif

__simd_callee__ inline void asc_prelu(vector_float& dst, vector_float src0, vector_float alpha, vector_bool mask);

__simd_callee__ inline void asc_prelu(vector_half& dst, vector_half src0, vector_half alpha, vector_bool mask);

__simd_callee__ inline vector_float asc_prelu(vector_float src0, vector_float alpha, vector_bool mask);

__simd_callee__ inline vector_half asc_prelu(vector_half src0, vector_half alpha, vector_bool mask);

//=================asc_mula==================
__simd_callee__ inline void asc_mula(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mula(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mula(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_mula(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_mula(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_mula(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_mula(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

//=================asc_fma==================
__simd_callee__ inline vector_half asc_fma(vector_half src0, vector_half src1, vector_half src2, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_fma(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask);

__simd_callee__ inline vector_float asc_fma(vector_float src0, vector_float src1, vector_float src2, vector_bool mask);

[[deprecated("NOTICE: asc_exp_sub with half inputs and without src_pos is deprecated. "
             "Please use asc_exp_sub_half2float with src_pos instead.")]] __simd_callee__ inline void
asc_exp_sub(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask);

[[deprecated("NOTICE: asc_exp_sub_v2 with half inputs is deprecated. "
             "Please use asc_exp_sub_half2float with src_pos instead.")]] __simd_callee__ inline void
asc_exp_sub_v2(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask);

[[deprecated("NOTICE: asc_exp_sub_v2 with float inputs is deprecated. "
             "Please use asc_exp_sub instead.")]] __simd_callee__ inline void
asc_exp_sub_v2(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
