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
#include <type_traits>
#include "tests/api/c_api/stub/cce_stub.h"
#include "c_api/reg_compute/compute/reg_mask.h"
#include "c_api/reg_compute/reg_copy.h"

class TestVectorComputeCopyMovvp : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void movvpu16_stub(vector_bool& dst, vector_uint16_t src, int16_t part) { EXPECT_EQ(part, static_cast<int16_t>(22)); }

void movvpu32_stub(vector_bool& dst, vector_uint32_t src, int16_t part) { EXPECT_EQ(part, static_cast<int16_t>(22)); }
} // namespace

TEST_F(TestVectorComputeCopyMovvp, Copyu16_Succ)
{
    vector_bool dst = asc_create_mask_b16(PAT_ALL);
    vector_uint16_t src;
    int16_t part = static_cast<int16_t>(22);

    MOCKER_CPP(movvp, void(vector_bool&, vector_uint16_t, int16_t)).times(1).will(invoke(movvpu16_stub));

    asc_copy(dst, src, part);
    GlobalMockObject::verify();
}

TEST_F(TestVectorComputeCopyMovvp, Copyu32_Succ)
{
    vector_bool dst = asc_create_mask_b16(PAT_ALL);
    vector_uint32_t src;
    int16_t part = static_cast<int16_t>(22);

    MOCKER_CPP(movvp, void(vector_bool&, vector_uint32_t, int16_t)).times(1).will(invoke(movvpu32_stub));

    asc_copy(dst, src, part);
    GlobalMockObject::verify();
}

// ==========asc_extract_mask return-value overloads(u16/u32)==========
TEST(TestVectorComputeExtractMaskReturn, VectorUint16)
{
    vector_uint16_t src;
    int16_t part = static_cast<int16_t>(22);

    static_assert(std::is_same_v<decltype(asc_extract_mask(src, part)), vector_bool>);
    MOCKER_CPP(movvp, void(vector_bool&, vector_uint16_t, int16_t)).times(1).will(invoke(movvpu16_stub));

    vector_bool dst = asc_extract_mask(src, part);
    (void)dst;
    GlobalMockObject::verify();
}

TEST(TestVectorComputeExtractMaskReturn, VectorUint32)
{
    vector_uint32_t src;
    int16_t part = static_cast<int16_t>(22);

    static_assert(std::is_same_v<decltype(asc_extract_mask(src, part)), vector_bool>);
    MOCKER_CPP(movvp, void(vector_bool&, vector_uint32_t, int16_t)).times(1).will(invoke(movvpu32_stub));

    vector_bool dst = asc_extract_mask(src, part);
    (void)dst;
    GlobalMockObject::verify();
}
