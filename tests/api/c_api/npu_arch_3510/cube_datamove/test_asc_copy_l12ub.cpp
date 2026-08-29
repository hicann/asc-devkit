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

class TestCubeDmamoveCopyL12UBCAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void copy_cbuf_to_ubuf_Stub(
    __ubuf__ void* dst_in, __cbuf__ void* src_in, bool sub_blockid_in, uint16_t n_burst_in, uint16_t len_burst_in,
    uint16_t src_gap, uint16_t dst_gap)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(1);
    __cbuf__ void* src = reinterpret_cast<__cbuf__ void*>(2);
    bool sub_blockid = false;
    uint16_t n_burst = 3;
    uint16_t len_burst = 4;
    uint16_t src_gap_size = 5;
    uint16_t dst_gap_size = 6;
    EXPECT_EQ(dst, dst_in);
    EXPECT_EQ(src, src_in);
    EXPECT_EQ(sub_blockid, sub_blockid_in);
    EXPECT_EQ(n_burst, n_burst_in);
    EXPECT_EQ(len_burst, len_burst_in);
    EXPECT_EQ(src_gap_size, src_gap);
    EXPECT_EQ(dst_gap_size, dst_gap);
}

} // namespace

TEST_F(TestCubeDmamoveCopyL12UBCAPI, c_api_asc_copy_l12ub_Succ)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(1);
    __cbuf__ void* src = reinterpret_cast<__cbuf__ void*>(2);
    int8_t sub_blockid = 0;
    uint16_t n_burst = 3;
    uint16_t len_burst = 4;
    uint16_t src_gap_size = 5;
    uint16_t dst_gap_size = 6;

    MOCKER(copy_cbuf_to_ubuf, void(__ubuf__ void*, __cbuf__ void*, bool, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_cbuf_to_ubuf_Stub));

    asc_copy_l12ub(dst, src, sub_blockid, n_burst, len_burst, src_gap_size, dst_gap_size);
    GlobalMockObject::verify();
}

TEST_F(TestCubeDmamoveCopyL12UBCAPI, c_api_asc_copy_l12ub_sync_Succ)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(1);
    __cbuf__ void* src = reinterpret_cast<__cbuf__ void*>(2);
    int8_t sub_blockid = 0;
    uint16_t n_burst = 3;
    uint16_t len_burst = 4;
    uint16_t src_gap_size = 5;
    uint16_t dst_gap_size = 6;

    MOCKER(copy_cbuf_to_ubuf, void(__ubuf__ void*, __cbuf__ void*, bool, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_cbuf_to_ubuf_Stub));

    asc_copy_l12ub_sync(dst, src, sub_blockid, n_burst, len_burst, src_gap_size, dst_gap_size);
    GlobalMockObject::verify();
}
