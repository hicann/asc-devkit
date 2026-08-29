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
#include <type_traits>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_UNSQUEEZE(data_type)                                                            \
                                                                                                            \
    class TestVectorComputeUnsqueeze##data_type##CApi : public testing::Test {                              \
    protected:                                                                                              \
        void SetUp() {}                                                                                     \
        void TearDown() {}                                                                                  \
    };                                                                                                      \
                                                                                                            \
    namespace {                                                                                             \
    void vusqz##_##data_type##_Stub(data_type& dst, vector_bool mask) {}                                    \
    }                                                                                                       \
                                                                                                            \
    TEST_F(TestVectorComputeUnsqueeze##data_type##CApi, c_api_asc_unsqueeze_##data_type##_Succ)             \
    {                                                                                                       \
        data_type dst;                                                                                      \
        vector_bool mask;                                                                                   \
                                                                                                            \
        MOCKER_CPP(vusqz, void(data_type&, vector_bool)).times(1).will(invoke(vusqz##_##data_type##_Stub)); \
                                                                                                            \
        asc_unsqueeze(dst, mask);                                                                           \
        GlobalMockObject::verify();                                                                         \
    }

TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_uint8_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_int8_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_uint16_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_int16_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_uint32_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE(vector_int32_t);

#define TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(c_api_name, data_type)                                   \
    TEST(TestVectorComputeUnsqueezeReturn, data_type)                                                       \
    {                                                                                                       \
        vector_bool mask;                                                                                   \
                                                                                                            \
        static_assert(std::is_same_v<decltype(c_api_name(mask)), data_type>);                               \
        MOCKER_CPP(vusqz, void(data_type&, vector_bool)).times(1).will(invoke(vusqz##_##data_type##_Stub)); \
                                                                                                            \
        data_type dst = c_api_name(mask);                                                                   \
        (void)dst;                                                                                          \
        GlobalMockObject::verify();                                                                         \
    }

TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_u8, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_s8, vector_int8_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_u16, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_s16, vector_int16_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_u32, vector_uint32_t);
TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR(asc_unsqueeze_s32, vector_int32_t);

#undef TEST_VECTOR_COMPUTE_UNSQUEEZE_RETURN_INSTR
