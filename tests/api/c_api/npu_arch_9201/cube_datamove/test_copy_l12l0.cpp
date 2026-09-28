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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#define TEST_CUBE_DATAMOVE_L12L0_IMPL(class_name, c_api_name, cce_name, data_type, cce_data_type, pos)                \
                                                                                                                      \
    class TestCubeDatamove##class_name##c_api_name##cce_name##data_type##CApi : public testing::Test {                \
    protected:                                                                                                        \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                 \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                              \
    };                                                                                                                \
                                                                                                                      \
    void mock##c_api_name##cce_name##_##data_type(                                                                    \
        pos cce_data_type* dst, __cbuf__ cce_data_type* src, uint16_t m_start_position, uint16_t k_start_position,    \
        uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride, bool transpose)                      \
    {                                                                                                                 \
        EXPECT_EQ(dst, reinterpret_cast<cce_data_type*>(11));                                                         \
        EXPECT_EQ(src, reinterpret_cast<cce_data_type*>(22));                                                         \
        EXPECT_EQ(m_start_position, static_cast<uint16_t>(33));                                                       \
        EXPECT_EQ(k_start_position, static_cast<uint16_t>(44));                                                       \
        EXPECT_EQ(m_step, static_cast<uint8_t>(55));                                                                  \
        EXPECT_EQ(k_step, static_cast<uint8_t>(66));                                                                  \
        EXPECT_EQ(src_stride, static_cast<int16_t>(77));                                                              \
        EXPECT_EQ(dst_stride, static_cast<uint16_t>(88));                                                             \
    }                                                                                                                 \
                                                                                                                      \
    TEST_F(TestCubeDatamove##class_name##c_api_name##cce_name##data_type##CApi, c_api_name##_Succ)                    \
    {                                                                                                                 \
        pos data_type* dst = reinterpret_cast<pos data_type*>(11);                                                    \
        __cbuf__ data_type* src = reinterpret_cast<__cbuf__ data_type*>(22);                                          \
                                                                                                                      \
        uint16_t m_start_position = static_cast<uint16_t>(33);                                                        \
        uint16_t k_start_position = static_cast<uint16_t>(44);                                                        \
        uint8_t m_step = static_cast<uint8_t>(55);                                                                    \
        uint8_t k_step = static_cast<uint8_t>(66);                                                                    \
        int16_t src_stride = static_cast<int16_t>(77);                                                                \
        uint16_t dst_stride = static_cast<uint16_t>(88);                                                              \
                                                                                                                      \
        MOCKER_CPP(                                                                                                   \
            cce_name, void(                                                                                           \
                          pos cce_data_type*, __cbuf__ cce_data_type*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, \
                          uint16_t, bool))                                                                            \
            .times(1)                                                                                                 \
            .will(invoke(mock##c_api_name##cce_name##_##data_type));                                                  \
        c_api_name(dst, src, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);             \
        GlobalMockObject::verify();                                                                                   \
    }

#define TEST_CUBE_DATAMOVE_L12L0(class_name, c_api_name, cce_name, data_type, pos) \
    TEST_CUBE_DATAMOVE_L12L0_IMPL(class_name, c_api_name, cce_name, data_type, data_type, pos)

#define TEST_CUBE_DATAMOVE_L12L0_S4_VOID(class_name, c_api_name, cce_name, data_type, pos) \
    TEST_CUBE_DATAMOVE_L12L0_IMPL(class_name, c_api_name, cce_name, data_type, void, pos)

TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, bfloat16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, float8_e4m3_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, float8_e5m2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, half, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, float, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, hifloat8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, int16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, int32_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, int8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, uint16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, uint32_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca, uint8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca_s4, fp4x2_e2m1_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca_s4, fp4x2_e1m2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca_s4, hif4x2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0A, asc_copy_l12l0a, load_cbuf_to_ca_s4, int4b_t, __ca__);

TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, bfloat16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, float8_e4m3_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, float8_e5m2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, half, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, float, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, hifloat8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, int16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, int32_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, int8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, uint16_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, uint32_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca, uint8_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca_s4, fp4x2_e2m1_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca_s4, fp4x2_e1m2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca_s4, hif4x2_t, __ca__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0A, asc_copy_l12l0a_transpose, load_cbuf_to_ca_s4, int4b_t, __ca__);

TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, bfloat16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, float8_e4m3_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, float8_e5m2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, half, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, float, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, hifloat8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, int16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, int32_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, int8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, uint16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, uint32_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb, uint8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb_s4, fp4x2_e2m1_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb_s4, fp4x2_e1m2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb_s4, hif4x2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0B, asc_copy_l12l0b, load_cbuf_to_cb_s4, int4b_t, __cb__);

TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, bfloat16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, float8_e4m3_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, float8_e5m2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, half, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, float, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, hifloat8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, int16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, int32_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, int8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, uint16_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, uint32_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb, uint8_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb_s4, fp4x2_e2m1_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb_s4, fp4x2_e1m2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb_s4, hif4x2_t, __cb__);
TEST_CUBE_DATAMOVE_L12L0_S4_VOID(CopyL12L0B, asc_copy_l12l0b_transpose, load_cbuf_to_cb_s4, int4b_t, __cb__);
