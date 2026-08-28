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

class TestVectorComputeSetVectorMask : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_vector_mask_Stub(uint64_t mask1, uint64_t mask0)
{
    EXPECT_EQ(mask1, 0x123456789abcdef0ULL);
    EXPECT_EQ(mask0, 0x0fedcba987654321ULL);
}
} // namespace

TEST_F(TestVectorComputeSetVectorMask, c_api_set_vector_mask_aiv_succ)
{
    MOCKER_CPP(set_vector_mask, void(uint64_t, uint64_t)).times(1).will(invoke(set_vector_mask_Stub));

    asc_set_vector_mask(0x123456789abcdef0ULL, 0x0fedcba987654321ULL);
    GlobalMockObject::verify();
}

TEST_F(TestVectorComputeSetVectorMask, c_api_set_vector_mask_aic_succ)
{
    g_coreType = C_API_AIC_TYPE;
    MOCKER_CPP(set_vector_mask, void(uint64_t, uint64_t)).times(0).will(invoke(set_vector_mask_Stub));

    asc_set_vector_mask(0x123456789abcdef0ULL, 0x0fedcba987654321ULL);
    GlobalMockObject::verify();
}
