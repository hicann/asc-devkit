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
#include "kernel_operator.h"

using AscendC::Std::ceil_div;

static_assert(ceil_div(8192U, 2048U) == 4U);
static_assert(ceil_div(8193U, 2048U) == 5U);
static_assert(ceil_div(0U, 2048U) == 0U);
static_assert(ceil_div(8192, 0) == 0);

class TEST_CEIL_DIV : public testing::Test {};

TEST_F(TEST_CEIL_DIV, UINT32_CASE)
{
    EXPECT_EQ(ceil_div(8192U, 2048U), 4U);
    EXPECT_EQ(ceil_div(8193U, 2048U), 5U);
    EXPECT_EQ(ceil_div(0U, 2048U), 0U);
}

TEST_F(TEST_CEIL_DIV, SIGNED_CASE)
{
    EXPECT_EQ(ceil_div(33, 16), 3);
    EXPECT_EQ(ceil_div(32, 16), 2);
    EXPECT_EQ(ceil_div(1, 0), 0);
}
