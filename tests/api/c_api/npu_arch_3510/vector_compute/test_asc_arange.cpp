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

#define TEST_VECTOR_COMPUTE_ARANGE(data_type, index_type)                                                       \
                                                                                                                \
    class TestVectorComputeArange##data_type##CApi : public testing::Test {                                     \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void vci##_##data_type##_Stub(data_type& dst, index_type index, Literal order) {}                           \
    }                                                                                                           \
                                                                                                                \
    TEST_F(TestVectorComputeArange##data_type##CApi, c_api_asc_arange_##data_type##_Succ)                       \
    {                                                                                                           \
        data_type dst;                                                                                          \
        index_type index;                                                                                       \
                                                                                                                \
        MOCKER_CPP(vci, void(data_type&, index_type, Literal)).times(2).will(invoke(vci##_##data_type##_Stub)); \
                                                                                                                \
        asc_arange(dst, index);                                                                                 \
        asc_arange_descend(dst, index);                                                                         \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_ARANGE_RETURN(data_type, index_type)                                                       \
                                                                                                                       \
    class TestVectorComputeReturnArange##data_type##CApi : public testing::Test {                                      \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    void vci##_##data_type##_Return_Stub(data_type& dst, index_type index, Literal order) {}                           \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestVectorComputeReturnArange##data_type##CApi, c_api_asc_arange_return_##data_type##_Succ)                 \
    {                                                                                                                  \
        index_type index;                                                                                              \
                                                                                                                       \
        MOCKER_CPP(vci, void(data_type&, index_type, Literal)).times(2).will(invoke(vci##_##data_type##_Return_Stub)); \
                                                                                                                       \
        data_type dst0 = asc_arange(index);                                                                            \
        data_type dst1 = asc_arange_descend(index);                                                                    \
        (void)dst0;                                                                                                    \
        (void)dst1;                                                                                                    \
        GlobalMockObject::verify();                                                                                    \
    }

TEST_VECTOR_COMPUTE_ARANGE(vector_int8_t, int8_t);
TEST_VECTOR_COMPUTE_ARANGE(vector_int16_t, int16_t);
TEST_VECTOR_COMPUTE_ARANGE(vector_int32_t, int32_t);
TEST_VECTOR_COMPUTE_ARANGE(vector_half, half);
TEST_VECTOR_COMPUTE_ARANGE(vector_float, float);

TEST_VECTOR_COMPUTE_ARANGE_RETURN(vector_int8_t, int8_t);
TEST_VECTOR_COMPUTE_ARANGE_RETURN(vector_int16_t, int16_t);
TEST_VECTOR_COMPUTE_ARANGE_RETURN(vector_int32_t, int32_t);
TEST_VECTOR_COMPUTE_ARANGE_RETURN(vector_half, half);
TEST_VECTOR_COMPUTE_ARANGE_RETURN(vector_float, float);
