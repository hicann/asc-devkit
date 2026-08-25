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

class TestSaturationFlagCAPI : public testing::Test {
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

void CheckSaturationFlag(asc_saturation_mode mode, int8_t ctrlBit, int64_t enabledValue, int64_t disabledValue)
{
    g_ctrlValue = 0;
    MockCtrlRw();

    asc_set_saturation_flag(mode, true);
    EXPECT_EQ(asc_get_saturation_flag(mode), true);
    EXPECT_EQ(GetCtrlBit(ctrlBit), enabledValue);

    asc_set_saturation_flag(mode, false);
    EXPECT_EQ(asc_get_saturation_flag(mode), false);
    EXPECT_EQ(GetCtrlBit(ctrlBit), disabledValue);

    asc_set_saturation_flag(mode, true);
    EXPECT_EQ(asc_get_saturation_flag(mode), true);
    EXPECT_EQ(GetCtrlBit(ctrlBit), enabledValue);
}
} // namespace

TEST_F(TestSaturationFlagCAPI, FloatMode)
{
    // FLOAT: CTRL[48], 0=sat, 1=no sat
    CheckSaturationFlag(asc_saturation_mode::FLOAT, 48, 0, 1);
}

TEST_F(TestSaturationFlagCAPI, Float8Mode)
{
    // FLOAT8: CTRL[50], 0=sat, 1=no sat
    CheckSaturationFlag(asc_saturation_mode::FLOAT8, 50, 0, 1);
}

TEST_F(TestSaturationFlagCAPI, IntMode)
{
    // INT: CTRL[53], 1=sat, 0=truncation
    CheckSaturationFlag(asc_saturation_mode::INT, 53, 1, 0);
}

TEST_F(TestSaturationFlagCAPI, CastMode)
{
    // CAST: CTRL[59], 0=sat, 1=truncation
    CheckSaturationFlag(asc_saturation_mode::CAST, 59, 0, 1);
}
