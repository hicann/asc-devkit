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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

TEST(TestVectorDatamoveCopyUB2GM, CopyConfigSucc)
{
    __gm__ void* dst = reinterpret_cast<__gm__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    reset_copy_ubuf_to_gm_align_v2_stub_state();
    asc_copy_ub2gm(dst, src, 1, 44, 5, 6);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 5U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 6U);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl);
}

TEST(TestVectorDatamoveCopyUB2GM, CopySizeSucc)
{
    __gm__ void* dst = reinterpret_cast<__gm__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    reset_copy_ubuf_to_gm_align_v2_stub_state();
    asc_copy_ub2gm(dst, src, 32U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 0U);
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 0U);
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl);
}
