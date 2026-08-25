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
#include "c_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class Tensor_Api_Int_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

TEST_F(Tensor_Api_Int_3510, IntMaxMinReturnStaticInt)
{
    using namespace AscendC::Std;

    auto maxResult = max(Int<16>{}, Int<32>{});
    auto minResult = min(Int<16>{}, Int<32>{});
    auto sameMaxResult = max(Int<64>{}, Int<64>{});
    auto sameMinResult = min(Int<64>{}, Int<64>{});

    static_assert(is_same_v<decltype(maxResult), Int<32>>);
    static_assert(is_same_v<decltype(minResult), Int<16>>);
    static_assert(is_same_v<decltype(sameMaxResult), Int<64>>);
    static_assert(is_same_v<decltype(sameMinResult), Int<64>>);
    EXPECT_EQ(maxResult(), 32);
    EXPECT_EQ(minResult(), 16);
    EXPECT_EQ(sameMaxResult(), 64);
    EXPECT_EQ(sameMinResult(), 64);
}

TEST_F(Tensor_Api_Int_3510, IntMixedIntegralMax)
{
    using namespace AscendC::Std;

    uint32_t largerU32 = 48;
    int32_t smallerI32 = 12;
    uint16_t smallerU16 = 8;
    size_t largerSize = 96;

    EXPECT_EQ(max(Int<32>{}, largerU32), 48);
    EXPECT_EQ(max(Int<32>{}, smallerI32), 32);
    EXPECT_EQ(max(smallerU16, Int<32>{}), 32);
    EXPECT_EQ(max(largerSize, Int<32>{}), 96);
}

TEST_F(Tensor_Api_Int_3510, IntMixedIntegralMin)
{
    using namespace AscendC::Std;

    uint32_t largerU32 = 48;
    int32_t smallerI32 = 12;
    uint16_t smallerU16 = 8;
    size_t largerSize = 96;

    EXPECT_EQ(min(Int<32>{}, largerU32), 32);
    EXPECT_EQ(min(Int<32>{}, smallerI32), 12);
    EXPECT_EQ(min(smallerU16, Int<32>{}), 8);
    EXPECT_EQ(min(largerSize, Int<32>{}), 32);
}
