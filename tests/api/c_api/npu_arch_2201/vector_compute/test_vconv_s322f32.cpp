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
#include "../utils/test_conv_instr_utils.h"

// ==========asc_int322float==========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322F, asc_int322float, vconv_s322f32, float, int32_t);
// ==========asc_int322float_a==========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322Fa, asc_int322float_rna, vconv_s322f32a, float, int32_t);
// ==========asc_int322float_c=========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322Fc, asc_int322float_ru, vconv_s322f32c, float, int32_t);
// ==========asc_int322float_f==========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322Ff, asc_int322float_rd, vconv_s322f32f, float, int32_t);
// ==========asc_int322float_r==========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322Fr, asc_int322float_rn, vconv_s322f32r, float, int32_t);
// ==========asc_int322float_z==========
TEST_VECTOR_COMPUTE_CONV_INSTR(AscI322Fz, asc_int322float_rz, vconv_s322f32z, float, int32_t);
