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

#define RESET_COPY_GM2UB() reset_copy_gm_to_ubuf_align_v2_stub_state()

#define VERIFY_COPY_GM2UB()                                             \
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.call_count, 1U);    \
    EXPECT_FALSE(g_copy_gm_to_ubuf_align_v2_stub_state.pre_allocation); \
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.l2_cache_mode, 0U); \
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.src_stride, 0U);    \
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.dst_stride, 0U);    \
    EXPECT_FALSE(g_copy_gm_to_ubuf_align_v2_stub_state.non_eod_ctrl)

class TestVectorDatamoveCopyGM2UBCApi : public testing::Test {};

TEST_F(TestVectorDatamoveCopyGM2UBCApi, CopyConfigSucc)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    RESET_COPY_GM2UB();
    asc_copy_gm2ub(
        dst, src, static_cast<uint32_t>(1), static_cast<uint32_t>(44), static_cast<uint64_t>(0),
        static_cast<uint32_t>(0));
    VERIFY_COPY_GM2UB();
}

TEST_F(TestVectorDatamoveCopyGM2UBCApi, CopySizeSucc)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    RESET_COPY_GM2UB();
    asc_copy_gm2ub(dst, src, static_cast<uint32_t>(44));
    VERIFY_COPY_GM2UB();
}

TEST_F(TestVectorDatamoveCopyGM2UBCApi, CopyExtendedConfigSucc)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    RESET_COPY_GM2UB();
    asc_copy_gm2ub(
        dst, src, static_cast<uint32_t>(1), static_cast<uint32_t>(44), true, static_cast<uint16_t>(5),
        static_cast<uint16_t>(6), true);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.call_count, 1U);
    EXPECT_TRUE(g_copy_gm_to_ubuf_align_v2_stub_state.pre_allocation);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.l2_cache_mode, 0U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.src_stride, 5U);
    EXPECT_EQ(g_copy_gm_to_ubuf_align_v2_stub_state.dst_stride, 6U);
    EXPECT_TRUE(g_copy_gm_to_ubuf_align_v2_stub_state.non_eod_ctrl);
}

#undef RESET_COPY_GM2UB
#undef VERIFY_COPY_GM2UB
