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

__aicore__ inline void wait_intra_block_stub(pipe_t pipe, uint64_t sync_id)
{
    EXPECT_EQ(pipe, static_cast<pipe_t>(pipe_t::PIPE_S));
    EXPECT_EQ(sync_id, static_cast<uint64_t>(22));
}

class TEST_ASC_SYNC_INTRA_WAIT : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIV_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(coreType, pipeVal, dtype)                              \
    void wait_intra_block_stub_##coreType##_##pipeVal##_##dtype(pipe_t pipe, uint64_t sync_id)  \
    {                                                                                           \
        EXPECT_EQ(pipe, pipeVal);                                                               \
        EXPECT_EQ(sync_id, static_cast<uint64_t>(22));                                          \
    }                                                                                           \
                                                                                                \
    TEST_F(TEST_ASC_SYNC_INTRA_WAIT, TEST_ASC_SYNC_INTRA_WAIT_##coreType##_##pipeVal##_##dtype) \
    {                                                                                           \
        g_coreType = coreType;                                                                  \
        MOCKER_CPP(wait_intra_block, void(pipe_t, uint64_t))                                    \
            .times(1)                                                                           \
            .will(invoke(wait_intra_block_stub_##coreType##_##pipeVal##_##dtype));              \
        pipe_t pipe = pipeVal;                                                                  \
        dtype sync_id = static_cast<dtype>(22);                                                 \
        asc_sync_intra_wait(pipe, sync_id);                                                     \
        GlobalMockObject::verify();                                                             \
    }

TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_S, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_S, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_M, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_M, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE1, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE1, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE2, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_MTE2, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_FIX, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIC_TYPE, PIPE_FIX, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_S, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_S, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE2, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE2, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE3, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_MTE3, uint64_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_V, uint8_t);
TEST_ASC_SYNC_INTRA_WAIT_BY_PIPE(C_API_AIV_TYPE, PIPE_V, uint64_t);
