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
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class tensor_api_int_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

TEST_F(tensor_api_int_3510, int_max_min_return_static_int)
{
    using namespace AscendC::Std;

    auto max_result = max(Int<16>{}, Int<32>{});
    auto min_result = min(Int<16>{}, Int<32>{});
    auto same_max_result = max(Int<64>{}, Int<64>{});
    auto same_min_result = min(Int<64>{}, Int<64>{});

    static_assert(is_same_v<decltype(max_result), Int<32>>);
    static_assert(is_same_v<decltype(min_result), Int<16>>);
    static_assert(is_same_v<decltype(same_max_result), Int<64>>);
    static_assert(is_same_v<decltype(same_min_result), Int<64>>);
    EXPECT_EQ(max_result(), 32);
    EXPECT_EQ(min_result(), 16);
    EXPECT_EQ(same_max_result(), 64);
    EXPECT_EQ(same_min_result(), 64);
}

TEST_F(tensor_api_int_3510, int_mixed_integral_max)
{
    using namespace AscendC::Std;

    uint32_t larger_u32 = 48;
    int32_t smaller_i32 = 12;
    uint16_t smaller_u16 = 8;
    size_t larger_size = 96;

    EXPECT_EQ(max(Int<32>{}, larger_u32), 48);
    EXPECT_EQ(max(Int<32>{}, smaller_i32), 32);
    EXPECT_EQ(max(smaller_u16, Int<32>{}), 32);
    EXPECT_EQ(max(larger_size, Int<32>{}), 96);
}

TEST_F(tensor_api_int_3510, int_mixed_integral_min)
{
    using namespace AscendC::Std;

    uint32_t larger_u32 = 48;
    int32_t smaller_i32 = 12;
    uint16_t smaller_u16 = 8;
    size_t larger_size = 96;

    EXPECT_EQ(min(Int<32>{}, larger_u32), 32);
    EXPECT_EQ(min(Int<32>{}, smaller_i32), 12);
    EXPECT_EQ(min(smaller_u16, Int<32>{}), 8);
    EXPECT_EQ(min(larger_size, Int<32>{}), 32);
}
