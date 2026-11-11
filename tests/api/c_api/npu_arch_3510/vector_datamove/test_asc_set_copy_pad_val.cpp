/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

class TestVectorDatamoveSetMovPadValue : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
uint64_t g_test_asc_set_copy_pad_val = 0;
void set_mov_pad_val_stub(uint64_t config) { EXPECT_EQ(g_test_asc_set_copy_pad_val, config); }
} // namespace

#define TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(data_type, type_value, u64_value)                 \
    TEST_F(TestVectorDatamoveSetMovPadValue, asc_set_mov_pad_value_##data_type##_succ)           \
    {                                                                                            \
        data_type value = type_value;                                                            \
        g_test_asc_set_copy_pad_val = u64_value;                                                 \
        MOCKER_CPP(set_mov_pad_val, void(uint64_t)).times(1).will(invoke(set_mov_pad_val_stub)); \
        asc_set_copy_pad_val(value);                                                             \
        GlobalMockObject::verify();                                                              \
    }

//===========asc_set_copy_pad_val(int8/uint8/e2m1/e1m2/e8m0/e5m2/e4m3fn/int16/uint16/half/bfloat16/int32/uint32/float)===========
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(int8_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(uint8_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(int16_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(uint16_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(half, 1, 0x3c00);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(bfloat16_t, 1, 0x3f80);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(int32_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(uint32_t, 1, 0x1);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(float, 1, 0x3f800000);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(int64_t, 0x123456789abcdefLL, 0x123456789abcdefULL);
TEST_VECTOR_DATAMOVE_SET_MOV_PAD_VALUE(uint64_t, 0xfedcba9876543210ULL, 0xfedcba9876543210ULL);
