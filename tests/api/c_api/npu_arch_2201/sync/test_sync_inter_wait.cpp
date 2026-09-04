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
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"

class TestSyncInterWait : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void asc_sync_inter_wait_stub(int64_t flagID) { EXPECT_EQ(33, flagID); }
} // namespace

TEST_F(TestSyncInterWait, sync_inter_wait_Succ)
{
    int64_t flagID = 33;
    MOCKER_CPP(wait_flag_dev, void(int64_t)).times(1).will(invoke(asc_sync_inter_wait_stub));

    asc_sync_inter_wait(pipe_t::PIPE_S, flagID);
    GlobalMockObject::verify();
}
