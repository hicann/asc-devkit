/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_UNIT_BASIC_API_UT_TESTCASE_C_API_NPU_ARCH_3510_UTILS_TEST_FIXPIPE_L0C2L1_INSTR_H
#define TESTS_UNIT_BASIC_API_UT_TESTCASE_C_API_NPU_ARCH_3510_UTILS_TEST_FIXPIPE_L0C2L1_INSTR_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#define TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(class_name, c_api_name, cce_name, dst_data_type, src_data_type)      \
                                                                                                                    \
    class TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type : public testing::Test {                 \
    protected:                                                                                                      \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                               \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                            \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void c_api_name##_##dst_data_type##_##src_data_type##_Stub(                                                     \
        __cbuf__ dst_data_type* dst_in, __cc__ src_data_type* src_in, uint8_t sid_in, uint16_t n_size_in,           \
        uint16_t m_size_in, uint32_t dst_stride_in, uint16_t src_stride_in, uint8_t enable_clip_relu_pre_in,        \
        uint8_t unit_flag_mode_in, QuantMode_t quant_pre_mode_in, uint8_t relu_pre_mode_in,                         \
        bool enable_channel_split_in, bool enable_nz2nd_in, QuantMode_post quant_post_in, uint8_t relu_post_in,     \
        bool clip_relu_post_in, uint8_t eltwise_op_in, uint8_t eltwise_antq_en_in, bool c0_pad_en_in)               \
    {                                                                                                               \
        __cbuf__ dst_data_type* dst = reinterpret_cast<__cbuf__ dst_data_type*>(1);                                 \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                     \
        uint8_t sid = 3;                                                                                            \
        uint16_t n_size = 4;                                                                                        \
        uint16_t m_size = 5;                                                                                        \
        uint32_t dst_stride = 6;                                                                                    \
        uint16_t src_stride = 7;                                                                                    \
        uint8_t enable_clip_relu_pre = 8;                                                                           \
        uint8_t unit_flag_mode = 10;                                                                                \
        uint64_t quant_pre_mode = 11;                                                                               \
        uint8_t relu_pre_mode = 12;                                                                                 \
        bool enable_channel_split = true;                                                                           \
        bool enable_nz2nd = true;                                                                                   \
        uint64_t quant_post = 13;                                                                                   \
        uint8_t relu_post = 14;                                                                                     \
        bool clip_relu_post = true;                                                                                 \
        uint8_t eltwise_op = 15;                                                                                    \
        uint8_t eltwise_antq_en = 15;                                                                               \
        bool c0_pad_en = true;                                                                                      \
                                                                                                                    \
        EXPECT_EQ(dst, dst_in);                                                                                     \
        EXPECT_EQ(src, src_in);                                                                                     \
        EXPECT_EQ(sid, sid_in);                                                                                     \
        EXPECT_EQ(n_size, n_size_in);                                                                               \
        EXPECT_EQ(m_size, m_size_in);                                                                               \
        EXPECT_EQ(dst_stride, dst_stride_in);                                                                       \
        EXPECT_EQ(src_stride, src_stride_in);                                                                       \
        EXPECT_EQ(enable_clip_relu_pre, enable_clip_relu_pre_in);                                                   \
        EXPECT_EQ(unit_flag_mode, unit_flag_mode_in);                                                               \
        EXPECT_EQ(relu_pre_mode, relu_pre_mode_in);                                                                 \
        EXPECT_EQ(enable_channel_split, enable_channel_split_in);                                                   \
        EXPECT_EQ(enable_nz2nd, enable_nz2nd_in);                                                                   \
        EXPECT_EQ(relu_post, relu_post_in);                                                                         \
        EXPECT_EQ(clip_relu_post, clip_relu_post_in);                                                               \
        EXPECT_EQ(eltwise_op, eltwise_op_in);                                                                       \
        EXPECT_EQ(eltwise_antq_en, eltwise_antq_en_in);                                                             \
        EXPECT_EQ(c0_pad_en, c0_pad_en_in);                                                                         \
    }                                                                                                               \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(                                                                                                         \
        TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type,                                           \
        c_api_name##dst_data_type##_##src_data_type##_Succ)                                                         \
    {                                                                                                               \
        __cbuf__ dst_data_type* dst = reinterpret_cast<__cbuf__ dst_data_type*>(1);                                 \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                     \
        uint8_t sid = 3;                                                                                            \
        uint16_t n_size = 4;                                                                                        \
        uint16_t m_size = 5;                                                                                        \
        uint32_t dst_stride = 6;                                                                                    \
        uint16_t src_stride = 7;                                                                                    \
        uint8_t enable_clip_relu_pre = 8;                                                                           \
        uint8_t unit_flag_mode = 10;                                                                                \
        uint64_t quant_pre_mode = 11;                                                                               \
        uint8_t relu_pre_mode = 12;                                                                                 \
        bool enable_channel_split = true;                                                                           \
        bool enable_nz2nd = true;                                                                                   \
        uint64_t quant_post = 13;                                                                                   \
        uint8_t relu_post = 14;                                                                                     \
        bool clip_relu_post = true;                                                                                 \
        uint8_t eltwise_op = 15;                                                                                    \
        uint8_t eltwise_antq_en = 15;                                                                               \
        bool c0_pad_en = true;                                                                                      \
                                                                                                                    \
        c_api_name(                                                                                                 \
            dst, src, n_size, m_size, dst_stride, src_stride, enable_clip_relu_pre, unit_flag_mode, quant_pre_mode, \
            relu_pre_mode, enable_channel_split, enable_nz2nd, quant_post, relu_post, clip_relu_post, eltwise_op,   \
            eltwise_antq_en, c0_pad_en);                                                                            \
        GlobalMockObject::verify();                                                                                 \
    }

// copy_matrix_cc_to_cbuf_s4
#define TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_S4_INSTR(class_name, c_api_name, cce_name, dst_data_type, src_data_type)     \
    class TestCubeDatamoveS4##class_name##_##dst_data_type##_##src_data_type : public testing::Test {                 \
    protected:                                                                                                        \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                 \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                              \
    };                                                                                                                \
    TEST_F(                                                                                                           \
        TestCubeDatamoveS4##class_name##_##dst_data_type##_##src_data_type,                                           \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                        \
    {                                                                                                                 \
        __cbuf__ dst_data_type* dst = reinterpret_cast<__cbuf__ dst_data_type*>(1);                                   \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                       \
        uint16_t n_size = 3;                                                                                          \
        uint16_t m_size = 4;                                                                                          \
        uint32_t dst_stride = 6;                                                                                      \
        uint16_t src_stride = 7;                                                                                      \
        uint8_t l2_cache_mode = 5;                                                                                    \
        uint8_t enable_clip_relu_pre = 8;                                                                             \
        uint8_t unit_flag_mode = 10;                                                                                  \
        uint64_t quant_pre_mode = 11;                                                                                 \
        uint8_t relu_pre_mode = 12;                                                                                   \
        bool enable_channel_split = true;                                                                             \
        bool enable_nz2nd = true;                                                                                     \
        uint64_t quant_post = 13;                                                                                     \
        uint8_t relu_post = 14;                                                                                       \
        bool clip_relu_post = true;                                                                                   \
        uint8_t eltwise_op = 15;                                                                                      \
        bool eltwise_antq_en = true;                                                                                  \
        bool c0_pad_en = true;                                                                                        \
        bool broadcast_en = false;                                                                                    \
        bool enable_nz2dn = false;                                                                                    \
        c_api_name(                                                                                                   \
            dst, src, n_size, m_size, dst_stride, src_stride, l2_cache_mode, enable_clip_relu_pre, unit_flag_mode,    \
            quant_pre_mode, relu_pre_mode, enable_channel_split, enable_nz2nd, quant_post, relu_post, clip_relu_post, \
            eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, enable_nz2dn);                                      \
        GlobalMockObject::verify();                                                                                   \
    }

#endif

TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, bfloat16_t, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, float, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, half, int32_t);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int8_t, int32_t);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, uint8_t, int32_t);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf, int32_t, int32_t);

TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_S4_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, int4b_t, float);
TEST_CUBE_COMPUTE_FIXPIPE_L0C2L1_S4_INSTR(L0C2L1, asc_copy_l0c2l1, copy_matrix_cc_to_cbuf_s4, int4b_t, int32_t);
