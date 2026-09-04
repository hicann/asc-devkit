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
#error "compute/reg_logic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#endif

__simd_callee__ inline void asc_and(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_and(vector_bool src0, vector_bool src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_and(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_and(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_and(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_and(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_and(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_and(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_and(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftleft_scalar(vector_uint8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftleft_scalar(vector_int8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftleft_scalar(vector_uint16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftleft_scalar(vector_int16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftleft_scalar(vector_uint32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftleft_scalar(vector_int32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftright_scalar(vector_uint8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftright_scalar(vector_int8_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftright_scalar(vector_uint16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftright_scalar(vector_int16_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftright_scalar(vector_uint32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftright_scalar(vector_int32_t src, int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_or(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_or(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_or(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_or(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_or(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_or(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_or(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_or(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_or(vector_bool src0, vector_bool src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftleft(vector_uint8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftleft(vector_int8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftleft(vector_uint16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftleft(vector_int16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftleft(vector_uint32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftleft(vector_int32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftright(vector_uint8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftright(vector_int8_t src, vector_int8_t shift, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftright(vector_uint16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftright(vector_int16_t src, vector_int16_t shift, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftright(vector_uint32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftright(vector_int32_t src, vector_int32_t shift, vector_bool mask);

__simd_callee__ inline void asc_not(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_not(vector_uint8_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_not(vector_int8_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_not(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_not(vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline vector_half asc_not(vector_half src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_not(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_not(vector_int32_t src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_float asc_not(vector_float src, vector_bool mask);

__simd_callee__ inline void asc_not(vector_bool& dst, vector_bool src, vector_bool mask);

__simd_callee__ inline vector_bool asc_not(vector_bool src, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_xor(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_xor(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_xor(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_xor(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_xor(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_xor(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline vector_bool asc_xor(vector_bool src0, vector_bool src1, vector_bool mask);

[[deprecated("NOTICE: asc_and floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_and(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask);

[[deprecated("NOTICE: asc_and floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_and(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask);

[[deprecated("NOTICE: asc_and floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_and(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

[[deprecated("NOTICE: asc_and floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_and(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

[[deprecated("NOTICE: asc_and floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_and(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

[[deprecated("NOTICE: asc_or floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_or(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

[[deprecated("NOTICE: asc_or floating-point overloads are deprecated since 9.2.0. Floating-point data types are not "
             "supported; use the same-width integer overload only when bitwise interpretation is intended.")]]
__simd_callee__ inline void asc_or(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#endif
