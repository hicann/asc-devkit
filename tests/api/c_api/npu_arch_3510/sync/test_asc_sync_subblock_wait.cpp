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

class TEST_ASC_SYNC_SUBBLOCK_WAIT : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIV_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(coreType, pipeVal)                                 \
    void wait_flag_dev_subblock_mode_stub_##coreType##_##pipeVal(pipe_t pipe, int64_t flag_id) \
    {                                                                                          \
        EXPECT_EQ(pipe, pipeVal);                                                              \
        EXPECT_EQ(flag_id, static_cast<int64_t>(11));                                          \
    }                                                                                          \
                                                                                               \
    TEST_F(TEST_ASC_SYNC_SUBBLOCK_WAIT, TEST_ASC_SYNC_SUBBLOCK_WAIT_##coreType##_##pipeVal)    \
    {                                                                                          \
        g_coreType = coreType;                                                                 \
        MOCKER_CPP(wait_flag_dev, void(pipe_t, int64_t))                                       \
            .times(1)                                                                          \
            .will(invoke(wait_flag_dev_subblock_mode_stub_##coreType##_##pipeVal));            \
        pipe_t pipe = pipeVal;                                                                 \
        int64_t flag_id = static_cast<int64_t>(11);                                            \
        asc_sync_subblock_wait(pipe, flag_id);                                                 \
        GlobalMockObject::verify();                                                            \
    }

TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_S);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_M);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE1);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_FIX);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_S);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE3);
TEST_ASC_SYNC_SUBBLOCK_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_V);
