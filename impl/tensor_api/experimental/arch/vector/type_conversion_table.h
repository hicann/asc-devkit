/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_TABLE_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_TABLE_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_TABLE_H

namespace asc {
namespace te {
namespace experimental {
namespace detail {

// bfloat16_t -> float
ASC_REG_CAST_OP(float, bfloat16_t, zero, rint, not_sat, asc_bfloat162float);
ASC_REG_CAST_OP(float, bfloat16_t, one, rint, not_sat, asc_bfloat162float);

// float -> bfloat16_t
ASC_REG_CAST_OP(bfloat16_t, float, zero, floor, not_sat, asc_float2bfloat16_rd);
ASC_REG_CAST_OP(bfloat16_t, float, zero, floor, sat, asc_float2bfloat16_rd_sat);
ASC_REG_CAST_OP(bfloat16_t, float, one, floor, not_sat, asc_float2bfloat16_rd);
ASC_REG_CAST_OP(bfloat16_t, float, one, floor, sat, asc_float2bfloat16_rd_sat);
ASC_REG_CAST_OP(bfloat16_t, float, zero, rint, not_sat, asc_float2bfloat16_rn);
ASC_REG_CAST_OP(bfloat16_t, float, zero, rint, sat, asc_float2bfloat16_rn_sat);
ASC_REG_CAST_OP(bfloat16_t, float, one, rint, not_sat, asc_float2bfloat16_rn);
ASC_REG_CAST_OP(bfloat16_t, float, one, rint, sat, asc_float2bfloat16_rn_sat);
ASC_REG_CAST_OP(bfloat16_t, float, zero, round, not_sat, asc_float2bfloat16_rna);
ASC_REG_CAST_OP(bfloat16_t, float, zero, round, sat, asc_float2bfloat16_rna_sat);
ASC_REG_CAST_OP(bfloat16_t, float, one, round, not_sat, asc_float2bfloat16_rna);
ASC_REG_CAST_OP(bfloat16_t, float, one, round, sat, asc_float2bfloat16_rna_sat);
ASC_REG_CAST_OP(bfloat16_t, float, zero, ceil, not_sat, asc_float2bfloat16_ru);
ASC_REG_CAST_OP(bfloat16_t, float, zero, ceil, sat, asc_float2bfloat16_ru_sat);
ASC_REG_CAST_OP(bfloat16_t, float, one, ceil, not_sat, asc_float2bfloat16_ru);
ASC_REG_CAST_OP(bfloat16_t, float, one, ceil, sat, asc_float2bfloat16_ru_sat);
ASC_REG_CAST_OP(bfloat16_t, float, zero, trunc, not_sat, asc_float2bfloat16_rz);
ASC_REG_CAST_OP(bfloat16_t, float, zero, trunc, sat, asc_float2bfloat16_rz_sat);
ASC_REG_CAST_OP(bfloat16_t, float, one, trunc, not_sat, asc_float2bfloat16_rz);
ASC_REG_CAST_OP(bfloat16_t, float, one, trunc, sat, asc_float2bfloat16_rz_sat);

// bfloat16_t -> half
ASC_REG_CAST_OP(half, bfloat16_t, zero, floor, not_sat, asc_bfloat162half_rd);
ASC_REG_CAST_OP(half, bfloat16_t, zero, floor, sat, asc_bfloat162half_rd_sat);
ASC_REG_CAST_OP(half, bfloat16_t, zero, rint, not_sat, asc_bfloat162half_rn);
ASC_REG_CAST_OP(half, bfloat16_t, zero, rint, sat, asc_bfloat162half_rn_sat);
ASC_REG_CAST_OP(half, bfloat16_t, zero, round, not_sat, asc_bfloat162half_rna);
ASC_REG_CAST_OP(half, bfloat16_t, zero, round, sat, asc_bfloat162half_rna_sat);
ASC_REG_CAST_OP(half, bfloat16_t, zero, ceil, not_sat, asc_bfloat162half_ru);
ASC_REG_CAST_OP(half, bfloat16_t, zero, ceil, sat, asc_bfloat162half_ru_sat);
ASC_REG_CAST_OP(half, bfloat16_t, zero, trunc, not_sat, asc_bfloat162half_rz);
ASC_REG_CAST_OP(half, bfloat16_t, zero, trunc, sat, asc_bfloat162half_rz_sat);

// half -> bfloat16_t
ASC_REG_CAST_OP(bfloat16_t, half, zero, floor, not_sat, asc_half2bfloat16_rd);
ASC_REG_CAST_OP(bfloat16_t, half, zero, rint, not_sat, asc_half2bfloat16_rn);
ASC_REG_CAST_OP(bfloat16_t, half, zero, round, not_sat, asc_half2bfloat16_rna);
ASC_REG_CAST_OP(bfloat16_t, half, zero, ceil, not_sat, asc_half2bfloat16_ru);
ASC_REG_CAST_OP(bfloat16_t, half, zero, trunc, not_sat, asc_half2bfloat16_rz);

// bfloat16_t -> int32_t
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, floor, not_sat, asc_bfloat162int32_rd);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, floor, sat, asc_bfloat162int32_rd_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, floor, not_sat, asc_bfloat162int32_rd);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, floor, sat, asc_bfloat162int32_rd_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, rint, not_sat, asc_bfloat162int32_rn);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, rint, sat, asc_bfloat162int32_rn_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, rint, not_sat, asc_bfloat162int32_rn);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, rint, sat, asc_bfloat162int32_rn_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, round, not_sat, asc_bfloat162int32_rna);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, round, sat, asc_bfloat162int32_rna_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, round, not_sat, asc_bfloat162int32_rna);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, round, sat, asc_bfloat162int32_rna_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, ceil, not_sat, asc_bfloat162int32_ru);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, ceil, sat, asc_bfloat162int32_ru_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, ceil, not_sat, asc_bfloat162int32_ru);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, ceil, sat, asc_bfloat162int32_ru_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, trunc, not_sat, asc_bfloat162int32_rz);
ASC_REG_CAST_OP(int32_t, bfloat16_t, zero, trunc, sat, asc_bfloat162int32_rz_sat);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, trunc, not_sat, asc_bfloat162int32_rz);
ASC_REG_CAST_OP(int32_t, bfloat16_t, one, trunc, sat, asc_bfloat162int32_rz_sat);

