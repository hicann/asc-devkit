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

#define TEST_VECTOR_COMPUTE_UNPACK(class_name, c_api_name, cce_name, dst_type, src_type)            \
                                                                                                    \
    class TestVectorCompute##class_name##dst_type##src_type##CApi : public testing::Test {          \
    protected:                                                                                      \
        void SetUp() {}                                                                             \
        void TearDown() {}                                                                          \
    };                                                                                              \
                                                                                                    \
    namespace {                                                                                     \
                                                                                                    \
    void c_api_name##_##dst_type##_##src_type##_Stub(dst_type& dst, src_type& src, Literal part) {} \
    }                                                                                               \
                                                                                                    \
    TEST_F(TestVectorCompute##class_name##dst_type##src_type##CApi, c_api_name##_Succ)              \
    {                                                                                               \
        dst_type dst;                                                                               \
        src_type src;                                                                               \
                                                                                                    \
        MOCKER_CPP(cce_name, void(dst_type&, src_type&, Literal))                                   \
            .times(1)                                                                               \
            .will(invoke(c_api_name##_##dst_type##_##src_type##_Stub));                             \
                                                                                                    \
        c_api_name(dst, src);                                                                       \
        GlobalMockObject::verify();                                                                 \
    }

TEST_VECTOR_COMPUTE_UNPACK(UnPackU, asc_unpack_upper, vunpack, vector_uint16_t, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackU, asc_unpack_upper, vunpack, vector_int16_t, vector_int8_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackU, asc_unpack_upper, vunpack, vector_uint32_t, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackU, asc_unpack_upper, vunpack, vector_int32_t, vector_int16_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackL, asc_unpack_lower, vunpack, vector_uint16_t, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackL, asc_unpack_lower, vunpack, vector_int16_t, vector_int8_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackL, asc_unpack_lower, vunpack, vector_uint32_t, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNPACK(UnPackL, asc_unpack_lower, vunpack, vector_int32_t, vector_int16_t);

#define TEST_VECTOR_COMPUTE_PUNPACK(class_name, c_api_name, cce_name)                        \
                                                                                             \
    class TestVectorCompute##class_name##vector_bool##CApi : public testing::Test {          \
    protected:                                                                               \
        void SetUp() {}                                                                      \
        void TearDown() {}                                                                   \
    };                                                                                       \
                                                                                             \
    namespace {                                                                              \
                                                                                             \
    void c_api_name##_##vector_bool_Stub(vector_bool& dst, vector_bool src, Literal part) {} \
    }                                                                                        \
                                                                                             \
    TEST_F(TestVectorCompute##class_name##vector_bool##CApi, c_api_name##_Succ)              \
    {                                                                                        \
        vector_bool dst;                                                                     \
        vector_bool src;                                                                     \
                                                                                             \
        MOCKER_CPP(cce_name, void(vector_bool&, vector_bool, Literal))                       \
            .times(1)                                                                        \
            .will(invoke(c_api_name##_##vector_bool_Stub));                                  \
                                                                                             \
        c_api_name(dst, src);                                                                \
        GlobalMockObject::verify();                                                          \
    }

TEST_VECTOR_COMPUTE_PUNPACK(PunpackU, asc_unpack_upper, punpack);
TEST_VECTOR_COMPUTE_PUNPACK(PunpackL, asc_unpack_lower, punpack);

// ==========asc_unpack return-value overloads(u8/s8/u16/s16)==========
#define TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(c_api_name, cce_name, dst_type, src_type) \
    TEST(TestVectorComputeUnpackReturn, c_api_name##_##src_type)                          \
    {                                                                                     \
        src_type src;                                                                     \
                                                                                          \
        static_assert(std::is_same_v<decltype(c_api_name(src)), dst_type>);               \
        MOCKER_CPP(cce_name, void(dst_type&, src_type&, Literal))                         \
            .times(1)                                                                     \
            .will(invoke(c_api_name##_##dst_type##_##src_type##_Stub));                   \
                                                                                          \
        dst_type dst = c_api_name(src);                                                   \
        (void)dst;                                                                        \
        GlobalMockObject::verify();                                                       \
    }

TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_upper, vunpack, vector_uint16_t, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_upper, vunpack, vector_int16_t, vector_int8_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_upper, vunpack, vector_uint32_t, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_upper, vunpack, vector_int32_t, vector_int16_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_lower, vunpack, vector_uint16_t, vector_uint8_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_lower, vunpack, vector_int16_t, vector_int8_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_lower, vunpack, vector_uint32_t, vector_uint16_t);
TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR(asc_unpack_lower, vunpack, vector_int32_t, vector_int16_t);

#undef TEST_VECTOR_COMPUTE_UNPACK_RETURN_INSTR

// ==========asc_unpack return-value overloads(bool)==========
#define TEST_VECTOR_COMPUTE_PUNPACK_RETURN_INSTR(c_api_name, cce_name)         \
    TEST(TestVectorComputePunpackReturn, c_api_name)                           \
    {                                                                          \
        vector_bool src;                                                       \
                                                                               \
        static_assert(std::is_same_v<decltype(c_api_name(src)), vector_bool>); \
        MOCKER_CPP(cce_name, void(vector_bool&, vector_bool, Literal))         \
            .times(1)                                                          \
            .will(invoke(c_api_name##_##vector_bool##_Stub));                  \
                                                                               \
        vector_bool dst = c_api_name(src);                                     \
        (void)dst;                                                             \
        GlobalMockObject::verify();                                            \
    }

TEST_VECTOR_COMPUTE_PUNPACK_RETURN_INSTR(asc_unpack_upper, punpack);
TEST_VECTOR_COMPUTE_PUNPACK_RETURN_INSTR(asc_unpack_lower, punpack);

#undef TEST_VECTOR_COMPUTE_PUNPACK_RETURN_INSTR
