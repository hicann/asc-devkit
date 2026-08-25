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

class TestSaturationStrategyCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

namespace {
int64_t g_ctrlValue = 0;

int64_t get_ctrl_stub() { return g_ctrlValue; }

void set_ctrl_stub(uint64_t config) { g_ctrlValue = static_cast<int64_t>(config); }

uint64_t sbitset0_stub(uint64_t bits, int64_t idx) { return bits & ~(static_cast<uint64_t>(1) << idx); }

uint64_t sbitset1_stub(uint64_t bits, int64_t idx) { return bits | (static_cast<uint64_t>(1) << idx); }

void MockCtrlRw()
{
    MOCKER_CPP(get_ctrl, int64_t(void)).stubs().will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).stubs().will(invoke(set_ctrl_stub));
    MOCKER_CPP(sbitset0, uint64_t(uint64_t, int64_t)).stubs().will(invoke(sbitset0_stub));
    MOCKER_CPP(sbitset1, uint64_t(uint64_t, int64_t)).stubs().will(invoke(sbitset1_stub));
}

int64_t GetCtrlBit(int8_t bit) { return (g_ctrlValue >> bit) & 1; }
} // namespace

TEST_F(TestSaturationStrategyCAPI, UseApi)
{
    // USE_API: CTRL[60]=0
    g_ctrlValue = 0;
    MockCtrlRw();

    asc_set_saturation_strategy(asc_override_strategy::USE_API);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_API);
    EXPECT_EQ(GetCtrlBit(60), 0);
}

TEST_F(TestSaturationStrategyCAPI, UseGlobal)
{
    // USE_GLOBAL: CTRL[60]=1
    g_ctrlValue = 0;
    MockCtrlRw();

    asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(GetCtrlBit(60), 1);
}

TEST_F(TestSaturationStrategyCAPI, RoundTrip)
{
    g_ctrlValue = 0;
    MockCtrlRw();

    asc_set_saturation_strategy(asc_override_strategy::USE_API);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_API);
    EXPECT_EQ(GetCtrlBit(60), 0);

    asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(GetCtrlBit(60), 1);

    asc_set_saturation_strategy(asc_override_strategy::USE_API);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_API);
    EXPECT_EQ(GetCtrlBit(60), 0);
}

TEST_F(TestSaturationStrategyCAPI, PreserveOtherBits)
{
    // Pre-set CTRL[48], ensure strategy only touches bit60.
    g_ctrlValue = static_cast<int64_t>(1) << 48;
    MockCtrlRw();

    asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_GLOBAL);
    EXPECT_EQ(GetCtrlBit(60), 1);
    EXPECT_EQ(GetCtrlBit(48), 1);

    asc_set_saturation_strategy(asc_override_strategy::USE_API);
    EXPECT_EQ(asc_get_saturation_strategy(), asc_override_strategy::USE_API);
    EXPECT_EQ(GetCtrlBit(60), 0);
    EXPECT_EQ(GetCtrlBit(48), 1);
}
