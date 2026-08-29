/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_VCMP_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_VCMP_INSTR_UTILS_H

#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_VCMP_INSTR(class_name, c_api_name, cce_name, data_type)                           \
                                                                                                              \
    class TestVectorCompute##class_name##_##data_type##_CApi : public testing::Test {                         \
    protected:                                                                                                \
        void SetUp() {}                                                                                       \
        void TearDown() {}                                                                                    \
    };                                                                                                        \
                                                                                                              \
    namespace {                                                                                               \
    void cce_name##_##data_type##_Stub(vector_bool& dst, data_type src0, data_type src1, vector_bool mask) {} \
    }                                                                                                         \
                                                                                                              \
    TEST_F(TestVectorCompute##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)               \
    {                                                                                                         \
        vector_bool dst;                                                                                      \
        data_type src0;                                                                                       \
        data_type src1;                                                                                       \
        vector_bool mask;                                                                                     \
                                                                                                              \
        MOCKER_CPP(cce_name, void(vector_bool&, data_type, data_type, vector_bool))                           \
            .times(1)                                                                                         \
            .will(invoke(cce_name##_##data_type##_Stub));                                                     \
                                                                                                              \
        c_api_name(dst, src0, src1, mask);                                                                    \
        GlobalMockObject::verify();                                                                           \
    }

#define TEST_VECTOR_COMPUTE_VCMP_INSTR_RETURN(class_name, c_api_name, cce_name, data_type)                       \
    namespace {                                                                                                  \
    void cce_name##_##data_type##_ReturnStub(vector_bool& dst, data_type src0, data_type src1, vector_bool mask) \
    {                                                                                                            \
        dst = mask;                                                                                              \
    }                                                                                                            \
    }                                                                                                            \
                                                                                                                 \
    TEST_F(TestVectorCompute##class_name##_##data_type##_CApi, c_api_name##_##data_type##_ReturnValueSucc)       \
    {                                                                                                            \
        data_type src0{};                                                                                        \
        data_type src1{};                                                                                        \
        vector_bool mask{};                                                                                      \
        static_assert(std::is_same_v<decltype(c_api_name(src0, src1, mask)), vector_bool>);                      \
                                                                                                                 \
        MOCKER_CPP(cce_name, void(vector_bool&, data_type, data_type, vector_bool))                              \
            .times(1)                                                                                            \
            .will(invoke(cce_name##_##data_type##_ReturnStub));                                                  \
                                                                                                                 \
        vector_bool result = c_api_name(src0, src1, mask);                                                       \
        (void)result;                                                                                            \
        GlobalMockObject::verify();                                                                              \
    }

#endif
