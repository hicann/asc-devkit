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

#define TEST_VECTOR_DATAMOVE_STORE(class_name, c_api_name, cce_name0, cce_name1, data_type)                           \
                                                                                                                      \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                                \
    protected:                                                                                                        \
        void SetUp() {}                                                                                               \
        void TearDown() {}                                                                                            \
    };                                                                                                                \
                                                                                                                      \
    namespace {                                                                                                       \
    void cce_name0##_##data_type##_Stub0(                                                                             \
        vector_store_unalign& src0, uint32_t offset, vector_##data_type src1, __ubuf__ data_type*& dst, Literal post) \
    {                                                                                                                 \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                                  \
    }                                                                                                                 \
    void cce_name1##_##data_type##_Stub1(vector_store_unalign src, __ubuf__ data_type* dst, int32_t offset) {}        \
    }                                                                                                                 \
                                                                                                                      \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                      \
    {                                                                                                                 \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                           \
        vector_##data_type src;                                                                                       \
                                                                                                                      \
        MOCKER_CPP(                                                                                                   \
            cce_name0, void(vector_store_unalign&, uint32_t, vector_##data_type, __ubuf__ data_type*&, Literal))      \
            .times(1)                                                                                                 \
            .will(invoke(cce_name0##_##data_type##_Stub0));                                                           \
                                                                                                                      \
        MOCKER_CPP(cce_name1, void(vector_store_unalign, __ubuf__ data_type*, int32_t))                               \
            .times(1)                                                                                                 \
            .will(invoke(cce_name1##_##data_type##_Stub1));                                                           \
                                                                                                                      \
        c_api_name(dst, src);                                                                                         \
        GlobalMockObject::verify();                                                                                   \
    }

#define TEST_VECTOR_DATAMOVE_STORE_HIF8(class_name, c_api_name, cce_name0, cce_name1, data_type)                  \
                                                                                                                  \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                            \
    protected:                                                                                                    \
        void SetUp() {}                                                                                           \
        void TearDown() {}                                                                                        \
    };                                                                                                            \
                                                                                                                  \
    namespace {                                                                                                   \
    void cce_name0##_##data_type##_Stub0(                                                                         \
        vector_store_unalign& src0, uint32_t offset, vector_uint8_t src1, __ubuf__ uint8_t*& dst, Literal post)   \
    {                                                                                                             \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                              \
    }                                                                                                             \
    void cce_name1##_##data_type##_Stub1(vector_store_unalign src, __ubuf__ uint8_t* dst, int32_t offset) {}      \
    }                                                                                                             \
                                                                                                                  \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                  \
    {                                                                                                             \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                       \
        vector_##data_type src;                                                                                   \
                                                                                                                  \
        MOCKER_CPP(cce_name0, void(vector_store_unalign&, uint32_t, vector_uint8_t, __ubuf__ uint8_t*&, Literal)) \
            .times(1)                                                                                             \
            .will(invoke(cce_name0##_##data_type##_Stub0));                                                       \
                                                                                                                  \
        MOCKER_CPP(cce_name1, void(vector_store_unalign, __ubuf__ uint8_t*, int32_t))                             \
            .times(1)                                                                                             \
            .will(invoke(cce_name1##_##data_type##_Stub1));                                                       \
                                                                                                                  \
        c_api_name(dst, src);                                                                                     \
        GlobalMockObject::verify();                                                                               \
    }

#define TEST_VECTOR_DATAMOVE_STORE_INT4B(class_name, c_api_name, cce_name0, cce_name1, data_type)                    \
                                                                                                                     \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                               \
    protected:                                                                                                       \
        void SetUp() {}                                                                                              \
        void TearDown() {}                                                                                           \
    };                                                                                                               \
                                                                                                                     \
    namespace {                                                                                                      \
    void cce_name0##_##data_type##_Stub0(                                                                            \
        vector_store_unalign& src0, uint32_t offset, vector_fp4x2_e1m2_t src1, __ubuf__ float4_e1m2x2_t*& dst,       \
        Literal post)                                                                                                \
    {                                                                                                                \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                                 \
    }                                                                                                                \
    void cce_name1##_##data_type##_Stub1(vector_store_unalign src, __ubuf__ float4_e1m2x2_t* dst, int32_t offset) {} \
    }                                                                                                                \
                                                                                                                     \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                     \
    {                                                                                                                \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                          \
        vector_int4x2_t src;                                                                                         \
                                                                                                                     \
        MOCKER_CPP(                                                                                                  \
            cce_name0,                                                                                               \
            void(vector_store_unalign&, uint32_t, vector_fp4x2_e1m2_t, __ubuf__ float4_e1m2x2_t*&, Literal))         \
            .times(1)                                                                                                \
            .will(invoke(cce_name0##_##data_type##_Stub0));                                                          \
                                                                                                                     \
        MOCKER_CPP(cce_name1, void(vector_store_unalign, __ubuf__ float4_e1m2x2_t*, int32_t))                        \
            .times(1)                                                                                                \
            .will(invoke(cce_name1##_##data_type##_Stub1));                                                          \
                                                                                                                     \
        c_api_name(dst, src);                                                                                        \
        GlobalMockObject::verify();                                                                                  \
    }

#define TEST_VECTOR_DATAMOVE_STORE3(class_name, c_api_name, cce_name2, cce_name3, data_type)                          \
                                                                                                                      \
    class TestVectorDataMove3##class_name##_##data_type##_CApi : public testing::Test {                               \
    protected:                                                                                                        \
        void SetUp() {}                                                                                               \
        void TearDown() {}                                                                                            \
    };                                                                                                                \
                                                                                                                      \
    namespace {                                                                                                       \
    void cce_name2##_##data_type##_Stub2(                                                                             \
        vector_store_unalign& src0, uint32_t offset, vector_##data_type src1, __ubuf__ data_type*& dst, Literal post) \
    {                                                                                                                 \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                                  \
    }                                                                                                                 \
    void cce_name3##_##data_type##_Stub3(vector_store_unalign src, __ubuf__ data_type* dst, int32_t offset) {}        \
    }                                                                                                                 \
                                                                                                                      \
    TEST_F(TestVectorDataMove3##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                     \
    {                                                                                                                 \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                           \
        vector_##data_type src;                                                                                       \
        uint32_t count = 32;                                                                                          \
                                                                                                                      \
        MOCKER_CPP(                                                                                                   \
            cce_name2, void(vector_store_unalign&, uint32_t, vector_##data_type, __ubuf__ data_type*&, Literal))      \
            .times(1)                                                                                                 \
            .will(invoke(cce_name2##_##data_type##_Stub2));                                                           \
                                                                                                                      \
        MOCKER_CPP(cce_name3, void(vector_store_unalign, __ubuf__ data_type*, int32_t))                               \
            .times(1)                                                                                                 \
            .will(invoke(cce_name3##_##data_type##_Stub3));                                                           \
                                                                                                                      \
        c_api_name(dst, src, count);                                                                                  \
        GlobalMockObject::verify();                                                                                   \
    }

#define TEST_VECTOR_DATAMOVE_STORE3_HIF8(class_name, c_api_name, cce_name2, cce_name3, data_type)                 \
                                                                                                                  \
    class TestVectorDataMove3##class_name##_##data_type##_CApi : public testing::Test {                           \
    protected:                                                                                                    \
        void SetUp() {}                                                                                           \
        void TearDown() {}                                                                                        \
    };                                                                                                            \
                                                                                                                  \
    namespace {                                                                                                   \
    void cce_name2##_##data_type##_Stub2(                                                                         \
        vector_store_unalign& src0, uint32_t offset, vector_uint8_t src1, __ubuf__ uint8_t*& dst, Literal post)   \
    {                                                                                                             \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                              \
    }                                                                                                             \
    void cce_name3##_##data_type##_Stub3(vector_store_unalign src, __ubuf__ uint8_t* dst, int32_t offset) {}      \
    }                                                                                                             \
                                                                                                                  \
    TEST_F(TestVectorDataMove3##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                 \
    {                                                                                                             \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                       \
        vector_##data_type src;                                                                                   \
        uint32_t count = 32;                                                                                      \
                                                                                                                  \
        MOCKER_CPP(cce_name2, void(vector_store_unalign&, uint32_t, vector_uint8_t, __ubuf__ uint8_t*&, Literal)) \
            .times(1)                                                                                             \
            .will(invoke(cce_name2##_##data_type##_Stub2));                                                       \
                                                                                                                  \
        MOCKER_CPP(cce_name3, void(vector_store_unalign, __ubuf__ uint8_t*, int32_t))                             \
            .times(1)                                                                                             \
            .will(invoke(cce_name3##_##data_type##_Stub3));                                                       \
                                                                                                                  \
        c_api_name(dst, src, count);                                                                              \
        GlobalMockObject::verify();                                                                               \
    }

#define TEST_VECTOR_DATAMOVE_STORE3_INT4B(class_name, c_api_name, cce_name2, cce_name3, data_type)                   \
                                                                                                                     \
    class TestVectorDataMove3##class_name##_##data_type##_CApi : public testing::Test {                              \
    protected:                                                                                                       \
        void SetUp() {}                                                                                              \
        void TearDown() {}                                                                                           \
    };                                                                                                               \
                                                                                                                     \
    namespace {                                                                                                      \
    void cce_name2##_##data_type##_Stub2(                                                                            \
        vector_store_unalign& src0, uint32_t offset, vector_fp4x2_e1m2_t src1, __ubuf__ float4_e1m2x2_t*& dst,       \
        Literal post)                                                                                                \
    {                                                                                                                \
        EXPECT_EQ(post, static_cast<Literal>(Literal::POST_UPDATE));                                                 \
    }                                                                                                                \
    void cce_name3##_##data_type##_Stub3(vector_store_unalign src, __ubuf__ float4_e1m2x2_t* dst, int32_t offset) {} \
    }                                                                                                                \
                                                                                                                     \
    TEST_F(TestVectorDataMove3##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                    \
    {                                                                                                                \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                          \
        vector_int4x2_t src;                                                                                         \
        uint32_t count = 32;                                                                                         \
                                                                                                                     \
        MOCKER_CPP(                                                                                                  \
            cce_name2,                                                                                               \
            void(vector_store_unalign&, uint32_t, vector_fp4x2_e1m2_t, __ubuf__ float4_e1m2x2_t*&, Literal))         \
            .times(1)                                                                                                \
            .will(invoke(cce_name2##_##data_type##_Stub2));                                                          \
                                                                                                                     \
        MOCKER_CPP(cce_name3, void(vector_store_unalign, __ubuf__ float4_e1m2x2_t*, int32_t))                        \
            .times(1)                                                                                                \
            .will(invoke(cce_name3##_##data_type##_Stub3));                                                          \
                                                                                                                     \
        c_api_name(dst, src, count);                                                                                 \
        GlobalMockObject::verify();                                                                                  \
    }

TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, int8_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, uint8_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, int16_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, uint16_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, int32_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, uint32_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, int64_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, half);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, float);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, bfloat16_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_STORE_HIF8(Vstusandvstas, asc_store, vstus, vstas, hifloat8_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_STORE(Vstusandvstas, asc_store, vstus, vstas, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_STORE_INT4B(Vstusandvstas, asc_store, vstus, vstas, int4b_t);

TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, int8_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, uint8_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, int16_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, uint16_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, int32_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, uint32_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, int64_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, half);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, float);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, bfloat16_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_STORE3_HIF8(Vstusandvstas, asc_store, vstus, vstas, hifloat8_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_STORE3(Vstusandvstas, asc_store, vstus, vstas, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_STORE3_INT4B(Vstusandvstas, asc_store, vstus, vstas, int4b_t);
