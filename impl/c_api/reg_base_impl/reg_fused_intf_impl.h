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
    "impl/c_api/reg_base_impl/reg_fused_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_FUSED_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_FUSED_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/reg_fused_intf_impl.h"
#endif

__simd_callee__ inline void asc_relu(vector_half& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vrelu(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_relu(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vrelu(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_relu(vector_float& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vrelu(dst, src, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vabsdif(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_abs_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vabsdif(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_leakyrelu(vector_float& dst, vector_float src, float alpha, vector_bool mask)
{
    if ASC_IS_AIV {
        vlrelu(dst, src, alpha, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_leakyrelu(vector_half& dst, vector_half src, half alpha, vector_bool mask)
{
    if ASC_IS_AIV {
        vlrelu(dst, src, alpha, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_float asc_leakyrelu(vector_float src, float alpha, vector_bool mask)
{
    vector_float dst;
    asc_leakyrelu(dst, src, alpha, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_leakyrelu(vector_half src, half alpha, vector_bool mask)
{
    vector_half dst;
    asc_leakyrelu(dst, src, alpha, mask);
    return dst;
}

__simd_callee__ inline void asc_mul_scalar_float2half_rn(
    vector_half& dst, vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vmulscvt(dst, src, value, mask, PART_EVEN);
    }
}

__simd_callee__ inline void asc_mul_scalar_float2half_rn(
    vector_half& dst, vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vmulscvt(dst, src, value, mask, PART_ODD);
    }
}

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_EVEN);
    }
}

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_ODD);
    }
}

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_float dst;
    asc_exp_sub_half2float(dst, src0, src1, mask, src_pos);
    return dst;
}

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_float dst;
    asc_exp_sub_half2float(dst, src0, src1, mask, src_pos);
    return dst;
}

__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_EVEN);
    }
}

__simd_callee__ inline vector_float asc_exp_sub(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_exp_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline void asc_prelu(vector_float& dst, vector_float src0, vector_float alpha, vector_bool mask)
{
    if ASC_IS_AIV {
        vprelu(dst, src0, alpha, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_prelu(vector_half& dst, vector_half src0, vector_half alpha, vector_bool mask)
{
    if ASC_IS_AIV {
        vprelu(dst, src0, alpha, mask, MODE_UNKNOWN);
    }
}

__simd_callee__ inline vector_float asc_prelu(vector_float src0, vector_float alpha, vector_bool mask)
{
    vector_float dst;
    asc_prelu(dst, src0, alpha, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_prelu(vector_half src0, vector_half alpha, vector_bool mask)
{
    vector_half dst;
    asc_prelu(dst, src0, alpha, mask);
    return dst;
}

__simd_callee__ inline void asc_madd(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_madd(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_madd(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmadd(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_relu(vector_half src, vector_bool mask)
{
    vector_half dst;
    asc_relu(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_int32_t asc_relu(vector_int32_t src, vector_bool mask)
{
    vector_int32_t dst;
    asc_relu(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_relu(vector_float src, vector_bool mask)
{
    vector_float dst;
    asc_relu(dst, src, mask);
    return dst;
}

__simd_callee__ inline vector_half asc_abs_sub(vector_half src0, vector_half src1, vector_bool mask)
{
    vector_half dst;
    asc_abs_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline vector_float asc_abs_sub(vector_float src0, vector_float src1, vector_bool mask)
{
    vector_float dst;
    asc_abs_sub(dst, src0, src1, mask);
    return dst;
}

__simd_callee__ inline void asc_axpy(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    if ASC_IS_AIV {
        vaxpy(dst, src, value, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_axpy(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vaxpy(dst, src, value, mask, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar_float2half_rn)
__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmulscvt(dst, src, value, mask, PART_EVEN);
    }
}

__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(
    vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_half dst;
    asc_mul_scalar_float2half_rn(dst, src, value, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(
    vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_half dst;
    asc_mul_scalar_float2half_rn(dst, src, value, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_mula(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_mula(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(dst, src0, src1, mask, MODE_ZEROING);
    }
}

__simd_callee__ inline vector_half asc_fma(vector_half src0, vector_half src1, vector_half src2, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(src2, src0, src1, mask, MODE_ZEROING);
    }
    return src2;
}

__simd_callee__ inline vector_bfloat16_t asc_fma(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(src2, src0, src1, mask, MODE_ZEROING);
    }
    return src2;
}

__simd_callee__ inline vector_float asc_fma(vector_float src0, vector_float src1, vector_float src2, vector_bool mask)
{
    if ASC_IS_AIV {
        vmula(src2, src0, src1, mask, MODE_ZEROING);
    }
    return src2;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
