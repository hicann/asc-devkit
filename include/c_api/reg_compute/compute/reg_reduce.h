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
#error "compute/reg_reduce.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_reduce_max(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_reduce_max(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_max(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_reduce_max(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_max(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_max(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_max(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_reduce_min(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_min(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_reduce_min(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_min(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_min(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_min(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_min_datablock(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_reduce_min_datablock(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_min_datablock(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_reduce_min_datablock(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_min_datablock(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_min_datablock(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_min_datablock(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_pair_reduce_sum(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_pair_reduce_sum(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_half asc_pair_reduce_sum(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_pair_reduce_sum(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_max_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_reduce_max_datablock(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_max_datablock(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_reduce_max_datablock(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_max_datablock(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_max_datablock(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_max_datablock(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_sum(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_sum(vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_sum(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_sum(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_sum(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_sum(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_reduce_sum_datablock(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_reduce_sum_datablock(vector_float src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_sum_datablock(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_sum_datablock(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_reduce_sum_datablock(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_reduce_sum_datablock(vector_int32_t src, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
