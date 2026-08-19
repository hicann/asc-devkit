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

class TestSetHf32RoundModeCAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {

constexpr uint64_t CTRL_VAL = 0x12345678ULL;
constexpr int32_t HF32_TRANS_MODE_BIT = 47;
constexpr uint64_t HF32_TRANS_MODE_BIT_MASK = 1ULL << HF32_TRANS_MODE_BIT;

int64_t get_ctrl_Stub() { return static_cast<int64_t>(CTRL_VAL); }

int64_t get_ctrl_with_hf32_trans_Stub() { return static_cast<int64_t>(CTRL_VAL | HF32_TRANS_MODE_BIT_MASK); }

uint64_t sbitset0_hf32_trans_stub(uint64_t bits, int64_t idx)
{
    EXPECT_EQ(static_cast<uint64_t>(CTRL_VAL | HF32_TRANS_MODE_BIT_MASK), bits);
    EXPECT_EQ(HF32_TRANS_MODE_BIT, idx);
    return CTRL_VAL;
}

uint64_t sbitset1_hf32_trans_stub(uint64_t bits, int64_t idx)
{
    EXPECT_EQ(CTRL_VAL, bits);
    EXPECT_EQ(HF32_TRANS_MODE_BIT, idx);
    return CTRL_VAL | HF32_TRANS_MODE_BIT_MASK;
}

void set_ctrl_nearest_even_stub(uint64_t ctrl_val) { EXPECT_EQ(CTRL_VAL, ctrl_val); }

void set_ctrl_nearest_away_stub(uint64_t ctrl_val) { EXPECT_EQ(CTRL_VAL | HF32_TRANS_MODE_BIT_MASK, ctrl_val); }

} // namespace

TEST_F(TestSetHf32RoundModeCAPI, SetHf32RoundMode_NearestAway_Success)
{
    MOCKER_CPP(get_ctrl, int64_t()).times(1).will(invoke(get_ctrl_Stub));

    MOCKER_CPP(sbitset1, uint64_t(uint64_t, int64_t)).times(1).will(invoke(sbitset1_hf32_trans_stub));

    MOCKER_CPP(set_ctrl, void(uint64_t ctrl_val)).times(1).will(invoke(set_ctrl_nearest_away_stub));

    asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_AWAY);
    GlobalMockObject::verify();
}

TEST_F(TestSetHf32RoundModeCAPI, SetHf32RoundMode_NearestEven_Success)
{
    MOCKER_CPP(get_ctrl, int64_t()).times(1).will(invoke(get_ctrl_with_hf32_trans_Stub));

    MOCKER_CPP(sbitset0, uint64_t(uint64_t, int64_t)).times(1).will(invoke(sbitset0_hf32_trans_stub));

    MOCKER_CPP(set_ctrl, void(uint64_t ctrl_val)).times(1).will(invoke(set_ctrl_nearest_even_stub));

    asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_EVEN);
    GlobalMockObject::verify();
}
