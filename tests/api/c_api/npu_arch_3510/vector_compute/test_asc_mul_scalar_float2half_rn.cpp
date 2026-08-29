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

#define TEST_VECTOR_COMPUTE_MUL_SCALAR_FLOAT2HALF_RN_INSTR(                                           \
    class_name, c_api_name, cce_name, dst_type, data_type, scalar_type)                               \
                                                                                                      \
    class TestVectorCompute##class_name##_##data_type##_##scalar_type##_CApi : public testing::Test { \
    protected:                                                                                        \
        void SetUp() {}                                                                               \
        void TearDown() {}                                                                            \
    };                                                                                                \
                                                                                                      \
    namespace {                                                                                       \
    void cce_name##c_api_name##_##data_type##_##scalar_type##_Stub(                                   \
        dst_type& dst, data_type src0, scalar_type src1, vector_bool mask, int mode)                  \
    {}                                                                                                \
    }                                                                                                 \
                                                                                                      \
    TEST_F(                                                                                           \
        TestVectorCompute##class_name##_##data_type##_##scalar_type##_CApi,                           \
        c_api_name##_##data_type##_##scalar_type##_Succ)                                              \
    {                                                                                                 \
        dst_type dst;                                                                                 \
        data_type src0;                                                                               \
        scalar_type src1;                                                                             \
        vector_bool mask;                                                                             \
                                                                                                      \
        MOCKER_CPP(cce_name, void(dst_type&, data_type, scalar_type, vector_bool, int))               \
            .times(2)                                                                                 \
            .will(invoke(cce_name##c_api_name##_##data_type##_##scalar_type##_Stub));                 \
                                                                                                      \
        c_api_name(dst, src0, src1, mask, ASC_POSITION_EVEN);                                         \
        c_api_name(dst, src0, src1, mask, ASC_POSITION_ODD);                                          \
        GlobalMockObject::verify();                                                                   \
    }

TEST_VECTOR_COMPUTE_MUL_SCALAR_FLOAT2HALF_RN_INSTR(
    MUL_SCALAR_FLOAT2HALF_RN, asc_mul_scalar_float2half_rn, vmulscvt, vector_half, vector_float, float);
