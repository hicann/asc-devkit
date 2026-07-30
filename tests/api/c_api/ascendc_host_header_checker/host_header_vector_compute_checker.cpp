/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/vector_compute/vector_compute.h"

static void test_host_c_api_vector_compute_0()
{
    using ::asc_abs;
    using ::asc_abs_sync;
    using ::asc_add;
    using ::asc_add_relu;
    using ::asc_add_relu_sync;
    using ::asc_add_scalar;
    using ::asc_add_scalar_sync;
    using ::asc_add_sync;
    using ::asc_and;
    using ::asc_and_sync;
    using ::asc_axpy;
    using ::asc_axpy_sync;
    using ::asc_bfloat162float;
    using ::asc_bfloat162float_sync;
    using ::asc_bfloat162int32_rd;
    using ::asc_bfloat162int32_rd_sync;
    using ::asc_bfloat162int32_rn;
    using ::asc_bfloat162int32_rn_sync;
    using ::asc_bfloat162int32_rna;
    using ::asc_bfloat162int32_rna_sync;
    using ::asc_bfloat162int32_ru;
    using ::asc_bfloat162int32_ru_sync;
    using ::asc_bfloat162int32_rz;
    using ::asc_bfloat162int32_rz_sync;
    using ::asc_bitsort;
    using ::asc_bitsort_sync;
    using ::asc_brcb;
    using ::asc_brcb_sync;
    using ::asc_copy;
    using ::asc_copy_sync;
}

static void test_host_c_api_vector_compute_1()
{
    using ::asc_datablock_reduce_max;
    using ::asc_datablock_reduce_max_sync;
    using ::asc_datablock_reduce_min;
    using ::asc_datablock_reduce_min_sync;
    using ::asc_datablock_reduce_sum;
    using ::asc_datablock_reduce_sum_sync;
    using ::asc_deq_int162b8_h;
    using ::asc_deq_int162b8_h_sync;
    using ::asc_deq_int162b8_l;
    using ::asc_deq_int162b8_l_sync;
    using ::asc_deq_int322half;
    using ::asc_deq_int322half_sync;
    using ::asc_div;
    using ::asc_div_sync;
    using ::asc_duplicate;
    using ::asc_duplicate_sync;
    using ::asc_eq;
    using ::asc_eq_scalar;
    using ::asc_eq_scalar_sync;
    using ::asc_eq_sync;
    using ::asc_exp;
    using ::asc_exp_sync;
    using ::asc_float2bfloat16_rd;
    using ::asc_float2bfloat16_rd_sync;
    using ::asc_float2bfloat16_rn;
    using ::asc_float2bfloat16_rn_sync;
    using ::asc_float2bfloat16_rna;
    using ::asc_float2bfloat16_rna_sync;
    using ::asc_float2bfloat16_ru;
    using ::asc_float2bfloat16_ru_sync;
}

static void test_host_c_api_vector_compute_2()
{
    using ::asc_float2bfloat16_rz;
    using ::asc_float2bfloat16_rz_sync;
    using ::asc_float2float_rd;
    using ::asc_float2float_rd_sync;
    using ::asc_float2float_rn;
    using ::asc_float2float_rn_sync;
    using ::asc_float2float_rna;
    using ::asc_float2float_rna_sync;
    using ::asc_float2float_ru;
    using ::asc_float2float_ru_sync;
    using ::asc_float2float_rz;
    using ::asc_float2float_rz_sync;
    using ::asc_float2half;
    using ::asc_float2half_rd;
    using ::asc_float2half_rd_sync;
    using ::asc_float2half_rn;
    using ::asc_float2half_rn_sync;
    using ::asc_float2half_rna;
    using ::asc_float2half_rna_sync;
    using ::asc_float2half_ro;
    using ::asc_float2half_ro_sync;
    using ::asc_float2half_ru;
    using ::asc_float2half_ru_sync;
    using ::asc_float2half_rz;
    using ::asc_float2half_rz_sync;
    using ::asc_float2half_sync;
    using ::asc_float2int16_rd;
    using ::asc_float2int16_rd_sync;
    using ::asc_float2int16_rn;
    using ::asc_float2int16_rn_sync;
}

static void test_host_c_api_vector_compute_3()
{
    using ::asc_float2int16_rna;
    using ::asc_float2int16_rna_sync;
    using ::asc_float2int16_ru;
    using ::asc_float2int16_ru_sync;
    using ::asc_float2int16_rz;
    using ::asc_float2int16_rz_sync;
    using ::asc_float2int32_rd;
    using ::asc_float2int32_rd_sync;
    using ::asc_float2int32_rn;
    using ::asc_float2int32_rn_sync;
    using ::asc_float2int32_rna;
    using ::asc_float2int32_rna_sync;
    using ::asc_float2int32_ru;
    using ::asc_float2int32_ru_sync;
    using ::asc_float2int32_rz;
    using ::asc_float2int32_rz_sync;
    using ::asc_float2int64_rd;
    using ::asc_float2int64_rd_sync;
    using ::asc_float2int64_rn;
    using ::asc_float2int64_rn_sync;
    using ::asc_float2int64_rna;
    using ::asc_float2int64_rna_sync;
    using ::asc_float2int64_ru;
    using ::asc_float2int64_ru_sync;
    using ::asc_float2int64_rz;
    using ::asc_float2int64_rz_sync;
    using ::asc_fma;
    using ::asc_fma_sync;
    using ::asc_gather;
    using ::asc_gather_datablock;
}

