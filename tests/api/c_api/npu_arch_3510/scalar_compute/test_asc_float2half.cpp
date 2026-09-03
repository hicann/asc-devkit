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
#include <type_traits>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

namespace {
constexpr float INPUT_VALUE = 1.25f;

half conv_f322f16o_stub(float value)
{
    EXPECT_FLOAT_EQ(value, INPUT_VALUE);
    return half(2.5f);
}
} // namespace

TEST(AscFloat2Half, Ro)
{
    static_assert(std::is_same_v<decltype(asc_float2half_ro(INPUT_VALUE)), half>);
    MOCKER_CPP(conv_f322f16o, half(float)).times(1).will(invoke(conv_f322f16o_stub));

    const half result = asc_float2half_ro(INPUT_VALUE);

    EXPECT_EQ(static_cast<float>(result), 2.5f);
    GlobalMockObject::verify();
}
