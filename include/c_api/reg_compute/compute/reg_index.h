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
#error "compute/reg_index.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_INDEX_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_INDEX_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif
__simd_callee__ inline vector_int8_t asc_arange(int8_t value);

__simd_callee__ inline vector_int16_t asc_arange(int16_t value);

__simd_callee__ inline vector_int32_t asc_arange(int32_t value);

__simd_callee__ inline vector_half asc_arange(half value);

__simd_callee__ inline vector_float asc_arange(float value);

__simd_callee__ inline vector_int8_t asc_arange_descend(int8_t value);

__simd_callee__ inline vector_int16_t asc_arange_descend(int16_t value);

__simd_callee__ inline vector_int32_t asc_arange_descend(int32_t value);

__simd_callee__ inline vector_half asc_arange_descend(half value);

__simd_callee__ inline vector_float asc_arange_descend(float value);

__simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value);

__simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value);

__simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value);

__simd_callee__ inline void asc_arange(vector_half& dst, half value);

__simd_callee__ inline void asc_arange(vector_float& dst, float value);

__simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value);

__simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value);

__simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value);

__simd_callee__ inline void asc_arange_descend(vector_half& dst, half value);

__simd_callee__ inline void asc_arange_descend(vector_float& dst, float value);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
