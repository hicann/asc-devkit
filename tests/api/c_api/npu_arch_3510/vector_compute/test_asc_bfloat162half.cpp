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

TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Bfloat162half_rn, asc_bfloat162half_rn, vcvt, vector_half, vector_bfloat16_t, 0);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(
    Bfloat162half_rn_sat, asc_bfloat162half_rn_sat, vcvt, vector_half, vector_bfloat16_t, 1);

TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Bfloat162half_rna, asc_bfloat162half_rna, vcvt, vector_half, vector_bfloat16_t, 2);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(
    Bfloat162half_rna_sat, asc_bfloat162half_rna_sat, vcvt, vector_half, vector_bfloat16_t, 3);

TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Bfloat162half_rd, asc_bfloat162half_rd, vcvt, vector_half, vector_bfloat16_t, 4);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(
    Bfloat162half_rd_sat, asc_bfloat162half_rd_sat, vcvt, vector_half, vector_bfloat16_t, 5);

TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Bfloat162half_ru, asc_bfloat162half_ru, vcvt, vector_half, vector_bfloat16_t, 6);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(
    Bfloat162half_ru_sat, asc_bfloat162half_ru_sat, vcvt, vector_half, vector_bfloat16_t, 7);

TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Bfloat162half_rz, asc_bfloat162half_rz, vcvt, vector_half, vector_bfloat16_t, 8);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(
    Bfloat162half_rz_sat, asc_bfloat162half_rz_sat, vcvt, vector_half, vector_bfloat16_t, 9);

TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rn, asc_bfloat162half_rn, vcvt, vector_half, vector_bfloat16_t, 0);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rn_sat, asc_bfloat162half_rn_sat, vcvt, vector_half, vector_bfloat16_t, 1);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rna, asc_bfloat162half_rna, vcvt, vector_half, vector_bfloat16_t, 2);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rna_sat, asc_bfloat162half_rna_sat, vcvt, vector_half, vector_bfloat16_t, 3);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rd, asc_bfloat162half_rd, vcvt, vector_half, vector_bfloat16_t, 4);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rd_sat, asc_bfloat162half_rd_sat, vcvt, vector_half, vector_bfloat16_t, 5);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_ru, asc_bfloat162half_ru, vcvt, vector_half, vector_bfloat16_t, 6);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_ru_sat, asc_bfloat162half_ru_sat, vcvt, vector_half, vector_bfloat16_t, 7);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rz, asc_bfloat162half_rz, vcvt, vector_half, vector_bfloat16_t, 8);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(
    Bfloat162half_rz_sat, asc_bfloat162half_rz_sat, vcvt, vector_half, vector_bfloat16_t, 9);