static void test_host_c_api_vector_compute_4()
{
    using ::asc_gather_datablock_sync;
    using ::asc_gather_sync;
    using ::asc_ge;
    using ::asc_ge_scalar;
    using ::asc_ge_scalar_sync;
    using ::asc_ge_sync;
    using ::asc_get_acc_val;
    using ::asc_get_cmp_mask;
    using ::asc_get_reduce_max_cnt;
    using ::asc_get_reduce_min_cnt;
    using ::asc_get_rsvd_count;
    using ::asc_get_vms4_sr;
    using ::asc_gt;
    using ::asc_gt_scalar;
    using ::asc_gt_scalar_sync;
    using ::asc_gt_sync;
    using ::asc_half2float;
    using ::asc_half2float_sync;
    using ::asc_half2int16_rd;
    using ::asc_half2int16_rd_sync;
    using ::asc_half2int16_rn;
    using ::asc_half2int16_rn_sync;
    using ::asc_half2int16_rna;
    using ::asc_half2int16_rna_sync;
    using ::asc_half2int16_ru;
    using ::asc_half2int16_ru_sync;
    using ::asc_half2int16_rz;
    using ::asc_half2int16_rz_sync;
    using ::asc_half2int32_rd;
    using ::asc_half2int32_rd_sync;
}

static void test_host_c_api_vector_compute_5()
{
    using ::asc_half2int32_rn;
    using ::asc_half2int32_rn_sync;
    using ::asc_half2int32_rna;
    using ::asc_half2int32_rna_sync;
    using ::asc_half2int32_ru;
    using ::asc_half2int32_ru_sync;
    using ::asc_half2int32_rz;
    using ::asc_half2int32_rz_sync;
    using ::asc_half2int4;
    using ::asc_half2int4_rd;
    using ::asc_half2int4_rd_sync;
    using ::asc_half2int4_rn;
    using ::asc_half2int4_rn_sync;
    using ::asc_half2int4_rna;
    using ::asc_half2int4_rna_sync;
    using ::asc_half2int4_ru;
    using ::asc_half2int4_ru_sync;
    using ::asc_half2int4_rz;
    using ::asc_half2int4_rz_sync;
    using ::asc_half2int4_sync;
    using ::asc_half2int8;
    using ::asc_half2int8_rd;
    using ::asc_half2int8_rd_sync;
    using ::asc_half2int8_rn;
    using ::asc_half2int8_rn_sync;
    using ::asc_half2int8_rna;
    using ::asc_half2int8_rna_sync;
    using ::asc_half2int8_ru;
    using ::asc_half2int8_ru_sync;
    using ::asc_half2int8_rz;
}

static void test_host_c_api_vector_compute_6()
{
    using ::asc_half2int8_rz_sync;
    using ::asc_half2int8_sync;
    using ::asc_half2uint8;
    using ::asc_half2uint8_rd;
    using ::asc_half2uint8_rd_sync;
    using ::asc_half2uint8_rn;
    using ::asc_half2uint8_rn_sync;
    using ::asc_half2uint8_rna;
    using ::asc_half2uint8_rna_sync;
    using ::asc_half2uint8_ru;
    using ::asc_half2uint8_ru_sync;
    using ::asc_half2uint8_rz;
    using ::asc_half2uint8_rz_sync;
    using ::asc_half2uint8_sync;
    using ::asc_int162float;
    using ::asc_int162float_sync;
    using ::asc_int162half;
    using ::asc_int162half_rd;
    using ::asc_int162half_rd_sync;
    using ::asc_int162half_rn;
    using ::asc_int162half_rn_sync;
    using ::asc_int162half_rna;
    using ::asc_int162half_rna_sync;
    using ::asc_int162half_ru;
    using ::asc_int162half_ru_sync;
    using ::asc_int162half_rz;
    using ::asc_int162half_rz_sync;
    using ::asc_int162half_sync;
    using ::asc_int322float;
    using ::asc_int322float_rd;
}

static void test_host_c_api_vector_compute_7()
{
    using ::asc_int322float_rd_sync;
    using ::asc_int322float_rn;
    using ::asc_int322float_rn_sync;
    using ::asc_int322float_rna;
    using ::asc_int322float_rna_sync;
    using ::asc_int322float_ru;
    using ::asc_int322float_ru_sync;
    using ::asc_int322float_rz;
    using ::asc_int322float_rz_sync;
    using ::asc_int322float_sync;
    using ::asc_int322int16;
    using ::asc_int322int16_sync;
    using ::asc_int322int64;
    using ::asc_int322int64_sync;
    using ::asc_int42half;
    using ::asc_int42half_sync;
    using ::asc_int642float_rd;
    using ::asc_int642float_rd_sync;
    using ::asc_int642float_rn;
    using ::asc_int642float_rn_sync;
    using ::asc_int642float_rna;
    using ::asc_int642float_rna_sync;
    using ::asc_int642float_ru;
    using ::asc_int642float_ru_sync;
    using ::asc_int642float_rz;
    using ::asc_int642float_rz_sync;
    using ::asc_int642int32;
    using ::asc_int642int32_sync;
    using ::asc_int82half;
    using ::asc_int82half_sync;
}

