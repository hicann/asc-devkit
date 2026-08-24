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

namespace {

constexpr uint32_t kCopySize = 32;

void VerifyCopyUb2GmParams(
    __gm__ void* dst, __ubuf__ void* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride)
{
    EXPECT_EQ(dst, reinterpret_cast<__gm__ void*>(11));
    EXPECT_EQ(src, reinterpret_cast<__ubuf__ void*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(burst_count, static_cast<uint32_t>(1));
    EXPECT_EQ(l2_cache_mode, static_cast<uint8_t>(0));
    EXPECT_EQ(dst_stride, static_cast<uint64_t>(0));
    EXPECT_EQ(src_stride, static_cast<uint32_t>(0));
}

void copy_ubuf_to_gm_align_v2_ConfigStub(
    __gm__ void* dst, __ubuf__ void* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride)
{
    VerifyCopyUb2GmParams(dst, src, sid, burst_count, burst_len, l2_cache_mode, dst_stride, src_stride);
    EXPECT_EQ(burst_len, static_cast<uint32_t>(1));
}

void copy_ubuf_to_gm_align_v2_SizeStub(
    __gm__ void* dst, __ubuf__ void* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride)
{
    VerifyCopyUb2GmParams(dst, src, sid, burst_count, burst_len, l2_cache_mode, dst_stride, src_stride);
    EXPECT_EQ(burst_len, kCopySize);
}

} // namespace

class TestVectorDatamoveCopyUB2GMCApi : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestVectorDatamoveCopyUB2GMCApi, asc_copy_ub2gm_CopyConfig_Succ)
{
    __gm__ void* dst = reinterpret_cast<__gm__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    MOCKER_CPP(
        copy_ubuf_to_gm_align_v2,
        void(__gm__ void*, __ubuf__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_ubuf_to_gm_align_v2_ConfigStub));
    asc_copy_ub2gm(
        dst, src, static_cast<uint16_t>(1), static_cast<uint16_t>(1), static_cast<uint16_t>(0),
        static_cast<uint16_t>(0));
    GlobalMockObject::verify();
}

TEST_F(TestVectorDatamoveCopyUB2GMCApi, asc_copy_ub2gm_size_Succ)
{
    __gm__ void* dst = reinterpret_cast<__gm__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    MOCKER_CPP(
        copy_ubuf_to_gm_align_v2,
        void(__gm__ void*, __ubuf__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_ubuf_to_gm_align_v2_SizeStub));
    asc_copy_ub2gm(dst, src, kCopySize);
    GlobalMockObject::verify();
}

TEST_F(TestVectorDatamoveCopyUB2GMCApi, asc_copy_ub2gm_sync_Succ)
{
    __gm__ void* dst = reinterpret_cast<__gm__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    MOCKER_CPP(
        copy_ubuf_to_gm_align_v2,
        void(__gm__ void*, __ubuf__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_ubuf_to_gm_align_v2_SizeStub));
    asc_copy_ub2gm_sync(dst, src, kCopySize);
    GlobalMockObject::verify();
}