// float -> half
ASC_REG_CAST_OP(half, float, zero, floor, not_sat, asc_float2half_rd);
ASC_REG_CAST_OP(half, float, zero, floor, sat, asc_float2half_rd_sat);
ASC_REG_CAST_OP(half, float, one, floor, not_sat, asc_float2half_rd);
ASC_REG_CAST_OP(half, float, one, floor, sat, asc_float2half_rd_sat);
ASC_REG_CAST_OP(half, float, zero, rint, not_sat, asc_float2half_rn);
ASC_REG_CAST_OP(half, float, zero, rint, sat, asc_float2half_rn_sat);
ASC_REG_CAST_OP(half, float, one, rint, not_sat, asc_float2half_rn);
ASC_REG_CAST_OP(half, float, one, rint, sat, asc_float2half_rn_sat);
ASC_REG_CAST_OP(half, float, zero, round, not_sat, asc_float2half_rna);
ASC_REG_CAST_OP(half, float, zero, round, sat, asc_float2half_rna_sat);
ASC_REG_CAST_OP(half, float, one, round, not_sat, asc_float2half_rna);
ASC_REG_CAST_OP(half, float, one, round, sat, asc_float2half_rna_sat);
ASC_REG_CAST_OP(half, float, zero, ceil, not_sat, asc_float2half_ru);
ASC_REG_CAST_OP(half, float, zero, ceil, sat, asc_float2half_ru_sat);
ASC_REG_CAST_OP(half, float, one, ceil, not_sat, asc_float2half_ru);
ASC_REG_CAST_OP(half, float, one, ceil, sat, asc_float2half_ru_sat);
ASC_REG_CAST_OP(half, float, zero, trunc, not_sat, asc_float2half_rz);
ASC_REG_CAST_OP(half, float, zero, trunc, sat, asc_float2half_rz_sat);
ASC_REG_CAST_OP(half, float, one, trunc, not_sat, asc_float2half_rz);
ASC_REG_CAST_OP(half, float, one, trunc, sat, asc_float2half_rz_sat);

// half -> float
ASC_REG_CAST_OP(float, half, zero, rint, not_sat, asc_half2float);
ASC_REG_CAST_OP(float, half, one, rint, not_sat, asc_half2float);

// float -> int16_t
ASC_REG_CAST_OP(int16_t, float, zero, floor, not_sat, asc_float2int16_rd);
ASC_REG_CAST_OP(int16_t, float, zero, floor, sat, asc_float2int16_rd_sat);
ASC_REG_CAST_OP(int16_t, float, one, floor, not_sat, asc_float2int16_rd);
ASC_REG_CAST_OP(int16_t, float, one, floor, sat, asc_float2int16_rd_sat);
ASC_REG_CAST_OP(int16_t, float, zero, rint, not_sat, asc_float2int16_rn);
ASC_REG_CAST_OP(int16_t, float, zero, rint, sat, asc_float2int16_rn_sat);
ASC_REG_CAST_OP(int16_t, float, one, rint, not_sat, asc_float2int16_rn);
ASC_REG_CAST_OP(int16_t, float, one, rint, sat, asc_float2int16_rn_sat);
ASC_REG_CAST_OP(int16_t, float, zero, round, not_sat, asc_float2int16_rna);
ASC_REG_CAST_OP(int16_t, float, zero, round, sat, asc_float2int16_rna_sat);
ASC_REG_CAST_OP(int16_t, float, one, round, not_sat, asc_float2int16_rna);
ASC_REG_CAST_OP(int16_t, float, one, round, sat, asc_float2int16_rna_sat);
ASC_REG_CAST_OP(int16_t, float, zero, ceil, not_sat, asc_float2int16_ru);
ASC_REG_CAST_OP(int16_t, float, zero, ceil, sat, asc_float2int16_ru_sat);
ASC_REG_CAST_OP(int16_t, float, one, ceil, not_sat, asc_float2int16_ru);
ASC_REG_CAST_OP(int16_t, float, one, ceil, sat, asc_float2int16_ru_sat);
ASC_REG_CAST_OP(int16_t, float, zero, trunc, not_sat, asc_float2int16_rz);
ASC_REG_CAST_OP(int16_t, float, zero, trunc, sat, asc_float2int16_rz_sat);
ASC_REG_CAST_OP(int16_t, float, one, trunc, not_sat, asc_float2int16_rz);
ASC_REG_CAST_OP(int16_t, float, one, trunc, sat, asc_float2int16_rz_sat);

