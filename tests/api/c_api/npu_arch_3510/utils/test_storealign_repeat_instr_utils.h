/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_STOREALIGN_REPEAT_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_STOREALIGN_REPEAT_INSTR_UTILS_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_INSTR(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                         \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                   \
    protected:                                                                                           \
        void SetUp() {}                                                                                  \
        void TearDown() {}                                                                               \
    };                                                                                                   \
                                                                                                         \
    namespace {                                                                                          \
    void cce_name##_##data_type##_Stub_##index(                                                          \
        vector_##data_type src, __ubuf__ data_type* dst, int32_t offset, vector_bool mask)               \
    {}                                                                                                   \
    }                                                                                                    \
                                                                                                         \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)         \
    {                                                                                                    \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                              \
        vector_##data_type src;                                                                          \
        vector_bool mask;                                                                                \
        uint16_t block_stride = 1;                                                                       \
        uint16_t repeat_stride = 1;                                                                      \
                                                                                                         \
        MOCKER_CPP(cce_name, void(vector_##data_type, __ubuf__ data_type*, int32_t, vector_bool))        \
            .times(1)                                                                                    \
            .will(invoke(cce_name##_##data_type##_Stub_##index));                                        \
                                                                                                         \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                         \
        GlobalMockObject::verify();                                                                      \
    }

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_INSTR_HIF8(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                              \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                        \
    protected:                                                                                                \
        void SetUp() {}                                                                                       \
        void TearDown() {}                                                                                    \
    };                                                                                                        \
                                                                                                              \
    namespace {                                                                                               \
    void cce_name##_##data_type##_Stub_##index(                                                               \
        vector_uint8_t src, __ubuf__ uint8_t* dst, int32_t offset, vector_bool mask)                          \
    {}                                                                                                        \
    }                                                                                                         \
                                                                                                              \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)              \
    {                                                                                                         \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                   \
        vector_##data_type src;                                                                               \
        vector_bool mask;                                                                                     \
        uint16_t block_stride = 1;                                                                            \
        uint16_t repeat_stride = 1;                                                                           \
                                                                                                              \
        MOCKER_CPP(cce_name, void(vector_uint8_t, __ubuf__ uint8_t*, int32_t, vector_bool))                   \
            .times(1)                                                                                         \
            .will(invoke(cce_name##_##data_type##_Stub_##index));                                             \
                                                                                                              \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                              \
        GlobalMockObject::verify();                                                                           \
    }

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_INSTR_INT4B(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                               \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                         \
    protected:                                                                                                 \
        void SetUp() {}                                                                                        \
        void TearDown() {}                                                                                     \
    };                                                                                                         \
                                                                                                               \
    namespace {                                                                                                \
    void cce_name##_##data_type##_Stub_##index(                                                                \
        vector_fp4x2_e1m2_t src, __ubuf__ float4_e1m2x2_t* dst, int32_t offset, vector_bool mask)              \
    {}                                                                                                         \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)               \
    {                                                                                                          \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                    \
        vector_int4x2_t src;                                                                                   \
        vector_bool mask;                                                                                      \
        uint16_t block_stride = 1;                                                                             \
        uint16_t repeat_stride = 1;                                                                            \
                                                                                                               \
        MOCKER_CPP(cce_name, void(vector_fp4x2_e1m2_t, __ubuf__ float4_e1m2x2_t*, int32_t, vector_bool))       \
            .times(1)                                                                                          \
            .will(invoke(cce_name##_##data_type##_Stub_##index));                                              \
                                                                                                               \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                               \
        GlobalMockObject::verify();                                                                            \
    }

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_POST_INSTR(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                              \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                        \
    protected:                                                                                                \
        void SetUp() {}                                                                                       \
        void TearDown() {}                                                                                    \
    };                                                                                                        \
                                                                                                              \
    namespace {                                                                                               \
    uint8_t cce_name##_##data_type##_UpdatedDst_##index;                                                      \
    void cce_name##_##data_type##_PostStub_##index(                                                           \
        vector_##data_type src, __ubuf__ data_type*& dst, int32_t offset, vector_bool mask, Literal post)     \
    {                                                                                                         \
        dst = reinterpret_cast<__ubuf__ data_type*>(&cce_name##_##data_type##_UpdatedDst_##index);            \
    }                                                                                                         \
    }                                                                                                         \
                                                                                                              \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)              \
    {                                                                                                         \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                   \
        vector_##data_type src;                                                                               \
        vector_bool mask;                                                                                     \
        uint16_t block_stride = 1;                                                                            \
        uint16_t repeat_stride = 1;                                                                           \
                                                                                                              \
        MOCKER_CPP(cce_name, void(vector_##data_type, __ubuf__ data_type*&, int32_t, vector_bool, Literal))   \
            .times(1)                                                                                         \
            .will(invoke(cce_name##_##data_type##_PostStub_##index));                                         \
                                                                                                              \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                              \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ data_type*>(&cce_name##_##data_type##_UpdatedDst_##index));  \
        GlobalMockObject::verify();                                                                           \
    }

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_POST_INSTR_HIF8(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                                   \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                             \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
    uint8_t cce_name##_##data_type##_UpdatedDst_##index;                                                           \
    void cce_name##_##data_type##_PostStub_##index(                                                                \
        vector_uint8_t src, __ubuf__ uint8_t*& dst, int32_t offset, vector_bool mask, Literal post)                \
    {                                                                                                              \
        dst = &cce_name##_##data_type##_UpdatedDst_##index;                                                        \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                   \
    {                                                                                                              \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                        \
        vector_##data_type src;                                                                                    \
        vector_bool mask;                                                                                          \
        uint16_t block_stride = 1;                                                                                 \
        uint16_t repeat_stride = 1;                                                                                \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(vector_uint8_t, __ubuf__ uint8_t*&, int32_t, vector_bool, Literal))              \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##data_type##_PostStub_##index));                                              \
                                                                                                                   \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                                   \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ data_type*>(&cce_name##_##data_type##_UpdatedDst_##index));       \
        GlobalMockObject::verify();                                                                                \
    }

#define TEST_VECTOR_DATAMOVE_STOREALIGN_REPEAT_POST_INSTR_INT4B(class_name, c_api_name, cce_name, data_type, index) \
                                                                                                                    \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                              \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void cce_name##_##data_type##_PostStub_##index(                                                                 \
        vector_fp4x2_e1m2_t src, __ubuf__ float4_e1m2x2_t*& dst, int32_t offset, vector_bool mask, Literal post)    \
    {}                                                                                                              \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                    \
    {                                                                                                               \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                         \
        vector_int4x2_t src;                                                                                        \
        vector_bool mask;                                                                                           \
        uint16_t block_stride = 1;                                                                                  \
        uint16_t repeat_stride = 1;                                                                                 \
                                                                                                                    \
        MOCKER_CPP(cce_name, void(vector_fp4x2_e1m2_t, __ubuf__ float4_e1m2x2_t*&, int32_t, vector_bool, Literal))  \
            .times(1)                                                                                               \
            .will(invoke(cce_name##_##data_type##_PostStub_##index));                                               \
                                                                                                                    \
        c_api_name(dst, src, block_stride, repeat_stride, mask);                                                    \
        GlobalMockObject::verify();                                                                                 \
    }

#endif
