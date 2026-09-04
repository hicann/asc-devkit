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
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"

#define TEST_VECTOR_COMPUTE_VDEQ_INSTR(class_name, c_api_name, cce_name, dst_data_type, src_data_type)                 \
                                                                                                                       \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type : public testing::Test {                   \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    void cce_name##_##dst_data_type##_##src_data_type##_UnaryCfg_Stub(                                                 \
        __ubuf__ dst_data_type* dst, __ubuf__ src_data_type* src, uint8_t repeat, uint16_t dst_block_stride,           \
        uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)                               \
    {                                                                                                                  \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ dst_data_type*>(11));                                                 \
        EXPECT_EQ(src, reinterpret_cast<__ubuf__ src_data_type*>(22));                                                 \
        EXPECT_EQ(repeat, static_cast<uint8_t>(1));                                                                    \
        EXPECT_EQ(dst_block_stride, static_cast<uint16_t>(1));                                                         \
        EXPECT_EQ(src_block_stride, static_cast<uint16_t>(1));                                                         \
        EXPECT_EQ(dst_repeat_stride, static_cast<uint8_t>(8));                                                         \
        EXPECT_EQ(src_repeat_stride, static_cast<uint8_t>(8));                                                         \
    }                                                                                                                  \
                                                                                                                       \
    void cce_name##_##dst_data_type##_##src_data_type##_uint32_t_Stub(                                                 \
        __ubuf__ dst_data_type* dst, __ubuf__ src_data_type* src, uint8_t repeat, uint16_t dst_block_stride,           \
        uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)                               \
    {                                                                                                                  \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ dst_data_type*>(11));                                                 \
        EXPECT_EQ(src, reinterpret_cast<__ubuf__ src_data_type*>(22));                                                 \
    }                                                                                                                  \
                                                                                                                       \
    void cce_name##_##dst_data_type##_##src_data_type##_set_vector_mask_Stub(uint64_t mask1, uint64_t mask0)           \
    {                                                                                                                  \
        EXPECT_EQ(mask1, static_cast<uint64_t>(0));                                                                    \
        EXPECT_EQ(mask0, static_cast<uint64_t>(44));                                                                   \
    }                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(                                                                                                            \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type,                                             \
        c_api_name##_##dst_data_type##_##src_data_type##_UnaryConfig_Succ)                                             \
    {                                                                                                                  \
        __ubuf__ dst_data_type* dst = reinterpret_cast<__ubuf__ dst_data_type*>(11);                                   \
        __ubuf__ src_data_type* src = reinterpret_cast<__ubuf__ src_data_type*>(22);                                   \
                                                                                                                       \
        uint16_t dst_block_stride = static_cast<uint16_t>(1);                                                          \
        uint16_t src_block_stride = static_cast<uint16_t>(1);                                                          \
        uint8_t dst_repeat_stride = static_cast<uint8_t>(8);                                                           \
        uint8_t src_repeat_stride = static_cast<uint8_t>(8);                                                           \
        uint8_t repeat = static_cast<uint8_t>(1);                                                                      \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name,                                                                                                  \
            void(__ubuf__ dst_data_type*, __ubuf__ src_data_type*, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t))     \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_UnaryCfg_Stub));                               \
                                                                                                                       \
        c_api_name(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);        \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(                                                                                                            \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type,                                             \
        c_api_name##_##dst_data_type##_##src_data_type##_uint32_t_Succ)                                                \
    {                                                                                                                  \
        __ubuf__ dst_data_type* dst = reinterpret_cast<__ubuf__ dst_data_type*>(11);                                   \
        __ubuf__ src_data_type* src = reinterpret_cast<__ubuf__ src_data_type*>(22);                                   \
        uint32_t count = static_cast<uint32_t>(44);                                                                    \
                                                                                                                       \
        MOCKER_CPP(set_vector_mask, void(uint64_t, uint64_t))                                                          \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_set_vector_mask_Stub));                        \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name,                                                                                                  \
            void (*)(__ubuf__ dst_data_type*, __ubuf__ src_data_type*, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t)) \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_uint32_t_Stub));                               \
                                                                                                                       \
        c_api_name(dst, src, count);                                                                                   \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(                                                                                                            \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type,                                             \
        c_api_name##_sync_##dst_data_type##_##src_data_type##_uint32_t_Succ)                                           \
    {                                                                                                                  \
        __ubuf__ dst_data_type* dst = reinterpret_cast<__ubuf__ dst_data_type*>(11);                                   \
        __ubuf__ src_data_type* src = reinterpret_cast<__ubuf__ src_data_type*>(22);                                   \
        uint32_t count = static_cast<uint32_t>(44);                                                                    \
                                                                                                                       \
        MOCKER_CPP(set_vector_mask, void(uint64_t, uint64_t))                                                          \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_set_vector_mask_Stub));                        \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name,                                                                                                  \
            void (*)(__ubuf__ dst_data_type*, __ubuf__ src_data_type*, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t)) \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_uint32_t_Stub));                               \
                                                                                                                       \
        c_api_name##_sync(dst, src, count);                                                                            \
        GlobalMockObject::verify();                                                                                    \
    }

// ========== asc_vdeq_int162b8_h==========
TEST_VECTOR_COMPUTE_VDEQ_INSTR(VdeqI162B8h_int8, asc_vdeq_int162b8_h, vconv_vdeqs162b8h, int8_t, int16_t);
TEST_VECTOR_COMPUTE_VDEQ_INSTR(VdeqI162B8h_uint8, asc_vdeq_int162b8_h, vconv_vdeqs162b8h, uint8_t, int16_t);

// ========== asc_vdeq_int162b8_l==========
TEST_VECTOR_COMPUTE_VDEQ_INSTR(VdeqI162B8l_int8, asc_vdeq_int162b8_l, vconv_vdeqs162b8l, int8_t, int16_t);
TEST_VECTOR_COMPUTE_VDEQ_INSTR(VdeqI162B8l_uint8, asc_vdeq_int162b8_l, vconv_vdeqs162b8l, uint8_t, int16_t);
