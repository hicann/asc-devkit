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

#define TEST_VECTOR_COMPUTE_TRANSPOSE(class_name, data_type)                            \
                                                                                        \
    class TestTranspose##class_name : public testing::Test {                            \
    protected:                                                                          \
        void SetUp() {}                                                                 \
        void TearDown() {}                                                              \
    };                                                                                  \
                                                                                        \
    namespace {                                                                         \
    void transpose_##data_type##_Stub(__ubuf__ data_type* dst, __ubuf__ data_type* src) \
    {                                                                                   \
        __ubuf__ data_type* dst_target = reinterpret_cast<__ubuf__ data_type*>(11);     \
        __ubuf__ data_type* src_target = reinterpret_cast<__ubuf__ data_type*>(22);     \
                                                                                        \
        EXPECT_EQ(dst_target, dst);                                                     \
        EXPECT_EQ(src_target, src);                                                     \
    }                                                                                   \
    }                                                                                   \
                                                                                        \
    TEST_F(TestTranspose##class_name, asc_transpose_##data_type##_Succ)                 \
    {                                                                                   \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);            \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);            \
                                                                                        \
        MOCKER_CPP(vtranspose, void(__ubuf__ data_type*, __ubuf__ data_type*))          \
            .times(1)                                                                   \
            .will(invoke(transpose_##data_type##_Stub));                                \
                                                                                        \
        asc_transpose(dst, src);                                                        \
        GlobalMockObject::verify();                                                     \
    }                                                                                   \
                                                                                        \
    TEST_F(TestTranspose##class_name, asc_transpose_##data_type##_sync_Succ)            \
    {                                                                                   \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);            \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);            \
                                                                                        \
        MOCKER_CPP(vtranspose, void(__ubuf__ data_type*, __ubuf__ data_type*))          \
            .times(1)                                                                   \
            .will(invoke(transpose_##data_type##_Stub));                                \
                                                                                        \
        asc_transpose_sync(dst, src);                                                   \
        GlobalMockObject::verify();                                                     \
    }

TEST_VECTOR_COMPUTE_TRANSPOSE(Int16, int16_t);
TEST_VECTOR_COMPUTE_TRANSPOSE(Uint16, uint16_t);
