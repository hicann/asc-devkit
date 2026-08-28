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
#error "compute/reg_gather.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_GATHER_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_GATHER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_datamove_impl.h"
#endif

__simd_callee__ inline vector_int8_t asc_gather(vector_int8_t src, vector_uint8_t index);

__simd_callee__ inline vector_uint8_t asc_gather(vector_uint8_t src, vector_uint8_t index);

__simd_callee__ inline vector_int16_t asc_gather(vector_int16_t src, vector_uint16_t index);

__simd_callee__ inline vector_uint16_t asc_gather(vector_uint16_t src, vector_uint16_t index);

__simd_callee__ inline vector_int32_t asc_gather(vector_int32_t src, vector_uint32_t index);

__simd_callee__ inline vector_uint32_t asc_gather(vector_uint32_t src, vector_uint32_t index);

__simd_callee__ inline vector_float asc_gather(vector_float src, vector_uint32_t index);

__simd_callee__ inline vector_half asc_gather(vector_half src, vector_uint16_t index);

__simd_callee__ inline vector_bfloat16_t asc_gather(vector_bfloat16_t src, vector_uint16_t index);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_gather(vector_fp8_e4m3fn_t src, vector_uint8_t index);

__simd_callee__ inline vector_hifloat8_t asc_gather(vector_hifloat8_t src, vector_uint8_t index);

__simd_callee__ inline vector_fp8_e5m2_t asc_gather(vector_fp8_e5m2_t src, vector_uint8_t index);

__simd_callee__ inline vector_fp8_e8m0_t asc_gather(vector_fp8_e8m0_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_int8_t& dst, vector_int8_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_uint8_t& dst, vector_uint8_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_int16_t& dst, vector_int16_t src, vector_uint16_t index);

__simd_callee__ inline void asc_gather(vector_uint16_t& dst, vector_uint16_t src, vector_uint16_t index);

__simd_callee__ inline void asc_gather(vector_int32_t& dst, vector_int32_t src, vector_uint32_t index);

__simd_callee__ inline void asc_gather(vector_uint32_t& dst, vector_uint32_t src, vector_uint32_t index);

__simd_callee__ inline void asc_gather(vector_float& dst, vector_float src, vector_uint32_t index);

__simd_callee__ inline void asc_gather(vector_half& dst, vector_half src, vector_uint16_t index);

__simd_callee__ inline void asc_gather(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_uint16_t index);

__simd_callee__ inline void asc_gather(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_uint8_t index);

__simd_callee__ inline void asc_gather(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_uint8_t index);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
