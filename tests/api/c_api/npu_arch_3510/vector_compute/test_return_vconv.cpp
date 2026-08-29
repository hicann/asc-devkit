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
using VcvtFloatToE5m2 = void (*)(vector_fp8_e5m2_t&, vector_float, vector_bool, int, int, int, int);

void VcvtHalfToFloatStub(vector_float&, vector_half, vector_bool, Literal, Literal) {}
void VcvtFloatToE5m2Stub(vector_fp8_e5m2_t&, vector_float, vector_bool, int, int, int, int) {}
void VexpdifStub(vector_float&, vector_half, vector_half, vector_bool, int) {}
void VexpdifFloatStub(vector_float&, vector_float, vector_float, vector_bool, int) {}
} // namespace

TEST(ReturnValueVconv, HalfToFloatUsesPositionAwareInstruction)
{
    vector_half src;
    vector_bool mask;
    MOCKER_CPP(vcvt, void(vector_float&, vector_half, vector_bool, Literal, Literal))
        .times(1)
        .will(invoke(VcvtHalfToFloatStub));

    auto result = asc_half2float(src, mask, ASC_POSITION_EVEN);
    static_assert(std::is_same<decltype(result), vector_float>::value);
    GlobalMockObject::verify();
}

TEST(ReturnValueVconv, FloatToE5m2KeepsQuarterSelection)
{
    vector_float src;
    vector_bool mask;
    MOCKER_CPP((static_cast<VcvtFloatToE5m2>(&vcvt)), VcvtFloatToE5m2).times(1).will(invoke(VcvtFloatToE5m2Stub));

    auto result = asc_float2e5m2_rn(src, mask, ASC_DISPERSE_FIRST_QUARTER);
    static_assert(std::is_same<decltype(result), vector_fp8_e5m2_t>::value);
    GlobalMockObject::verify();
}

TEST(ReturnValueVconv, ExpSubHalfToFloatUsesVexpdif)
{
    vector_half src0;
    vector_half src1;
    vector_bool mask;
    MOCKER_CPP(vexpdif, void(vector_float&, vector_half, vector_half, vector_bool, int))
        .times(1)
        .will(invoke(VexpdifStub));

    auto result = asc_exp_sub_half2float(src0, src1, mask, ASC_POSITION_ODD);
    static_assert(std::is_same<decltype(result), vector_float>::value);
    GlobalMockObject::verify();
}

TEST(ReturnValueVconv, ExpSubUsesVexpdif)
{
    vector_float src0;
    vector_float src1;
    vector_bool mask;
    MOCKER_CPP(vexpdif, void(vector_float&, vector_float, vector_float, vector_bool, int))
        .times(1)
        .will(invoke(VexpdifFloatStub));

    auto result = asc_exp_sub(src0, src1, mask);
    static_assert(std::is_same<decltype(result), vector_float>::value);
    GlobalMockObject::verify();
}
