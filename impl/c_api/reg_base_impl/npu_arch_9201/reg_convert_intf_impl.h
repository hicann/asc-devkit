/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License).
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/npu_arch_9201/reg_convert_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_CONVERT_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_REG_CONVERT_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

// ==========asc_half2int4x2 merge mode conversion==========
__simd_callee__ inline void asc_half2int4x2_rd_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rd_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline void asc_half2int4x2_rn_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rn_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rna - sat:dis/en
__simd_callee__ inline void asc_half2int4x2_rna_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rna_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ru - sat:dis/en
__simd_callee__ inline void asc_half2int4x2_ru_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_ru_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_half2int4x2_rz_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int4x2_rz_sat_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ==========asc_half2int8 merge mode conversion==========

// rd - sat:dis/en
__simd_callee__ inline void asc_half2int8_rd_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rd_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rd_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rd_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline void asc_half2int8_rn_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rn_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rn_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rn_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rna - sat:dis/en
__simd_callee__ inline void asc_half2int8_rna_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rna_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rna_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rna_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ru - sat:dis/en
__simd_callee__ inline void asc_half2int8_ru_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_ru_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_ru_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_ru_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_half2int8_rz_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rz_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rz_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2int8_rz_sat_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_bfloat162e4m3 conversion==========

// rn - sat:dis/en
__simd_callee__ inline void asc_bfloat162e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline vector_fp8_e4m3fn_t asc_bfloat162e4m3_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_bfloat162e4m3_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_bfloat162e4m3_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_bfloat162e4m3_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_bfloat162e4m3_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_bfloat162e4m3_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_bfloat162e4m3_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_bfloat162e4m3_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
// ==========asc_bfloat162e5m2 conversion==========

// rn - sat:dis/en
__simd_callee__ inline void asc_bfloat162e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_bfloat162e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline vector_fp8_e5m2_t asc_bfloat162e5m2_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_bfloat162e5m2_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_bfloat162e5m2_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_bfloat162e5m2_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_bfloat162e5m2_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_bfloat162e5m2_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_bfloat162e5m2_rn_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_bfloat162e5m2_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
// ==========asc_bfloat162e6m2 conversion==========

