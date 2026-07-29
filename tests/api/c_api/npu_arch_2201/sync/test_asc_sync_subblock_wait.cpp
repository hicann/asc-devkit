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

__aicore__ inline void wait_flag_dev_subblock_stub(int64_t flag_id) { EXPECT_EQ(flag_id, static_cast<int64_t>(11)); }

class TEST_ASC_SYNC_SUBBLOCK_WAIT : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TEST_ASC_SYNC_SUBBLOCK_WAIT, TEST_ASC_SYNC_SUBBLOCK_WAIT_MODE)
{
    MOCKER_CPP(wait_flag_dev, void(int64_t)).times(1).will(invoke(wait_flag_dev_subblock_stub));

    pipe_t pipe = static_cast<pipe_t>(pipe_t::PIPE_S);
    int64_t flag_id = static_cast<int64_t>(11);

    asc_sync_subblock_wait(pipe, flag_id);
    GlobalMockObject::verify();
}
