/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_unary_instr_utils.h"

TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_int8_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_int8_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_int16_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_int16_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_half);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_half);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_uint32_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_uint32_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_int32_t);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_int32_t);
TEST_VECTOR_COMPUTE_UNARY_INSTR(VNot, asc_not, vnot, vector_float);
TEST_VECTOR_COMPUTE_UNARY_RETURN_INSTR(VNot, asc_not, vnot, vector_float);

class TestVectorComputePNotVectorBoolCApi : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void pnot_vector_bool_Stub(vector_bool& dst, vector_bool src, vector_bool mask) {}

void pnot_vector_bool_ReturnStub(vector_bool& dst, vector_bool src, vector_bool mask) { dst = src; }
} // namespace

TEST_F(TestVectorComputePNotVectorBoolCApi, asc_not_vector_bool_Succ)
{
    vector_bool dst;
    vector_bool src;
    vector_bool mask;

    MOCKER_CPP(pnot, void(vector_bool&, vector_bool, vector_bool)).times(1).will(invoke(pnot_vector_bool_Stub));
    asc_not(dst, src, mask);
    GlobalMockObject::verify();
}

TEST_F(TestVectorComputePNotVectorBoolCApi, asc_not_vector_bool_ReturnValueSucc)
{
    vector_bool src{};
    vector_bool mask{};
    static_assert(std::is_same_v<decltype(asc_not(src, mask)), vector_bool>);

    MOCKER_CPP(pnot, void(vector_bool&, vector_bool, vector_bool)).times(1).will(invoke(pnot_vector_bool_ReturnStub));
    vector_bool result = asc_not(src, mask);
    (void)result;
    GlobalMockObject::verify();
}
