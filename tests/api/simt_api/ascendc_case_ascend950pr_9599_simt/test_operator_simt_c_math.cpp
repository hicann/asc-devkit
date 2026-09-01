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
#include <climits>
#include <type_traits>
#include "simt_api/asc_bf16.h"
using namespace std;
using namespace AscendC;
using namespace AscendC::Simt;

#define THREAD_DIM 128

struct FloatComputeParams1 {
    int32_t mode;
};

class FloatComputeTestsuite1 : public testing::Test, public testing::WithParamInterface<FloatComputeParams1> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_absfloat)
{
    float x = 123.0;
    float y = -123.0;
    EXPECT_EQ(static_cast<float>(123.0), fabsf(x));
    EXPECT_EQ(static_cast<float>(123.0), fabsf(y));
    EXPECT_EQ(static_cast<float>(123.0), abs(y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_nanf)
{
    EXPECT_TRUE(std::isnan(nanf("0x1")));
    EXPECT_TRUE(std::isnan(nanf("123")));
    EXPECT_TRUE(std::isnan(nanf("abc")));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_abshalf)
{
    half x = 123.0;
    half y = -123.0;
    EXPECT_EQ(static_cast<half>(123.0), __habs(x));
    EXPECT_EQ(static_cast<half>(123.0), __habs(y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_absbfloat16t)
{
    bfloat16_t x = 123.0;
    bfloat16_t y = -123.0;
    EXPECT_EQ(static_cast<bfloat16_t>(123.0), __habs(x));
    EXPECT_EQ(static_cast<bfloat16_t>(123.0), __habs(y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_fmahalf)
{
    half x = 1.0;
    half y = 2.0;
    half z = 3.0;
    EXPECT_EQ(static_cast<half>(5.0), __hfma(x, y, z));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_fmabfloat16t)
{
    bfloat16_t x = 1.0;
    bfloat16_t y = 2.0;
    bfloat16_t z = 3.0;
    EXPECT_EQ(static_cast<bfloat16_t>(5.0), __hfma(x, y, z));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_maxhalf)
{
    half x = 1.0;
    half y = 2.0;
    EXPECT_EQ(static_cast<half>(2.0), __hmax(x, y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_maxbfloat16t)
{
    bfloat16_t x = 1.0;
    bfloat16_t y = 2.0;
    EXPECT_EQ(static_cast<bfloat16_t>(2.0), __hmax(x, y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_minhalf)
{
    half x = 1.0;
    half y = 2.0;
    EXPECT_EQ(static_cast<half>(1.0), __hmin(x, y));
}

TEST_F(FloatComputeTestsuite1, FloatComputeTestCase_minbfloat16t)
{
    bfloat16_t x = 1.0;
    bfloat16_t y = 2.0;
    EXPECT_EQ(static_cast<bfloat16_t>(1.0), __hmin(x, y));
}

TEST_F(FloatComputeTestsuite1, MathIntegerTest)
{
    float src = 0.0f;
    src = 1.0f;

    long long int src_lln_x = -1;
    long long int src_lln_y = 1;
    long long int dst_lln = llmax(src_lln_x, src_lln_y);
    EXPECT_EQ(dst_lln, 1);

    long int x_ln = -1;
    long int dst_ln = labs(x_ln);
    EXPECT_EQ(dst_ln, 1);

    dst_lln = llabs(src_lln_x);
    EXPECT_EQ(dst_lln, 1);

    unsigned long long int src_ulln_x = 2;
    unsigned long long int src_ulln_y = 1;
    unsigned long long int dst_ulln = ullmax(src_ulln_x, src_ulln_y);
    EXPECT_EQ(dst_ulln, 2);

    unsigned int src_ui_x = 2;
    unsigned int src_ui_y = 1;
    unsigned int dst_ui = umax(src_ui_x, src_ui_y);
    EXPECT_EQ(dst_ui, 2);

    src_lln_x = -1;
    src_lln_y = 1;
    dst_lln = llmin(src_lln_x, src_lln_y);
    EXPECT_EQ(dst_lln, -1);

    src_ulln_x = 0;
    src_ulln_y = 1;
    dst_ulln = ullmin(src_ulln_x, src_ulln_y);
    EXPECT_EQ(dst_ulln, 0);

    src_ui_x = 0;
    src_ui_y = 1;
    dst_ui = umin(src_ui_x, src_ui_y);
    EXPECT_EQ(dst_ui, 0);

    EXPECT_EQ(max((unsigned int)2, -1), UINT32_MAX);
    EXPECT_EQ(max(-1, (unsigned int)2), UINT32_MAX);
    EXPECT_EQ(max((unsigned long)2, -1L), ULONG_MAX);
    EXPECT_EQ(max(-1L, (unsigned long int)2), ULONG_MAX);
    EXPECT_EQ(max((unsigned long long int)2, -1LL), ULLONG_MAX);
    EXPECT_EQ(max(-1LL, (unsigned long long int)2), ULLONG_MAX);
    EXPECT_EQ(min((unsigned int)2, -1), 2U);
    EXPECT_EQ(min(-1, (unsigned int)2), 2U);
    EXPECT_EQ(min((unsigned long int)2, -1L), 2UL);
    EXPECT_EQ(min(-1L, (unsigned long int)2), 2UL);
    EXPECT_EQ(min((unsigned long long int)2, -1LL), 2ULL);
    EXPECT_EQ(min(-1LL, (unsigned long long int)2), 2ULL);

    float x_f = 1.0f;
    float y_f = 2.0f;
    float res_div = fdividef(x_f, y_f);
    EXPECT_EQ(res_div, 0.5f);

    x_f = 0.0f;
    y_f = -1.0f;
    float res_after = nextafterf(x_f, y_f);
    EXPECT_EQ(res_after, -1.4013e-45f);

    x_f = 0.0f;
    y_f = 1.0f;
    res_after = nextafterf(x_f, y_f);
    EXPECT_EQ(res_after, 1.4013e-45f);

    x_f = NAN;
    y_f = 1.0f;
    res_after = nextafterf(x_f, y_f);
    EXPECT_TRUE(std::isnan(res_after));

    x_f = -1.0f;
    y_f = 0.0f;
    res_after = copysignf(x_f, y_f);
    EXPECT_EQ(res_after, 1.0f);
}

void VerifyFloatNumberMath(float x, float xExpected, float epsilon = 1e-5)
{
    if (std::isnan(xExpected)) {
        EXPECT_TRUE(std::isnan(x));
    } else if (std::isinf(xExpected)) {
        EXPECT_TRUE(std::isinf(x));
        if (xExpected > 0.0) {
            EXPECT_GT(x, 0.0);
        } else {
            EXPECT_LT(x, 0.0);
        }
    } else {
        EXPECT_NEAR(x, xExpected, epsilon);
    }
}
