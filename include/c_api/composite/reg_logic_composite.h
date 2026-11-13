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
#error "composite/reg_logic_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_REG_LOGIC_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_REG_LOGIC_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_REG_LOGIC_COMPOSITE_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_logic_composite_intf_impl.h"
#endif

__simd_callee__ inline void asc_shiftleft(
    vector_uint64_t& dst, vector_uint64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline void asc_shiftleft(
    vector_int64_t& dst, vector_int64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_uint64_t asc_shiftleft(vector_uint64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_int64_t asc_shiftleft(vector_int64_t src0, vector_uint32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint64_t& dst, vector_uint64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline void asc_shiftright(
    vector_int64_t& dst, vector_int64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_uint64_t asc_shiftright(vector_uint64_t src0, vector_uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_int64_t asc_shiftright(vector_int64_t src0, vector_uint32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint64_t& dst, vector_uint64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int64_t& dst, vector_int64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_uint64_t asc_shiftleft_scalar(vector_uint64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_int64_t asc_shiftleft_scalar(vector_int64_t src, uint32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint64_t& dst, vector_uint64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline void asc_shiftright_scalar(
    vector_int64_t& dst, vector_int64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_uint64_t asc_shiftright_scalar(vector_uint64_t src, uint32_t shift, vector_bool mask);
__simd_callee__ inline vector_int64_t asc_shiftright_scalar(vector_int64_t src, uint32_t shift, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_REG_LOGIC_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_REG_LOGIC_COMPOSITE_H
#endif
