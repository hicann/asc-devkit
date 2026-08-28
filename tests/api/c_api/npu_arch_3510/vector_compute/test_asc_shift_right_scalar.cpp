/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/npu_arch_3510/utils/test_binary_scalar_instr_utils.h"
#include "tests/api/c_api/npu_arch_3510/utils/test_return_instr_utils.h"

TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint8_t, int16_t);
TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int8_t, int16_t);
TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint16_t, int16_t);
TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int16_t, int16_t);
TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint32_t, int16_t);
TEST_VECTOR_COMPUTE_BINARY_SCALAR_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int32_t, int16_t);

TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint8_t, int16_t)
TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int8_t, int16_t)
TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint16_t, int16_t)
TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int16_t, int16_t)
TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_uint32_t, int16_t)
TEST_VECTOR_COMPUTE_BINARY_SCALAR_RETURN_INSTR(Vshrs, asc_shiftright_scalar, vshrs, vector_int32_t, int16_t)
