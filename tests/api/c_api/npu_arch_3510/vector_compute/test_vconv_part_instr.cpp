/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_vcvt_instr_utils.h"

// ==========asc_half2int32==========
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rd, asc_half2int32_rd, vcvt, vector_int32_t, vector_half, 0);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rd_v2, asc_half2int32_rd_v2, vcvt, vector_int32_t, vector_half, 1);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rn, asc_half2int32_rn, vcvt, vector_int32_t, vector_half, 2);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rn_v2, asc_half2int32_rn_v2, vcvt, vector_int32_t, vector_half, 3);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rna, asc_half2int32_rna, vcvt, vector_int32_t, vector_half, 4);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rna_v2, asc_half2int32_rna_v2, vcvt, vector_int32_t, vector_half, 5);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_ru, asc_half2int32_ru, vcvt, vector_int32_t, vector_half, 6);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_ru_v2, asc_half2int32_ru_v2, vcvt, vector_int32_t, vector_half, 7);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rz, asc_half2int32_rz, vcvt, vector_int32_t, vector_half, 8);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_rz_v2, asc_half2int32_rz_v2, vcvt, vector_int32_t, vector_half, 9);

TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(Vcvt_rd, asc_half2int32_rd, vcvt, vector_int32_t, vector_half, 0);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(Vcvt_rn, asc_half2int32_rn, vcvt, vector_int32_t, vector_half, 1);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(Vcvt_rna, asc_half2int32_rna, vcvt, vector_int32_t, vector_half, 2);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(Vcvt_ru, asc_half2int32_ru, vcvt, vector_int32_t, vector_half, 3);
TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(Vcvt_rz, asc_half2int32_rz, vcvt, vector_int32_t, vector_half, 4);

// ==========asc_int322uint16==========
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt, asc_int322uint16, vcvt, vector_uint16_t, vector_int32_t, 0);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_sat, asc_int322uint16_sat, vcvt, vector_uint16_t, vector_int32_t, 1);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_v2, asc_int322uint16_v2, vcvt, vector_uint16_t, vector_int32_t, 2);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_sat_v2, asc_int322uint16_sat_v2, vcvt, vector_uint16_t, vector_int32_t, 3);

// ==========asc_int642int32==========
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt, asc_int642int32, vcvt, vector_int32_t, vector_int64_t, 0);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_sat, asc_int642int32_sat, vcvt, vector_int32_t, vector_int64_t, 1);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_v2, asc_int642int32_v2, vcvt, vector_int32_t, vector_int64_t, 2);
TEST_VECTOR_COMPUTE_VCVT_INSTR_4(Vcvt_sat_v2, asc_int642int32_sat_v2, vcvt, vector_int32_t, vector_int64_t, 3);
