/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/reg_logic_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_LOGIC_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_LOGIC_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/reg_logic_intf_impl.h"
#endif

__simd_callee__ inline void asc_and(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    if ASC_IS_AIV {
        pand(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_and(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_and(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_and(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_and(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_and(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_and(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vand(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshls(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_shiftleft_scalar(vector_uint8_t src, int16_t shift, vector_bool mask)
{
    vector_uint8_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_shiftleft_scalar(vector_int8_t src, int16_t shift, vector_bool mask)
{
    vector_int8_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_shiftleft_scalar(vector_uint16_t src, int16_t shift, vector_bool mask)
{
    vector_uint16_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_shiftleft_scalar(vector_int16_t src, int16_t shift, vector_bool mask)
{
    vector_int16_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_shiftleft_scalar(vector_uint32_t src, int16_t shift, vector_bool mask)
{
    vector_uint32_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_shiftleft_scalar(vector_int32_t src, int16_t shift, vector_bool mask)
{
    vector_int32_t dst;
    asc_shiftleft_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshrs(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_shiftright_scalar(vector_uint8_t src, int16_t shift, vector_bool mask)
{
    vector_uint8_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_shiftright_scalar(vector_int8_t src, int16_t shift, vector_bool mask)
{
    vector_int8_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_shiftright_scalar(vector_uint16_t src, int16_t shift, vector_bool mask)
{
    vector_uint16_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_shiftright_scalar(vector_int16_t src, int16_t shift, vector_bool mask)
{
    vector_int16_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_shiftright_scalar(vector_uint32_t src, int16_t shift, vector_bool mask)
{
    vector_uint32_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_shiftright_scalar(vector_int32_t src, int16_t shift, vector_bool mask)
{
    vector_int32_t dst;
    asc_shiftright_scalar(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftleft(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftleft(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshl(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_shiftleft(vector_uint8_t src, vector_int8_t shift, vector_bool mask)
{
    vector_uint8_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_shiftleft(vector_int8_t src, vector_int8_t shift, vector_bool mask)
{
    vector_int8_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_shiftleft(vector_uint16_t src, vector_int16_t shift, vector_bool mask)
{
    vector_uint16_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_shiftleft(vector_int16_t src, vector_int16_t shift, vector_bool mask)
{
    vector_int16_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_shiftleft(vector_uint32_t src, vector_int32_t shift, vector_bool mask)
{
    vector_uint32_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_shiftleft(vector_int32_t src, vector_int32_t shift, vector_bool mask)
{
    vector_int32_t dst;
    asc_shiftleft(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_shiftright(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_shiftright(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask)
{
    if ASC_IS_AIV {
        vshr(dst, src, shift, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_shiftright(vector_uint8_t src, vector_int8_t shift, vector_bool mask)
{
    vector_uint8_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_shiftright(vector_int8_t src, vector_int8_t shift, vector_bool mask)
{
    vector_int8_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_shiftright(vector_uint16_t src, vector_int16_t shift, vector_bool mask)
{
    vector_uint16_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_shiftright(vector_int16_t src, vector_int16_t shift, vector_bool mask)
{
    vector_int16_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_shiftright(vector_uint32_t src, vector_int32_t shift, vector_bool mask)
{
    vector_uint32_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_shiftright(vector_int32_t src, vector_int32_t shift, vector_bool mask)
{
    vector_int32_t dst;
    asc_shiftright(dst, src, shift, mask);
    return dst;
}

__simd_callee__ inline void asc_not(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vnot(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_not(vector_bool& dst, vector_bool src, vector_bool mask)
{
    if ASC_IS_AIV {
        pnot(dst, src, mask);
    }
}

__simd_callee__ inline void asc_or(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vor(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_or(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    if ASC_IS_AIV {
        por(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vxor(dst, src0, src1, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    if ASC_IS_AIV {
        pxor(dst, src0, src1, mask);
    }
}

__simd_callee__ inline vector_uint8_t asc_not(vector_uint8_t src, vector_bool mask)
{
    vector_uint8_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_not(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_not(vector_uint16_t src, vector_bool mask)
{
    vector_uint16_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_not(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_not(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_not(vector_uint32_t src, vector_bool mask)
{
    vector_uint32_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_not(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_not(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_not(vector_bool src, vector_bool mask)
{
    vector_bool dst;
    asc_not(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_and(vector_bool src0, vector_bool src1, vector_bool mask)
{
    vector_bool dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_and(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_and(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_and(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_and(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_and(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_and(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_and(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_or(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_or(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_or(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_or(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_or(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_or(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_or(vector_bool src0, vector_bool src1, vector_bool mask)
{
    vector_bool dst;
    asc_or(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_xor(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_xor(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_xor(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_xor(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_xor(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_xor(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bool asc_xor(vector_bool src0, vector_bool src1, vector_bool mask)
{
    vector_bool dst;
    asc_xor(dst, src0, src1, mask);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
