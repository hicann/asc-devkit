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
    "impl/c_api/reg_base_impl/npu_arch_3510/reg_convert_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_REG_CONVERT_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_REG_CONVERT_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd)
__simd_callee__ inline void asc_float2half_rd(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd_sat)
__simd_callee__ inline void asc_float2half_rd_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd)
__simd_callee__ inline void asc_float2half_rd_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd_sat)
__simd_callee__ inline void asc_float2half_rd_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru)
__simd_callee__ inline void asc_float2half_ru(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru_sat)
__simd_callee__ inline void asc_float2half_ru_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru)
__simd_callee__ inline void asc_float2half_ru_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru_sat)
__simd_callee__ inline void asc_float2half_ru_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz)
__simd_callee__ inline void asc_float2half_rz(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz_sat)
__simd_callee__ inline void asc_float2half_rz_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz)
__simd_callee__ inline void asc_float2half_rz_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz_sat)
__simd_callee__ inline void asc_float2half_rz_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn)
__simd_callee__ inline void asc_float2half_rn(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn_sat)
__simd_callee__ inline void asc_float2half_rn_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn)
__simd_callee__ inline void asc_float2half_rn_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn_sat)
__simd_callee__ inline void asc_float2half_rn_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna)
__simd_callee__ inline void asc_float2half_rna(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna_sat)
__simd_callee__ inline void asc_float2half_rna_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna)
__simd_callee__ inline void asc_float2half_rna_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna_sat)
__simd_callee__ inline void asc_float2half_rna_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro)
__simd_callee__ inline void asc_float2half_ro(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro_sat)
__simd_callee__ inline void asc_float2half_ro_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro)
__simd_callee__ inline void asc_float2half_ro_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro_sat)
__simd_callee__ inline void asc_float2half_ro_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322int16)
__simd_callee__ inline void asc_uint322int16(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322int16_sat)
__simd_callee__ inline void asc_uint322int16_sat(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322int16)
__simd_callee__ inline void asc_uint322int16_v2(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322int16_sat)
__simd_callee__ inline void asc_uint322int16_sat_v2(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e4m32float)
__simd_callee__ inline void asc_e4m32float(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e4m32float)
__simd_callee__ inline void asc_e4m32float_v2(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e4m32float)
__simd_callee__ inline void asc_e4m32float_v3(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e4m32float)
__simd_callee__ inline void asc_e4m32float_v4(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162float)
__simd_callee__ inline void asc_int162float(vector_float& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162float)
__simd_callee__ inline void asc_int162float_v2(vector_float& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22int16)
__simd_callee__ inline void asc_int4x22int16_v2(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42s16(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22int16)
__simd_callee__ inline void asc_int4x22int16_v3(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42s16(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22int16)
__simd_callee__ inline void asc_int4x22int16_v4(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42s16(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16)
__simd_callee__ inline void asc_int322int16(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16_sat)
__simd_callee__ inline void asc_int322int16_sat(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16)
__simd_callee__ inline void asc_int322int16_v2(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16_sat)
__simd_callee__ inline void asc_int322int16_sat_v2(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8)
__simd_callee__ inline void asc_int322uint8(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8_sat)
__simd_callee__ inline void asc_int322uint8_sat(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8)
__simd_callee__ inline void asc_int322uint8_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8_sat)
__simd_callee__ inline void asc_int322uint8_sat_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8)
__simd_callee__ inline void asc_int322uint8_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8_sat)
__simd_callee__ inline void asc_int322uint8_sat_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8)
__simd_callee__ inline void asc_int322uint8_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint8_sat)
__simd_callee__ inline void asc_int322uint8_sat_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd)
__simd_callee__ inline void asc_half2int8_rd(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd_sat)
__simd_callee__ inline void asc_half2int8_rd_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd)
__simd_callee__ inline void asc_half2int8_rd_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd_sat)
__simd_callee__ inline void asc_half2int8_rd_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru)
__simd_callee__ inline void asc_half2int8_ru(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru_sat)
__simd_callee__ inline void asc_half2int8_ru_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru)
__simd_callee__ inline void asc_half2int8_ru_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru_sat)
__simd_callee__ inline void asc_half2int8_ru_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz)
__simd_callee__ inline void asc_half2int8_rz(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz_sat)
__simd_callee__ inline void asc_half2int8_rz_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz)
__simd_callee__ inline void asc_half2int8_rz_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz_sat)
__simd_callee__ inline void asc_half2int8_rz_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn)
__simd_callee__ inline void asc_half2int8_rn(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn_sat)
__simd_callee__ inline void asc_half2int8_rn_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn)
__simd_callee__ inline void asc_half2int8_rn_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn_sat)
__simd_callee__ inline void asc_half2int8_rn_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna)
__simd_callee__ inline void asc_half2int8_rna(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna_sat)
__simd_callee__ inline void asc_half2int8_rna_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna)
__simd_callee__ inline void asc_half2int8_rna_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna_sat)
__simd_callee__ inline void asc_half2int8_rna_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rh)
__simd_callee__ inline void asc_half2hif8_rh(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rh_sat)
__simd_callee__ inline void asc_half2hif8_rh_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rh)
__simd_callee__ inline void asc_half2hif8_rh_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rh_sat)
__simd_callee__ inline void asc_half2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rna)
__simd_callee__ inline void asc_half2hif8_rna(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rna_sat)
__simd_callee__ inline void asc_half2hif8_rna_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rna)
__simd_callee__ inline void asc_half2hif8_rna_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2hif8_rna_sat)
__simd_callee__ inline void asc_half2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162float)
__simd_callee__ inline void asc_bfloat162float(vector_float& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162float)
__simd_callee__ inline void asc_bfloat162float_v2(vector_float& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22bfloat16)
__simd_callee__ inline void asc_int4x22bfloat16_v2(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42bf16(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22bfloat16)
__simd_callee__ inline void asc_int4x22bfloat16_v3(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42bf16(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22bfloat16)
__simd_callee__ inline void asc_int4x22bfloat16_v4(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42bf16(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint32)
__simd_callee__ inline void asc_int162uint32(vector_uint32_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint32)
__simd_callee__ inline void asc_int162uint32_v2(vector_uint32_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint8)
__simd_callee__ inline void asc_uint162uint8(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint8_sat)
__simd_callee__ inline void asc_uint162uint8_sat(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint8)
__simd_callee__ inline void asc_uint162uint8_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint8_sat)
__simd_callee__ inline void asc_uint162uint8_sat_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn)
__simd_callee__ inline void asc_bfloat162int32_rn(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn_sat)
__simd_callee__ inline void asc_bfloat162int32_rn_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn)
__simd_callee__ inline void asc_bfloat162int32_rn_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn_sat)
__simd_callee__ inline void asc_bfloat162int32_rn_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna)
__simd_callee__ inline void asc_bfloat162int32_rna(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna_sat)
__simd_callee__ inline void asc_bfloat162int32_rna_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna)
__simd_callee__ inline void asc_bfloat162int32_rna_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna_sat)
__simd_callee__ inline void asc_bfloat162int32_rna_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd)
__simd_callee__ inline void asc_bfloat162int32_rd(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd_sat)
__simd_callee__ inline void asc_bfloat162int32_rd_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd)
__simd_callee__ inline void asc_bfloat162int32_rd_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd_sat)
__simd_callee__ inline void asc_bfloat162int32_rd_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru)
__simd_callee__ inline void asc_bfloat162int32_ru(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru_sat)
__simd_callee__ inline void asc_bfloat162int32_ru_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru)
__simd_callee__ inline void asc_bfloat162int32_ru_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru_sat)
__simd_callee__ inline void asc_bfloat162int32_ru_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz)
__simd_callee__ inline void asc_bfloat162int32_rz(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz_sat)
__simd_callee__ inline void asc_bfloat162int32_rz_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz)
__simd_callee__ inline void asc_bfloat162int32_rz_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz_sat)
__simd_callee__ inline void asc_bfloat162int32_rz_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rn)
__simd_callee__ inline void asc_int642float_rn(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rn)
__simd_callee__ inline void asc_int642float_rn_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rna)
__simd_callee__ inline void asc_int642float_rna(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rna)
__simd_callee__ inline void asc_int642float_rna_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rd)
__simd_callee__ inline void asc_int642float_rd(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rd)
__simd_callee__ inline void asc_int642float_rd_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_ru)
__simd_callee__ inline void asc_int642float_ru(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_ru)
__simd_callee__ inline void asc_int642float_ru_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rz)
__simd_callee__ inline void asc_int642float_rz(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rz)
__simd_callee__ inline void asc_int642float_rz_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82half)
__simd_callee__ inline void asc_int82half(vector_half& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82half)
__simd_callee__ inline void asc_int82half_v2(vector_half& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162int32)
__simd_callee__ inline void asc_int162int32(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162int32)
__simd_callee__ inline void asc_int162int32_v2(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8)
__simd_callee__ inline void asc_uint322uint8(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8_sat)
__simd_callee__ inline void asc_uint322uint8_sat(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8)
__simd_callee__ inline void asc_uint322uint8_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8_sat)
__simd_callee__ inline void asc_uint322uint8_sat_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8)
__simd_callee__ inline void asc_uint322uint8_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8_sat)
__simd_callee__ inline void asc_uint322uint8_sat_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8)
__simd_callee__ inline void asc_uint322uint8_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint8_sat)
__simd_callee__ inline void asc_uint322uint8_sat_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82half)
__simd_callee__ inline void asc_hif82half(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82half)
__simd_callee__ inline void asc_hif82half_v2(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd)
__simd_callee__ inline void asc_half2int4x2_rd_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd_sat)
__simd_callee__ inline void asc_half2int4x2_rd_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd)
__simd_callee__ inline void asc_half2int4x2_rd_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd_sat)
__simd_callee__ inline void asc_half2int4x2_rd_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd)
__simd_callee__ inline void asc_half2int4x2_rd_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rd_sat)
__simd_callee__ inline void asc_half2int4x2_rd_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru)
__simd_callee__ inline void asc_half2int4x2_ru_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru_sat)
__simd_callee__ inline void asc_half2int4x2_ru_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru)
__simd_callee__ inline void asc_half2int4x2_ru_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru_sat)
__simd_callee__ inline void asc_half2int4x2_ru_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru)
__simd_callee__ inline void asc_half2int4x2_ru_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_ru_sat)
__simd_callee__ inline void asc_half2int4x2_ru_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz)
__simd_callee__ inline void asc_half2int4x2_rz_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz_sat)
__simd_callee__ inline void asc_half2int4x2_rz_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz)
__simd_callee__ inline void asc_half2int4x2_rz_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz_sat)
__simd_callee__ inline void asc_half2int4x2_rz_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz)
__simd_callee__ inline void asc_half2int4x2_rz_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rz_sat)
__simd_callee__ inline void asc_half2int4x2_rz_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn)
__simd_callee__ inline void asc_half2int4x2_rn_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn_sat)
__simd_callee__ inline void asc_half2int4x2_rn_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn)
__simd_callee__ inline void asc_half2int4x2_rn_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn_sat)
__simd_callee__ inline void asc_half2int4x2_rn_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn)
__simd_callee__ inline void asc_half2int4x2_rn_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rn_sat)
__simd_callee__ inline void asc_half2int4x2_rn_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna)
__simd_callee__ inline void asc_half2int4x2_rna_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna_sat)
__simd_callee__ inline void asc_half2int4x2_rna_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna)
__simd_callee__ inline void asc_half2int4x2_rna_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna_sat)
__simd_callee__ inline void asc_half2int4x2_rna_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna)
__simd_callee__ inline void asc_half2int4x2_rna_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4x2_rna_sat)
__simd_callee__ inline void asc_half2int4x2_rna_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82half)
__simd_callee__ inline void asc_uint82half(vector_half& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82half)
__simd_callee__ inline void asc_uint82half_v2(vector_half& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint32)
__simd_callee__ inline void asc_uint162uint32(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint162uint32)
__simd_callee__ inline void asc_uint162uint32_v2(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82float)
__simd_callee__ inline void asc_hif82float(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82float)
__simd_callee__ inline void asc_hif82float_v2(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82float)
__simd_callee__ inline void asc_hif82float_v3(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_hif82float)
__simd_callee__ inline void asc_hif82float_v4(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd)
__simd_callee__ inline void asc_half2uint8_rd(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd_sat)
__simd_callee__ inline void asc_half2uint8_rd_sat(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd)
__simd_callee__ inline void asc_half2uint8_rd_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd_sat)
__simd_callee__ inline void asc_half2uint8_rd_sat_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn)
__simd_callee__ inline void asc_half2uint8_rn(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn_sat)
__simd_callee__ inline void asc_half2uint8_rn_sat(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn)
__simd_callee__ inline void asc_half2uint8_rn_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn_sat)
__simd_callee__ inline void asc_half2uint8_rn_sat_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna)
__simd_callee__ inline void asc_half2uint8_rna(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna_sat)
__simd_callee__ inline void asc_half2uint8_rna_sat(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna)
__simd_callee__ inline void asc_half2uint8_rna_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna_sat)
__simd_callee__ inline void asc_half2uint8_rna_sat_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru)
__simd_callee__ inline void asc_half2uint8_ru(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru_sat)
__simd_callee__ inline void asc_half2uint8_ru_sat(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru)
__simd_callee__ inline void asc_half2uint8_ru_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru_sat)
__simd_callee__ inline void asc_half2uint8_ru_sat_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz)
__simd_callee__ inline void asc_half2uint8_rz(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz_sat)
__simd_callee__ inline void asc_half2uint8_rz_sat(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz)
__simd_callee__ inline void asc_half2uint8_rz_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz_sat)
__simd_callee__ inline void asc_half2uint8_rz_sat_v2(vector_uint8_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22half)
__simd_callee__ inline void asc_int4x22half_v2(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42f16(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22half)
__simd_callee__ inline void asc_int4x22half_v3(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42f16(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int4x22half)
__simd_callee__ inline void asc_int4x22half_v4(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt_s42f16(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int16)
__simd_callee__ inline void asc_int82int16(vector_int16_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int16)
__simd_callee__ inline void asc_int82int16_v2(vector_int16_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint8)
__simd_callee__ inline void asc_int162uint8(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint8_sat)
__simd_callee__ inline void asc_int162uint8_sat(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint8)
__simd_callee__ inline void asc_int162uint8_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162uint8_sat)
__simd_callee__ inline void asc_int162uint8_sat_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd)
__simd_callee__ inline void asc_float2int16_rd(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd_sat)
__simd_callee__ inline void asc_float2int16_rd_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd)
__simd_callee__ inline void asc_float2int16_rd_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd_sat)
__simd_callee__ inline void asc_float2int16_rd_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn)
__simd_callee__ inline void asc_float2int16_rn(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn_sat)
__simd_callee__ inline void asc_float2int16_rn_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn)
__simd_callee__ inline void asc_float2int16_rn_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn_sat)
__simd_callee__ inline void asc_float2int16_rn_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna)
__simd_callee__ inline void asc_float2int16_rna(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna_sat)
__simd_callee__ inline void asc_float2int16_rna_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna)
__simd_callee__ inline void asc_float2int16_rna_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna_sat)
__simd_callee__ inline void asc_float2int16_rna_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru)
__simd_callee__ inline void asc_float2int16_ru(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru_sat)
__simd_callee__ inline void asc_float2int16_ru_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru)
__simd_callee__ inline void asc_float2int16_ru_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru_sat)
__simd_callee__ inline void asc_float2int16_ru_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz)
__simd_callee__ inline void asc_float2int16_rz(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz_sat)
__simd_callee__ inline void asc_float2int16_rz_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz)
__simd_callee__ inline void asc_float2int16_rz_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz_sat)
__simd_callee__ inline void asc_float2int16_rz_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd)
__simd_callee__ inline void asc_float2int64_rd(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd_sat)
__simd_callee__ inline void asc_float2int64_rd_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd)
__simd_callee__ inline void asc_float2int64_rd_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd_sat)
__simd_callee__ inline void asc_float2int64_rd_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn)
__simd_callee__ inline void asc_float2int64_rn(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn_sat)
__simd_callee__ inline void asc_float2int64_rn_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn)
__simd_callee__ inline void asc_float2int64_rn_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn_sat)
__simd_callee__ inline void asc_float2int64_rn_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna)
__simd_callee__ inline void asc_float2int64_rna(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna_sat)
__simd_callee__ inline void asc_float2int64_rna_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna)
__simd_callee__ inline void asc_float2int64_rna_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna_sat)
__simd_callee__ inline void asc_float2int64_rna_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru)
__simd_callee__ inline void asc_float2int64_ru(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru_sat)
__simd_callee__ inline void asc_float2int64_ru_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru)
__simd_callee__ inline void asc_float2int64_ru_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru_sat)
__simd_callee__ inline void asc_float2int64_ru_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz)
__simd_callee__ inline void asc_float2int64_rz(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz_sat)
__simd_callee__ inline void asc_float2int64_rz_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz)
__simd_callee__ inline void asc_float2int64_rz_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz_sat)
__simd_callee__ inline void asc_float2int64_rz_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rd)
__simd_callee__ inline void asc_bfloat162e2m1x2_rd(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rd)
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rd)
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rd)
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rn)
__simd_callee__ inline void asc_bfloat162e2m1x2_rn(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rn)
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rn)
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rn)
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rna)
__simd_callee__ inline void asc_bfloat162e2m1x2_rna(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rna)
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v2(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rna)
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v3(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rna)
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v4(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_ru)
__simd_callee__ inline void asc_bfloat162e2m1x2_ru(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_ru)
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_ru)
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_ru)
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rz)
__simd_callee__ inline void asc_bfloat162e2m1x2_rz(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rz)
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rz)
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e2m1x2_rz)
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh_sat)
__simd_callee__ inline void asc_float2hif8_rh_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh)
__simd_callee__ inline void asc_float2hif8_rh(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh_sat)
__simd_callee__ inline void asc_float2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh)
__simd_callee__ inline void asc_float2hif8_rh_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh_sat)
__simd_callee__ inline void asc_float2hif8_rh_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh)
__simd_callee__ inline void asc_float2hif8_rh_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh_sat)
__simd_callee__ inline void asc_float2hif8_rh_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rh)
__simd_callee__ inline void asc_float2hif8_rh_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna_sat)
__simd_callee__ inline void asc_float2hif8_rna_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna)
__simd_callee__ inline void asc_float2hif8_rna(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna_sat)
__simd_callee__ inline void asc_float2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna)
__simd_callee__ inline void asc_float2hif8_rna_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna_sat)
__simd_callee__ inline void asc_float2hif8_rna_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna)
__simd_callee__ inline void asc_float2hif8_rna_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna_sat)
__simd_callee__ inline void asc_float2hif8_rna_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2hif8_rna)
__simd_callee__ inline void asc_float2hif8_rna_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint32)
__simd_callee__ inline void asc_uint82uint32(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint32)
__simd_callee__ inline void asc_uint82uint32_v2(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint32)
__simd_callee__ inline void asc_uint82uint32_v3(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint32)
__simd_callee__ inline void asc_uint82uint32_v4(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint16_sat)
__simd_callee__ inline void asc_uint322uint16_sat(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint16)
__simd_callee__ inline void asc_uint322uint16(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint16_sat)
__simd_callee__ inline void asc_uint322uint16_sat_v2(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint322uint16)
__simd_callee__ inline void asc_uint322uint16_v2(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e2m1x22bfloat16)
__simd_callee__ inline void asc_e2m1x22bfloat16(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e2m1x22bfloat16)
__simd_callee__ inline void asc_e2m1x22bfloat16_v2(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e2m1x22bfloat16)
__simd_callee__ inline void asc_e2m1x22bfloat16_v3(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e2m1x22bfloat16)
__simd_callee__ inline void asc_e2m1x22bfloat16_v4(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd)
__simd_callee__ inline void asc_float2bfloat16_rd(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd_sat)
__simd_callee__ inline void asc_float2bfloat16_rd_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd)
__simd_callee__ inline void asc_float2bfloat16_rd_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd_sat)
__simd_callee__ inline void asc_float2bfloat16_rd_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn)
__simd_callee__ inline void asc_float2bfloat16_rn(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn_sat)
__simd_callee__ inline void asc_float2bfloat16_rn_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn)
__simd_callee__ inline void asc_float2bfloat16_rn_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn_sat)
__simd_callee__ inline void asc_float2bfloat16_rn_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna)
__simd_callee__ inline void asc_float2bfloat16_rna(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna_sat)
__simd_callee__ inline void asc_float2bfloat16_rna_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna)
__simd_callee__ inline void asc_float2bfloat16_rna_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna_sat)
__simd_callee__ inline void asc_float2bfloat16_rna_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru)
__simd_callee__ inline void asc_float2bfloat16_ru(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru_sat)
__simd_callee__ inline void asc_float2bfloat16_ru_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru)
__simd_callee__ inline void asc_float2bfloat16_ru_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru_sat)
__simd_callee__ inline void asc_float2bfloat16_ru_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz)
__simd_callee__ inline void asc_float2bfloat16_rz(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz_sat)
__simd_callee__ inline void asc_float2bfloat16_rz_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz)
__simd_callee__ inline void asc_float2bfloat16_rz_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz_sat)
__simd_callee__ inline void asc_float2bfloat16_rz_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn)
__simd_callee__ inline void asc_float2e5m2_rn(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn)
__simd_callee__ inline void asc_float2e5m2_rn_v2(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn)
__simd_callee__ inline void asc_float2e5m2_rn_v3(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn)
__simd_callee__ inline void asc_float2e5m2_rn_v4(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn_sat)
__simd_callee__ inline void asc_float2e5m2_rn_sat(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn_sat)
__simd_callee__ inline void asc_float2e5m2_rn_sat_v2(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn_sat)
__simd_callee__ inline void asc_float2e5m2_rn_sat_v3(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e5m2_rn_sat)
__simd_callee__ inline void asc_float2e5m2_rn_sat_v4(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2float)
__simd_callee__ inline void asc_half2float(vector_float& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2float)
__simd_callee__ inline void asc_half2float_v2(vector_float& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rd)
__simd_callee__ inline void asc_half2int32_rd(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rd)
__simd_callee__ inline void asc_half2int32_rd_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rn)
__simd_callee__ inline void asc_half2int32_rn(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rn)
__simd_callee__ inline void asc_half2int32_rn_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rna)
__simd_callee__ inline void asc_half2int32_rna(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rna)
__simd_callee__ inline void asc_half2int32_rna_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_ru)
__simd_callee__ inline void asc_half2int32_ru(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_ru)
__simd_callee__ inline void asc_half2int32_ru_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rz)
__simd_callee__ inline void asc_half2int32_rz(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rz)
__simd_callee__ inline void asc_half2int32_rz_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e5m22float)
__simd_callee__ inline void asc_e5m22float(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e5m22float)
__simd_callee__ inline void asc_e5m22float_v2(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e5m22float)
__simd_callee__ inline void asc_e5m22float_v3(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e5m22float)
__simd_callee__ inline void asc_e5m22float_v4(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int64)
__simd_callee__ inline void asc_int322int64(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int64)
__simd_callee__ inline void asc_int322int64_v2(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint16)
__simd_callee__ inline void asc_int322uint16(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint16_sat)
__simd_callee__ inline void asc_int322uint16_sat(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint16)
__simd_callee__ inline void asc_int322uint16_v2(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322uint16_sat)
__simd_callee__ inline void asc_int322uint16_sat_v2(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32)
__simd_callee__ inline void asc_int642int32(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32_sat)
__simd_callee__ inline void asc_int642int32_sat(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32)
__simd_callee__ inline void asc_int642int32_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32_sat)
__simd_callee__ inline void asc_int642int32_sat_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint16)
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_EVEN, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82uint16)
__simd_callee__ inline void asc_uint82uint16_v2(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_ODD, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn_sat)
__simd_callee__ inline void asc_float2e4m3_rn_sat(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn)
__simd_callee__ inline void asc_float2e4m3_rn(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn_sat)
__simd_callee__ inline void asc_float2e4m3_rn_sat_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn)
__simd_callee__ inline void asc_float2e4m3_rn_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn_sat)
__simd_callee__ inline void asc_float2e4m3_rn_sat_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn)
__simd_callee__ inline void asc_float2e4m3_rn_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn_sat)
__simd_callee__ inline void asc_float2e4m3_rn_sat_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2e4m3_rn)
__simd_callee__ inline void asc_float2e4m3_rn_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rd)
__simd_callee__ inline void asc_bfloat162e1m2x2_rd(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rd)
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rd)
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rd)
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rn)
__simd_callee__ inline void asc_bfloat162e1m2x2_rn(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rn)
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rn)
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rn)
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rna)
__simd_callee__ inline void asc_bfloat162e1m2x2_rna(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rna)
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v2(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rna)
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v3(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rna)
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v4(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_ru)
__simd_callee__ inline void asc_bfloat162e1m2x2_ru(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_ru)
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_ru)
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_ru)
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rz)
__simd_callee__ inline void asc_bfloat162e1m2x2_rz(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rz)
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rz)
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162e1m2x2_rz)
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e1m2x22bfloat16)
__simd_callee__ inline void asc_e1m2x22bfloat16(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e1m2x22bfloat16)
__simd_callee__ inline void asc_e1m2x22bfloat16_v2(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e1m2x22bfloat16)
__simd_callee__ inline void asc_e1m2x22bfloat16_v3(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_e1m2x22bfloat16)
__simd_callee__ inline void asc_e1m2x22bfloat16_v4(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int32)
__simd_callee__ inline void asc_int82int32(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P0, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int32)
__simd_callee__ inline void asc_int82int32_v2(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P1, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int32)
__simd_callee__ inline void asc_int82int32_v3(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P2, MODE_ZEROING);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82int32)
__simd_callee__ inline void asc_int82int32_v4(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, PART_P3, MODE_ZEROING);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