// float -> int32_t
ASC_REG_CAST_OP(int32_t, float, zero, floor, not_sat, asc_float2int32_rd);
ASC_REG_CAST_OP(int32_t, float, zero, floor, sat, asc_float2int32_rd_sat);
ASC_REG_CAST_OP(int32_t, float, zero, rint, not_sat, asc_float2int32_rn);
ASC_REG_CAST_OP(int32_t, float, zero, rint, sat, asc_float2int32_rn_sat);
ASC_REG_CAST_OP(int32_t, float, zero, round, not_sat, asc_float2int32_rna);
ASC_REG_CAST_OP(int32_t, float, zero, round, sat, asc_float2int32_rna_sat);
ASC_REG_CAST_OP(int32_t, float, zero, ceil, not_sat, asc_float2int32_ru);
ASC_REG_CAST_OP(int32_t, float, zero, ceil, sat, asc_float2int32_ru_sat);
ASC_REG_CAST_OP(int32_t, float, zero, trunc, not_sat, asc_float2int32_rz);
ASC_REG_CAST_OP(int32_t, float, zero, trunc, sat, asc_float2int32_rz_sat);

// float -> int64_t
ASC_REG_CAST_OP(int64_t, float, zero, floor, not_sat, asc_float2int64_rd);
ASC_REG_CAST_OP(int64_t, float, zero, floor, sat, asc_float2int64_rd_sat);
ASC_REG_CAST_OP(int64_t, float, one, floor, not_sat, asc_float2int64_rd);
ASC_REG_CAST_OP(int64_t, float, one, floor, sat, asc_float2int64_rd_sat);
ASC_REG_CAST_OP(int64_t, float, zero, rint, not_sat, asc_float2int64_rn);
ASC_REG_CAST_OP(int64_t, float, zero, rint, sat, asc_float2int64_rn_sat);
ASC_REG_CAST_OP(int64_t, float, one, rint, not_sat, asc_float2int64_rn);
ASC_REG_CAST_OP(int64_t, float, one, rint, sat, asc_float2int64_rn_sat);
ASC_REG_CAST_OP(int64_t, float, zero, round, not_sat, asc_float2int64_rna);
ASC_REG_CAST_OP(int64_t, float, zero, round, sat, asc_float2int64_rna_sat);
ASC_REG_CAST_OP(int64_t, float, one, round, not_sat, asc_float2int64_rna);
ASC_REG_CAST_OP(int64_t, float, one, round, sat, asc_float2int64_rna_sat);
ASC_REG_CAST_OP(int64_t, float, zero, ceil, not_sat, asc_float2int64_ru);
ASC_REG_CAST_OP(int64_t, float, zero, ceil, sat, asc_float2int64_ru_sat);
ASC_REG_CAST_OP(int64_t, float, one, ceil, not_sat, asc_float2int64_ru);
ASC_REG_CAST_OP(int64_t, float, one, ceil, sat, asc_float2int64_ru_sat);
ASC_REG_CAST_OP(int64_t, float, zero, trunc, not_sat, asc_float2int64_rz);
ASC_REG_CAST_OP(int64_t, float, zero, trunc, sat, asc_float2int64_rz_sat);
ASC_REG_CAST_OP(int64_t, float, one, trunc, not_sat, asc_float2int64_rz);
ASC_REG_CAST_OP(int64_t, float, one, trunc, sat, asc_float2int64_rz_sat);

// int16_t -> float
ASC_REG_CAST_OP(float, int16_t, zero, rint, not_sat, asc_int162float);
ASC_REG_CAST_OP(float, int16_t, one, rint, not_sat, asc_int162float);

// int32_t -> float
ASC_REG_CAST_OP(float, int32_t, zero, floor, not_sat, asc_int322float_rd);
ASC_REG_CAST_OP(float, int32_t, zero, rint, not_sat, asc_int322float_rn);
ASC_REG_CAST_OP(float, int32_t, zero, round, not_sat, asc_int322float_rna);
ASC_REG_CAST_OP(float, int32_t, zero, ceil, not_sat, asc_int322float_ru);
ASC_REG_CAST_OP(float, int32_t, zero, trunc, not_sat, asc_int322float_rz);

// int64_t -> float
ASC_REG_CAST_OP(float, int64_t, zero, floor, not_sat, asc_int642float_rd);
ASC_REG_CAST_OP(float, int64_t, one, floor, not_sat, asc_int642float_rd);
ASC_REG_CAST_OP(float, int64_t, zero, rint, not_sat, asc_int642float_rn);
ASC_REG_CAST_OP(float, int64_t, one, rint, not_sat, asc_int642float_rn);
ASC_REG_CAST_OP(float, int64_t, zero, round, not_sat, asc_int642float_rna);
ASC_REG_CAST_OP(float, int64_t, one, round, not_sat, asc_int642float_rna);
ASC_REG_CAST_OP(float, int64_t, zero, ceil, not_sat, asc_int642float_ru);
ASC_REG_CAST_OP(float, int64_t, one, ceil, not_sat, asc_int642float_ru);
ASC_REG_CAST_OP(float, int64_t, zero, trunc, not_sat, asc_int642float_rz);
ASC_REG_CAST_OP(float, int64_t, one, trunc, not_sat, asc_int642float_rz);

