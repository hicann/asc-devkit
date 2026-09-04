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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"
#include "c_api/npu_arch_2201/utils/test_unary_scalar_instr_utils.h"

TEST_VECTOR_COMPUTE_UNARY_SCALAR_INSTR(Adds, asc_add_scalar, vadds, int16_t);
TEST_VECTOR_COMPUTE_UNARY_SCALAR_INSTR(Adds, asc_add_scalar, vadds, half);
TEST_VECTOR_COMPUTE_UNARY_SCALAR_INSTR(Adds, asc_add_scalar, vadds, int32_t);
TEST_VECTOR_COMPUTE_UNARY_SCALAR_INSTR(Adds, asc_add_scalar, vadds, float);
