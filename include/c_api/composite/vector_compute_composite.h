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
#error "composite/vector_compute_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/vector_compute_impl.h"
#include "impl/c_api/instr_impl/npu_arch_2201/vector_datamove_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/vector_datamove_impl.h"
#endif
ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_brcb and asc_sync)
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_brcb and asc_sync)
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_sum and asc_sync)
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_sum and asc_sync)
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_max and asc_sync)
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_max and asc_sync)
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_min and asc_sync)
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_datablock_reduce_min and asc_sync)
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_sum and asc_sync)
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_sum and asc_sync)
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_max_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_repeat_reduce_min_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162float and asc_sync)
__aicore__ inline void asc_bfloat162float_sync(__ubuf__ float* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162int32_rna and asc_sync)
__aicore__ inline void asc_bfloat162int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162int32_ru and asc_sync)
__aicore__ inline void asc_bfloat162int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162int32_rd and asc_sync)
__aicore__ inline void asc_bfloat162int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162int32_rn and asc_sync)
__aicore__ inline void asc_bfloat162int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_bfloat162int32_rz and asc_sync)
__aicore__ inline void asc_bfloat162int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2bfloat16_rn and asc_sync)
__aicore__ inline void asc_float2bfloat16_rn_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2bfloat16_rna and asc_sync)
__aicore__ inline void asc_float2bfloat16_rna_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2bfloat16_rd and asc_sync)
__aicore__ inline void asc_float2bfloat16_rd_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2bfloat16_ru and asc_sync)
__aicore__ inline void asc_float2bfloat16_ru_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2bfloat16_rz and asc_sync)
__aicore__ inline void asc_float2bfloat16_rz_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2float_rn and asc_sync)
__aicore__ inline void asc_float2float_rn_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2float_rd and asc_sync)
__aicore__ inline void asc_float2float_rd_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2float_ru and asc_sync)
__aicore__ inline void asc_float2float_ru_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2float_rna and asc_sync)
__aicore__ inline void asc_float2float_rna_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2float_rz and asc_sync)
__aicore__ inline void asc_float2float_rz_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half and asc_sync)
__aicore__ inline void asc_float2half_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_rn and asc_sync)
__aicore__ inline void asc_float2half_rn_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_rna and asc_sync)
__aicore__ inline void asc_float2half_rna_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_rd and asc_sync)
__aicore__ inline void asc_float2half_rd_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_ru and asc_sync)
__aicore__ inline void asc_float2half_ru_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_rz and asc_sync)
__aicore__ inline void asc_float2half_rz_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2half_ro and asc_sync)
__aicore__ inline void asc_float2half_ro_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int32_rna and asc_sync)
__aicore__ inline void asc_float2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int32_ru and asc_sync)
__aicore__ inline void asc_float2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int32_rd and asc_sync)
__aicore__ inline void asc_float2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int32_rn and asc_sync)
__aicore__ inline void asc_float2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int32_rz and asc_sync)
__aicore__ inline void asc_float2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int64_rna and asc_sync)
__aicore__ inline void asc_float2int64_rna_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int64_ru and asc_sync)
__aicore__ inline void asc_float2int64_ru_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int64_rd and asc_sync)
__aicore__ inline void asc_float2int64_rd_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int64_rn and asc_sync)
__aicore__ inline void asc_float2int64_rn_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int64_rz and asc_sync)
__aicore__ inline void asc_float2int64_rz_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2float and asc_sync)
__aicore__ inline void asc_half2float_sync(__ubuf__ float* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4 and asc_sync)
__aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4_rna and asc_sync)
__aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4_ru and asc_sync)
__aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4_rd and asc_sync)
__aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4_rn and asc_sync)
__aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int4_rz and asc_sync)
__aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8 and asc_sync)
__aicore__ inline void asc_half2int8_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8_rna and asc_sync)
__aicore__ inline void asc_half2int8_rna_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8_ru and asc_sync)
__aicore__ inline void asc_half2int8_ru_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8_rd and asc_sync)
__aicore__ inline void asc_half2int8_rd_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8_rn and asc_sync)
__aicore__ inline void asc_half2int8_rn_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int8_rz and asc_sync)
__aicore__ inline void asc_half2int8_rz_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int16_rna and asc_sync)
__aicore__ inline void asc_half2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int16_ru and asc_sync)
__aicore__ inline void asc_half2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int16_rd and asc_sync)
__aicore__ inline void asc_half2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int16_rn and asc_sync)
__aicore__ inline void asc_half2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int16_rz and asc_sync)
__aicore__ inline void asc_half2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int32_rna and asc_sync)
__aicore__ inline void asc_half2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int32_ru and asc_sync)
__aicore__ inline void asc_half2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int32_rd and asc_sync)
__aicore__ inline void asc_half2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int32_rn and asc_sync)
__aicore__ inline void asc_half2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2int32_rz and asc_sync)
__aicore__ inline void asc_half2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_div and asc_sync)
__aicore__ inline void asc_div_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_div and asc_sync)
__aicore__ inline void asc_div_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ half* dst, half src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ int16_t* dst, int16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ float* dst, float src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ int32_t* dst, int32_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_exp and asc_sync)
__aicore__ inline void asc_exp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_exp and asc_sync)
__aicore__ inline void asc_exp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_log and asc_sync)
__aicore__ inline void asc_log_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_log and asc_sync)
__aicore__ inline void asc_log_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt_scalar and asc_sync)
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt_scalar and asc_sync)
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_abs and asc_sync)
__aicore__ inline void asc_abs_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_abs and asc_sync)
__aicore__ inline void asc_abs_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_cast_half2int8 and asc_sync)
__aicore__ inline void asc_mul_cast_half2int8_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_cast_half2uint8 and asc_sync)
__aicore__ inline void asc_mul_cast_half2uint8_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge_scalar and asc_sync)
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge_scalar and asc_sync)
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_deq_int322half and asc_sync)
__aicore__ inline void asc_deq_int322half_sync(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_select and asc_sync)
__aicore__ inline void asc_select_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_select and asc_sync)
__aicore__ inline void asc_select_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_or and asc_sync)
__aicore__ inline void asc_or_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_or and asc_sync)
__aicore__ inline void asc_or_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_not and asc_sync)
__aicore__ inline void asc_not_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_not and asc_sync)
__aicore__ inline void asc_not_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_rcp and asc_sync)
__aicore__ inline void asc_rcp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_rcp and asc_sync)
__aicore__ inline void asc_rcp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright_round and asc_sync)
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_shiftright_round and asc_sync)
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_add and asc_sync)
__aicore__ inline void asc_mul_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_add and asc_sync)
__aicore__ inline void asc_mul_add_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_add_relu and asc_sync)
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_mul_add_relu and asc_sync)
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_deq_int162b8_h and asc_sync)
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_deq_int162b8_h and asc_sync)
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_deq_int162b8_l and asc_sync)
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_deq_int162b8_l and asc_sync)
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int42half and asc_sync)
__aicore__ inline void asc_int42half_sync(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int82half and asc_sync)
__aicore__ inline void asc_int82half_sync(__ubuf__ half* dst, __ubuf__ int8_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_uint82half and asc_sync)
__aicore__ inline void asc_uint82half_sync(__ubuf__ half* dst, __ubuf__ uint8_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162float and asc_sync)
__aicore__ inline void asc_int162float_sync(__ubuf__ float* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float and asc_sync)
__aicore__ inline void asc_int322float_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float_rna and asc_sync)
__aicore__ inline void asc_int322float_rna_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float_ru and asc_sync)
__aicore__ inline void asc_int322float_ru_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float_rd and asc_sync)
__aicore__ inline void asc_int322float_rd_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float_rn and asc_sync)
__aicore__ inline void asc_int322float_rn_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322float_rz and asc_sync)
__aicore__ inline void asc_int322float_rz_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322int16 and asc_sync)
__aicore__ inline void asc_int322int16_sync(__ubuf__ int16_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int322int64 and asc_sync)
__aicore__ inline void asc_int322int64_sync(__ubuf__ int64_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642int32 and asc_sync)
__aicore__ inline void asc_int642int32_sync(__ubuf__ int32_t* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_and and asc_sync)
__aicore__ inline void asc_and_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_and and asc_sync)
__aicore__ inline void asc_and_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_leakyrelu and asc_sync)
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_leakyrelu and asc_sync)
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt_scalar and asc_sync)
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt_scalar and asc_sync)
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int16_rna and asc_sync)
__aicore__ inline void asc_float2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int16_ru and asc_sync)
__aicore__ inline void asc_float2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int16_rd and asc_sync)
__aicore__ inline void asc_float2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int16_rn and asc_sync)
__aicore__ inline void asc_float2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_float2int16_rz and asc_sync)
__aicore__ inline void asc_float2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_rsqrt and asc_sync)
__aicore__ inline void asc_rsqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_rsqrt and asc_sync)
__aicore__ inline void asc_rsqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_reduce and asc_sync)
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_reduce and asc_sync)
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642float_rna and asc_sync)
__aicore__ inline void asc_int642float_rna_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642float_ru and asc_sync)
__aicore__ inline void asc_int642float_ru_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642float_rd and asc_sync)
__aicore__ inline void asc_int642float_rd_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642float_rn and asc_sync)
__aicore__ inline void asc_int642float_rn_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int642float_rz and asc_sync)
__aicore__ inline void asc_int642float_rz_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_vdeq_int162b8_h and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_vdeq_int162b8_h and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_vdeq_int162b8_l and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_vdeq_int162b8_l and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sqrt and asc_sync)
__aicore__ inline void asc_sqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_sqrt and asc_sync)
__aicore__ inline void asc_sqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne_scalar and asc_sync)
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_ne_scalar and asc_sync)
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half and asc_sync)
__aicore__ inline void asc_int162half_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half_rna and asc_sync)
__aicore__ inline void asc_int162half_rna_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half_ru and asc_sync)
__aicore__ inline void asc_int162half_ru_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half_rd and asc_sync)
__aicore__ inline void asc_int162half_rd_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half_rn and asc_sync)
__aicore__ inline void asc_int162half_rn_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_int162half_rz and asc_sync)
__aicore__ inline void asc_int162half_rz_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather_datablock and asc_sync)
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gather_datablock and asc_sync)
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8 and asc_sync)
__aicore__ inline void asc_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8_rna and asc_sync)
__aicore__ inline void asc_half2uint8_rna_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8_ru and asc_sync)
__aicore__ inline void asc_half2uint8_ru_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8_rd and asc_sync)
__aicore__ inline void asc_half2uint8_rd_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8_rn and asc_sync)
__aicore__ inline void asc_half2uint8_rn_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_half2uint8_rz and asc_sync)
__aicore__ inline void asc_half2uint8_rz_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le_scalar and asc_sync)
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_le_scalar and asc_sync)
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_pair_reduce_sum and asc_sync)
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_pair_reduce_sum and asc_sync)
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2ub and asc_sync)
__aicore__ inline void asc_copy_ub2ub_sync(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub and asc_sync)
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm and asc_sync)
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ double* dst, __ubuf__ double* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2028/09/03", asc_copy_ub2l1 and asc_sync)
__aicore__ inline void asc_copy_ub2l1_sync(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#endif
