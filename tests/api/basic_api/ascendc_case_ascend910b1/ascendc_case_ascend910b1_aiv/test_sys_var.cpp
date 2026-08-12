/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include "kernel_operator.h"
#include "mockcpp/mockcpp.hpp"
using namespace AscendC;

class TEST_SYS_VAR : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

TEST_F(TEST_SYS_VAR, GetSystemVariable)
{
    const int64_t virtualAddress = 0;
    const int64_t subBlockNum = 2;

    uint32_t coreVersion = 0;
    GetArchVersion(coreVersion);
    EXPECT_EQ(coreVersion, 2201U);

    int64_t actualSubBlockNum = GetSubBlockNum();
    EXPECT_EQ(actualSubBlockNum, subBlockNum);
    int64_t pc = GetProgramCounter();
    int64_t systemCycle = GetSystemCycle();
}

TEST_F(TEST_SYS_VAR, TestL2Cache)
{
    GlobalTensor<uint8_t> gmTensor;
    gmTensor.SetGlobalBuffer((__gm__ uint8_t*)(0x70000000));
    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);
    EXPECT_EQ(reinterpret_cast<uint64_t>(gmTensor.GetPhyAddr()), 0x70000000);
}

int32_t RaiseStubForTrap(int32_t input) { return 0; }

TEST_F(TEST_SYS_VAR, Trap)
{
    MOCKER(raise, int32_t(*)(int32_t)).times(1).will(invoke(RaiseStubForTrap));
    Trap();
}
