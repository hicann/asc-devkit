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

class TestVectorComputeClearNthbit : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {

uint64_t clear_nthbit_Stub(uint64_t bits, int64_t idx)
{
    if (idx >= 0 && idx < 64) {
        bits &= ~(1ULL << idx);
    }
    return bits;
}

} // namespace

TEST_F(TestVectorComputeClearNthbit, clear_nthbit_Succ)
{
    uint64_t bits = 0xFFFFFFFFFFFFFFFFULL;
    int64_t idx = 3;
    uint64_t expected = 0xFFFFFFFFFFFFFFF7ULL;

    MOCKER_CPP(sbitset0, uint64_t(uint64_t, int64_t)).times(1).will(invoke(clear_nthbit_Stub));

    uint64_t result = asc_clear_nthbit(bits, idx);

    EXPECT_EQ(expected, result);
    GlobalMockObject::verify();
}
