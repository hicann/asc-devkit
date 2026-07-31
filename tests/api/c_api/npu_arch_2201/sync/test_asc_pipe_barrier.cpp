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
#include "include/c_api/asc_simd.h"

class TestSyncVec : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void pipe_barrier_Stub(pipe_t pipe) {}

void pipe_barrier_v_Stub(pipe_t pipe) { EXPECT_EQ(pipe_t::PIPE_V, pipe); }
} // namespace

TEST_F(TestSyncVec, c_api_asc_sync_vec_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_Stub));
    asc_sync_vec();
    GlobalMockObject::verify();
}

TEST_F(TestSyncVec, c_api_asc_sync_vec_new_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_v_Stub));
    asc_sync_vec(0);
    GlobalMockObject::verify();
}

class TestSyncMte2 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void asc_sync_mte2_Stub(int id) {}
} // namespace

TEST_F(TestSyncMte2, c_api_asc_sync_mte2_Succ)
{
    MOCKER_CPP(asc_sync_mte2, void(int)).times(1).will(invoke(asc_sync_mte2_Stub));
    asc_sync_mte2(0);
    GlobalMockObject::verify();
}

class TestSyncMte3 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void asc_sync_mte3_Stub(int id) {}
} // namespace

TEST_F(TestSyncMte3, c_api_asc_sync_mte3_Succ)
{
    MOCKER_CPP(asc_sync_mte3, void(int)).times(1).will(invoke(asc_sync_mte3_Stub));
    asc_sync_mte3(0);
    GlobalMockObject::verify();
}

class TestSync : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestSync, c_api_asc_sync_Succ)
{
    MOCKER_CPP(pipe_barrier, void(pipe_t)).times(1).will(invoke(pipe_barrier_Stub));
    asc_sync();
    GlobalMockObject::verify();
}
