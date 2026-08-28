/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_vcvt_instr_utils.h"

TEST_VECTOR_COMPUTE_VCVT_INSTR_1(Bfloat162int32_rn, asc_bfloat162int32_rn, vcvt, vector_int32_t, vector_bfloat16_t, 0);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rn_sat, asc_bfloat162int32_rn_sat, vcvt, vector_int32_t, vector_bfloat16_t, 1);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rn_v2, asc_bfloat162int32_rn_v2, vcvt, vector_int32_t, vector_bfloat16_t, 2);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rn_sat_v2, asc_bfloat162int32_rn_sat_v2, vcvt, vector_int32_t, vector_bfloat16_t, 3);

TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rna, asc_bfloat162int32_rna, vcvt, vector_int32_t, vector_bfloat16_t, 4);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rna_sat, asc_bfloat162int32_rna_sat, vcvt, vector_int32_t, vector_bfloat16_t, 5);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rna_v2, asc_bfloat162int32_rna_v2, vcvt, vector_int32_t, vector_bfloat16_t, 6);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rna_sat_v2, asc_bfloat162int32_rna_sat_v2, vcvt, vector_int32_t, vector_bfloat16_t, 7);

TEST_VECTOR_COMPUTE_VCVT_INSTR_1(Bfloat162int32_rd, asc_bfloat162int32_rd, vcvt, vector_int32_t, vector_bfloat16_t, 8);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rd_sat, asc_bfloat162int32_rd_sat, vcvt, vector_int32_t, vector_bfloat16_t, 9);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rd_v2, asc_bfloat162int32_rd_v2, vcvt, vector_int32_t, vector_bfloat16_t, 10);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rd_sat_v2, asc_bfloat162int32_rd_sat_v2, vcvt, vector_int32_t, vector_bfloat16_t, 11);

TEST_VECTOR_COMPUTE_VCVT_INSTR_1(Bfloat162int32_ru, asc_bfloat162int32_ru, vcvt, vector_int32_t, vector_bfloat16_t, 12);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_ru_sat, asc_bfloat162int32_ru_sat, vcvt, vector_int32_t, vector_bfloat16_t, 13);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_ru_v2, asc_bfloat162int32_ru_v2, vcvt, vector_int32_t, vector_bfloat16_t, 14);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_ru_sat_v2, asc_bfloat162int32_ru_sat_v2, vcvt, vector_int32_t, vector_bfloat16_t, 15);

TEST_VECTOR_COMPUTE_VCVT_INSTR_1(Bfloat162int32_rz, asc_bfloat162int32_rz, vcvt, vector_int32_t, vector_bfloat16_t, 16);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rz_sat, asc_bfloat162int32_rz_sat, vcvt, vector_int32_t, vector_bfloat16_t, 17);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rz_v2, asc_bfloat162int32_rz_v2, vcvt, vector_int32_t, vector_bfloat16_t, 18);
TEST_VECTOR_COMPUTE_VCVT_INSTR_1(
    Bfloat162int32_rz_sat_v2, asc_bfloat162int32_rz_sat_v2, vcvt, vector_int32_t, vector_bfloat16_t, 19);

TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rn, asc_bfloat162int32_rn, vcvt, vector_int32_t, vector_bfloat16_t, 0);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rn_sat, asc_bfloat162int32_rn_sat, vcvt, vector_int32_t, vector_bfloat16_t, 1);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rna, asc_bfloat162int32_rna, vcvt, vector_int32_t, vector_bfloat16_t, 2);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rna_sat, asc_bfloat162int32_rna_sat, vcvt, vector_int32_t, vector_bfloat16_t, 3);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rd, asc_bfloat162int32_rd, vcvt, vector_int32_t, vector_bfloat16_t, 4);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rd_sat, asc_bfloat162int32_rd_sat, vcvt, vector_int32_t, vector_bfloat16_t, 5);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_ru, asc_bfloat162int32_ru, vcvt, vector_int32_t, vector_bfloat16_t, 6);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_ru_sat, asc_bfloat162int32_ru_sat, vcvt, vector_int32_t, vector_bfloat16_t, 7);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rz, asc_bfloat162int32_rz, vcvt, vector_int32_t, vector_bfloat16_t, 8);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(
    Bfloat162int32_rz_sat, asc_bfloat162int32_rz_sat, vcvt, vector_int32_t, vector_bfloat16_t, 9);
