/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/reg_compute/reg_convert.h"
#include "c_api/reg_compute/reg_load.h"
#include "c_api/reg_compute/reg_store.h"
#include "c_api/reg_compute/reg_vector.h"

static void test_host_c_api_reg_compute_0()
{
    using ::asc_abs;
    using ::asc_abs_sub;
    using ::asc_add;
    using ::asc_add_scalar;
    using ::asc_addc;
    using ::asc_and;
    using ::asc_arange;
    using ::asc_arange_descend;
    using ::asc_axpy;
    using ::asc_bfloat162e1m2x2_rd;
    using ::asc_bfloat162e1m2x2_rd_v2;
    using ::asc_bfloat162e1m2x2_rd_v3;
    using ::asc_bfloat162e1m2x2_rd_v4;
    using ::asc_bfloat162e1m2x2_rn;
    using ::asc_bfloat162e1m2x2_rn_v2;
    using ::asc_bfloat162e1m2x2_rn_v3;
    using ::asc_bfloat162e1m2x2_rn_v4;
    using ::asc_bfloat162e1m2x2_rna;
    using ::asc_bfloat162e1m2x2_rna_v2;
    using ::asc_bfloat162e1m2x2_rna_v3;
    using ::asc_bfloat162e1m2x2_rna_v4;
    using ::asc_bfloat162e1m2x2_ru;
    using ::asc_bfloat162e1m2x2_ru_v2;
    using ::asc_bfloat162e1m2x2_ru_v3;
    using ::asc_bfloat162e1m2x2_ru_v4;
    using ::asc_bfloat162e1m2x2_rz;
    using ::asc_bfloat162e1m2x2_rz_v2;
    using ::asc_bfloat162e1m2x2_rz_v3;
    using ::asc_bfloat162e1m2x2_rz_v4;
    using ::asc_bfloat162e2m1x2_rd;
}

static void test_host_c_api_reg_compute_1()
{
    using ::asc_bfloat162e2m1x2_rd_v2;
    using ::asc_bfloat162e2m1x2_rd_v3;
    using ::asc_bfloat162e2m1x2_rd_v4;
    using ::asc_bfloat162e2m1x2_rn;
    using ::asc_bfloat162e2m1x2_rn_v2;
    using ::asc_bfloat162e2m1x2_rn_v3;
    using ::asc_bfloat162e2m1x2_rn_v4;
    using ::asc_bfloat162e2m1x2_rna;
    using ::asc_bfloat162e2m1x2_rna_v2;
    using ::asc_bfloat162e2m1x2_rna_v3;
    using ::asc_bfloat162e2m1x2_rna_v4;
    using ::asc_bfloat162e2m1x2_ru;
    using ::asc_bfloat162e2m1x2_ru_v2;
    using ::asc_bfloat162e2m1x2_ru_v3;
    using ::asc_bfloat162e2m1x2_ru_v4;
    using ::asc_bfloat162e2m1x2_rz;
    using ::asc_bfloat162e2m1x2_rz_v2;
    using ::asc_bfloat162e2m1x2_rz_v3;
    using ::asc_bfloat162e2m1x2_rz_v4;
    using ::asc_bfloat162float;
    using ::asc_bfloat162float_v2;
    using ::asc_bfloat162half_rd;
    using ::asc_bfloat162half_rd_sat;
    using ::asc_bfloat162half_rn;
    using ::asc_bfloat162half_rn_sat;
    using ::asc_bfloat162half_rna;
    using ::asc_bfloat162half_rna_sat;
    using ::asc_bfloat162half_ru;
    using ::asc_bfloat162half_ru_sat;
    using ::asc_bfloat162half_rz;
}

