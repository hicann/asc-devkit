/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "tests/api/c_api/npu_arch_3510/utils/test_vcmp_instr_utils.h"
#include "tests/api/c_api/npu_arch_3510/utils/test_vcmps_instr_utils.h"

// ==========asc_eq(vcmp int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_int8_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_int8_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_uint8_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_uint8_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_int16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_int16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_uint16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_uint16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_half);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_half);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_int32_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_int32_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_uint32_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_uint32_t);
TEST_VECTOR_COMPUTE_VCMP_INSTR(ASCVEQ, asc_eq, vcmp_eq, vector_float);
TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(ASCVEQ, asc_eq, vcmp_eq, vector_float);

// ==========asc_eq_scalar(vcmps int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int8_t, int8_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int8_t, int8_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint8_t, uint8_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint8_t, uint8_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int16_t, int16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int16_t, int16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint16_t, uint16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint16_t, uint16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_half, half);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_half, half);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int32_t, int32_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_int32_t, int32_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint32_t, uint32_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_uint32_t, uint32_t);
TEST_VECTOR_COMPUTE_VCMPS_INSTR(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_float, float);
TEST_VECTOR_COMPUTE_VCMPS_INSTR_RETURN(ASCVEQS, asc_eq_scalar, vcmps_eq, vector_float, float);
