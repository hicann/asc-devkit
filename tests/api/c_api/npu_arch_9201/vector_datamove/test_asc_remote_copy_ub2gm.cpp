/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software; you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#define TEST_REMOTE_COPY_UB2GM(data_type)                                      \
    TEST(TestVectorDatamoveRemoteCopyUB2GM, CopyConfigSucc_##data_type)        \
    {                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);   \
        reset_copy_ubuf_to_gm_align_v2_stub_state();                           \
        asc_remote_copy_ub2gm(dst, src, 3, 44, true, false, 9, 5, 6, true);    \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);       \
        EXPECT_TRUE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl);           \
        EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl);   \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size, 9U); \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode, 0U);    \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 5U);       \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 6U);       \
        EXPECT_TRUE(g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl);       \
    }                                                                          \
    TEST(TestVectorDatamoveRemoteCopyUB2GM, CopySizeSucc_##data_type)          \
    {                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);   \
        reset_copy_ubuf_to_gm_align_v2_stub_state();                           \
        asc_remote_copy_ub2gm(dst, src, 44, false, true, 7, false);            \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);       \
        EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl);          \
        EXPECT_TRUE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl);    \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size, 7U); \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode, 0U);    \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 0U);       \
        EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 0U);       \
        EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl);      \
    }

TEST_REMOTE_COPY_UB2GM(uint8_t);
TEST_REMOTE_COPY_UB2GM(int8_t);
TEST_REMOTE_COPY_UB2GM(uint16_t);
TEST_REMOTE_COPY_UB2GM(int16_t);
TEST_REMOTE_COPY_UB2GM(uint32_t);
TEST_REMOTE_COPY_UB2GM(int32_t);
TEST_REMOTE_COPY_UB2GM(half);
TEST_REMOTE_COPY_UB2GM(float);
TEST_REMOTE_COPY_UB2GM(hifloat8_t);
TEST_REMOTE_COPY_UB2GM(bfloat16_t);
TEST_REMOTE_COPY_UB2GM(fp8_e5m2_t);
TEST_REMOTE_COPY_UB2GM(fp8_e4m3fn_t);
TEST_REMOTE_COPY_UB2GM(fp8_e8m0_t);
TEST_REMOTE_COPY_UB2GM(fp8_e6m2_t);

#undef TEST_REMOTE_COPY_UB2GM