static void test_host_c_api_reg_compute_2()
{
    using ::asc_bfloat162half_rz_sat;
    using ::asc_bfloat162int32_rd;
    using ::asc_bfloat162int32_rd_sat;
    using ::asc_bfloat162int32_rd_sat_v2;
    using ::asc_bfloat162int32_rd_v2;
    using ::asc_bfloat162int32_rn;
    using ::asc_bfloat162int32_rn_sat;
    using ::asc_bfloat162int32_rn_sat_v2;
    using ::asc_bfloat162int32_rn_v2;
    using ::asc_bfloat162int32_rna;
    using ::asc_bfloat162int32_rna_sat;
    using ::asc_bfloat162int32_rna_sat_v2;
    using ::asc_bfloat162int32_rna_v2;
    using ::asc_bfloat162int32_ru;
    using ::asc_bfloat162int32_ru_sat;
    using ::asc_bfloat162int32_ru_sat_v2;
    using ::asc_bfloat162int32_ru_v2;
    using ::asc_bfloat162int32_rz;
    using ::asc_bfloat162int32_rz_sat;
    using ::asc_bfloat162int32_rz_sat_v2;
    using ::asc_bfloat162int32_rz_v2;
    using ::asc_ceil;
    using ::asc_clear_ar_spr;
    using ::asc_copy;
    using ::asc_update_addr_reg_b16;
    using ::asc_update_addr_reg_b32;
    using ::asc_update_addr_reg_b8;
}

static void test_host_c_api_reg_compute_3()
{
    using ::asc_cumulative_histogram_bin0;
    using ::asc_cumulative_histogram_bin1;
    using ::asc_deintlv;
    using ::asc_deintlv_b16;
    using ::asc_deintlv_b32;
    using ::asc_deintlv_b8;
    using ::asc_div;
    using ::asc_duplicate;
    using ::asc_duplicate_scalar;
    using ::asc_e1m2x22bfloat16;
    using ::asc_e1m2x22bfloat16_v2;
    using ::asc_e1m2x22bfloat16_v3;
    using ::asc_e1m2x22bfloat16_v4;
    using ::asc_e2m1x22bfloat16;
    using ::asc_e2m1x22bfloat16_v2;
    using ::asc_e2m1x22bfloat16_v3;
    using ::asc_e2m1x22bfloat16_v4;
    using ::asc_e4m32float;
    using ::asc_e4m32float_v2;
    using ::asc_e4m32float_v3;
    using ::asc_e4m32float_v4;
    using ::asc_e5m22float;
    using ::asc_e5m22float_v2;
    using ::asc_e5m22float_v3;
    using ::asc_e5m22float_v4;
    using ::asc_eq;
    using ::asc_eq_scalar;
    using ::asc_exp;
    using ::asc_exp_sub;
    using ::asc_exp_sub_half2float;
    using ::asc_exp_sub_v2;
}

static void test_host_c_api_reg_compute_4()
{
    using ::asc_float2bfloat16_rd;
    using ::asc_float2bfloat16_rd_sat;
    using ::asc_float2bfloat16_rd_sat_v2;
    using ::asc_float2bfloat16_rd_v2;
    using ::asc_float2bfloat16_rn;
    using ::asc_float2bfloat16_rn_sat;
    using ::asc_float2bfloat16_rn_sat_v2;
    using ::asc_float2bfloat16_rn_v2;
    using ::asc_float2bfloat16_rna;
    using ::asc_float2bfloat16_rna_sat;
    using ::asc_float2bfloat16_rna_sat_v2;
    using ::asc_float2bfloat16_rna_v2;
    using ::asc_float2bfloat16_ru;
    using ::asc_float2bfloat16_ru_sat;
    using ::asc_float2bfloat16_ru_sat_v2;
    using ::asc_float2bfloat16_ru_v2;
    using ::asc_float2bfloat16_rz;
    using ::asc_float2bfloat16_rz_sat;
    using ::asc_float2bfloat16_rz_sat_v2;
    using ::asc_float2bfloat16_rz_v2;
    using ::asc_float2e4m3_rn;
    using ::asc_float2e4m3_rn_sat;
    using ::asc_float2e4m3_rn_sat_v2;
    using ::asc_float2e4m3_rn_sat_v3;
    using ::asc_float2e4m3_rn_sat_v4;
    using ::asc_float2e4m3_rn_v2;
    using ::asc_float2e4m3_rn_v3;
    using ::asc_float2e4m3_rn_v4;
    using ::asc_float2e5m2_rn;
    using ::asc_float2e5m2_rn_sat;
}