// half -> int8_t
ASC_REG_CAST_OP(int8_t, half, zero, floor, not_sat, asc_half2int8_rd);
ASC_REG_CAST_OP(int8_t, half, zero, floor, sat, asc_half2int8_rd_sat);
ASC_REG_CAST_OP(int8_t, half, one, floor, not_sat, asc_half2int8_rd);
ASC_REG_CAST_OP(int8_t, half, one, floor, sat, asc_half2int8_rd_sat);
ASC_REG_CAST_OP(int8_t, half, zero, rint, not_sat, asc_half2int8_rn);
ASC_REG_CAST_OP(int8_t, half, zero, rint, sat, asc_half2int8_rn_sat);
ASC_REG_CAST_OP(int8_t, half, one, rint, not_sat, asc_half2int8_rn);
ASC_REG_CAST_OP(int8_t, half, one, rint, sat, asc_half2int8_rn_sat);
ASC_REG_CAST_OP(int8_t, half, zero, round, not_sat, asc_half2int8_rna);
ASC_REG_CAST_OP(int8_t, half, zero, round, sat, asc_half2int8_rna_sat);
ASC_REG_CAST_OP(int8_t, half, one, round, not_sat, asc_half2int8_rna);
ASC_REG_CAST_OP(int8_t, half, one, round, sat, asc_half2int8_rna_sat);
ASC_REG_CAST_OP(int8_t, half, zero, ceil, not_sat, asc_half2int8_ru);
ASC_REG_CAST_OP(int8_t, half, zero, ceil, sat, asc_half2int8_ru_sat);
ASC_REG_CAST_OP(int8_t, half, one, ceil, not_sat, asc_half2int8_ru);
ASC_REG_CAST_OP(int8_t, half, one, ceil, sat, asc_half2int8_ru_sat);
ASC_REG_CAST_OP(int8_t, half, zero, trunc, not_sat, asc_half2int8_rz);
ASC_REG_CAST_OP(int8_t, half, zero, trunc, sat, asc_half2int8_rz_sat);
ASC_REG_CAST_OP(int8_t, half, one, trunc, not_sat, asc_half2int8_rz);
ASC_REG_CAST_OP(int8_t, half, one, trunc, sat, asc_half2int8_rz_sat);

// int16_t -> half
ASC_REG_CAST_OP(half, int16_t, zero, floor, not_sat, asc_int162half_rd);
ASC_REG_CAST_OP(half, int16_t, zero, rint, not_sat, asc_int162half_rn);
ASC_REG_CAST_OP(half, int16_t, zero, round, not_sat, asc_int162half_rna);
ASC_REG_CAST_OP(half, int16_t, zero, ceil, not_sat, asc_int162half_ru);
ASC_REG_CAST_OP(half, int16_t, zero, trunc, not_sat, asc_int162half_rz);

// int8_t -> half
ASC_REG_CAST_OP(half, int8_t, zero, rint, not_sat, asc_int82half);
ASC_REG_CAST_OP(half, int8_t, one, rint, not_sat, asc_int82half);

// half -> uint8_t
ASC_REG_CAST_OP(uint8_t, half, zero, floor, not_sat, asc_half2uint8_rd);
ASC_REG_CAST_OP(uint8_t, half, zero, floor, sat, asc_half2uint8_rd_sat);
ASC_REG_CAST_OP(uint8_t, half, one, floor, not_sat, asc_half2uint8_rd);
ASC_REG_CAST_OP(uint8_t, half, one, floor, sat, asc_half2uint8_rd_sat);
ASC_REG_CAST_OP(uint8_t, half, zero, rint, not_sat, asc_half2uint8_rn);
ASC_REG_CAST_OP(uint8_t, half, zero, rint, sat, asc_half2uint8_rn_sat);
ASC_REG_CAST_OP(uint8_t, half, one, rint, not_sat, asc_half2uint8_rn);
ASC_REG_CAST_OP(uint8_t, half, one, rint, sat, asc_half2uint8_rn_sat);
ASC_REG_CAST_OP(uint8_t, half, zero, round, not_sat, asc_half2uint8_rna);
ASC_REG_CAST_OP(uint8_t, half, zero, round, sat, asc_half2uint8_rna_sat);
ASC_REG_CAST_OP(uint8_t, half, one, round, not_sat, asc_half2uint8_rna);
ASC_REG_CAST_OP(uint8_t, half, one, round, sat, asc_half2uint8_rna_sat);
ASC_REG_CAST_OP(uint8_t, half, zero, ceil, not_sat, asc_half2uint8_ru);
ASC_REG_CAST_OP(uint8_t, half, zero, ceil, sat, asc_half2uint8_ru_sat);
ASC_REG_CAST_OP(uint8_t, half, one, ceil, not_sat, asc_half2uint8_ru);
ASC_REG_CAST_OP(uint8_t, half, one, ceil, sat, asc_half2uint8_ru_sat);
ASC_REG_CAST_OP(uint8_t, half, zero, trunc, not_sat, asc_half2uint8_rz);
ASC_REG_CAST_OP(uint8_t, half, zero, trunc, sat, asc_half2uint8_rz_sat);
ASC_REG_CAST_OP(uint8_t, half, one, trunc, not_sat, asc_half2uint8_rz);
ASC_REG_CAST_OP(uint8_t, half, one, trunc, sat, asc_half2uint8_rz_sat);

