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

class TestAscSyncInterArriveCAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIV_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_ASC_SYNC_INTER_ARRIVE(coreType, pipeVal)                                                          \
    void sync_inter_arrive_3510_ffts_cross_core_sync_stub_##coreType##_##pipeVal(pipe_t pipe, uint64_t config) \
    {                                                                                                          \
        constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;                                                          \
        constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;                                                          \
        uint16_t mode = 0x00;                                                                                  \
        uint64_t flag_id = 5;                                                                                  \
        uint64_t expectedConfig =                                                                              \
            (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flag_id & 0xf) << SYNC_FLAG_SHIFT_VALUE));      \
        EXPECT_EQ(pipe, pipeVal);                                                                              \
        EXPECT_EQ(config, expectedConfig);                                                                     \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestAscSyncInterArriveCAPI, c_api_asc_sync_inter_arrive_##coreType##_##pipeVal)                     \
    {                                                                                                          \
        g_coreType = coreType;                                                                                 \
        MOCKER_CPP(ffts_cross_core_sync, void(pipe_t, uint64_t))                                               \
            .times(1)                                                                                          \
            .will(invoke(sync_inter_arrive_3510_ffts_cross_core_sync_stub_##coreType##_##pipeVal));            \
        pipe_t pipe = pipeVal;                                                                                 \
        int64_t flag_id = 5;                                                                                   \
        asc_sync_inter_arrive(pipe, flag_id);                                                                  \
        GlobalMockObject::verify();                                                                            \
    }

TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIC_TYPE, PIPE_M);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIC_TYPE, PIPE_MTE1);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIC_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIC_TYPE, PIPE_FIX);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIV_TYPE, PIPE_MTE2);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIV_TYPE, PIPE_MTE3);
TEST_ASC_SYNC_INTER_ARRIVE(C_API_AIV_TYPE, PIPE_V);