static void test_host_c_api_reg_compute_5()
{
    using ::asc_float2e5m2_rn_sat_v2;
    using ::asc_float2e5m2_rn_sat_v3;
    using ::asc_float2e5m2_rn_sat_v4;
    using ::asc_float2e5m2_rn_v2;
    using ::asc_float2e5m2_rn_v3;
    using ::asc_float2e5m2_rn_v4;
    using ::asc_float2half_rd;
    using ::asc_float2half_rd_sat;
    using ::asc_float2half_rd_sat_v2;
    using ::asc_float2half_rd_v2;
    using ::asc_float2half_rn;
    using ::asc_float2half_rn_sat;
    using ::asc_float2half_rn_sat_v2;
    using ::asc_float2half_rn_v2;
    using ::asc_float2half_rna;
    using ::asc_float2half_rna_sat;
    using ::asc_float2half_rna_sat_v2;
    using ::asc_float2half_rna_v2;
    using ::asc_float2half_ro;
    using ::asc_float2half_ro_sat;
    using ::asc_float2half_ro_sat_v2;
    using ::asc_float2half_ro_v2;
    using ::asc_float2half_ru;
    using ::asc_float2half_ru_sat;
    using ::asc_float2half_ru_sat_v2;
    using ::asc_float2half_ru_v2;
    using ::asc_float2half_rz;
    using ::asc_float2half_rz_sat;
    using ::asc_float2half_rz_sat_v2;
    using ::asc_float2half_rz_v2;
}

static void test_host_c_api_reg_compute_6()
{
    using ::asc_float2hif8_rh;
    using ::asc_float2hif8_rh_sat;
    using ::asc_float2hif8_rh_sat_v2;
    using ::asc_float2hif8_rh_sat_v3;
    using ::asc_float2hif8_rh_sat_v4;
    using ::asc_float2hif8_rh_v2;
    using ::asc_float2hif8_rh_v3;
    using ::asc_float2hif8_rh_v4;
    using ::asc_float2hif8_rna;
    using ::asc_float2hif8_rna_sat;
    using ::asc_float2hif8_rna_sat_v2;
    using ::asc_float2hif8_rna_sat_v3;
    using ::asc_float2hif8_rna_sat_v4;
    using ::asc_float2hif8_rna_v2;
    using ::asc_float2hif8_rna_v3;
    using ::asc_float2hif8_rna_v4;
    using ::asc_float2int16_rd;
    using ::asc_float2int16_rd_sat;
    using ::asc_float2int16_rd_sat_v2;
    using ::asc_float2int16_rd_v2;
    using ::asc_float2int16_rn;
    using ::asc_float2int16_rn_sat;
    using ::asc_float2int16_rn_sat_v2;
    using ::asc_float2int16_rn_v2;
    using ::asc_float2int16_rna;
    using ::asc_float2int16_rna_sat;
    using ::asc_float2int16_rna_sat_v2;
    using ::asc_float2int16_rna_v2;
    using ::asc_float2int16_ru;
    using ::asc_float2int16_ru_sat;
}

static void test_host_c_api_reg_compute_7()
{
    using ::asc_float2int16_ru_sat_v2;
    using ::asc_float2int16_ru_v2;
    using ::asc_float2int16_rz;
    using ::asc_float2int16_rz_sat;
    using ::asc_float2int16_rz_sat_v2;
    using ::asc_float2int16_rz_v2;
    using ::asc_float2int32_rd;
    using ::asc_float2int32_rd_sat;
    using ::asc_float2int32_rn;
    using ::asc_float2int32_rn_sat;
    using ::asc_float2int32_rna;
    using ::asc_float2int32_rna_sat;
    using ::asc_float2int32_ru;
    using ::asc_float2int32_ru_sat;
    using ::asc_float2int32_rz;
    using ::asc_float2int32_rz_sat;
    using ::asc_float2int64_rd;
    using ::asc_float2int64_rd_sat;
    using ::asc_float2int64_rd_sat_v2;
    using ::asc_float2int64_rd_v2;
    using ::asc_float2int64_rn;
    using ::asc_float2int64_rn_sat;
    using ::asc_float2int64_rn_sat_v2;
    using ::asc_float2int64_rn_v2;
    using ::asc_float2int64_rna;
    using ::asc_float2int64_rna_sat;
    using ::asc_float2int64_rna_sat_v2;
    using ::asc_float2int64_rna_v2;
    using ::asc_float2int64_ru;
    using ::asc_float2int64_ru_sat;
}

