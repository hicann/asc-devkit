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
    "impl/c_api/reg_base_impl/reg_convert_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_CONVERT_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_CONVERT_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/reg_convert_intf_impl.h"
#endif

__simd_callee__ inline void asc_float2half_rd(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rd(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rd_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rd_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2half_rn(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rn(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rn_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rn_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2half_rna(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rna(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rna_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rna_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2half_ro(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ro(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ro_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ro_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_ro(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_ro(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ro(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_ro(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ro_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_ro_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ro_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_ro_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2half_ru(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ru(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ru_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_ru_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2half_rz(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rz(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rz_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2half_rz_sat(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_float2half_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_float2half_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_float2half_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_float2half_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162half_rd(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, ROUND_F, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rd_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, ROUND_F, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rn(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rn_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rna(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, ROUND_A, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rna_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, ROUND_A, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_ru(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_ru_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rz(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, ROUND_Z, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162half_rz_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, ROUND_Z, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322int16(
    vector_int16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322int16(
    vector_int16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322int16_sat(
    vector_int16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322int16_sat(
    vector_int16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_uint322int16(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_uint322int16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_uint322int16(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_uint322int16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_uint322int16_sat(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_uint322int16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_uint322int16_sat(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_uint322int16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2bfloat16_rd(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2bfloat16_rn(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2bfloat16_rna(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2bfloat16_ru(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2bfloat16_rz(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e5m2_t asc_float2e5m2_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e5m2_t dst;
    asc_float2e5m2_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_half2float(
    vector_float& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2float(
    vector_float& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_half2float(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_float dst;
    asc_half2float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_half2float(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_float dst;
    asc_half2float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_e4m32float(
    vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e4m32float(
    vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e4m32float(
    vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e4m32float(
    vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_e4m32float(
    vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e4m32float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e4m32float(
    vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e4m32float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e4m32float(
    vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e4m32float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e4m32float(
    vector_fp8_e4m3fn_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e4m32float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_int162float(
    vector_float& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162float(
    vector_float& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int162float(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_float dst;
    asc_int162float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int162float(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_float dst;
    asc_int162float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_int322int16(
    vector_int16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322int16(
    vector_int16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322int16_sat(
    vector_int16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322int16_sat(
    vector_int16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_int322int16(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_int322int16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_int322int16(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_int322int16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_int322int16_sat(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_int322int16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_int322int16_sat(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_int322int16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int322uint8(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8_sat(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8_sat(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8_sat(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint8_sat(
    vector_uint8_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_int322uint8(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8_sat(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8_sat(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8_sat(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int322uint8_sat(
    vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_int322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162float(
    vector_float& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162float(
    vector_float& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_bfloat162float(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_float dst;
    asc_bfloat162float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_bfloat162float(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_float dst;
    asc_bfloat162float(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2hif8_rh(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rh(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rna(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rna(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rh(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rh(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rh(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rh(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rh_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rh_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rh_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rh_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_half2hif8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_hifloat8_t dst;
    asc_half2hif8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int8_rd(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rd(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rd_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rd_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rn(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rn(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rn_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rn_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rna(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rna(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rna_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rna_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_ru(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_ru(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_ru_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_ru_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rz(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rz(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rz_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int8_rz_sat(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int8_t asc_half2int8_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rd_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rd_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_ru_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_ru_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rz_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int8_t asc_half2int8_rz_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int8_t dst;
    asc_half2int8_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int162uint32(
    vector_uint32_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162uint32(
    vector_uint32_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint32_t asc_int162uint32(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_uint32_t dst;
    asc_int162uint32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_int162uint32(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_uint32_t dst;
    asc_int162uint32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_uint162uint8(
    vector_uint8_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint162uint8(
    vector_uint8_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint162uint8_sat(
    vector_uint8_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint162uint8_sat(
    vector_uint8_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_uint162uint8(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_uint162uint8(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint162uint8(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_uint162uint8(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint162uint8_sat(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_uint162uint8_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint162uint8_sat(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_uint162uint8_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_hif82float(
    vector_float& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_hif82float(
    vector_float& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_hif82float(
    vector_float& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_hif82float(
    vector_float& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_hif82float(
    vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_hif82float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_hif82float(
    vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_hif82float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_hif82float(
    vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_hif82float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_hif82float(
    vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_hif82float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_hif82half(
    vector_half& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_hif82half(
    vector_half& dst, vector_hifloat8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_hif82half(
    vector_hifloat8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_half dst;
    asc_hif82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_hif82half(
    vector_hifloat8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_half dst;
    asc_hif82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_uint82half(
    vector_half& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82half(
    vector_half& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_uint82half(
    vector_uint8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_half dst;
    asc_uint82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_uint82half(
    vector_uint8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_half dst;
    asc_uint82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_uint162uint32(
    vector_uint32_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint162uint32(
    vector_uint32_t& dst, vector_uint16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint32_t asc_uint162uint32(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_uint32_t dst;
    asc_uint162uint32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_uint162uint32(
    vector_uint16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_uint32_t dst;
    asc_uint162uint32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2uint8_rd(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rd(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rd_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rd_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rd_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rd_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2uint8_rn(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rn(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rn_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rn_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2uint8_rna(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rna(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rna_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rna_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rna_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2uint8_ru(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_ru(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_ru_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_ru_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_ru_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_ru_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2uint8_rz(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rz(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rz_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2uint8_rz_sat(
    vector_uint8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rz_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_half2uint8_rz_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_half2uint8_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int82int16(
    vector_int16_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int82int16(
    vector_int16_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_int82int16(
    vector_int8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int16_t dst;
    asc_int82int16(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_int82int16(
    vector_int8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int16_t dst;
    asc_int82int16(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_int162uint8(
    vector_uint8_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162uint8(
    vector_uint8_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162uint8_sat(
    vector_uint8_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162uint8_sat(
    vector_uint8_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_int162uint8(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_int162uint8(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int162uint8(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_int162uint8(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int162uint8_sat(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint8_t dst;
    asc_int162uint8_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_int162uint8_sat(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint8_t dst;
    asc_int162uint8_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162int32_rn(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rn(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rn_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rn_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rn_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rn_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162int32_rna(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rna(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rna_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rna_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rna(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rna(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rna_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rna_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rna_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rna_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162int32_rd(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rd(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rd_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rd_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rd(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rd(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rd_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rd_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rd_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rd_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162int32_ru(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_ru(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_ru_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_ru_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_ru(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_ru(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_ru_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_ru_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_ru_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_ru_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162int32_rz(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rz(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rz_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162int32_rz_sat(
    vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rz(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rz(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rz_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rz_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_bfloat162int32_rz_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_bfloat162int32_rz_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int16_rn(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rn_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rna(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rna_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rd(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rd_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_ru(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_ru_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rz(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int16_rz_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_rn(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_rn(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int642float_rn(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_float dst;
    asc_int642float_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int642float_rn(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_float dst;
    asc_int642float_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int642float_rna(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_rna(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int642float_rna(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_float dst;
    asc_int642float_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int642float_rna(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_float dst;
    asc_int642float_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int642float_rd(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_rd(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int642float_rd(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_float dst;
    asc_int642float_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int642float_rd(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_float dst;
    asc_int642float_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int642float_ru(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_ru(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int642float_ru(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_float dst;
    asc_int642float_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int642float_ru(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_float dst;
    asc_int642float_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int642float_rz(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642float_rz(
    vector_float& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_int642float_rz(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_float dst;
    asc_int642float_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_int642float_rz(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_float dst;
    asc_int642float_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int82half(
    vector_half& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int82half(
    vector_half& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_int82half(
    vector_int8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_half dst;
    asc_int82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_int82half(
    vector_int8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_half dst;
    asc_int82half(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_int162int32(
    vector_int32_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162int32(
    vector_int32_t& dst, vector_int16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_int162int32(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_int162int32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int162int32(
    vector_int16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_int162int32(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_uint322uint8(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8_sat(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8_sat(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8_sat(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint8_sat(
    vector_uint8_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8_sat(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8_sat(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8_sat(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint8_t asc_uint322uint8_sat(
    vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_uint8_t dst;
    asc_uint322uint8_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int16_rd(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rd(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rd_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rd_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_float2int16_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int16_rn(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rn(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rn_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rn_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_float2int16_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int16_rna(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rna(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rna_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rna_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_float2int16_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int16_ru(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_ru(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_ru_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_ru_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_float2int16_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int16_rz(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rz(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rz_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int16_rz_sat(
    vector_int16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_float2int16_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_float2int16_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int16_t dst;
    asc_float2int16_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e2m1_t asc_bfloat162e2m1x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e2m1_t dst;
    asc_bfloat162e2m1x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_float2hif8_rh(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rh_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rh_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_float2hif8_rna(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_hifloat8_t asc_float2hif8_rna_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_hifloat8_t dst;
    asc_float2hif8_rna_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_ceil(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_C, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_ceil(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_C, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_ceil(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_C, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e2m1x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e2m1x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e2m1x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e2m1x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(
    vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e2m1x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(
    vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e2m1x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(
    vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e2m1x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e2m1x22bfloat16(
    vector_fp4x2_e2m1_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e2m1x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_floor(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_F, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_floor(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_F, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_floor(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_F, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_rint(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_R, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_rint(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_R, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_rint(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_R, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_round(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_A, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_round(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_A, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_round(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_A, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_trunc(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_Z, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_trunc(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_Z, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_trunc(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vtrc(dst, src, ROUND_Z, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82uint32(
    vector_uint32_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82uint32(
    vector_uint32_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82uint32(
    vector_uint32_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82uint32(
    vector_uint32_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint32_t asc_uint82uint32(
    vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_uint32_t dst;
    asc_uint82uint32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_uint82uint32(
    vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_uint32_t dst;
    asc_uint82uint32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_uint82uint32(
    vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_uint32_t dst;
    asc_uint82uint32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_uint32_t asc_uint82uint32(
    vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_uint32_t dst;
    asc_uint82uint32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_uint322uint16(
    vector_uint16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint16(
    vector_uint16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint16_sat(
    vector_uint16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint322uint16_sat(
    vector_uint16_t& dst, vector_uint32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint16_t asc_uint322uint16(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint16_t dst;
    asc_uint322uint16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_uint322uint16(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint16_t dst;
    asc_uint322uint16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_uint322uint16_sat(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint16_t dst;
    asc_uint322uint16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_uint322uint16_sat(
    vector_uint32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint16_t dst;
    asc_uint322uint16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2bfloat16_rd(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rd(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rd_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2bfloat16_rn(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rn(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2bfloat16_rna(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rna(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rna_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2bfloat16_ru(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_ru(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2bfloat16_rz(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rz(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_float2bfloat16_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int64_rd(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rd(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rd_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rd_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_float2int64_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rd(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rd_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rd_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rd_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int64_rn(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rn(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rn_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rn_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_float2int64_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rn(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rn_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rn_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rn_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int64_rna(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rna(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rna_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rna_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_float2int64_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rna(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rna_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rna_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rna_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int64_ru(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_ru(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_ru_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_ru_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_float2int64_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_ru(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_ru_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_ru_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_ru_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_float2int64_rz(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rz(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rz_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int64_rz_sat(
    vector_int64_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_float2int64_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rz(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rz_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_float2int64_rz_sat(
    vector_float src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_float2int64_rz_sat(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int32_rd(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int32_rd(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_half2int32_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_half2int32_rd(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rd(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int32_rn(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int32_rn(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_half2int32_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_half2int32_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rn(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int32_rna(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int32_rna(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_half2int32_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_half2int32_rna(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rna(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int32_ru(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int32_ru(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_half2int32_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_half2int32_ru(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_ru(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_half2int32_rz(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int32_rz(
    vector_int32_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_half2int32_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_half2int32_rz(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int32_t dst;
    asc_half2int32_rz(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_e5m22float(
    vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e5m22float(
    vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e5m22float(
    vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e5m22float(
    vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_e5m22float(
    vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e5m22float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e5m22float(
    vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e5m22float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e5m22float(
    vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e5m22float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_e5m22float(
    vector_fp8_e5m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_float dst;
    asc_e5m22float(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_float2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_float2e4m3_rn_sat(
    vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_float2e4m3_rn_sat(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_int322int64(
    vector_int64_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322int64(
    vector_int64_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int64_t asc_int322int64(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_int64_t dst;
    asc_int322int64(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_int64_t asc_int322int64(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_int64_t dst;
    asc_int322int64(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_int322uint16(
    vector_uint16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint16(
    vector_uint16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint16_sat(
    vector_uint16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322uint16_sat(
    vector_uint16_t& dst, vector_int32_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint16_t asc_int322uint16(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint16_t dst;
    asc_int322uint16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_int322uint16(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint16_t dst;
    asc_int322uint16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_int322uint16_sat(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_uint16_t dst;
    asc_int322uint16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_int322uint16_sat(
    vector_int32_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_uint16_t dst;
    asc_int322uint16_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_int642int32(
    vector_int32_t& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642int32(
    vector_int32_t& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642int32_sat(
    vector_int32_t& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int642int32_sat(
    vector_int32_t& dst, vector_int64_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_int642int32(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int32_t dst;
    asc_int642int32(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int642int32(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int32_t dst;
    asc_int642int32(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int642int32_sat(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_int32_t dst;
    asc_int642int32_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int642int32_sat(
    vector_int64_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_int32_t dst;
    asc_int642int32_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_uint82uint16(
    vector_uint16_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_uint82uint16(
    vector_uint16_t& dst, vector_uint8_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_uint16_t asc_uint82uint16(
    vector_uint8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_uint16_t dst;
    asc_uint82uint16(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_uint16_t asc_uint82uint16(
    vector_uint8_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_uint16_t dst;
    asc_uint82uint16(dst, src, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rd(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rd(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rn(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rn(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rna(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rna(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_ru(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_ru(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_fp4x2_e1m2_t asc_bfloat162e1m2x2_rz(
    vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    vector_fp4x2_e1m2_t dst;
    asc_bfloat162e1m2x2_rz(dst, src, mask, dst_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_e1m2x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e1m2x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e1m2x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_e1m2x22bfloat16(
    vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_e1m2x22bfloat16(
    vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e1m2x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e1m2x22bfloat16(
    vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e1m2x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e1m2x22bfloat16(
    vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e1m2x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_e1m2x22bfloat16(
    vector_fp4x2_e1m2_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_bfloat16_t dst;
    asc_e1m2x22bfloat16(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline void asc_int82int32(
    vector_int32_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int82int32(
    vector_int32_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int82int32(
    vector_int32_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int82int32(
    vector_int32_t& dst, vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int32_t asc_int82int32(
    vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        src_quarter_pos)
{
    vector_int32_t dst;
    asc_int82int32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int82int32(
    vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        src_quarter_pos)
{
    vector_int32_t dst;
    asc_int82int32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int82int32(
    vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        src_quarter_pos)
{
    vector_int32_t dst;
    asc_int82int32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_int82int32(
    vector_int8_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        src_quarter_pos)
{
    vector_int32_t dst;
    asc_int82int32(dst, src, mask, src_quarter_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_half2bfloat16_rd(vector_half src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_half2bfloat16_rd(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_half2bfloat16_rn(vector_half src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_half2bfloat16_rn(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_half2bfloat16_rna(vector_half src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_half2bfloat16_rna(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_half2bfloat16_ru(vector_half src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_half2bfloat16_ru(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_half2bfloat16_rz(vector_half src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_half2bfloat16_rz(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rd(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rd(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rd_sat(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rd_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_ru(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_ru(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_ru_sat(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_ru_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rz(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rz(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rz_sat(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rz_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rn(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rn(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rn_sat(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rn_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rna(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rna(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_bfloat162half_rna_sat(vector_bfloat16_t src, vector_bool mask)
{
    vector_half dst;
    asc_bfloat162half_rna_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline void asc_int4x22int16(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42s16(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int4x22bfloat16(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42bf16(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322float_rd(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322float_ru(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322float_rz(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322float_rn(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int322float_rna(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_int16_t asc_half2int16_rn(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rn(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rn_sat(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rn_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rna(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rna(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rna_sat(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rna_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rd(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rd(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rd_sat(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rd_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_ru(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_ru(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_ru_sat(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_ru_sat(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rz(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rz(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int16_t asc_half2int16_rz_sat(vector_half src, vector_bool mask)
{
    vector_int16_t dst;
    asc_half2int16_rz_sat(dst, src, mask);
    return dst;
}

// rd - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rd(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int4x2_rd_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

// ru - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_ru(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int4x2_ru_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

// rz - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rz(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int4x2_rz_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

// rn - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rn(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int4x2_rn_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

// rna - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rna(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2int4x2_rna_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162half_rd(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162half_rn(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162half_rna(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162half_ru(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int162half_rz(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_int4x22half(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42f16(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rd(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rd_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_ru(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_ru_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rz(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rz_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rn(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rn_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rna(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_float2int32_rna_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_ceil(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_ceil(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_ceil(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_ceil(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_ceil(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_ceil(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_floor(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_floor(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_floor(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_floor(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_floor(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_floor(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_rint(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_rint(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_rint(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_rint(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_rint(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_rint(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_round(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_round(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_round(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_round(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_round(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_round(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_trunc(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_trunc(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_trunc(vector_bfloat16_t src, vector_bool mask)
{
    vector_bfloat16_t dst;
    asc_trunc(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_trunc(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_trunc(dst, src, mask);
    return dst;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
