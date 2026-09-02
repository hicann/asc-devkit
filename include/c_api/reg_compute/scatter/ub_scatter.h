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
#error "scatter/ub_scatter.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_SCATTER_UB_SCATTER_H
#define INCLUDE_C_API_REG_COMPUTE_SCATTER_UB_SCATTER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_datamove_impl.h"
#endif

[[deprecated("NOTICE: asc_scatter(vector_int8_t& src, __ubuf__ int8_t* dst, vector_uint16_t index, vector_bool mask) "
             "is deprecated. "
             "Please use asc_scatter(__ubuf__ int8_t* dst, vector_int8_t src, vector_uint16_t index, vector_bool mask) "
             "instead.")]] __simd_callee__ inline void
asc_scatter(vector_int8_t& src, __ubuf__ int8_t* dst, vector_uint16_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_uint8_t& src, __ubuf__ uint8_t* dst, vector_uint16_t index, vector_bool mask) "
             "is deprecated. "
             "Please use asc_scatter(__ubuf__ uint8_t* dst, vector_uint8_t src, vector_uint16_t index, vector_bool "
             "mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_uint8_t& src, __ubuf__ uint8_t* dst, vector_uint16_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_int16_t& src, __ubuf__ int16_t* dst, vector_uint16_t index, vector_bool mask) "
             "is deprecated. "
             "Please use asc_scatter(__ubuf__ int16_t* dst, vector_int16_t src, vector_uint16_t index, vector_bool "
             "mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_int16_t& src, __ubuf__ int16_t* dst, vector_uint16_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_uint16_t& src, __ubuf__ uint16_t* dst, vector_uint16_t index, vector_bool "
             "mask) is deprecated. "
             "Please use asc_scatter(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_uint16_t index, vector_bool "
             "mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_uint16_t& src, __ubuf__ uint16_t* dst, vector_uint16_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_int32_t& src, __ubuf__ int32_t* dst, vector_uint32_t index, vector_bool mask) "
             "is deprecated. "
             "Please use asc_scatter(__ubuf__ int32_t* dst, vector_int32_t src, vector_uint32_t index, vector_bool "
             "mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_int32_t& src, __ubuf__ int32_t* dst, vector_uint32_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_uint32_t& src, __ubuf__ uint32_t* dst, vector_uint32_t index, vector_bool "
             "mask) is deprecated. "
             "Please use asc_scatter(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_uint32_t index, vector_bool "
             "mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_uint32_t& src, __ubuf__ uint32_t* dst, vector_uint32_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_bfloat16_t& src, __ubuf__ bfloat16_t* dst, vector_uint16_t index, vector_bool "
             "mask) is deprecated. "
             "Please use asc_scatter(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_uint16_t index, "
             "vector_bool mask) instead.")]] __simd_callee__ inline void
asc_scatter(vector_bfloat16_t& src, __ubuf__ bfloat16_t* dst, vector_uint16_t index, vector_bool mask);

[[deprecated(
    "NOTICE: asc_scatter(vector_half& src, __ubuf__ half* dst, vector_uint16_t index, vector_bool mask) is deprecated. "
    "Please use asc_scatter(__ubuf__ half* dst, vector_half src, vector_uint16_t index, vector_bool mask) "
    "instead.")]] __simd_callee__ inline void
asc_scatter(vector_half& src, __ubuf__ half* dst, vector_uint16_t index, vector_bool mask);

[[deprecated("NOTICE: asc_scatter(vector_float& src, __ubuf__ float* dst, vector_uint32_t index, vector_bool mask) is "
             "deprecated. "
             "Please use asc_scatter(__ubuf__ float* dst, vector_float src, vector_uint32_t index, vector_bool mask) "
             "instead.")]] __simd_callee__ inline void
asc_scatter(vector_float& src, __ubuf__ float* dst, vector_uint32_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ int8_t* dst, vector_int8_t src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ uint8_t* dst, vector_uint8_t src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ int16_t* dst, vector_int16_t src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ uint16_t* dst, vector_uint16_t src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ int32_t* dst, vector_int32_t src, vector_uint32_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, vector_uint32_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(__ubuf__ half* dst, vector_half src, vector_uint16_t index, vector_bool mask);

__simd_callee__ inline void asc_scatter(__ubuf__ float* dst, vector_float src, vector_uint32_t index, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H
#endif