static void test_host_c_api_reg_compute_8()
{
    using ::asc_float2int64_ru_sat_v2;
    using ::asc_float2int64_ru_v2;
    using ::asc_float2int64_rz;
    using ::asc_float2int64_rz_sat;
    using ::asc_float2int64_rz_sat_v2;
    using ::asc_float2int64_rz_v2;
    using ::asc_floor;
    using ::asc_frequency_histogram_bin0;
    using ::asc_frequency_histogram_bin1;
    using ::asc_gather;
    using ::asc_gather_datablock;
    using ::asc_ge;
    using ::asc_ge_scalar;
    using ::asc_get_mask_spr_b16;
    using ::asc_get_mask_spr_b32;
    using ::asc_gt;
    using ::asc_gt_scalar;
    using ::asc_half2bfloat16_rd;
    using ::asc_half2bfloat16_rn;
    using ::asc_half2bfloat16_rna;
    using ::asc_half2bfloat16_ru;
    using ::asc_half2bfloat16_rz;
    using ::asc_half2float;
    using ::asc_half2float_v2;
    using ::asc_half2hif8_rh;
    using ::asc_half2hif8_rh_sat;
    using ::asc_half2hif8_rh_sat_v2;
    using ::asc_half2hif8_rh_v2;
    using ::asc_half2hif8_rna;
    using ::asc_half2hif8_rna_sat;
}

static void test_host_c_api_reg_compute_9()
{
    using ::asc_half2hif8_rna_sat_v2;
    using ::asc_half2hif8_rna_v2;
    using ::asc_half2int16_rd;
    using ::asc_half2int16_rd_sat;
    using ::asc_half2int16_rn;
    using ::asc_half2int16_rn_sat;
    using ::asc_half2int16_rna;
    using ::asc_half2int16_rna_sat;
    using ::asc_half2int16_ru;
    using ::asc_half2int16_ru_sat;
    using ::asc_half2int16_rz;
    using ::asc_half2int16_rz_sat;
    using ::asc_half2int32_rd;
    using ::asc_half2int32_rd_v2;
    using ::asc_half2int32_rn;
    using ::asc_half2int32_rn_v2;
    using ::asc_half2int32_rna;
    using ::asc_half2int32_rna_v2;
    using ::asc_half2int32_ru;
    using ::asc_half2int32_ru_v2;
    using ::asc_half2int32_rz;
    using ::asc_half2int32_rz_v2;
    using ::asc_half2int4x2_rd;
    using ::asc_half2int4x2_rd_sat;
    using ::asc_half2int4x2_rd_sat_v2;
    using ::asc_half2int4x2_rd_sat_v3;
    using ::asc_half2int4x2_rd_sat_v4;
    using ::asc_half2int4x2_rd_v2;
    using ::asc_half2int4x2_rd_v3;
    using ::asc_half2int4x2_rd_v4;
}

