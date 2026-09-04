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

class TestSysVarSetCtrl : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void asc_set_ctrl_stub(uint64_t config) { EXPECT_EQ(33U, config); }
} // namespace

TEST_F(TestSysVarSetCtrl, set_ctrl_Succ)
{
    uint64_t config = 33;
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(asc_set_ctrl_stub));

    asc_set_ctrl(config);
    GlobalMockObject::verify();
}