// uint8_t -> half
ASC_REG_CAST_OP(half, uint8_t, zero, rint, not_sat, asc_uint82half);
ASC_REG_CAST_OP(half, uint8_t, one, rint, not_sat, asc_uint82half);

// int16_t -> int32_t
ASC_REG_CAST_OP(int32_t, int16_t, zero, rint, not_sat, asc_int162int32);
ASC_REG_CAST_OP(int32_t, int16_t, one, rint, not_sat, asc_int162int32);

// int32_t -> int16_t
ASC_REG_CAST_OP(int16_t, int32_t, zero, rint, not_sat, asc_int322int16);
ASC_REG_CAST_OP(int16_t, int32_t, zero, rint, sat, asc_int322int16_sat);
ASC_REG_CAST_OP(int16_t, int32_t, one, rint, not_sat, asc_int322int16);
ASC_REG_CAST_OP(int16_t, int32_t, one, rint, sat, asc_int322int16_sat);

// int32_t -> int64_t
ASC_REG_CAST_OP(int64_t, int32_t, zero, rint, not_sat, asc_int322int64);
ASC_REG_CAST_OP(int64_t, int32_t, one, rint, not_sat, asc_int322int64);

// int64_t -> int32_t
ASC_REG_CAST_OP(int32_t, int64_t, zero, rint, not_sat, asc_int642int32);
ASC_REG_CAST_OP(int32_t, int64_t, zero, rint, sat, asc_int642int32_sat);
ASC_REG_CAST_OP(int32_t, int64_t, one, rint, not_sat, asc_int642int32);
ASC_REG_CAST_OP(int32_t, int64_t, one, rint, sat, asc_int642int32_sat);

// int8_t -> int16_t
ASC_REG_CAST_OP(int16_t, int8_t, zero, rint, not_sat, asc_int82int16);
ASC_REG_CAST_OP(int16_t, int8_t, one, rint, not_sat, asc_int82int16);

// int8_t -> int32_t
ASC_REG_CAST_OP(int32_t, int8_t, zero, rint, not_sat, asc_int82int32);
ASC_REG_CAST_OP(int32_t, int8_t, one, rint, not_sat, asc_int82int32);
ASC_REG_CAST_OP(int32_t, int8_t, two, rint, not_sat, asc_int82int32);
ASC_REG_CAST_OP(int32_t, int8_t, three, rint, not_sat, asc_int82int32);

// int16_t -> uint32_t
ASC_REG_CAST_OP(uint32_t, int16_t, zero, rint, not_sat, asc_int162uint32);
ASC_REG_CAST_OP(uint32_t, int16_t, one, rint, not_sat, asc_int162uint32);

// int16_t -> uint8_t
ASC_REG_CAST_OP(uint8_t, int16_t, zero, rint, not_sat, asc_int162uint8);
ASC_REG_CAST_OP(uint8_t, int16_t, zero, rint, sat, asc_int162uint8_sat);
ASC_REG_CAST_OP(uint8_t, int16_t, one, rint, not_sat, asc_int162uint8);
ASC_REG_CAST_OP(uint8_t, int16_t, one, rint, sat, asc_int162uint8_sat);

// int32_t -> uint16_t
ASC_REG_CAST_OP(uint16_t, int32_t, zero, rint, not_sat, asc_int322uint16);
ASC_REG_CAST_OP(uint16_t, int32_t, zero, rint, sat, asc_int322uint16_sat);
ASC_REG_CAST_OP(uint16_t, int32_t, one, rint, not_sat, asc_int322uint16);
ASC_REG_CAST_OP(uint16_t, int32_t, one, rint, sat, asc_int322uint16_sat);

// int32_t -> uint8_t
ASC_REG_CAST_OP(uint8_t, int32_t, zero, rint, not_sat, asc_int322uint8);
ASC_REG_CAST_OP(uint8_t, int32_t, zero, rint, sat, asc_int322uint8_sat);
ASC_REG_CAST_OP(uint8_t, int32_t, one, rint, not_sat, asc_int322uint8);
ASC_REG_CAST_OP(uint8_t, int32_t, one, rint, sat, asc_int322uint8_sat);
ASC_REG_CAST_OP(uint8_t, int32_t, two, rint, not_sat, asc_int322uint8);
ASC_REG_CAST_OP(uint8_t, int32_t, two, rint, sat, asc_int322uint8_sat);
ASC_REG_CAST_OP(uint8_t, int32_t, three, rint, not_sat, asc_int322uint8);
ASC_REG_CAST_OP(uint8_t, int32_t, three, rint, sat, asc_int322uint8_sat);

