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

class TestSyncBlkWait : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIV_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_ASC_SYNC_BLOCK_WAIT(coreType, pipeVal)                                                 \
    void asc_sync_block_wait_stub_##coreType##_##pipeVal(int64_t flagID) { EXPECT_EQ(33, flagID); } \
                                                                                                    \
    TEST_F(TestSyncBlkWait, sync_block_wait_##coreType##_##pipeVal)                                 \
    {                                                                                               \
        g_coreType = coreType;                                                                      \
        MOCKER_CPP(wait_flag_dev, void(int64_t))                                                    \
            .times(1)                                                                               \
            .will(invoke(asc_sync_block_wait_stub_##coreType##_##pipeVal));                         \
                                                                                                    \
        pipe_t pipe = pipeVal;                                                                      \
        int64_t flagID = 33;                                                                        \
                                                                                                    \
        asc_sync_block_wait(pipe, flagID);                                                          \
        GlobalMockObject::verify();                                                                 \
    }

TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIC_TYPE, PIPE_M);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIC_TYPE, PIPE_MTE1);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIC_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIC_TYPE, PIPE_FIX);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIV_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIV_TYPE, PIPE_MTE3);
TEST_ASC_SYNC_BLOCK_WAIT(C_API_AIV_TYPE, PIPE_V);
