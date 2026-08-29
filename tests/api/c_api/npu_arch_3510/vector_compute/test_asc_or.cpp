/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_binary_instr_utils.h"

// ==========asc_or(int8_t/uint8_t/int16_t/uint16_t/half/int32_t/uint32_t/float/bool)==========
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_int16_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_int16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_uint16_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_uint16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_int8_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_int8_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_uint8_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_uint8_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_half);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_int32_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_int32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_uint32_t);
TEST_VECTOR_COMPUTE_BINARY_RETURN_INSTR(ASCVOR, asc_or, vor, vector_uint32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(ASCVOR, asc_or, vor, vector_float);

#define TEST_VECTOR_COMPUTE_BINARY_INSTR_NOLI(class_name, c_api_name, cce_name, data_type)                  \
                                                                                                            \
    class TestVectorCompute##class_name####data_type##CApi : public testing::Test {                         \
    protected:                                                                                              \
        void SetUp() {}                                                                                     \
        void TearDown() {}                                                                                  \
    };                                                                                                      \
                                                                                                            \
    namespace {                                                                                             \
    void cce_name##_##data_type##_Stub(data_type& dst, data_type src0, data_type src1, vector_bool mask) {} \
    }                                                                                                       \
                                                                                                            \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_Succ)               \
    {                                                                                                       \
        data_type dst;                                                                                      \
        data_type src0;                                                                                     \
        data_type src1;                                                                                     \
        vector_bool mask;                                                                                   \
                                                                                                            \
        MOCKER_CPP(cce_name, void(data_type&, data_type, data_type, vector_bool))                           \
            .times(1)                                                                                       \
            .will(invoke(cce_name##_##data_type##_Stub));                                                   \
                                                                                                            \
        c_api_name(dst, src0, src1, mask);                                                                  \
        GlobalMockObject::verify();                                                                         \
    }

#define TEST_VECTOR_COMPUTE_BINARY_INSTR_NOLI_RETURN(class_name, c_api_name, cce_name, data_type)              \
                                                                                                               \
    namespace {                                                                                                \
    void cce_name##_##data_type##_ReturnStub(data_type& dst, data_type src0, data_type src1, vector_bool mask) \
    {                                                                                                          \
        dst = src0;                                                                                            \
    }                                                                                                          \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_ReturnValueSucc)       \
    {                                                                                                          \
        data_type src0{};                                                                                      \
        data_type src1{};                                                                                      \
        vector_bool mask{};                                                                                    \
        static_assert(std::is_same_v<decltype(c_api_name(src0, src1, mask)), data_type>);                      \
                                                                                                               \
        MOCKER_CPP(cce_name, void(data_type&, data_type, data_type, vector_bool))                              \
            .times(1)                                                                                          \
            .will(invoke(cce_name##_##data_type##_ReturnStub));                                                \
                                                                                                               \
        data_type result = c_api_name(src0, src1, mask);                                                       \
        (void)result;                                                                                          \
        GlobalMockObject::verify();                                                                            \
    }

TEST_VECTOR_COMPUTE_BINARY_INSTR_NOLI(ASCVOR, asc_or, por, vector_bool);
TEST_VECTOR_COMPUTE_BINARY_INSTR_NOLI_RETURN(ASCVOR, asc_or, por, vector_bool);
