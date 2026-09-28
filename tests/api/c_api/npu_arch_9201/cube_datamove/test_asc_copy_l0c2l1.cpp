/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_9201/utils/test_copy_l0c2l1_instr_utils.h"
#include "c_api/asc_simd.h"

//================asc_copy_l0c2l1 basic (enum + enable_nz2dn)================
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, float, 1);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, bfloat16_t, float, 2);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, float, 3);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, float, 4);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, float, float, 5);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, int32_t, 6);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, int32_t, 7);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, int32_t, 8);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int32_t, int32_t, 9);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, hifloat8_t, float, 10);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, fp8_e4m3fn_t, float, 11);
TEST_CUBE_DATAMOVE_L0C2L1_BASIC(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, bfloat16_t, int32_t, 12);

//================asc_copy_l0c2l1 quant_pre_rnd (9201 unique)================
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, float, 51);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, bfloat16_t, float, 52);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, float, 53);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, float, 54);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, float, float, 55);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, int32_t, 56);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, int32_t, 57);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, int32_t, 58);
TEST_CUBE_DATAMOVE_L0C2L1_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int32_t, int32_t, 59);

//================asc_copy_l0c2l1 int4b_t s4================
TEST_CUBE_DATAMOVE_L0C2L1_S4(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, float, 301);
TEST_CUBE_DATAMOVE_L0C2L1_S4(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, int32_t, 302);
TEST_CUBE_DATAMOVE_L0C2L1_S4_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, float, 303);
TEST_CUBE_DATAMOVE_L0C2L1_S4_RND(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, int32_t, 304);
