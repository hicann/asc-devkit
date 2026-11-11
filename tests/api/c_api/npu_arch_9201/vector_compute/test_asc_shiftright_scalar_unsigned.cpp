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
#include "c_api/asc_simd.h"

#define TEST_SCALAR_SHIFTRIGHT_INSTR(data_type, api_scalar_type, instr_scalar_type)             \
                                                                                                \
    class TestScalarShiftRight##data_type##_##api_scalar_type##_CApi : public testing::Test {   \
    protected:                                                                                  \
        void SetUp() {}                                                                         \
        void TearDown() {}                                                                      \
    };                                                                                          \
                                                                                                \
    namespace {                                                                                 \
    void vshrs##_##data_type##_##api_scalar_type##_UnsignedStub(                                \
        data_type& dst, data_type src, instr_scalar_type shift, vector_bool mask, Literal mode) \
    {}                                                                                          \
    }                                                                                           \
                                                                                                \
    TEST_F(                                                                                     \
        TestScalarShiftRight##data_type##_##api_scalar_type##_CApi,                             \
        asc_shiftright_scalar_##data_type##_##api_scalar_type##_Succ)                           \
    {                                                                                           \
        data_type dst;                                                                          \
        data_type src;                                                                          \
        api_scalar_type shift;                                                                  \
        vector_bool mask;                                                                       \
                                                                                                \
        MOCKER_CPP(vshrs, void(data_type&, data_type, instr_scalar_type, vector_bool, Literal)) \
            .times(1)                                                                           \
            .will(invoke(vshrs##_##data_type##_##api_scalar_type##_UnsignedStub));              \
                                                                                                \
        asc_shiftright_scalar(dst, src, shift, mask);                                           \
        GlobalMockObject::verify();                                                             \
    }

#define TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(data_type, api_scalar_type, instr_scalar_type)      \
                                                                                                \
    namespace {                                                                                 \
    void vshrs##_##data_type##_##api_scalar_type##_UnsignedRetStub(                             \
        data_type& dst, data_type src, instr_scalar_type shift, vector_bool mask, Literal mode) \
    {}                                                                                          \
    }                                                                                           \
                                                                                                \
    TEST(                                                                                       \
        TestScalarShiftRightReturn##data_type##_##api_scalar_type##_CApi,                       \
        asc_shiftright_scalar_##data_type##_##api_scalar_type##_RetSucc)                        \
    {                                                                                           \
        data_type src;                                                                          \
        api_scalar_type shift;                                                                  \
        vector_bool mask;                                                                       \
                                                                                                \
        MOCKER_CPP(vshrs, void(data_type&, data_type, instr_scalar_type, vector_bool, Literal)) \
            .times(1)                                                                           \
            .will(invoke(vshrs##_##data_type##_##api_scalar_type##_UnsignedRetStub));           \
                                                                                                \
        data_type result = asc_shiftright_scalar(src, shift, mask);                             \
        (void)result;                                                                           \
        GlobalMockObject::verify();                                                             \
    }

TEST_SCALAR_SHIFTRIGHT_INSTR(vector_uint8_t, uint8_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_INSTR(vector_int8_t, uint8_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_INSTR(vector_uint16_t, uint16_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_INSTR(vector_int16_t, uint16_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_INSTR(vector_uint32_t, uint32_t, uint32_t);
TEST_SCALAR_SHIFTRIGHT_INSTR(vector_int32_t, uint32_t, uint32_t);

TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_uint8_t, uint8_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_int8_t, uint8_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_uint16_t, uint16_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_int16_t, uint16_t, uint16_t);
TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_uint32_t, uint32_t, uint32_t);
TEST_SCALAR_SHIFTRIGHT_RETURN_INSTR(vector_int32_t, uint32_t, uint32_t);

TEST(TestScalarShiftRightLiteralCApi, asc_shiftright_scalar_vector_uint16_t_literal_Succ)
{
    vector_uint16_t dst;
    vector_uint16_t src;
    vector_bool mask;

    MOCKER_CPP(vshrs, void(vector_uint16_t&, vector_uint16_t, uint16_t, vector_bool, Literal)).times(2);

    asc_shiftright_scalar(dst, src, 1, mask);
    vector_uint16_t result = asc_shiftright_scalar(src, 1, mask);
    (void)result;
    GlobalMockObject::verify();
}
