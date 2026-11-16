/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

namespace {
void E8m0ToBf16EvenStub(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal part)
{
    EXPECT_EQ(part, PART_EVEN);
}

void E8m0ToBf16OddStub(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal part) { EXPECT_EQ(part, PART_ODD); }
} // namespace

TEST(E8m0ToBf16, PositionEven)
{
    vector_bfloat16_t dst;
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16EvenStub));

    asc_e8m02bfloat16(dst, src, mask, ASC_POSITION_EVEN);
    GlobalMockObject::verify();
}

TEST(E8m0ToBf16, PositionOdd)
{
    vector_bfloat16_t dst;
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16OddStub));

    asc_e8m02bfloat16(dst, src, mask, ASC_POSITION_ODD);
    GlobalMockObject::verify();
}

TEST(E8m0ToBf16, ReturnEven)
{
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16EvenStub));

    auto dst = asc_e8m02bfloat16(src, mask, ASC_POSITION_EVEN);
    static_assert(std::is_same<decltype(dst), vector_bfloat16_t>::value);
    (void)dst;
    GlobalMockObject::verify();
}

TEST(E8m0ToBf16, ReturnOdd)
{
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16OddStub));

    auto dst = asc_e8m02bfloat16(src, mask, ASC_POSITION_ODD);
    static_assert(std::is_same<decltype(dst), vector_bfloat16_t>::value);
    (void)dst;
    GlobalMockObject::verify();
}

TEST(AscCastE8m0ToBf16, Even)
{
    vector_bfloat16_t dst;
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16EvenStub));

    asc_cast(dst, src, mask, ASC_POSITION_EVEN);
    GlobalMockObject::verify();
}

TEST(AscCastE8m0ToBf16, Odd)
{
    vector_bfloat16_t dst;
    vector_fp8_e8m0_t src;
    vector_bool mask;
    MOCKER_CPP(vcvt_e8m02bf16, void(vector_bfloat16_t&, vector_fp8_e8m0_t, vector_bool, Literal))
        .times(1)
        .will(invoke(E8m0ToBf16OddStub));

    asc_cast(dst, src, mask, ASC_POSITION_ODD);
    GlobalMockObject::verify();
}