// uint32_t -> int16_t
ASC_REG_CAST_OP(int16_t, uint32_t, zero, rint, not_sat, asc_uint322int16);
ASC_REG_CAST_OP(int16_t, uint32_t, zero, rint, sat, asc_uint322int16_sat);
ASC_REG_CAST_OP(int16_t, uint32_t, one, rint, not_sat, asc_uint322int16);
ASC_REG_CAST_OP(int16_t, uint32_t, one, rint, sat, asc_uint322int16_sat);

// uint16_t -> uint32_t
ASC_REG_CAST_OP(uint32_t, uint16_t, zero, rint, not_sat, asc_uint162uint32);
ASC_REG_CAST_OP(uint32_t, uint16_t, one, rint, not_sat, asc_uint162uint32);

// uint16_t -> uint8_t
ASC_REG_CAST_OP(uint8_t, uint16_t, zero, rint, not_sat, asc_uint162uint8);
ASC_REG_CAST_OP(uint8_t, uint16_t, zero, rint, sat, asc_uint162uint8_sat);
ASC_REG_CAST_OP(uint8_t, uint16_t, one, rint, not_sat, asc_uint162uint8);
ASC_REG_CAST_OP(uint8_t, uint16_t, one, rint, sat, asc_uint162uint8_sat);

// uint32_t -> uint16_t
ASC_REG_CAST_OP(uint16_t, uint32_t, zero, rint, not_sat, asc_uint322uint16);
ASC_REG_CAST_OP(uint16_t, uint32_t, zero, rint, sat, asc_uint322uint16_sat);
ASC_REG_CAST_OP(uint16_t, uint32_t, one, rint, not_sat, asc_uint322uint16);
ASC_REG_CAST_OP(uint16_t, uint32_t, one, rint, sat, asc_uint322uint16_sat);

// uint32_t -> uint8_t
ASC_REG_CAST_OP(uint8_t, uint32_t, zero, rint, not_sat, asc_uint322uint8);
ASC_REG_CAST_OP(uint8_t, uint32_t, zero, rint, sat, asc_uint322uint8_sat);
ASC_REG_CAST_OP(uint8_t, uint32_t, one, rint, not_sat, asc_uint322uint8);
ASC_REG_CAST_OP(uint8_t, uint32_t, one, rint, sat, asc_uint322uint8_sat);
ASC_REG_CAST_OP(uint8_t, uint32_t, two, rint, not_sat, asc_uint322uint8);
ASC_REG_CAST_OP(uint8_t, uint32_t, two, rint, sat, asc_uint322uint8_sat);
ASC_REG_CAST_OP(uint8_t, uint32_t, three, rint, not_sat, asc_uint322uint8);
ASC_REG_CAST_OP(uint8_t, uint32_t, three, rint, sat, asc_uint322uint8_sat);

// uint8_t -> uint16_t
ASC_REG_CAST_OP(uint16_t, uint8_t, zero, rint, not_sat, asc_uint82uint16);
ASC_REG_CAST_OP(uint16_t, uint8_t, one, rint, not_sat, asc_uint82uint16);

// uint8_t -> uint32_t
ASC_REG_CAST_OP(uint32_t, uint8_t, zero, rint, not_sat, asc_uint82uint32);
ASC_REG_CAST_OP(uint32_t, uint8_t, one, rint, not_sat, asc_uint82uint32);
ASC_REG_CAST_OP(uint32_t, uint8_t, two, rint, not_sat, asc_uint82uint32);
ASC_REG_CAST_OP(uint32_t, uint8_t, three, rint, not_sat, asc_uint82uint32);

// half -> int16_t
ASC_REG_CAST_OP(int16_t, half, zero, floor, not_sat, asc_half2int16_rd);
ASC_REG_CAST_OP(int16_t, half, zero, floor, sat, asc_half2int16_rd_sat);
ASC_REG_CAST_OP(int16_t, half, zero, rint, not_sat, asc_half2int16_rn);
ASC_REG_CAST_OP(int16_t, half, zero, rint, sat, asc_half2int16_rn_sat);
ASC_REG_CAST_OP(int16_t, half, zero, round, not_sat, asc_half2int16_rna);
ASC_REG_CAST_OP(int16_t, half, zero, round, sat, asc_half2int16_rna_sat);
ASC_REG_CAST_OP(int16_t, half, zero, ceil, not_sat, asc_half2int16_ru);
ASC_REG_CAST_OP(int16_t, half, zero, ceil, sat, asc_half2int16_ru_sat);
ASC_REG_CAST_OP(int16_t, half, zero, trunc, not_sat, asc_half2int16_rz);
ASC_REG_CAST_OP(int16_t, half, zero, trunc, sat, asc_half2int16_rz_sat);

// half -> int32_t
ASC_REG_CAST_OP(int32_t, half, zero, floor, not_sat, asc_half2int32_rd);
ASC_REG_CAST_OP(int32_t, half, one, floor, not_sat, asc_half2int32_rd);
ASC_REG_CAST_OP(int32_t, half, zero, rint, not_sat, asc_half2int32_rn);
ASC_REG_CAST_OP(int32_t, half, one, rint, not_sat, asc_half2int32_rn);
ASC_REG_CAST_OP(int32_t, half, zero, round, not_sat, asc_half2int32_rna);
ASC_REG_CAST_OP(int32_t, half, one, round, not_sat, asc_half2int32_rna);
ASC_REG_CAST_OP(int32_t, half, zero, ceil, not_sat, asc_half2int32_ru);
ASC_REG_CAST_OP(int32_t, half, one, ceil, not_sat, asc_half2int32_ru);
ASC_REG_CAST_OP(int32_t, half, zero, trunc, not_sat, asc_half2int32_rz);
ASC_REG_CAST_OP(int32_t, half, one, trunc, not_sat, asc_half2int32_rz);
#if !defined(ASCENDC_CPU_DEBUG)
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)) || defined(__ASC_NPU_HOST__)