static void test_host_c_api_reg_compute_10()
{
    using ::asc_half2int4x2_rn;
    using ::asc_half2int4x2_rn_sat;
    using ::asc_half2int4x2_rn_sat_v2;
    using ::asc_half2int4x2_rn_sat_v3;
    using ::asc_half2int4x2_rn_sat_v4;
    using ::asc_half2int4x2_rn_v2;
    using ::asc_half2int4x2_rn_v3;
    using ::asc_half2int4x2_rn_v4;
    using ::asc_half2int4x2_rna;
    using ::asc_half2int4x2_rna_sat;
    using ::asc_half2int4x2_rna_sat_v2;
    using ::asc_half2int4x2_rna_sat_v3;
    using ::asc_half2int4x2_rna_sat_v4;
    using ::asc_half2int4x2_rna_v2;
    using ::asc_half2int4x2_rna_v3;
    using ::asc_half2int4x2_rna_v4;
    using ::asc_half2int4x2_ru;
    using ::asc_half2int4x2_ru_sat;
    using ::asc_half2int4x2_ru_sat_v2;
    using ::asc_half2int4x2_ru_sat_v3;
    using ::asc_half2int4x2_ru_sat_v4;
    using ::asc_half2int4x2_ru_v2;
    using ::asc_half2int4x2_ru_v3;
    using ::asc_half2int4x2_ru_v4;
    using ::asc_half2int4x2_rz;
    using ::asc_half2int4x2_rz_sat;
    using ::asc_half2int4x2_rz_sat_v2;
    using ::asc_half2int4x2_rz_sat_v3;
    using ::asc_half2int4x2_rz_sat_v4;
    using ::asc_half2int4x2_rz_v2;
}

static void test_host_c_api_reg_compute_11()
{
    using ::asc_half2int4x2_rz_v3;
    using ::asc_half2int4x2_rz_v4;
    using ::asc_half2int8_rd;
    using ::asc_half2int8_rd_sat;
    using ::asc_half2int8_rd_sat_v2;
    using ::asc_half2int8_rd_v2;
    using ::asc_half2int8_rn;
    using ::asc_half2int8_rn_sat;
    using ::asc_half2int8_rn_sat_v2;
    using ::asc_half2int8_rn_v2;
    using ::asc_half2int8_rna;
    using ::asc_half2int8_rna_sat;
    using ::asc_half2int8_rna_sat_v2;
    using ::asc_half2int8_rna_v2;
    using ::asc_half2int8_ru;
    using ::asc_half2int8_ru_sat;
    using ::asc_half2int8_ru_sat_v2;
    using ::asc_half2int8_ru_v2;
    using ::asc_half2int8_rz;
    using ::asc_half2int8_rz_sat;
    using ::asc_half2int8_rz_sat_v2;
    using ::asc_half2int8_rz_v2;
    using ::asc_half2uint8_rd;
    using ::asc_half2uint8_rd_sat;
    using ::asc_half2uint8_rd_sat_v2;
    using ::asc_half2uint8_rd_v2;
    using ::asc_half2uint8_rn;
    using ::asc_half2uint8_rn_sat;
    using ::asc_half2uint8_rn_sat_v2;
    using ::asc_half2uint8_rn_v2;
}

static void test_host_c_api_reg_compute_12()
{
    using ::asc_half2uint8_rna;
    using ::asc_half2uint8_rna_sat;
    using ::asc_half2uint8_rna_sat_v2;
    using ::asc_half2uint8_rna_v2;
    using ::asc_half2uint8_ru;
    using ::asc_half2uint8_ru_sat;
    using ::asc_half2uint8_ru_sat_v2;
    using ::asc_half2uint8_ru_v2;
    using ::asc_half2uint8_rz;
    using ::asc_half2uint8_rz_sat;
    using ::asc_half2uint8_rz_sat_v2;
    using ::asc_half2uint8_rz_v2;
    using ::asc_hif82float;
    using ::asc_hif82float_v2;
    using ::asc_hif82float_v3;
    using ::asc_hif82float_v4;
    using ::asc_hif82half;
    using ::asc_hif82half_v2;
    using ::asc_int162float;
    using ::asc_int162float_v2;
    using ::asc_int162half_rd;
    using ::asc_int162half_rn;
    using ::asc_int162half_rna;
    using ::asc_int162half_ru;
    using ::asc_int162half_rz;
    using ::asc_int162int32;
    using ::asc_int162int32_v2;
    using ::asc_int162uint32;
    using ::asc_int162uint32_v2;
    using ::asc_int162uint8;
}