// rn - sat:dis
__simd_callee__ inline void asc_bfloat162e6m2_rn(
    vector_fp8_e6m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e6m2(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e6m2_rn(
    vector_fp8_e6m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e6m2(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

// rn - sat:dis
__simd_callee__ inline vector_fp8_e6m2_t asc_bfloat162e6m2_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e6m2_t dst;
    asc_bfloat162e6m2_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e6m2_t asc_bfloat162e6m2_rn(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e6m2_t dst;
    asc_bfloat162e6m2_rn(dst, src, mask, dst_pos);
    return dst;
}

// ==========asc_bfloat162e8m0 conversion==========

// ru - sat:dis/en
__simd_callee__ inline void asc_bfloat162e8m0_ru(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_ru_sat(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_ru(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_ru_sat(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_bfloat162e8m0_rz(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_rz_sat(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_rz(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_bfloat162e8m0_rz_sat(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

// ru - sat:dis/en
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_ru(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_ru(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_ru_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_ru_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_ru(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_ru(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_ru_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_ru_sat(dst, src, mask, dst_pos);
    return dst;
}

// rz - sat:dis/en
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_rz(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_rz(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_rz_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_rz_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_rz(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_rz(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e8m0_t asc_bfloat162e8m0_rz_sat(
    vector_bfloat16_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e8m0_t dst;
    asc_bfloat162e8m0_rz_sat(dst, src, mask, dst_pos);
    return dst;
}

// ==========asc_cast_merge half to int4x2==========

// rd - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_F, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rna - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ru - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_C, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int4x2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_f162s4(dst, src, mask, ROUND_Z, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ==========asc_cast_merge half to int8==========

// rd - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rn - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rna - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ru - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_int8_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_cast/asc_cast_merge bfloat16 to e4m3==========

// rn - sat:dis/en
__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_cast/asc_cast_merge bfloat16 to e5m2==========

// rn - sat:dis/en
__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_cast bfloat16 to e6m2==========

// rn - sat:dis
__simd_callee__ inline void asc_cast(
    vector_fp8_e6m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e6m2(dst, src, mask, ROUND_R, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e6m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e6m2(dst, src, mask, ROUND_R, PART_ODD, MODE_ZEROING);
    }
}

// ==========asc_cast bfloat16 to e8m0==========

// ru - sat:dis/en
__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

// rz - sat:dis/en
__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e8m0_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_bf162e8m0(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

// ================asc_float2half_*_merge================
__simd_callee__ inline void asc_float2half_rd_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rd_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rd_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rd_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rn_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rn_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rn_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rn_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rna_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rna_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rna_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rna_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ro_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ro_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ro_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ro_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ru_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ru_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ru_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_ru_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rz_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rz_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rz_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2half_rz_sat_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ================asc_cast_merge for float2half================
__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RO> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RO> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RO> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_half& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RO> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_O, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ================asc_rcpe6m22bfloat16================
__simd_callee__ inline void asc_rcpe6m22bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_rcpe6m22bf16(dst, src, mask, PART_EVEN);
    }
}

__simd_callee__ inline void asc_rcpe6m22bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt_rcpe6m22bf16(dst, src, mask, PART_ODD);
    }
}

__simd_callee__ inline vector_bfloat16_t asc_rcpe6m22bfloat16(
    vector_fp8_e6m2_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_bfloat16_t dst;
    asc_rcpe6m22bfloat16(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline vector_bfloat16_t asc_rcpe6m22bfloat16(
    vector_fp8_e6m2_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_bfloat16_t dst;
    asc_rcpe6m22bfloat16(dst, src, mask, dst_pos);
    return dst;
}

// ==========9201 PR merging conversion (A6 float/bf16/fp4/hif8)==========
// ---------- asc_bfloat162e1m2x2 *_merge ----------
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_MERGING);
    }
}

// ---------- asc_bfloat162e2m1x2 *_merge ----------
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_MERGING);
    }
}

// ---------- asc_float2bfloat16 *_merge ----------
__simd_callee__ inline void asc_float2bfloat16_rd_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ---------- asc_float2e4m3 *_merge ----------
__simd_callee__ inline void asc_float2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ---------- asc_float2e5m2 *_merge ----------
__simd_callee__ inline void asc_float2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ---------- asc_float2hif8 *_merge ----------
__simd_callee__ inline void asc_float2hif8_rh_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rh_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_float2hif8_rna_sat_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
        dst_quarter_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

// ========== asc_cast_merge (9201 merging, void only) ==========

// ---------- asc_cast_merge PR-only overloads ----------
__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RD> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_F, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RU> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_C, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_bfloat16_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RZ> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_Z, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RNA> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_A, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_DISABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P0, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P1, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P2, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_hifloat8_t& dst, vector_float src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RH> round_mode,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT> saturation_mode,
    std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER> dst_pos)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_H, RS_ENABLE, PART_P3, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_sat(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_half2e4m3_rn_sat_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline vector_fp8_e4m3fn_t asc_half2e4m3_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_half2e4m3_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_half2e4m3_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_half2e4m3_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_half2e4m3_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_half2e4m3_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e4m3fn_t asc_half2e4m3_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e4m3fn_t dst;
    asc_half2e4m3_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_half2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_sat(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}
__simd_callee__ inline void asc_half2e5m2_rn_sat_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline vector_fp8_e5m2_t asc_half2e5m2_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_half2e5m2_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_half2e5m2_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_half2e5m2_rn_sat(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_half2e5m2_rn(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_half2e5m2_rn(dst, src, mask, dst_pos);
    return dst;
}
__simd_callee__ inline vector_fp8_e5m2_t asc_half2e5m2_rn_sat(
    vector_half src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
{
    vector_fp8_e5m2_t dst;
    asc_half2e5m2_rn_sat(dst, src, mask, dst_pos);
    return dst;
}

__simd_callee__ inline void asc_e6m22bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt_e6m22bf16(dst, src, mask, PART_EVEN);
    }
}
__simd_callee__ inline void asc_e6m22bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt_e6m22bf16(dst, src, mask, PART_ODD);
    }
}
__simd_callee__ inline vector_bfloat16_t asc_e6m22bfloat16(
    vector_fp8_e6m2_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_bfloat16_t dst;
    asc_e6m22bfloat16(dst, src, mask, src_pos);
    return dst;
}
__simd_callee__ inline vector_bfloat16_t asc_e6m22bfloat16(
    vector_fp8_e6m2_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_bfloat16_t dst;
    asc_e6m22bfloat16(dst, src, mask, src_pos);
    return dst;
}
__simd_callee__ inline void asc_e8m02bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e8m0_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt_e8m02bf16(dst, src, mask, PART_EVEN);
    }
}
__simd_callee__ inline void asc_e8m02bfloat16(
    vector_bfloat16_t& dst, vector_fp8_e8m0_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt_e8m02bf16(dst, src, mask, PART_ODD);
    }
}
__simd_callee__ inline vector_bfloat16_t asc_e8m02bfloat16(
    vector_fp8_e8m0_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)
{
    vector_bfloat16_t dst;
    asc_e8m02bfloat16(dst, src, mask, src_pos);
    return dst;
}
__simd_callee__ inline vector_bfloat16_t asc_e8m02bfloat16(
    vector_fp8_e8m0_t src, vector_bool mask, std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
{
    vector_bfloat16_t dst;
    asc_e8m02bfloat16(dst, src, mask, src_pos);
    return dst;
}
// ==========asc_cast/asc_cast_merge half to e4m3==========

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e4m3fn_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_cast/asc_cast_merge half to e5m2==========

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_ZEROING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::NO_SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_DISABLE, PART_ODD, MODE_MERGING);
    }
}

__simd_callee__ inline void asc_cast_merge(
    vector_fp8_e5m2_t& dst, vector_half src, vector_bool mask,
    std::integral_constant<asc_cast_round_mode, asc_cast_round_mode::RN>,
    std::integral_constant<asc_cast_saturation_mode, asc_cast_saturation_mode::SAT>,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt(dst, src, mask, ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
    }
}

// ==========asc_cast e6m2/e8m0 to bfloat16==========

__simd_callee__ inline void asc_cast(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt_e6m22bf16(dst, src, mask, PART_EVEN);
    }
}

__simd_callee__ inline void asc_cast(
    vector_bfloat16_t& dst, vector_fp8_e6m2_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt_e6m22bf16(dst, src, mask, PART_ODD);
    }
}

__simd_callee__ inline void asc_cast(
    vector_bfloat16_t& dst, vector_fp8_e8m0_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN>)
{
    if ASC_IS_AIV {
        vcvt_e8m02bf16(dst, src, mask, PART_EVEN);
    }
}

__simd_callee__ inline void asc_cast(
    vector_bfloat16_t& dst, vector_fp8_e8m0_t src, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD>)
{
    if ASC_IS_AIV {
        vcvt_e8m02bf16(dst, src, mask, PART_ODD);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
