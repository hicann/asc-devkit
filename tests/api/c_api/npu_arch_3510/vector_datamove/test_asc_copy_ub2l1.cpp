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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/vector_datamove/vector_datamove.h"

class TestAscCopyUB2L1 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void copy_ubuf_to_cbuf_stub(
    __cbuf__ void* dst, __ubuf__ void* src, uint8_t sid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap)
{
    EXPECT_EQ(dst, reinterpret_cast<__cbuf__ void*>(11));
    EXPECT_EQ(src, reinterpret_cast<__ubuf__ void*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(burst_count, static_cast<uint16_t>(1));
    EXPECT_EQ(burst_len, static_cast<uint16_t>(1));
    EXPECT_EQ(src_gap, static_cast<uint16_t>(0));
    EXPECT_EQ(dst_gap, static_cast<uint16_t>(0));
}
} // namespace

TEST_F(TestAscCopyUB2L1, copy_ubuf_to_cbuf_CopyConfig_Succ)
{
    __cbuf__ void* dst = reinterpret_cast<__cbuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);

    uint16_t burst_count = static_cast<uint64_t>(1);
    uint16_t burst_len = static_cast<uint64_t>(1);
    uint16_t src_gap = static_cast<uint64_t>(0);
    uint16_t dst_gap = static_cast<uint64_t>(0);
    MOCKER_CPP(copy_ubuf_to_cbuf, void(__cbuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_cbuf_stub));
    asc_copy_ub2l1(dst, src, burst_count, burst_len, src_gap, dst_gap);
    GlobalMockObject::verify();
}

TEST_F(TestAscCopyUB2L1, copy_ubuf_to_cbuf_Size_Succ)
{
    __cbuf__ void* dst = reinterpret_cast<__cbuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    uint32_t size = static_cast<uint32_t>(44);

    MOCKER_CPP(copy_ubuf_to_cbuf, void(__cbuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_cbuf_stub));
    asc_copy_ub2l1(dst, src, size);
    GlobalMockObject::verify();
}

TEST_F(TestAscCopyUB2L1, copy_ubuf_to_cbuf_Sync_Succ)
{
    __cbuf__ void* dst = reinterpret_cast<__cbuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    uint32_t size = static_cast<uint32_t>(44);

    MOCKER_CPP(copy_ubuf_to_cbuf, void(__cbuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_cbuf_stub));
    asc_copy_ub2l1(dst, src, size);
    GlobalMockObject::verify();
}
