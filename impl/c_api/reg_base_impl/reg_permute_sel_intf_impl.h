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
    "impl/c_api/reg_base_impl/reg_permute_sel_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_PERMUTE_SEL_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_PERMUTE_SEL_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/reg_permute_sel_intf_impl.h"
#endif

__simd_callee__ inline void asc_select(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    if ASC_IS_AIV {
        psel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(
            reinterpret_cast<vector_uint8_t&>(dst), *reinterpret_cast<vector_uint8_t*>(&src0),
            *reinterpret_cast<vector_uint8_t*>(&src1), mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_select(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vsel(dst, src0, src1, mask);
    }
}

__simd_callee__ inline void asc_squeeze(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src, mask, MODE_NO_STORED);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask)
{
    if ASC_IS_AIV {
        vusqz(dst, mask);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_low(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        ppack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint8_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint8_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint16_t& dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_uint16_t& dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_to_high(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        ppack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vunpack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        punpack(dst, src, HIGHER);
    }
}

__simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        punpack(dst, src, LOWER);
    }
}

__simd_callee__ inline vector_bool asc_select(vector_bool src0, vector_bool src1, vector_bool mask)
{
    vector_bool dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_select(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    vector_uint8_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_select(vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    vector_int8_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_select(
    vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask)
{
    vector_fp8_e4m3fn_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_select(vector_hifloat8_t src0, vector_hifloat8_t src1, vector_bool mask)
{
    vector_hifloat8_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_select(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask)
{
    vector_fp8_e5m2_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_select(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    vector_uint16_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_select(vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    vector_int16_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_select(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_select(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_select(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    vector_uint32_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_select(vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    vector_int32_t dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_select(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_select(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_squeeze(vector_uint8_t src, vector_bool mask)
{
    vector_uint8_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_squeeze(vector_int8_t src, vector_bool mask)
{
    vector_int8_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_squeeze(vector_uint16_t src, vector_bool mask)
{
    vector_uint16_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_squeeze(vector_int16_t src, vector_bool mask)
{
    vector_int16_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_squeeze(vector_uint32_t src, vector_bool mask)
{
    vector_uint32_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_squeeze(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_squeeze(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_squeeze(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_squeeze(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_squeeze(vector_hifloat8_t src, vector_bool mask)
{
    vector_hifloat8_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_squeeze(vector_fp8_e4m3fn_t src, vector_bool mask)
{
    vector_fp8_e4m3fn_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_squeeze(vector_fp8_e5m2_t src, vector_bool mask)
{
    vector_fp8_e5m2_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_fp8_e8m0_t asc_squeeze(vector_fp8_e8m0_t src, vector_bool mask)
{
    vector_fp8_e8m0_t dst;
    asc_squeeze(dst, src, mask);
    return dst;
}

__simd_callee__ inline void asc_intlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlv_b8(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlv_b16(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pintlv_b32(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_intlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vintlv(
            reinterpret_cast<vector_uint8_t&>(dst0), reinterpret_cast<vector_uint8_t&>(dst1),
            *reinterpret_cast<vector_uint8_t*>(&src0), *reinterpret_cast<vector_uint8_t*>(&src1));
    }
}

__simd_callee__ inline void asc_intlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline vector_uint8_t asc_unsqueeze_u8(vector_bool mask)
{
    vector_uint8_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_unsqueeze_s8(vector_bool mask)
{
    vector_int8_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_unsqueeze_u16(vector_bool mask)
{
    vector_uint16_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_unsqueeze_s16(vector_bool mask)
{
    vector_int16_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_unsqueeze_u32(vector_bool mask)
{
    vector_uint32_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_unsqueeze_s32(vector_bool mask)
{
    vector_int32_t dst;
    asc_unsqueeze(dst, mask);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_pack_to_low(vector_uint16_t src)
{
    vector_uint8_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_pack_to_low(vector_int16_t src)
{
    vector_uint8_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_pack_to_low(vector_uint32_t src)
{
    vector_uint16_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_pack_to_low(vector_int32_t src)
{
    vector_uint16_t dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_bool asc_pack_to_low(vector_bool src)
{
    vector_bool dst;
    asc_pack_to_low(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_pack_to_high(vector_uint16_t src)
{
    vector_uint8_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_pack_to_high(vector_int16_t src)
{
    vector_uint8_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_pack_to_high(vector_uint32_t src)
{
    vector_uint16_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_pack_to_high(vector_int32_t src)
{
    vector_uint16_t dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline vector_bool asc_pack_to_high(vector_bool src)
{
    vector_bool dst;
    asc_pack_to_high(dst, src);
    return dst;
}

__simd_callee__ inline void asc_deintlv(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    if ASC_IS_AIV {
        vdintlv(
            reinterpret_cast<vector_uint8_t&>(dst0), reinterpret_cast<vector_uint8_t&>(dst1),
            *reinterpret_cast<vector_uint8_t*>(&src0), *reinterpret_cast<vector_uint8_t*>(&src1));
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
{
    if ASC_IS_AIV {
        vdintlv(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlv_b8(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlv_b16(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    if ASC_IS_AIV {
        pdintlv_b32(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline vector_uint16_t asc_unpack_upper(vector_uint8_t src)
{
    vector_uint16_t dst;
    asc_unpack_upper(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_unpack_lower(vector_uint8_t src)
{
    vector_uint16_t dst;
    asc_unpack_lower(dst, src);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_unpack_upper(vector_int8_t src)
{
    vector_int16_t dst;
    asc_unpack_upper(dst, src);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_unpack_lower(vector_int8_t src)
{
    vector_int16_t dst;
    asc_unpack_lower(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_unpack_upper(vector_uint16_t src)
{
    vector_uint32_t dst;
    asc_unpack_upper(dst, src);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_unpack_lower(vector_uint16_t src)
{
    vector_uint32_t dst;
    asc_unpack_lower(dst, src);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_unpack_upper(vector_int16_t src)
{
    vector_int32_t dst;
    asc_unpack_upper(dst, src);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_unpack_lower(vector_int16_t src)
{
    vector_int32_t dst;
    asc_unpack_lower(dst, src);
    return dst;
}

__simd_callee__ inline vector_bool asc_unpack_upper(vector_bool src)
{
    vector_bool dst;
    asc_unpack_upper(dst, src);
    return dst;
}

__simd_callee__ inline vector_bool asc_unpack_lower(vector_bool src)
{
    vector_bool dst;
    asc_unpack_lower(dst, src);
    return dst;
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_init()
{
    if ASC_IS_AIV {
        sprclr(SPR_AR);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int8_t* dst_addr, vector_int8_t& dst, vector_store_unalign& src0, vector_int8_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint8_t* dst_addr, vector_uint8_t& dst, vector_store_unalign& src0, vector_uint8_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int16_t* dst_addr, vector_int16_t& dst, vector_store_unalign& src0, vector_int16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint16_t* dst_addr, vector_uint16_t& dst, vector_store_unalign& src0, vector_uint16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int32_t* dst_addr, vector_int32_t& dst, vector_store_unalign& src0, vector_int32_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint32_t* dst_addr, vector_uint32_t& dst, vector_store_unalign& src0, vector_uint32_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ half* dst_addr, vector_half& dst, vector_store_unalign& src0, vector_half src1, vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ float* dst_addr, vector_float& dst, vector_store_unalign& src0, vector_float src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ bfloat16_t* dst_addr, vector_bfloat16_t& dst, vector_store_unalign& src0, vector_bfloat16_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e4m3fn_t* dst_addr, vector_fp8_e4m3fn_t& dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e5m2_t* dst_addr, vector_fp8_e5m2_t& dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e8m0_t* dst_addr, vector_fp8_e8m0_t& dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz(dst, src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp4x2_e2m1_t* dst_addr, vector_fp4x2_e2m1_t& dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp4x2_e1m2_t* dst_addr, vector_fp4x2_e1m2_t& dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        vstur(src0, dst, dst_addr, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int4b_t* dst_addr, vector_int4x2_t& dst, vector_store_unalign& src0, vector_int4x2_t src1,
    vector_bool squeeze_mask)
{
    if ASC_IS_AIV {
        vsqz((vector_int8_t&)dst, (vector_int8_t&)src1, squeeze_mask, MODE_STORED);
        __ubuf__ float4_e1m2x2_t*& dst_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*&>(dst_addr);
        vstur(src0, *reinterpret_cast<vector_fp4x2_e1m2_t*>(&dst), dst_tmp, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int8_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint8_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int4b_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        __ubuf__ float4_e1m2x2_t* dst_tmp = reinterpret_cast<__ubuf__ float4_e1m2x2_t*>(dst);
        vstar(src, dst_tmp);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int16_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint16_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ half* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ bfloat16_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int32_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint32_t* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ float* dst, vector_store_unalign src)
{
    if ASC_IS_AIV {
        vstar(src, dst);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
