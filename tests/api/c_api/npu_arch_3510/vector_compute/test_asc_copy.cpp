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
#include "c_api/reg_compute/reg_copy.h"

#define TEST_VECTOR_COMPUTE_COPY_INSTR_0(class_name, c_api_name, cce_name, data_type)                     \
                                                                                                          \
    class TestVectorCompute##class_name##_##data_type##_CApi : public testing::Test {                     \
    protected:                                                                                            \
        void SetUp() {}                                                                                   \
        void TearDown() {}                                                                                \
    };                                                                                                    \
                                                                                                          \
    namespace {                                                                                           \
    void cce_name##_##data_type##_Stub(data_type& dst, data_type src0, vector_bool mask, int32_t mode) {} \
    }                                                                                                     \
                                                                                                          \
    TEST_F(TestVectorCompute##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)           \
    {                                                                                                     \
        data_type dst;                                                                                    \
        data_type src0;                                                                                   \
        vector_bool mask;                                                                                 \
                                                                                                          \
        MOCKER_CPP(cce_name, void(data_type&, data_type, vector_bool, int32_t))                           \
            .times(1)                                                                                     \
            .will(invoke(cce_name##_##data_type##_Stub));                                                 \
                                                                                                          \
        c_api_name(dst, src0, mask);                                                                      \
        GlobalMockObject::verify();                                                                       \
    }

#define TEST_VECTOR_COMPUTE_COPY_INSTR_1(class_name, c_api_name, cce_name, data_type)           \
                                                                                                \
    class TestVectorCompute##class_name##_##data_type##_CApi : public testing::Test {           \
    protected:                                                                                  \
        void SetUp() {}                                                                         \
        void TearDown() {}                                                                      \
    };                                                                                          \
                                                                                                \
    namespace {                                                                                 \
    void cce_name##_##data_type##_Stub1(data_type& dst, data_type src0, vector_bool mask) {}    \
    }                                                                                           \
                                                                                                \
    TEST_F(TestVectorCompute##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ) \
    {                                                                                           \
        data_type dst;                                                                          \
        data_type src0;                                                                         \
        vector_bool mask;                                                                       \
                                                                                                \
        MOCKER_CPP(cce_name, void(data_type&, data_type, vector_bool))                          \
            .times(1)                                                                           \
            .will(invoke(cce_name##_##data_type##_Stub1));                                      \
                                                                                                \
        c_api_name(dst, src0, mask);                                                            \
        GlobalMockObject::verify();                                                             \
    }

#define TEST_VECTOR_COMPUTE_COPY_INSTR_2(class_name, c_api_name, cce_name, data_type)                            \
                                                                                                                 \
    class TestVectorCompute##class_name##_##data_type##_CApi : public testing::Test {                            \
    protected:                                                                                                   \
        void SetUp() {}                                                                                          \
        void TearDown() {}                                                                                       \
    };                                                                                                           \
                                                                                                                 \
    namespace {                                                                                                  \
    void cce_name##_##data_type##_Stub2(data_type& dst, data_type src0) {}                                       \
    }                                                                                                            \
                                                                                                                 \
    TEST_F(TestVectorCompute##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                  \
    {                                                                                                            \
        data_type dst;                                                                                           \
        data_type src0;                                                                                          \
                                                                                                                 \
        MOCKER_CPP(cce_name, void(data_type&, data_type)).times(1).will(invoke(cce_name##_##data_type##_Stub2)); \
                                                                                                                 \
        c_api_name(dst, src0);                                                                                   \
        GlobalMockObject::verify();                                                                              \
    }

// ==========asc_copy(u8/s8/half/u16/s16/float/u32/s32/s64/bf16)=========
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_int8_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_uint8_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_int16_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_uint16_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_int32_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_uint32_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_half);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_float);
TEST_VECTOR_COMPUTE_COPY_INSTR_0(COPY, asc_copy, vmov, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_COPY_INSTR_1(COPY1, asc_copy, pmov, vector_bool);
TEST_VECTOR_COMPUTE_COPY_INSTR_2(COPY2, asc_copy, pmov, vector_bool);
