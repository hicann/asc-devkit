/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software: you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License).
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

class TestAscExpPrecision9201CApi : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestAscExpPrecision9201CApi, normal_dst_Succ)
{
    vector_half dst;
    vector_half src;
    vector_bool mask;

    MOCKER_CPP(vexp, void(vector_f16&, vector_f16, vector_bool, Literal)).times(1);

    asc_exp(dst, src, mask, ASC_EXP_PRECISION_NORMAL);
    GlobalMockObject::verify();
}

TEST_F(TestAscExpPrecision9201CApi, normal_return_Succ)
{
    vector_half src;
    vector_bool mask;

    MOCKER_CPP(vexp, void(vector_f16&, vector_f16, vector_bool, Literal)).times(1);

    vector_half dst = asc_exp(src, mask, ASC_EXP_PRECISION_NORMAL);
    (void)dst;
    GlobalMockObject::verify();
}

TEST_F(TestAscExpPrecision9201CApi, high_dst_Succ)
{
    vector_half dst;
    vector_half src;
    vector_bool mask;

    MOCKER_CPP(pset_b16, vector_bool(Literal)).times(1).will(returnValue(static_cast<vector_bool>(0xFFFF)));
    MOCKER_CPP(vcvt, void(vector_f32&, vector_f16, vector_bool, Literal, Literal)).times(2);
    MOCKER_CPP(vexp, void(vector_f32&, vector_f32, vector_bool, Literal)).times(2);
    MOCKER_CPP(vcvt, void(vector_f16&, vector_f32, vector_bool, Literal, Literal, Literal, Literal)).times(2);
    MOCKER_CPP(vdup, void(vector_f16&, half, vector_bool, Literal)).times(1);
    MOCKER_CPP(vsel, void(vector_f16&, vector_f16, vector_f16, vector_bool)).times(1);

    asc_exp(dst, src, mask, ASC_EXP_PRECISION_HIGH);
    GlobalMockObject::verify();
}

TEST_F(TestAscExpPrecision9201CApi, high_return_Succ)
{
    vector_half src;
    vector_bool mask;

    MOCKER_CPP(pset_b16, vector_bool(Literal)).times(1).will(returnValue(static_cast<vector_bool>(0xFFFF)));
    MOCKER_CPP(vcvt, void(vector_f32&, vector_f16, vector_bool, Literal, Literal)).times(2);
    MOCKER_CPP(vexp, void(vector_f32&, vector_f32, vector_bool, Literal)).times(2);
    MOCKER_CPP(vcvt, void(vector_f16&, vector_f32, vector_bool, Literal, Literal, Literal, Literal)).times(2);
    MOCKER_CPP(vdup, void(vector_f16&, half, vector_bool, Literal)).times(1);
    MOCKER_CPP(vsel, void(vector_f16&, vector_f16, vector_f16, vector_bool)).times(1);

    vector_half dst = asc_exp(src, mask, ASC_EXP_PRECISION_HIGH);
    (void)dst;
    GlobalMockObject::verify();
}