static void test_host_c_api_reg_compute_13()
{
    using ::asc_int162uint8_sat;
    using ::asc_int162uint8_sat_v2;
    using ::asc_int162uint8_v2;
    using ::asc_int322float_rd;
    using ::asc_int322float_rn;
    using ::asc_int322float_rna;
    using ::asc_int322float_ru;
    using ::asc_int322float_rz;
    using ::asc_int322int16;
    using ::asc_int322int16_sat;
    using ::asc_int322int16_sat_v2;
    using ::asc_int322int16_v2;
    using ::asc_int322int64;
    using ::asc_int322int64_v2;
    using ::asc_int322uint16;
    using ::asc_int322uint16_sat;
    using ::asc_int322uint16_sat_v2;
    using ::asc_int322uint16_v2;
    using ::asc_int322uint8;
    using ::asc_int322uint8_sat;
    using ::asc_int322uint8_sat_v2;
    using ::asc_int322uint8_sat_v3;
    using ::asc_int322uint8_sat_v4;
    using ::asc_int322uint8_v2;
    using ::asc_int322uint8_v3;
    using ::asc_int322uint8_v4;
    using ::asc_int4x22bfloat16;
    using ::asc_int4x22bfloat16_v2;
    using ::asc_int4x22bfloat16_v3;
    using ::asc_int4x22bfloat16_v4;
}

static void test_host_c_api_reg_compute_14()
{
    using ::asc_int4x22half;
    using ::asc_int4x22half_v2;
    using ::asc_int4x22half_v3;
    using ::asc_int4x22half_v4;
    using ::asc_int4x22int16;
    using ::asc_int4x22int16_v2;
    using ::asc_int4x22int16_v3;
    using ::asc_int4x22int16_v4;
    using ::asc_int642float_rd;
    using ::asc_int642float_rd_v2;
    using ::asc_int642float_rn;
    using ::asc_int642float_rn_v2;
    using ::asc_int642float_rna;
    using ::asc_int642float_rna_v2;
    using ::asc_int642float_ru;
    using ::asc_int642float_ru_v2;
    using ::asc_int642float_rz;
    using ::asc_int642float_rz_v2;
    using ::asc_int642int32;
    using ::asc_int642int32_sat;
    using ::asc_int642int32_sat_v2;
    using ::asc_int642int32_v2;
    using ::asc_int82half;
    using ::asc_int82half_v2;
    using ::asc_int82int16;
    using ::asc_int82int16_v2;
    using ::asc_int82int32;
    using ::asc_int82int32_v2;
    using ::asc_int82int32_v3;
    using ::asc_int82int32_v4;
}

static void test_host_c_api_reg_compute_15()
{
    using ::asc_intlv;
    using ::asc_intlv_b16;
    using ::asc_intlv_b32;
    using ::asc_intlv_b8;
    using ::asc_le;
    using ::asc_le_scalar;
    using ::asc_leakyrelu;
    using ::asc_ln;
    using ::asc_load;
    using ::asc_loadalign;
    using ::asc_loadalign_brc;
    using ::asc_loadalign_brc_datablock;
    using ::asc_loadalign_brc_datablock_postupdate;
    using ::asc_loadalign_brc_elem;
    using ::asc_loadalign_brc_elem2datablock;
    using ::asc_loadalign_brc_elem2datablock_postupdate;
    using ::asc_loadalign_brc_elem_postupdate;
    using ::asc_loadalign_brc_postupdate;
    using ::asc_loadalign_brc_postupdate_v2;
    using ::asc_loadalign_brc_postupdate_v3;
    using ::asc_loadalign_brc_v2;
    using ::asc_loadalign_brc_v3;
    using ::asc_loadalign_deintlv;
    using ::asc_loadalign_deintlv_postupdate;
    using ::asc_loadalign_downsample;
    using ::asc_loadalign_downsample_postupdate;
    using ::asc_loadalign_postupdate;
    using ::asc_loadalign_unpack;
    using ::asc_loadalign_unpack4;
    using ::asc_loadalign_unpack4_postupdate;
}

