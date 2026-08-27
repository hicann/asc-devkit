/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
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
#include "include/c_api/asc_simd.h"

class TestSyncInstrCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void asc_sync_notify_stub(pipe_t pipe, pipe_t tpipe, event_t id)
{
    EXPECT_EQ(pipe_t::PIPE_V, pipe);
    EXPECT_EQ(pipe_t::PIPE_MTE2, tpipe);
    EXPECT_EQ(event_t::EVENT_ID0, id);
}

void asc_sync_wait_stub(pipe_t pipe, pipe_t tpipe, event_t id)
{
    EXPECT_EQ(pipe_t::PIPE_MTE2, pipe);
    EXPECT_EQ(pipe_t::PIPE_V, tpipe);
    EXPECT_EQ(event_t::EVENT_ID0, id);
}

void pipe_barrier_all_stub(pipe_t pipe) { EXPECT_EQ(pipe_t::PIPE_ALL, pipe); }

void pipe_barrier_v_stub(pipe_t pipe) { EXPECT_EQ(pipe_t::PIPE_V, pipe); }

void pipe_barrier_mte2_stub(pipe_t pipe) { EXPECT_EQ(pipe_t::PIPE_MTE2, pipe); }

void pipe_barrier_mte3_stub(pipe_t pipe) { EXPECT_EQ(pipe_t::PIPE_MTE3, pipe); }
} // namespace

TEST_F(TestSyncInstrCAPI, c_api_sync_notify_Succ)
{
    pipe_t pipe = pipe_t::PIPE_V;
    pipe_t tpipe = pipe_t::PIPE_MTE2;
    event_t id = event_t::EVENT_ID0;
    MOCKER_CPP(set_flag, void(pipe_t, pipe_t, event_t)).times(1).will(invoke(asc_sync_notify_stub));

    asc_sync_notify(pipe, tpipe, id);
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, c_api_sync_wait_Succ)
{
    pipe_t pipe = pipe_t::PIPE_MTE2;
    pipe_t tpipe = pipe_t::PIPE_V;
    event_t id = event_t::EVENT_ID0;
    MOCKER_CPP(wait_flag, void(pipe_t, pipe_t, event_t)).times(1).will(invoke(asc_sync_wait_stub));

    asc_sync_wait(pipe, tpipe, id);
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, asc_sync_vec_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_all_stub));

    asc_sync_vec();
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, asc_sync_vec_new_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_v_stub));

    asc_sync_vec(0);
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, asc_sync_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_all_stub));

    asc_sync();
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, asc_sync_mte2_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_mte2_stub));

    asc_sync_mte2(0);
    GlobalMockObject::verify();
}

TEST_F(TestSyncInstrCAPI, asc_sync_mte3_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_mte3_stub));

    asc_sync_mte3(0);
    GlobalMockObject::verify();
}
