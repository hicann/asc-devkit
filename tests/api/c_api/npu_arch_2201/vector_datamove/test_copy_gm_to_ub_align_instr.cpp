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

#define TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(                                                          \
    class_name, c_api_name, cce_name, dst_date_type, src_date_type)                                                \
                                                                                                                   \
    class TestVectorDatamove##class_name : public testing::Test {                                                  \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
                                                                                                                   \
    void cce_name##_##dst_date_type##_##src_date_type##_uint64_t_Stub(                                             \
        __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint16_t burst_count, uint32_t burst_len,               \
        uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)                   \
    {                                                                                                              \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ void*>(11));                                                      \
        EXPECT_EQ(src, reinterpret_cast<__gm__ void*>(22));                                                        \
        EXPECT_EQ(sid, static_cast<uint8_t>(0));                                                                   \
        EXPECT_EQ(burst_count, static_cast<uint16_t>(1));                                                          \
        EXPECT_EQ(burst_len, static_cast<uint32_t>(44));                                                           \
        EXPECT_EQ(left_padding_num, static_cast<uint8_t>(0));                                                      \
        EXPECT_EQ(right_padding_num, static_cast<uint8_t>(0));                                                     \
        EXPECT_EQ(src_gap, static_cast<uint32_t>(0));                                                              \
        EXPECT_EQ(dst_gap, static_cast<uint32_t>(0));                                                              \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_CopyConfig_Succ)                                           \
    {                                                                                                              \
        __ubuf__ dst_date_type* dst = reinterpret_cast<__ubuf__ dst_date_type*>(11);                               \
        __gm__ src_date_type* src = reinterpret_cast<__gm__ src_date_type*>(22);                                   \
                                                                                                                   \
        uint16_t burst_count = static_cast<uint64_t>(1);                                                           \
        uint32_t burst_len = static_cast<uint64_t>(44);                                                            \
        uint8_t left_padding_num = static_cast<uint64_t>(0);                                                       \
        uint8_t right_padding_num = static_cast<uint64_t>(0);                                                      \
        uint32_t src_gap = static_cast<uint64_t>(0);                                                               \
        uint32_t dst_gap = static_cast<uint64_t>(0);                                                               \
                                                                                                                   \
        MOCKER_CPP(                                                                                                \
            cce_name,                                                                                              \
            void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint32_t, uint8_t, uint8_t, uint32_t, uint32_t)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##dst_date_type##_##src_date_type##_uint64_t_Stub));                           \
                                                                                                                   \
        c_api_name(dst, src, burst_count, burst_len, left_padding_num, right_padding_num, src_gap, dst_gap);       \
        GlobalMockObject::verify();                                                                                \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_size_Succ)                                                 \
    {                                                                                                              \
        __ubuf__ dst_date_type* dst = reinterpret_cast<__ubuf__ dst_date_type*>(11);                               \
        __gm__ src_date_type* src = reinterpret_cast<__gm__ src_date_type*>(22);                                   \
        uint32_t size = static_cast<uint32_t>(44);                                                                 \
                                                                                                                   \
        MOCKER_CPP(                                                                                                \
            cce_name,                                                                                              \
            void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint32_t, uint8_t, uint8_t, uint32_t, uint32_t)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##dst_date_type##_##src_date_type##_uint64_t_Stub));                           \
                                                                                                                   \
        c_api_name(dst, src, size);                                                                                \
        GlobalMockObject::verify();                                                                                \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_sync_Succ)                                                 \
    {                                                                                                              \
        __ubuf__ dst_date_type* dst = reinterpret_cast<__ubuf__ dst_date_type*>(11);                               \
        __gm__ src_date_type* src = reinterpret_cast<__gm__ src_date_type*>(22);                                   \
        uint32_t size = static_cast<uint32_t>(44);                                                                 \
                                                                                                                   \
        MOCKER_CPP(                                                                                                \
            cce_name,                                                                                              \
            void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint32_t, uint8_t, uint8_t, uint32_t, uint32_t)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##dst_date_type##_##src_date_type##_uint64_t_Stub));                           \
        c_api_name##_sync(dst, src, size);                                                                         \
        GlobalMockObject::verify();                                                                                \
    }

// ==========asc_copy_ub2gm_align int8_t / uint8_t / half / bfloat16_t / int16_t / uint16_t / float / int32_t /
// uint32_t==========
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNUINT8, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b8, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNINT8, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b8, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBHALF, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b16, half, half);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBBF16, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b16, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBINT16, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b16, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBUINT16, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b16, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBFLOAT, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b32, float, float);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBINT32, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b32, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN_INSTR(
    CopyGM2UBALIGNBUINT32, asc_copy_gm2ub_align, copy_gm_to_ubuf_align_b32, uint32_t, uint32_t);
