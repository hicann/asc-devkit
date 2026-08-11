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
#include "include/c_api/asc_simd.h"

class TestClearNthbitCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
uint64_t sbitset0_stub(uint64_t bits, int64_t idx)
{
    EXPECT_EQ(bits, 0xFF);
    EXPECT_EQ(idx, 3);
    return 0xF7;
}
} // namespace

TEST_F(TestClearNthbitCAPI, c_api_clear_nthbit_succ)
{
    uint64_t bits = 0xFF;
    int64_t idx = 3;
    MOCKER_CPP(sbitset0, uint64_t(uint64_t, int64_t)).times(1).will(invoke(sbitset0_stub));
    uint64_t res = asc_clear_nthbit(bits, idx);
    EXPECT_EQ(res, 0xF7);
    GlobalMockObject::verify();
}
