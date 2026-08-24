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

void VerifyCopyGm2UbParams(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint8_t*>(11));
    EXPECT_EQ(src, reinterpret_cast<__gm__ uint8_t*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(burst_count, static_cast<uint32_t>(1));
    EXPECT_EQ(left_padding_num, static_cast<uint8_t>(0));
    EXPECT_EQ(right_padding_num, static_cast<uint8_t>(0));
    EXPECT_EQ(enable_constant_pad, false);
    EXPECT_EQ(l2_cache_mode, static_cast<uint8_t>(0));
    EXPECT_EQ(src_stride, static_cast<uint64_t>(0));
    EXPECT_EQ(dst_stride, static_cast<uint32_t>(0));
}

void copy_gm_to_ubuf_align_v2_ConfigStub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    VerifyCopyGm2UbParams(
        dst, src, sid, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, l2_cache_mode,
        src_stride, dst_stride);
    EXPECT_EQ(burst_len, static_cast<uint32_t>(1));
}

void copy_gm_to_ubuf_align_v2_SizeStub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint8_t sid, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    VerifyCopyGm2UbParams(
        dst, src, sid, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, l2_cache_mode,
        src_stride, dst_stride);
    EXPECT_EQ(burst_len, kCopySize);
}

} // namespace

class TestVectorDatamoveCopyGM2UBCApi : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestVectorDatamoveCopyGM2UBCApi, asc_copy_gm2ub_CopyConfig_Succ)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    MOCKER_CPP(
        copy_gm_to_ubuf_align_v2, void(
                                      __ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t,
                                      bool, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_gm_to_ubuf_align_v2_ConfigStub));
    asc_copy_gm2ub(
        dst, src, static_cast<uint16_t>(1), static_cast<uint16_t>(1), static_cast<uint16_t>(0),
        static_cast<uint16_t>(0));
    GlobalMockObject::verify();
}

TEST_F(TestVectorDatamoveCopyGM2UBCApi, asc_copy_gm2ub_size_Succ)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    MOCKER_CPP(
        copy_gm_to_ubuf_align_v2, void(
                                      __ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t,
                                      bool, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_gm_to_ubuf_align_v2_SizeStub));
    asc_copy_gm2ub(dst, src, kCopySize);
    GlobalMockObject::verify();
}

TEST_F(TestVectorDatamoveCopyGM2UBCApi, asc_copy_gm2ub_sync_Succ)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __gm__ void* src = reinterpret_cast<__gm__ void*>(22);
    MOCKER_CPP(
        copy_gm_to_ubuf_align_v2, void(
                                      __ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t,
                                      bool, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(copy_gm_to_ubuf_align_v2_SizeStub));
    asc_copy_gm2ub_sync(dst, src, kCopySize);
    GlobalMockObject::verify();
}
