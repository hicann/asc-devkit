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
#include "tests/api/c_api/stub/cce_stub.h"
#include "tests/api/c_api/npu_arch_3510/utils/test_binary_instr_utils.h"
#include "include/c_api/asc_simd.h"

TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_uint8_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_int8_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_uint16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_int16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_half);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_uint32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_int32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(VAdd, asc_add, vadd, vector_float);

TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_uint8_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_int8_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_uint16_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_int16_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_half);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_uint32_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_int32_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(VAdd, asc_add, vadd, vector_float);

#define TEST_VECTOR_COMPUTE_ADDC_INSTR(class_name, c_api_name, cce_name, data_type)             \
                                                                                                \
    class TestVectorCompute##class_name####data_type##CApi : public testing::Test {             \
    protected:                                                                                  \
        void SetUp() {}                                                                         \
        void TearDown() {}                                                                      \
    };                                                                                          \
                                                                                                \
    namespace {                                                                                 \
    void cce_name##_##data_type##_Stub(                                                         \
        vector_bool& carry, data_type& dst, data_type src0, data_type src1, vector_bool mask)   \
    {}                                                                                          \
    }                                                                                           \
                                                                                                \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_Succ)   \
    {                                                                                           \
        vector_bool carry;                                                                      \
        data_type dst;                                                                          \
        data_type src0;                                                                         \
        data_type src1;                                                                         \
        vector_bool mask;                                                                       \
                                                                                                \
        MOCKER_CPP(cce_name, void(vector_bool&, data_type&, data_type, data_type, vector_bool)) \
            .times(1)                                                                           \
            .will(invoke(cce_name##_##data_type##_Stub));                                       \
                                                                                                \
        c_api_name(carry, dst, src0, src1, mask);                                               \
        GlobalMockObject::verify();                                                             \
    }

TEST_VECTOR_COMPUTE_ADDC_INSTR(VAddc, asc_add, vaddc, vector_uint32_t);
TEST_VECTOR_COMPUTE_ADDC_INSTR(VAddc, asc_add, vaddc, vector_int32_t);