// float -> hifloat8_t
ASC_REG_CAST_OP(hifloat8_t, float, zero, round, not_sat, asc_float2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, float, zero, round, sat, asc_float2hif8_rna_sat);
ASC_REG_CAST_OP(hifloat8_t, float, one, round, not_sat, asc_float2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, float, one, round, sat, asc_float2hif8_rna_sat);
ASC_REG_CAST_OP(hifloat8_t, float, two, round, not_sat, asc_float2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, float, two, round, sat, asc_float2hif8_rna_sat);
ASC_REG_CAST_OP(hifloat8_t, float, three, round, not_sat, asc_float2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, float, three, round, sat, asc_float2hif8_rna_sat);

// hifloat8_t -> float
ASC_REG_CAST_OP(float, hifloat8_t, zero, rint, not_sat, asc_hif82float);
ASC_REG_CAST_OP(float, hifloat8_t, one, rint, not_sat, asc_hif82float);
ASC_REG_CAST_OP(float, hifloat8_t, two, rint, not_sat, asc_hif82float);
ASC_REG_CAST_OP(float, hifloat8_t, three, rint, not_sat, asc_hif82float);

// half -> hifloat8_t
ASC_REG_CAST_OP(hifloat8_t, half, zero, round, not_sat, asc_half2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, half, zero, round, sat, asc_half2hif8_rna_sat);
ASC_REG_CAST_OP(hifloat8_t, half, one, round, not_sat, asc_half2hif8_rna);
ASC_REG_CAST_OP(hifloat8_t, half, one, round, sat, asc_half2hif8_rna_sat);

// hifloat8_t -> half
ASC_REG_CAST_OP(half, hifloat8_t, zero, rint, not_sat, asc_hif82half);
ASC_REG_CAST_OP(half, hifloat8_t, one, rint, not_sat, asc_hif82half);

// half -> int4x2_t
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, floor, not_sat, asc_half2int4x2_rd);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, floor, sat, asc_half2int4x2_rd_sat);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, rint, not_sat, asc_half2int4x2_rn);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, rint, sat, asc_half2int4x2_rn_sat);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, round, not_sat, asc_half2int4x2_rna);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, round, sat, asc_half2int4x2_rna_sat);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, ceil, not_sat, asc_half2int4x2_ru);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, ceil, sat, asc_half2int4x2_ru_sat);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, trunc, not_sat, asc_half2int4x2_rz);
ASC_REG_CAST_OP_NO_POSITION(int4x2_t, half, zero, trunc, sat, asc_half2int4x2_rz_sat);

// int4x2_t -> half
ASC_REG_CAST_OP_NO_POSITION(half, int4x2_t, zero, rint, not_sat, asc_int4x22half);

// int4x2_t -> int16_t
ASC_REG_CAST_OP_NO_POSITION(int16_t, int4x2_t, zero, rint, not_sat, asc_int4x22int16);

// fp4x2_e1m2_t -> bfloat16_t
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e1m2_t, zero, rint, not_sat, asc_e1m2x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e1m2_t, one, rint, not_sat, asc_e1m2x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e1m2_t, two, rint, not_sat, asc_e1m2x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e1m2_t, three, rint, not_sat, asc_e1m2x22bfloat16);

// fp4x2_e2m1_t -> bfloat16_t
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e2m1_t, zero, rint, not_sat, asc_e2m1x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e2m1_t, one, rint, not_sat, asc_e2m1x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e2m1_t, two, rint, not_sat, asc_e2m1x22bfloat16);
ASC_REG_CAST_OP(bfloat16_t, fp4x2_e2m1_t, three, rint, not_sat, asc_e2m1x22bfloat16);

// int4x2_t -> bfloat16_t
ASC_REG_CAST_OP_NO_POSITION(bfloat16_t, int4x2_t, zero, rint, not_sat, asc_int4x22bfloat16);

// float -> fp8_e4m3fn_t
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, zero, rint, not_sat, asc_float2e4m3_rn);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, zero, rint, sat, asc_float2e4m3_rn_sat);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, one, rint, not_sat, asc_float2e4m3_rn);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, one, rint, sat, asc_float2e4m3_rn_sat);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, two, rint, not_sat, asc_float2e4m3_rn);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, two, rint, sat, asc_float2e4m3_rn_sat);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, three, rint, not_sat, asc_float2e4m3_rn);
ASC_REG_CAST_OP(fp8_e4m3fn_t, float, three, rint, sat, asc_float2e4m3_rn_sat);

