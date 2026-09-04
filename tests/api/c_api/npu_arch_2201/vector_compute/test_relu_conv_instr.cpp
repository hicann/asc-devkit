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
#include "../utils/test_binary_instr_utils.h"

// ==========asc_add_relu_cast_f162s8==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(AddReluF162S8, asc_add_relu, vaddreluconv_f162s8, int8_t, half);
// ==========asc_add_relu_cast_f322f16==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(AddReluF322F16, asc_add_relu, vaddreluconv_f322f16, half, float);
// ==========asc_add_relu_cast_s162s8==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(AddReluS162S8, asc_add_relu, vaddreluconv_s162s8, int8_t, int16_t);

// ==========asc_sub_relu_cast_f162s8==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(SubReluF162S8, asc_sub_relu, vsubreluconv_f162s8, int8_t, half);
// ==========asc_sub_relu_cast_f322f16==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(SubReluF322F16, asc_sub_relu, vsubreluconv_f322f16, half, float);
// ==========asc_sub_relu_cast_s162s8==========
TEST_VECTOR_COMPUTE_RELU_CONV_INSTR(SubReluS162S8, asc_sub_relu, vsubreluconv_s162s8, int8_t, int16_t);
