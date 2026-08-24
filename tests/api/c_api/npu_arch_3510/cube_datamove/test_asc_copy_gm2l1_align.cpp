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

template <typename DTYPE>
__aicore__ inline void copy_gm_to_cbuf_align_v2_stub(
    __cbuf__ DTYPE* dst, __gm__ DTYPE* src, uint8_t sid, uint32_t n_burst, uint32_t len_burst,
    uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl,
    uint64_t burst_src_stride, uint32_t burst_dst_stride)
{
    EXPECT_EQ(dst, reinterpret_cast<__cbuf__ DTYPE*>(11));
    EXPECT_EQ(src, reinterpret_cast<__gm__ DTYPE*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(n_burst, static_cast<uint32_t>(33));
    EXPECT_EQ(len_burst, static_cast<uint32_t>(44));
    EXPECT_EQ(left_padding_count, static_cast<uint8_t>(55));
    EXPECT_EQ(right_padding_count, static_cast<uint8_t>(66));
    EXPECT_EQ(data_select_bit, static_cast<bool>(false));
    EXPECT_EQ(l2_cache_ctl, static_cast<uint8_t>(4));
    EXPECT_EQ(burst_src_stride, static_cast<uint64_t>(88));
    EXPECT_EQ(burst_dst_stride, static_cast<uint32_t>(99));
}

class TEST_COPY_GM_TO_L1_ALIGN : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(dtype)                                                              \
                                                                                                                \
    TEST_F(TEST_COPY_GM_TO_L1_ALIGN, TEST_COPY_GM_TO_L1_ALIGN_##dtype)                                          \
    {                                                                                                           \
        MOCKER_CPP(                                                                                             \
            copy_gm_to_cbuf_align_v2, void(                                                                     \
                                          __cbuf__ dtype*, __gm__ dtype*, uint8_t, uint32_t, uint32_t, uint8_t, \
                                          uint8_t, bool, uint8_t, uint64_t, uint32_t))                          \
            .times(1)                                                                                           \
            .will(invoke(copy_gm_to_cbuf_align_v2_stub<dtype>));                                                \
                                                                                                                \
        __cbuf__ dtype* dst = reinterpret_cast<__cbuf__ dtype*>(11);                                            \
        __gm__ dtype* src = reinterpret_cast<__gm__ dtype*>(22);                                                \
                                                                                                                \
        uint32_t burst_count = 33;                                                                              \
        uint32_t burst_len = 44;                                                                                \
        uint8_t left_padding_count = 55;                                                                        \
        uint8_t right_padding_count = 66;                                                                       \
        bool enable_data_select = false;                                                                        \
        asc_load_l2_cache_mode l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP;                           \
        uint64_t src_stride = 88;                                                                               \
        uint32_t dst_stride = 99;                                                                               \
                                                                                                                \
        asc_copy_gm2l1_align(                                                                                   \
            dst, src, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,      \
            l2_cache_mode, src_stride, dst_stride);                                                             \
        GlobalMockObject::verify();                                                                             \
    }

// ==========asc_copy_gm2l1_align==========
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(bfloat16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(half);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(float);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(hifloat8_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(int16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(int32_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(int8_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(uint16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(uint32_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ALIGN(uint8_t);
