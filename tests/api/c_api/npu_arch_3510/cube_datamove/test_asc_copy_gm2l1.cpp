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

class TestCubeDatamoveCopyGM2L1 : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

template <typename DTYPE>
void load_gm_to_cbuf_2dv2_Stub(
    __cbuf__ DTYPE* dst_in, __gm__ DTYPE* src_in, uint32_t m_start_position_in, uint32_t k_start_position_in,
    uint16_t dst_stride_in, uint16_t m_step_in, uint16_t k_step_in, uint8_t sid_in, uint8_t decomp_mode_in,
    uint8_t l2_cache_ctl_in)
{
    __cbuf__ DTYPE* dst = reinterpret_cast<__cbuf__ DTYPE*>(1);
    __gm__ DTYPE* src = reinterpret_cast<__gm__ DTYPE*>(2);
    uint32_t m_start_position = 3;
    uint32_t k_start_position = 4;
    uint16_t dst_stride = 5;
    uint16_t m_step = 6;
    uint16_t k_step = 7;
    uint8_t sid = 0;
    uint8_t decomp_mode = 8;
    uint8_t l2_cache_ctl = 9;

    EXPECT_EQ(dst, dst_in);
    EXPECT_EQ(src, src_in);
    EXPECT_EQ(m_start_position, m_start_position_in);
    EXPECT_EQ(k_start_position, k_start_position_in);
    EXPECT_EQ(dst_stride, dst_stride_in);
    EXPECT_EQ(m_step, m_step_in);
    EXPECT_EQ(k_step, k_step_in);
    EXPECT_EQ(sid, sid_in);
    EXPECT_EQ(decomp_mode, decomp_mode_in);
    EXPECT_EQ(l2_cache_ctl, l2_cache_ctl_in);
}

#define TEST_CUBE_DATAMOVE_GM2L1(class_name, c_api_name, cce_name, data_type)                                       \
    TEST_F(TestCubeDatamoveCopyGM2L1, c_api_name##_##data_type##_Succ)                                              \
    {                                                                                                               \
        __cbuf__ data_type* dst = reinterpret_cast<__cbuf__ data_type*>(1);                                         \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(2);                                             \
        uint32_t m_start_position = 3;                                                                              \
        uint32_t k_start_position = 4;                                                                              \
        uint16_t dst_stride = 5;                                                                                    \
        uint16_t m_step = 6;                                                                                        \
        uint16_t k_step = 7;                                                                                        \
        uint8_t decomp_mode = 8;                                                                                    \
        uint8_t l2_cache_ctl = 9;                                                                                   \
        MOCKER(                                                                                                     \
            cce_name, void(                                                                                         \
                          __cbuf__ data_type*, __gm__ data_type*, uint32_t, uint32_t, uint16_t, uint16_t, uint16_t, \
                          uint8_t, uint8_t, uint8_t))                                                               \
            .times(1)                                                                                               \
            .will(invoke(&load_gm_to_cbuf_2dv2_Stub<data_type>));                                                   \
        c_api_name(                                                                                                 \
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, decomp_mode, l2_cache_ctl);   \
        GlobalMockObject::verify();                                                                                 \
    }

// asc_copy_gm2l1 tests
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, bfloat16_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, float);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, float8_e4m3_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, float8_e5m2_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, half);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, hifloat8_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, int16_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, int32_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, int8_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, uint16_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, uint32_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2, uint8_t);

TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2_s4, float4_e1m2x2_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2_s4, float4_e2m1x2_t);
TEST_CUBE_DATAMOVE_GM2L1(GM2L1, asc_copy_gm2l1, load_gm_to_cbuf_2dv2_s4, void);

__aicore__ inline void copy_gm_to_cbuf_v2_stub(
    __cbuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode,
    uint8_t l2_cache_ctl, uint64_t src_stride, uint32_t dst_stride)
{
    EXPECT_EQ(dst, reinterpret_cast<__cbuf__ void*>(11));
    EXPECT_EQ(src, reinterpret_cast<__gm__ void*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(n_burst, static_cast<uint32_t>(33));
    EXPECT_EQ(len_burst, static_cast<uint32_t>(44));
    EXPECT_EQ(pad_func_mode, static_cast<uint8_t>(55));
    EXPECT_EQ(l2_cache_ctl, static_cast<uint8_t>(0));
    EXPECT_EQ(src_stride, static_cast<uint64_t>(66));
    EXPECT_EQ(dst_stride, static_cast<uint64_t>(77));
}

class TEST_COPY_GM_TO_L1 : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

TEST_F(TEST_COPY_GM_TO_L1, TEST_COPY_GM_TO_L1)
{
    MOCKER_CPP(
        copy_gm_to_cbuf_v2,
        void(__cbuf__ void*, __gm__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_gm_to_cbuf_v2_stub));

    __cbuf__ void* dst = reinterpret_cast<__cbuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);

    uint16_t n_burst = static_cast<uint32_t>(33);
    uint16_t len_burst = static_cast<uint32_t>(44);
    uint16_t pad_func_mode = static_cast<uint8_t>(55);
    uint16_t src_stride = static_cast<uint64_t>(66);
    uint16_t dst_stride = static_cast<uint64_t>(77);

    asc_copy_gm2l1(dst, src, n_burst, len_burst, pad_func_mode, src_stride, dst_stride);
    GlobalMockObject::verify();
}

TEST_F(TEST_COPY_GM_TO_L1, TEST_COPY_GM_TO_L1_SYNC)
{
    MOCKER_CPP(
        copy_gm_to_cbuf_v2,
        void(__cbuf__ void*, __gm__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_gm_to_cbuf_v2_stub));

    __cbuf__ void* dst = reinterpret_cast<__cbuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);

    uint16_t n_burst = static_cast<uint32_t>(33);
    uint16_t len_burst = static_cast<uint32_t>(44);
    uint16_t pad_func_mode = static_cast<uint8_t>(55);
    uint16_t src_stride = static_cast<uint64_t>(66);
    uint16_t dst_stride = static_cast<uint64_t>(77);

    asc_copy_gm2l1_sync(dst, src, n_burst, len_burst, pad_func_mode, src_stride, dst_stride);
    GlobalMockObject::verify();
}
