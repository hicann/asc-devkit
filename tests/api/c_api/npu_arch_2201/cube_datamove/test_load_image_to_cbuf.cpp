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

#define TEST_CUBE_DMAMOVE_LOAD_IMAGE_TO_CBUF(class_name, c_api_name, cce_name, data_type)                              \
                                                                                                                       \
    class TestCubeDmamove##class_name##data_type : public testing::Test {                                              \
    protected:                                                                                                         \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                  \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                               \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
                                                                                                                       \
    void cce_name##_##data_type##_uint16_uint16_uint16_uint16_uint16_uint8_uint8_uint16_uint16_uint8_Stub(             \
        __ubuf__ data_type* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, \
        uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size,                     \
        uint16_t right_pad_size, uint8_t sid)                                                                          \
    {                                                                                                                  \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ data_type*>(11));                                                     \
        EXPECT_EQ(hor_size, static_cast<uint16_t>(0));                                                                 \
        EXPECT_EQ(ver_size, static_cast<uint16_t>(0));                                                                 \
        EXPECT_EQ(hor_start_pos, static_cast<uint16_t>(0));                                                            \
        EXPECT_EQ(ver_start_pos, static_cast<uint16_t>(0));                                                            \
        EXPECT_EQ(src_hor_size, static_cast<uint16_t>(0));                                                             \
        EXPECT_EQ(top_pad_size, static_cast<uint8_t>(0));                                                              \
        EXPECT_EQ(bot_pad_size, static_cast<uint8_t>(0));                                                              \
        EXPECT_EQ(left_pad_size, static_cast<uint16_t>(0));                                                            \
        EXPECT_EQ(right_pad_size, static_cast<uint16_t>(0));                                                           \
        EXPECT_EQ(sid, static_cast<uint8_t>(0));                                                                       \
    }                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestCubeDmamove##class_name##data_type, c_api_name##_LoadImageConfig_Succ)                                  \
    {                                                                                                                  \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                           \
                                                                                                                       \
        uint16_t hor_size = static_cast<uint16_t>(1);                                                                  \
        uint16_t ver_size = static_cast<uint16_t>(1);                                                                  \
        uint16_t hor_start_pos = static_cast<uint16_t>(0);                                                             \
        uint16_t ver_start_pos = static_cast<uint16_t>(0);                                                             \
        uint16_t src_hor_size = static_cast<uint16_t>(1);                                                              \
        uint8_t top_pad_size = static_cast<uint8_t>(0);                                                                \
        uint8_t bot_pad_size = static_cast<uint8_t>(0);                                                                \
        uint16_t left_pad_size = static_cast<uint16_t>(0);                                                             \
        uint16_t right_pad_size = static_cast<uint16_t>(0);                                                            \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name, void(                                                                                            \
                          __ubuf__ data_type*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t,     \
                          uint16_t, uint16_t, uint8_t))                                                                \
            .times(1)                                                                                                  \
            .will(invoke(                                                                                              \
                cce_name##_##data_type##_uint16_uint16_uint16_uint16_uint16_uint8_uint8_uint16_uint16_uint8_Stub));    \
                                                                                                                       \
        c_api_name(                                                                                                    \
            dst, hor_size, ver_size, hor_start_pos, ver_start_pos, src_hor_size, top_pad_size, bot_pad_size,           \
            left_pad_size, right_pad_size);                                                                            \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestCubeDmamove##class_name##data_type, c_api_name##_sync_Succ)                                             \
    {                                                                                                                  \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                           \
                                                                                                                       \
        uint16_t hor_size = static_cast<uint16_t>(1);                                                                  \
        uint16_t ver_size = static_cast<uint16_t>(1);                                                                  \
        uint16_t hor_start_pos = static_cast<uint16_t>(0);                                                             \
        uint16_t ver_start_pos = static_cast<uint16_t>(0);                                                             \
        uint16_t src_hor_size = static_cast<uint16_t>(1);                                                              \
        uint8_t top_pad_size = static_cast<uint8_t>(0);                                                                \
        uint8_t bot_pad_size = static_cast<uint8_t>(0);                                                                \
        uint16_t left_pad_size = static_cast<uint16_t>(0);                                                             \
        uint16_t right_pad_size = static_cast<uint16_t>(0);                                                            \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name, void(                                                                                            \
                          __ubuf__ data_type*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t,     \
                          uint16_t, uint16_t, uint8_t))                                                                \
            .times(1)                                                                                                  \
            .will(invoke(                                                                                              \
                cce_name##_##data_type##_uint16_uint16_uint16_uint16_uint16_uint8_uint8_uint16_uint16_uint8_Stub));    \
                                                                                                                       \
        c_api_name##_sync(                                                                                             \
            dst, hor_size, ver_size, hor_start_pos, ver_start_pos, src_hor_size, top_pad_size, bot_pad_size,           \
            left_pad_size, right_pad_size);                                                                            \
        GlobalMockObject::verify();                                                                                    \
    }

// ==========asc_load_image_to_cbuf(half/int8_t)==========
TEST_CUBE_DMAMOVE_LOAD_IMAGE_TO_CBUF(LoadImageToCbuf, asc_load_image_to_cbuf, load_image_to_cbuf, half);
TEST_CUBE_DMAMOVE_LOAD_IMAGE_TO_CBUF(LoadImageToCbuf, asc_load_image_to_cbuf, load_image_to_cbuf, int8_t);
