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
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_EXP_SUB(class_name, c_api_name, cce_name, dst_type, src_type)  \
                                                                                           \
    class TestVectorCompute##class_name##dst_type##src_type##CApi : public testing::Test { \
    protected:                                                                             \
        void SetUp() {}                                                                    \
        void TearDown() {}                                                                 \
    };                                                                                     \
                                                                                           \
    namespace {                                                                            \
                                                                                           \
    void c_api_name##_##dst_type##_##src_type##_vector_bool_Stub(                          \
        dst_type& dst, src_type src0, src_type src1, vector_bool mask, int part)           \
    {}                                                                                     \
    }                                                                                      \
                                                                                           \
    TEST_F(TestVectorCompute##class_name##dst_type##src_type##CApi, c_api_name##_Succ)     \
    {                                                                                      \
        dst_type dst;                                                                      \
        src_type src0;                                                                     \
        src_type src1;                                                                     \
        vector_bool mask;                                                                  \
                                                                                           \
        MOCKER_CPP(cce_name, void(dst_type&, src_type, src_type, vector_bool, int))        \
            .times(1)                                                                      \
            .will(invoke(c_api_name##_##dst_type##_##src_type##_vector_bool_Stub));        \
                                                                                           \
        c_api_name(dst, src0, src1, mask);                                                 \
        GlobalMockObject::verify();                                                        \
    }

TEST_VECTOR_COMPUTE_EXP_SUB(ExpSub, asc_exp_sub, vexpdif, vector_float, vector_half);
TEST_VECTOR_COMPUTE_EXP_SUB(ExpSub, asc_exp_sub, vexpdif, vector_float, vector_float);
TEST_VECTOR_COMPUTE_EXP_SUB(ExpSubV2, asc_exp_sub_v2, vexpdif, vector_float, vector_half);
TEST_VECTOR_COMPUTE_EXP_SUB(ExpSubV2, asc_exp_sub_v2, vexpdif, vector_float, vector_float);

TEST(RegExpSubHalf2FloatPositionOverloads, AcceptAllLegalSourcePositionArguments)
{
    vector_float dst;
    vector_half src0;
    vector_half src1;
    vector_bool mask;

    asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_EVEN);
    asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_ODD);
}
