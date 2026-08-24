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

#define TEST_VECTOR_DATAMOVE_COPY_GM_TO_UBUF_INSTR(class_name, c_api_name, cce_name)                                   \
                                                                                                                       \
    class TestVectorDatamove##class_name : public testing::Test {                                                      \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
                                                                                                                       \
    void cce_name##_uint8_uint16_uint16_uint16_uint16_Stub(                                                            \
        __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, \
        uint16_t dst_gap)                                                                                              \
    {                                                                                                                  \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ void*>(11));                                                          \
        EXPECT_EQ(src, reinterpret_cast<__gm__ void*>(22));                                                            \
        EXPECT_EQ(sid, static_cast<uint8_t>(0));                                                                       \
        EXPECT_EQ(burst_count, static_cast<uint16_t>(1));                                                              \
        EXPECT_EQ(burst_len, static_cast<uint16_t>(1));                                                                \
        EXPECT_EQ(src_gap, static_cast<uint16_t>(0));                                                                  \
        EXPECT_EQ(dst_gap, static_cast<uint16_t>(0));                                                                  \
    }                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_CopyConfig_Succ)                                               \
    {                                                                                                                  \
        __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);                                                     \
        __gm__ void* src = reinterpret_cast<__gm__ void*>(22);                                                         \
                                                                                                                       \
        uint16_t burst_count = static_cast<uint64_t>(1);                                                               \
        uint16_t burst_len = static_cast<uint64_t>(1);                                                                 \
        uint16_t src_gap = static_cast<uint64_t>(0);                                                                   \
        uint16_t dst_gap = static_cast<uint64_t>(0);                                                                   \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))      \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_uint8_uint16_uint16_uint16_uint16_Stub));                                          \
                                                                                                                       \
        c_api_name(dst, src, burst_count, burst_len, src_gap, dst_gap);                                                \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_size_Succ)                                                     \
    {                                                                                                                  \
        __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);                                                     \
        __gm__ void* src = reinterpret_cast<__gm__ void*>(22);                                                         \
        uint32_t size = static_cast<uint32_t>(44);                                                                     \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))      \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_uint8_uint16_uint16_uint16_uint16_Stub));                                          \
                                                                                                                       \
        c_api_name(dst, src, size);                                                                                    \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestVectorDatamove##class_name, c_api_name##_sync_Succ)                                                     \
    {                                                                                                                  \
        __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);                                                     \
        __gm__ void* src = reinterpret_cast<__gm__ void*>(22);                                                         \
        uint32_t size = static_cast<uint32_t>(44);                                                                     \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(__ubuf__ void*, __gm__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))      \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_uint8_uint16_uint16_uint16_uint16_Stub));                                          \
        c_api_name##_sync(dst, src, size);                                                                             \
        GlobalMockObject::verify();                                                                                    \
    }

// ==========asc_copy_gm2ub==========
TEST_VECTOR_DATAMOVE_COPY_GM_TO_UBUF_INSTR(CopyGM2UBCApi, asc_copy_gm2ub, copy_gm_to_ubuf);
