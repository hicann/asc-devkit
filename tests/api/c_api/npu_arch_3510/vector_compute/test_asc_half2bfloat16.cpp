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

// rd(sat/no-sat)
TEST_VECTOR_COMPUTE_VCVT_INSTR_2(F322Bf16Rd, asc_half2bfloat16_rd, vcvt, vector_bfloat16_t, vector_half, 0);

// rn(sat/no-sat)
TEST_VECTOR_COMPUTE_VCVT_INSTR_2(F322Bf16Rn, asc_half2bfloat16_rn, vcvt, vector_bfloat16_t, vector_half, 10);

// rna(sat/no-sat)
TEST_VECTOR_COMPUTE_VCVT_INSTR_2(F322Bf16Rna, asc_half2bfloat16_rna, vcvt, vector_bfloat16_t, vector_half, 20);

// ru(sat/no-sat)
TEST_VECTOR_COMPUTE_VCVT_INSTR_2(F322Bf16Ru, asc_half2bfloat16_ru, vcvt, vector_bfloat16_t, vector_half, 40);

// rz(sat/no-sat)
TEST_VECTOR_COMPUTE_VCVT_INSTR_2(F322Bf16Rz, asc_half2bfloat16_rz, vcvt, vector_bfloat16_t, vector_half, 50);

TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(F322Bf16Rd, asc_half2bfloat16_rd, vcvt, vector_bfloat16_t, vector_half, 0);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(F322Bf16Rn, asc_half2bfloat16_rn, vcvt, vector_bfloat16_t, vector_half, 10);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(F322Bf16Rna, asc_half2bfloat16_rna, vcvt, vector_bfloat16_t, vector_half, 20);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(F322Bf16Ru, asc_half2bfloat16_ru, vcvt, vector_bfloat16_t, vector_half, 40);
TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(F322Bf16Rz, asc_half2bfloat16_rz, vcvt, vector_bfloat16_t, vector_half, 50);
