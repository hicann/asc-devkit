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

class TestHalf2Float : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void vcvt_Stub(vector_float& dst, vector_half src, vector_bool mask, Literal part, Literal mode) {}
void vcvt_Return_Stub(vector_float& dst, vector_half src, vector_bool mask, Literal part, Literal mode) {}
} // namespace

TEST_F(TestHalf2Float, half_to_float_Succ)
{
    vector_float dst;
    vector_half src;
    vector_bool mask;
    MOCKER(vcvt, void(vector_float&, vector_half, vector_bool, Literal, Literal)).times(1).will(invoke(vcvt_Stub));

    asc_half2float(dst, src, mask);
    GlobalMockObject::verify();
}

TEST_F(TestHalf2Float, half_to_float_return_Succ)
{
    vector_half src;
    vector_bool mask;
    MOCKER(vcvt, void(vector_float&, vector_half, vector_bool, Literal, Literal))
        .times(1)
        .will(invoke(vcvt_Return_Stub));

    vector_float dst = asc_half2float(src, mask, ASC_POSITION_EVEN);
    (void)dst;
    GlobalMockObject::verify();
}