static void test_host_c_api_reg_compute_16()
{
    using ::asc_loadalign_unpack_postupdate;
    using ::asc_loadalign_unpack_postupdate_v2;
    using ::asc_loadalign_unpack_v2;
    using ::asc_loadalign_upsample;
    using ::asc_loadalign_upsample_postupdate;
    using ::asc_loadunalign;
    using ::asc_loadunalign_postupdate;
    using ::asc_loadunalign_pre;
    using ::asc_lt;
    using ::asc_lt_scalar;
    using ::asc_madd;
    using ::asc_max;
    using ::asc_max_scalar;
    using ::asc_min;
    using ::asc_min_scalar;
    using ::asc_mul;
    using ::asc_mul_scalar;
    using ::asc_mull;
    using ::asc_muls;
    using ::asc_muls_v2;
    using ::asc_ne;
    using ::asc_ne_scalar;
    using ::asc_neg;
    using ::asc_not;
    using ::asc_or;
    using ::asc_pack;
    using ::asc_pack_v2;
    using ::asc_pair_reduce_sum;
    using ::asc_prelu;
    using ::asc_reduce_max;
}

static void test_host_c_api_reg_compute_17()
{
    using ::asc_reduce_max_datablock;
    using ::asc_reduce_min;
    using ::asc_reduce_min_datablock;
    using ::asc_reduce_sum;
    using ::asc_reduce_sum_datablock;
    using ::asc_relu;
    using ::asc_rint;
    using ::asc_round;
    using ::asc_scatter;
    using ::asc_select;
    using ::asc_set_va_reg;
    using ::asc_shiftleft;
    using ::asc_shiftleft_scalar;
    using ::asc_shiftright;
    using ::asc_shiftright_scalar;
    using ::asc_sqrt;
    using ::asc_squeeze;
    using ::asc_squeeze_v2;
    using ::asc_squeeze_with_status;
    using ::asc_store;
    using ::asc_storealign;
    using ::asc_storealign_1st;
    using ::asc_storealign_1st_postupdate;
    using ::asc_storealign_intlv;
    using ::asc_storealign_pack;
    using ::asc_storealign_pack_postupdate;
    using ::asc_storealign_pack_postupdate_v2;
    using ::asc_storealign_pack_quarter;
    using ::asc_storealign_pack_quarter_postupdate;
    using ::asc_storealign_pack_v2;
}

static void test_host_c_api_reg_compute_18()
{
    using ::asc_storealign_postupdate;
    using ::asc_storeunalign;
    using ::asc_storeunalign_post;
    using ::asc_storeunalign_post_postupdate;
    using ::asc_storeunalign_postupdate;
    using ::asc_sub;
    using ::asc_subc;
    using ::asc_trunc;
    using ::asc_uint162uint32;
    using ::asc_uint162uint32_v2;
    using ::asc_uint162uint8;
    using ::asc_uint162uint8_sat;
    using ::asc_uint162uint8_sat_v2;
    using ::asc_uint162uint8_v2;
    using ::asc_uint322int16;
    using ::asc_uint322int16_sat;
    using ::asc_uint322int16_sat_v2;
    using ::asc_uint322int16_v2;
    using ::asc_uint322uint16;
    using ::asc_uint322uint16_sat;
    using ::asc_uint322uint16_sat_v2;
    using ::asc_uint322uint16_v2;
    using ::asc_uint322uint8;
    using ::asc_uint322uint8_sat;
    using ::asc_uint322uint8_sat_v2;
    using ::asc_uint322uint8_sat_v3;
    using ::asc_uint322uint8_sat_v4;
    using ::asc_uint322uint8_v2;
    using ::asc_uint322uint8_v3;
    using ::asc_uint322uint8_v4;
}

static void test_host_c_api_reg_compute_19()
{
    using ::asc_uint82half;
    using ::asc_uint82half_v2;
    using ::asc_uint82uint16;
    using ::asc_uint82uint16_v2;
    using ::asc_uint82uint32;
    using ::asc_uint82uint32_v2;
    using ::asc_uint82uint32_v3;
    using ::asc_uint82uint32_v4;
    using ::asc_unpack_lower;
    using ::asc_unpack_upper;
    using ::asc_unsqueeze;
    using ::asc_update_mask_b16;
    using ::asc_update_mask_b32;
    using ::asc_update_mask_b8;
    using ::asc_xor;
}
