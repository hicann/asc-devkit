/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_logic_compute_instr_utils.h"
#include "tests/api/c_api/npu_arch_3510/utils/test_return_instr_utils.h"

// TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint8_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int8_t);
// TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint16_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int16_t);
// TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint32_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int32_t);

// TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint8_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_int8_t);
// TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint16_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_int16_t);
// TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint32_t);
TEST_LOGIC_COMPUTE_INSTR(VShiftRight, asc_shiftright, vshr, vector_int32_t);

TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint8_t, vector_int8_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int8_t, vector_int8_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint16_t, vector_int16_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int16_t, vector_int16_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_uint32_t, vector_int32_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftLeft, asc_shiftleft, vshl, vector_int32_t, vector_int32_t)

TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint8_t, vector_int8_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_int8_t, vector_int8_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint16_t, vector_int16_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_int16_t, vector_int16_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_uint32_t, vector_int32_t)
TEST_LOGIC_COMPUTE_RETURN_INSTR(VShiftRight, asc_shiftright, vshr, vector_int32_t, vector_int32_t)