static void test_host_c_api_vector_compute_8()
{
    using ::asc_le;
    using ::asc_le_scalar;
    using ::asc_le_scalar_sync;
    using ::asc_le_sync;
    using ::asc_leakyrelu;
    using ::asc_leakyrelu_sync;
    using ::asc_log;
    using ::asc_log_sync;
    using ::asc_lt;
    using ::asc_lt_scalar;
    using ::asc_lt_scalar_sync;
    using ::asc_lt_sync;
    using ::asc_max;
    using ::asc_max_scalar;
    using ::asc_max_scalar_sync;
    using ::asc_max_sync;
    using ::asc_min;
    using ::asc_min_scalar;
    using ::asc_min_scalar_sync;
    using ::asc_min_sync;
    using ::asc_mrgsort4;
    using ::asc_mrgsort4_sync;
    using ::asc_mul;
    using ::asc_mul_add;
    using ::asc_mul_add_relu;
    using ::asc_mul_add_relu_sync;
    using ::asc_mul_add_sync;
    using ::asc_mul_cast_half2int8;
    using ::asc_mul_cast_half2int8_sync;
    using ::asc_mul_cast_half2uint8;
}

static void test_host_c_api_vector_compute_9()
{
    using ::asc_mul_cast_half2uint8_sync;
    using ::asc_mul_scalar;
    using ::asc_mul_scalar_sync;
    using ::asc_mul_sync;
    using ::asc_ne;
    using ::asc_ne_scalar;
    using ::asc_ne_scalar_sync;
    using ::asc_ne_sync;
    using ::asc_not;
    using ::asc_not_sync;
    using ::asc_or;
    using ::asc_or_sync;
    using ::asc_pair_reduce_sum;
    using ::asc_pair_reduce_sum_sync;
    using ::asc_rcp;
    using ::asc_rcp_sync;
    using ::asc_reduce;
    using ::asc_reduce_sync;
    using ::asc_relu;
    using ::asc_relu_sync;
    using ::asc_repeat_reduce_max_index_value;
    using ::asc_repeat_reduce_max_index_value_sync;
    using ::asc_repeat_reduce_max_only_index;
    using ::asc_repeat_reduce_max_only_index_sync;
    using ::asc_repeat_reduce_max_only_value;
    using ::asc_repeat_reduce_max_only_value_sync;
    using ::asc_repeat_reduce_max_value_index;
    using ::asc_repeat_reduce_max_value_index_sync;
    using ::asc_repeat_reduce_min_index_value;
    using ::asc_repeat_reduce_min_index_value_sync;
}

static void test_host_c_api_vector_compute_10()
{
    using ::asc_repeat_reduce_min_only_index;
    using ::asc_repeat_reduce_min_only_index_sync;
    using ::asc_repeat_reduce_min_only_value;
    using ::asc_repeat_reduce_min_only_value_sync;
    using ::asc_repeat_reduce_min_value_index;
    using ::asc_repeat_reduce_min_value_index_sync;
    using ::asc_repeat_reduce_sum;
    using ::asc_repeat_reduce_sum_sync;
    using ::asc_rsqrt;
    using ::asc_rsqrt_sync;
    using ::asc_select;
    using ::asc_select_sync;
    using ::asc_set_cmp_mask;
    using ::asc_set_deq_scale;
    using ::asc_set_mask_count;
    using ::asc_set_mask_norm;
    using ::asc_set_va_reg;
    using ::asc_set_vector_mask;
    using ::asc_shiftleft;
    using ::asc_shiftleft_sync;
    using ::asc_shiftright;
    using ::asc_shiftright_round;
    using ::asc_shiftright_round_sync;
    using ::asc_shiftright_sync;
    using ::asc_sqrt;
    using ::asc_sqrt_sync;
    using ::asc_squeeze;
    using ::asc_sub;
    using ::asc_sub_relu;
    using ::asc_sub_relu_sync;
}

static void test_host_c_api_vector_compute_11()
{
    using ::asc_sub_scalar;
    using ::asc_sub_scalar_sync;
    using ::asc_sub_sync;
    using ::asc_transpose;
    using ::asc_transpose_sync;
    using ::asc_uint82half;
    using ::asc_uint82half_sync;
    using ::asc_vdeq_int162b8_h;
    using ::asc_vdeq_int162b8_h_sync;
    using ::asc_vdeq_int162b8_l;
    using ::asc_vdeq_int162b8_l_sync;
}
