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

class TestSetNthbitCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
uint64_t sbitset1_stub(uint64_t bits, int64_t idx)
{
    EXPECT_EQ(0x0, bits);
    EXPECT_EQ(0x2, idx);
    return 0x4;
}
} // namespace

TEST_F(TestSetNthbitCAPI, c_api_set_nthbit_succ)
{
    uint64_t bits = 0x0;
    int64_t idx = 0x2;
    MOCKER_CPP(sbitset1, uint64_t(uint64_t, int64_t)).times(1).will(invoke(sbitset1_stub));
    uint64_t res = asc_set_nthbit(bits, idx);
    EXPECT_EQ(res, 0x4);
    GlobalMockObject::verify();
}
