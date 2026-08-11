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

class TestAscSyncBlockArriveCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void ffts_cross_core_sync_stub(pipe_t pipe, uint64_t config)
{
    constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;
    constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;

    uint16_t mode = 0x02;
    uint64_t flag_id = 5;
    uint64_t expectedConfig =
        (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flag_id & 0xf) << SYNC_FLAG_SHIFT_VALUE));
    EXPECT_EQ(pipe, pipe_t::PIPE_V);
    EXPECT_EQ(config, expectedConfig);
}
} // namespace

TEST_F(TestAscSyncBlockArriveCAPI, c_api_asc_sync_block_arrive_succ)
{
    pipe_t pipe = pipe_t::PIPE_V;
    int64_t flag_id = 5;
    MOCKER_CPP(ffts_cross_core_sync, void(pipe_t, uint64_t)).times(1).will(invoke(ffts_cross_core_sync_stub));

    asc_sync_block_arrive(pipe, flag_id);
    GlobalMockObject::verify();
}
