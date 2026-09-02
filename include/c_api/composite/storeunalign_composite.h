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
#error "composite/storeunalign_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_datamove_impl.h"
#endif

__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src);

__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src);

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src);

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src);

__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src);

__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src);

__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src);

__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src);

__simd_callee__ inline void asc_store(__ubuf__ half* dst, vector_half src);

__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src);

__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src);

__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src);

__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src);

__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src);

__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, uint32_t count);

__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#endif