// float -> fp8_e5m2_t
ASC_REG_CAST_OP(fp8_e5m2_t, float, zero, rint, not_sat, asc_float2e5m2_rn);
ASC_REG_CAST_OP(fp8_e5m2_t, float, zero, rint, sat, asc_float2e5m2_rn_sat);
ASC_REG_CAST_OP(fp8_e5m2_t, float, one, rint, not_sat, asc_float2e5m2_rn);
ASC_REG_CAST_OP(fp8_e5m2_t, float, one, rint, sat, asc_float2e5m2_rn_sat);
ASC_REG_CAST_OP(fp8_e5m2_t, float, two, rint, not_sat, asc_float2e5m2_rn);
ASC_REG_CAST_OP(fp8_e5m2_t, float, two, rint, sat, asc_float2e5m2_rn_sat);
ASC_REG_CAST_OP(fp8_e5m2_t, float, three, rint, not_sat, asc_float2e5m2_rn);
ASC_REG_CAST_OP(fp8_e5m2_t, float, three, rint, sat, asc_float2e5m2_rn_sat);

// fp8_e4m3fn_t -> float
ASC_REG_CAST_OP(float, fp8_e4m3fn_t, zero, rint, not_sat, asc_e4m32float);
ASC_REG_CAST_OP(float, fp8_e4m3fn_t, one, rint, not_sat, asc_e4m32float);
ASC_REG_CAST_OP(float, fp8_e4m3fn_t, two, rint, not_sat, asc_e4m32float);
ASC_REG_CAST_OP(float, fp8_e4m3fn_t, three, rint, not_sat, asc_e4m32float);

// fp8_e5m2_t -> float
ASC_REG_CAST_OP(float, fp8_e5m2_t, zero, rint, not_sat, asc_e5m22float);
ASC_REG_CAST_OP(float, fp8_e5m2_t, one, rint, not_sat, asc_e5m22float);
ASC_REG_CAST_OP(float, fp8_e5m2_t, two, rint, not_sat, asc_e5m22float);
ASC_REG_CAST_OP(float, fp8_e5m2_t, three, rint, not_sat, asc_e5m22float);

// bfloat16_t -> fp4x2_e1m2_t
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, zero, floor, not_sat, asc_bfloat162e1m2x2_rd);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, one, floor, not_sat, asc_bfloat162e1m2x2_rd);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, two, floor, not_sat, asc_bfloat162e1m2x2_rd);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, three, floor, not_sat, asc_bfloat162e1m2x2_rd);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, zero, rint, not_sat, asc_bfloat162e1m2x2_rn);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, one, rint, not_sat, asc_bfloat162e1m2x2_rn);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, two, rint, not_sat, asc_bfloat162e1m2x2_rn);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, three, rint, not_sat, asc_bfloat162e1m2x2_rn);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, zero, round, not_sat, asc_bfloat162e1m2x2_rna);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, one, round, not_sat, asc_bfloat162e1m2x2_rna);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, two, round, not_sat, asc_bfloat162e1m2x2_rna);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, three, round, not_sat, asc_bfloat162e1m2x2_rna);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, zero, ceil, not_sat, asc_bfloat162e1m2x2_ru);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, one, ceil, not_sat, asc_bfloat162e1m2x2_ru);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, two, ceil, not_sat, asc_bfloat162e1m2x2_ru);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, three, ceil, not_sat, asc_bfloat162e1m2x2_ru);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, zero, trunc, not_sat, asc_bfloat162e1m2x2_rz);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, one, trunc, not_sat, asc_bfloat162e1m2x2_rz);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, two, trunc, not_sat, asc_bfloat162e1m2x2_rz);
ASC_REG_CAST_OP(fp4x2_e1m2_t, bfloat16_t, three, trunc, not_sat, asc_bfloat162e1m2x2_rz);

// bfloat16_t -> fp4x2_e2m1_t
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, zero, floor, not_sat, asc_bfloat162e2m1x2_rd);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, one, floor, not_sat, asc_bfloat162e2m1x2_rd);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, two, floor, not_sat, asc_bfloat162e2m1x2_rd);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, three, floor, not_sat, asc_bfloat162e2m1x2_rd);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, zero, rint, not_sat, asc_bfloat162e2m1x2_rn);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, one, rint, not_sat, asc_bfloat162e2m1x2_rn);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, two, rint, not_sat, asc_bfloat162e2m1x2_rn);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, three, rint, not_sat, asc_bfloat162e2m1x2_rn);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, zero, round, not_sat, asc_bfloat162e2m1x2_rna);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, one, round, not_sat, asc_bfloat162e2m1x2_rna);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, two, round, not_sat, asc_bfloat162e2m1x2_rna);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, three, round, not_sat, asc_bfloat162e2m1x2_rna);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, zero, ceil, not_sat, asc_bfloat162e2m1x2_ru);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, one, ceil, not_sat, asc_bfloat162e2m1x2_ru);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, two, ceil, not_sat, asc_bfloat162e2m1x2_ru);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, three, ceil, not_sat, asc_bfloat162e2m1x2_ru);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, zero, trunc, not_sat, asc_bfloat162e2m1x2_rz);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, one, trunc, not_sat, asc_bfloat162e2m1x2_rz);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, two, trunc, not_sat, asc_bfloat162e2m1x2_rz);
ASC_REG_CAST_OP(fp4x2_e2m1_t, bfloat16_t, three, trunc, not_sat, asc_bfloat162e2m1x2_rz);
#endif
#endif

} // namespace detail
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_TABLE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_TABLE_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_TABLE_H
#endif
