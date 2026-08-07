/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/reg_compute/reg_convert.h"

// These calls validate only that each supported compile-time position argument is accepted.
#define CHECK_POSITION_ARGUMENTS(api_name, dst_type, src_type) \
    do {                                                       \
        dst_type dst;                                          \
        src_type src;                                          \
        vector_bool mask;                                      \
        api_name(dst, src, mask, ASC_POSITION_EVEN);           \
        api_name(dst, src, mask, ASC_POSITION_ODD);            \
    } while (false)

#define CHECK_QUARTER_POSITION_ARGUMENTS(api_name, dst_type, src_type) \
    do {                                                               \
        dst_type dst;                                                  \
        src_type src;                                                  \
        vector_bool mask;                                              \
        api_name(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);          \
        api_name(dst, src, mask, ASC_DISPERSE_SECOND_QUARTER);         \
        api_name(dst, src, mask, ASC_DISPERSE_THIRD_QUARTER);          \
        api_name(dst, src, mask, ASC_DISPERSE_FOURTH_QUARTER);         \
    } while (false)

TEST(RegConvertPositionOverloads, AcceptAllLegalPositionArguments)
{
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2e4m3_rn, vector_fp8_e4m3fn_t, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2e4m3_rn_sat, vector_fp8_e4m3fn_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_int162uint32, vector_uint32_t, vector_int16_t);
    CHECK_POSITION_ARGUMENTS(asc_int642int32, vector_int32_t, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_int642int32_sat, vector_int32_t, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rd, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rd_sat, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rn, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rn_sat, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rna, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rna_sat, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_ru, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_ru_sat, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rz, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int8_rz_sat, vector_int8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_int82half, vector_half, vector_int8_t);
    CHECK_POSITION_ARGUMENTS(asc_uint162uint8, vector_uint8_t, vector_uint16_t);
    CHECK_POSITION_ARGUMENTS(asc_uint162uint8_sat, vector_uint8_t, vector_uint16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_int82int32, vector_int32_t, vector_int8_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_e2m1x22bfloat16, vector_bfloat16_t, vector_fp4x2_e2m1_t);
    CHECK_POSITION_ARGUMENTS(asc_int322uint16, vector_uint16_t, vector_int32_t);
    CHECK_POSITION_ARGUMENTS(asc_int322uint16_sat, vector_uint16_t, vector_int32_t);
    CHECK_POSITION_ARGUMENTS(asc_int162float, vector_float, vector_int16_t);
    CHECK_POSITION_ARGUMENTS(asc_half2float, vector_float, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_int82int16, vector_int16_t, vector_int8_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_uint82uint32, vector_uint32_t, vector_uint8_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_int322uint8, vector_uint8_t, vector_int32_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_int322uint8_sat, vector_uint8_t, vector_int32_t);
    CHECK_POSITION_ARGUMENTS(asc_int322int64, vector_int64_t, vector_int32_t);
    CHECK_POSITION_ARGUMENTS(asc_half2hif8_rh, vector_hifloat8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2hif8_rh_sat, vector_hifloat8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2hif8_rna, vector_hifloat8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2hif8_rna_sat, vector_hifloat8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_int162int32, vector_int32_t, vector_int16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162float, vector_float, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_int162uint8, vector_uint8_t, vector_int16_t);
    CHECK_POSITION_ARGUMENTS(asc_int162uint8_sat, vector_uint8_t, vector_int16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_e1m2x22bfloat16, vector_bfloat16_t, vector_fp4x2_e1m2_t);
    CHECK_POSITION_ARGUMENTS(asc_uint82uint16, vector_uint16_t, vector_uint8_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_e5m22float, vector_float, vector_fp8_e5m2_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2e5m2_rn, vector_fp8_e5m2_t, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2e5m2_rn_sat, vector_fp8_e5m2_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_uint322uint16, vector_uint16_t, vector_uint32_t);
    CHECK_POSITION_ARGUMENTS(asc_uint322uint16_sat, vector_uint16_t, vector_uint32_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_uint322uint8, vector_uint8_t, vector_uint32_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_uint322uint8_sat, vector_uint8_t, vector_uint32_t);
    CHECK_POSITION_ARGUMENTS(asc_hif82half, vector_half, vector_hifloat8_t);
    CHECK_POSITION_ARGUMENTS(asc_int322int16, vector_int16_t, vector_int32_t);
    CHECK_POSITION_ARGUMENTS(asc_int322int16_sat, vector_int16_t, vector_int32_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_e4m32float, vector_float, vector_fp8_e4m3fn_t);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_ru, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_ru_sat, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rna, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rna_sat, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rd, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rd_sat, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rn, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rn_sat, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rz, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2bfloat16_rz_sat, vector_bfloat16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_half2int32_rn, vector_int32_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int32_ru, vector_int32_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int32_rd, vector_int32_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int32_rz, vector_int32_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2int32_rna, vector_int32_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rz, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rz_sat, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rn, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rn_sat, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rna, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rna_sat, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rd, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_rd_sat, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_ro, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_ro_sat, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_ru, vector_half, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2half_ru_sat, vector_half, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2hif8_rna, vector_hifloat8_t, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2hif8_rna_sat, vector_hifloat8_t, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2hif8_rh, vector_hifloat8_t, vector_float);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_float2hif8_rh_sat, vector_hifloat8_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rz, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rz_sat, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rn, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rn_sat, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rna, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rna_sat, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_ru, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_ru_sat, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rd, vector_int32_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_bfloat162int32_rd_sat, vector_int32_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_hif82float, vector_float, vector_hifloat8_t);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rz, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rz_sat, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rd, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rd_sat, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rna, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rna_sat, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_ru, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_ru_sat, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rn, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int16_rn_sat, vector_int16_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rn, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rn_sat, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rz, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rz_sat, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_ru, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_ru_sat, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rd, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rd_sat, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rna, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_float2int64_rna_sat, vector_int64_t, vector_float);
    CHECK_POSITION_ARGUMENTS(asc_uint322int16, vector_int16_t, vector_uint32_t);
    CHECK_POSITION_ARGUMENTS(asc_uint322int16_sat, vector_int16_t, vector_uint32_t);
    CHECK_POSITION_ARGUMENTS(asc_uint82half, vector_half, vector_uint8_t);
    CHECK_POSITION_ARGUMENTS(asc_uint162uint32, vector_uint32_t, vector_uint16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e1m2x2_rn, vector_fp4x2_e1m2_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e1m2x2_ru, vector_fp4x2_e1m2_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e1m2x2_rd, vector_fp4x2_e1m2_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e1m2x2_rna, vector_fp4x2_e1m2_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e1m2x2_rz, vector_fp4x2_e1m2_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e2m1x2_rd, vector_fp4x2_e2m1_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e2m1x2_rz, vector_fp4x2_e2m1_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e2m1x2_rn, vector_fp4x2_e2m1_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e2m1x2_ru, vector_fp4x2_e2m1_t, vector_bfloat16_t);
    CHECK_QUARTER_POSITION_ARGUMENTS(asc_bfloat162e2m1x2_rna, vector_fp4x2_e2m1_t, vector_bfloat16_t);
    CHECK_POSITION_ARGUMENTS(asc_int642float_ru, vector_float, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_int642float_rd, vector_float, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_int642float_rn, vector_float, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_int642float_rz, vector_float, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_int642float_rna, vector_float, vector_int64_t);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rd, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rd_sat, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_ru, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_ru_sat, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rna, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rna_sat, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rn, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rn_sat, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rz, vector_uint8_t, vector_half);
    CHECK_POSITION_ARGUMENTS(asc_half2uint8_rz_sat, vector_uint8_t, vector_half);
}

#undef CHECK_POSITION_ARGUMENTS
#undef CHECK_QUARTER_POSITION_ARGUMENTS
