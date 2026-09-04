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
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"

class TestSysVarGetSystemCycle : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
int64_t asc_get_system_cycle_Stub() { return 2; }
} // namespace

TEST_F(TestSysVarGetSystemCycle, get_system_cycle_Succ)
{
    MOCKER_CPP(get_sys_cnt, int64_t(void)).times(1).will(invoke(asc_get_system_cycle_Stub));

    int64_t val = asc_get_system_cycle();
    EXPECT_EQ(2, val);
    GlobalMockObject::verify();
}
