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
#include <type_traits>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(class_name, c_api_name, cce_name, data_type)                               \
                                                                                                                       \
    class TestVectorCompute##class_name####data_type##CApi : public testing::Test {                                    \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    void cce_name##_##data_type##_Stub(data_type& dst, data_type src0, vector_bool mask, int32_t pos, Literal mode) {} \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_Succ)                          \
    {                                                                                                                  \
        data_type dst;                                                                                                 \
        data_type src0;                                                                                                \
        vector_bool mask;                                                                                              \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(data_type&, data_type, vector_bool, int32_t, Literal))                               \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##data_type##_Stub));                                                              \
                                                                                                                       \
        c_api_name(dst, src0, mask);                                                                                   \
        GlobalMockObject::verify();                                                                                    \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_uint8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_int8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_fp8_e4m3fn_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_fp8_e5m2_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_fp8_e8m0_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_uint16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_int16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_half);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_uint32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_int32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_INSTR(Vdup, asc_duplicate, vdup, vector_float);

#define TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(c_api_name, cce_name, data_type)      \
    TEST(TestVectorComputeVdupReturn, data_type)                                         \
    {                                                                                    \
        data_type src0;                                                                  \
        vector_bool mask;                                                                \
                                                                                         \
        static_assert(std::is_same_v<decltype(c_api_name(src0, mask)), data_type>);      \
        MOCKER_CPP(cce_name, void(data_type&, data_type, vector_bool, int32_t, Literal)) \
            .times(1)                                                                    \
            .will(invoke(cce_name##_##data_type##_Stub));                                \
                                                                                         \
        data_type dst = c_api_name(src0, mask);                                          \
        (void)dst;                                                                       \
        GlobalMockObject::verify();                                                      \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_uint8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_int8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_fp8_e4m3fn_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_fp8_e5m2_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_fp8_e8m0_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_uint16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_int16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_half);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_uint32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_int32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR(asc_duplicate, vdup, vector_float);

#undef TEST_VECTOR_COMPUTE_DUPLICATE_RETURN_INSTR

#define TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(data_type)                                                        \
    namespace {                                                                                                       \
    void vdup_highest_##data_type##_Stub(data_type& dst, data_type src0, vector_bool mask, int32_t pos, Literal mode) \
    {                                                                                                                 \
        EXPECT_EQ(pos, static_cast<int32_t>(POS_HIGHEST.value));                                                      \
        EXPECT_EQ(mode, MODE_ZEROING);                                                                                \
    }                                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
    TEST(TestVectorComputeVdupHighest, data_type)                                                                     \
    {                                                                                                                 \
        data_type dst;                                                                                                \
        data_type src0;                                                                                               \
        vector_bool mask;                                                                                             \
        MOCKER_CPP(vdup, void(data_type&, data_type, vector_bool, int32_t, Literal))                                  \
            .times(1)                                                                                                 \
            .will(invoke(vdup_highest_##data_type##_Stub));                                                           \
        asc_duplicate_highest(dst, src0, mask);                                                                       \
        GlobalMockObject::verify();                                                                                   \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_uint8_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_int8_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_fp8_e4m3fn_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_fp8_e5m2_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_fp8_e8m0_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_uint16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_int16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_half)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_bfloat16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_uint32_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_int32_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR(vector_float)

#undef TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_INSTR

#define TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(class_name, c_api_name, cce_name, data_type)          \
                                                                                                        \
    class TestVectorCompute##class_name####data_type##MergeCApi : public testing::Test {                \
    protected:                                                                                          \
        void SetUp() {}                                                                                 \
        void TearDown() {}                                                                              \
    };                                                                                                  \
                                                                                                        \
    namespace {                                                                                         \
    void cce_name##_merging_##data_type##_Stub(                                                         \
        data_type& dst, data_type src0, vector_bool mask, int32_t pos, Literal mode)                    \
    {                                                                                                   \
        EXPECT_EQ(pos, static_cast<int32_t>(POS_LOWEST.value));                                         \
        EXPECT_EQ(mode, MODE_MERGING);                                                                  \
    }                                                                                                   \
    }                                                                                                   \
                                                                                                        \
    TEST_F(TestVectorCompute##class_name####data_type##MergeCApi, c_api_name##_##data_type##_MergeSucc) \
    {                                                                                                   \
        data_type dst;                                                                                  \
        data_type src0;                                                                                 \
        vector_bool mask;                                                                               \
                                                                                                        \
        MOCKER_CPP(cce_name, void(data_type&, data_type, vector_bool, int32_t, Literal))                \
            .times(1)                                                                                   \
            .will(invoke(cce_name##_merging_##data_type##_Stub));                                       \
                                                                                                        \
        c_api_name(dst, src0, mask);                                                                    \
        GlobalMockObject::verify();                                                                     \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_uint8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_int8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_fp8_e4m3fn_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_fp8_e5m2_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_fp8_e8m0_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_uint16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_int16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_half);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_uint32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_int32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_MERGE_INSTR(Vdup, asc_duplicate_merge, vdup, vector_float);

#define TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(data_type)                 \
    namespace {                                                                      \
    void vdup_highest_merge_##data_type##_Stub(                                      \
        data_type& dst, data_type src0, vector_bool mask, int32_t pos, Literal mode) \
    {                                                                                \
        EXPECT_EQ(pos, static_cast<int32_t>(POS_HIGHEST.value));                     \
        EXPECT_EQ(mode, MODE_MERGING);                                               \
    }                                                                                \
    }                                                                                \
                                                                                     \
    TEST(TestVectorComputeVdupHighestMerge, data_type)                               \
    {                                                                                \
        data_type dst;                                                               \
        data_type src0;                                                              \
        vector_bool mask;                                                            \
        MOCKER_CPP(vdup, void(data_type&, data_type, vector_bool, int32_t, Literal)) \
            .times(1)                                                                \
            .will(invoke(vdup_highest_merge_##data_type##_Stub));                    \
        asc_duplicate_highest_merge(dst, src0, mask);                                \
        GlobalMockObject::verify();                                                  \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_uint8_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_int8_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_fp8_e4m3fn_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_fp8_e5m2_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_fp8_e8m0_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_uint16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_int16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_half)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_bfloat16_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_uint32_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_int32_t)
TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR(vector_float)

#undef TEST_VECTOR_COMPUTE_DUPLICATE_HIGHEST_MERGE_INSTR
