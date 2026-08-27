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
#include "include/c_api/asc_simd.h"

class TestAscLockCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void get_buf_block_stub(pipe_t pipe, uint8_t mutex_id, bool mode)
{
    EXPECT_EQ(pipe, pipe_t::PIPE_MTE3);
    EXPECT_EQ(mutex_id, static_cast<uint64_t>(10));
    EXPECT_EQ(mode, false);
}

void get_buf_non_block_stub(pipe_t pipe, uint8_t mutex_id, bool mode)
{
    EXPECT_EQ(pipe, pipe_t::PIPE_MTE3);
    EXPECT_EQ(mutex_id, static_cast<uint8_t>(10));
    EXPECT_EQ(mode, true);
}
} // namespace

TEST_F(TestAscLockCAPI, asc_lock_DefaultBlockSucc)
{
    MOCKER_CPP(get_buf, void(pipe_t, uint8_t, bool)).times(1).will(invoke(get_buf_block_stub));

    asc_lock(pipe_t::PIPE_MTE3, static_cast<uint8_t>(10));
    GlobalMockObject::verify();
}

TEST_F(TestAscLockCAPI, asc_lock_BlockSucc)
{
    MOCKER_CPP(get_buf, void(pipe_t, uint8_t, bool)).times(1).will(invoke(get_buf_block_stub));

    asc_lock(pipe_t::PIPE_MTE3, static_cast<uint8_t>(10), ASC_LOCK_BLOCK);
    GlobalMockObject::verify();
}

TEST_F(TestAscLockCAPI, asc_lock_NonBlockSucc)
{
    MOCKER_CPP(get_buf, void(pipe_t, uint8_t, bool)).times(1).will(invoke(get_buf_non_block_stub));

    asc_lock(pipe_t::PIPE_MTE3, static_cast<uint8_t>(10), ASC_LOCK_NON_BLOCK);
    GlobalMockObject::verify();
}
