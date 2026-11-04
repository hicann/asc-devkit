/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <climits>
#include <type_traits>
#include "simt_api/asc_fp16.h"

using namespace std;
using namespace AscendC;
using namespace AscendC::Simt;

struct FP16MathParams {
    int32_t mode;
};

class FP16MathTestsuite : public testing::Test, public testing::WithParamInterface<FP16MathParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(FP16MathTestsuite, ExtendedMathApiTest)
{
#ifdef __BISHENG_DISABLE_SOFT_IMPL
    half x = 1.0;
    half y = 2.0;
    half nanValue = __ushort_as_half(0x7e00U);
    half2 x2 = {x, y};
    half2 y2 = {y, x};
    half2 z2 = {3.0, 4.0};

    EXPECT_TRUE(__heq(x, x));
    EXPECT_TRUE(__hne(x, y));
    EXPECT_TRUE(__hle(x, y));
    EXPECT_TRUE(__hge(y, x));
    EXPECT_TRUE(__hlt(x, y));
    EXPECT_TRUE(__hgt(y, x));
    EXPECT_TRUE(__hequ(x, x));
    EXPECT_TRUE(__hneu(x, y));
    EXPECT_TRUE(__hleu(x, y));
    EXPECT_TRUE(__hgeu(y, x));
    EXPECT_TRUE(__hltu(x, y));
    EXPECT_TRUE(__hgtu(y, x));
    EXPECT_TRUE(__hbeqx2(x2, x2));
    EXPECT_TRUE(__hbnex2(x2, y2));
    EXPECT_FALSE(__hblex2(x2, y2));
    EXPECT_FALSE(__hbgex2(y2, x2));
    EXPECT_FALSE(__hbltx2(x2, y2));
    EXPECT_FALSE(__hbgtx2(y2, x2));
    EXPECT_TRUE(__hbequx2(x2, x2));
    EXPECT_TRUE(__hbneux2(x2, y2));
    EXPECT_FALSE(__hbleux2(x2, y2));
    EXPECT_FALSE(__hbgeux2(y2, x2));
    EXPECT_FALSE(__hbltux2(x2, y2));
    EXPECT_FALSE(__hbgtux2(y2, x2));

    EXPECT_EQ(static_cast<half>(__heqx2(x2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hnex2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hlex2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hgex2(y2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hltx2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hgtx2(y2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hequx2(x2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hneux2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hleux2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hgeux2(y2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hltux2(x2, y2).x), static_cast<half>(1.0));
    EXPECT_EQ(static_cast<half>(__hgtux2(y2, x2).x), static_cast<half>(1.0));
    EXPECT_EQ(0xFFFFFFFFU, __heqx2_mask(x2, x2));
    EXPECT_EQ(0xFFFFFFFFU, __hnex2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hlex2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hgex2_mask(y2, x2));
    EXPECT_EQ(0x0000FFFFU, __hltx2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hgtx2_mask(y2, x2));
    EXPECT_EQ(0xFFFFFFFFU, __hequx2_mask(x2, x2));
    EXPECT_EQ(0xFFFFFFFFU, __hneux2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hleux2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hgeux2_mask(y2, x2));
    EXPECT_EQ(0x0000FFFFU, __hltux2_mask(x2, y2));
    EXPECT_EQ(0x0000FFFFU, __hgtux2_mask(y2, x2));
    EXPECT_EQ((__isnanx2(half2{nanValue, x}).x), static_cast<half>(1.0));
    EXPECT_EQ((__isnanx2(half2{nanValue, x}).y), static_cast<half>(0.0));

    half2 add = __haddx2(x2, y2);
    half2 sub = __hsubx2(y2, x2);
    half2 mul = __hmulx2(x2, y2);
    half2 div = __hdivx2(y2, x2);
    half2 abs = __habsx2(half2{-x, -y});
    half2 neg = __hnegx2(x2);
    EXPECT_EQ(static_cast<half>(3.0), add.x);
    EXPECT_EQ(x, sub.x);
    EXPECT_EQ(y, mul.x);
    EXPECT_FLOAT_EQ(2.0f, static_cast<float>(div.x));
    EXPECT_EQ(x, abs.x);
    EXPECT_EQ(-x, neg.x);
    EXPECT_TRUE(__hisnan(__hmax_nan(nanValue, x)));
    EXPECT_TRUE(__hisnan(__hmin_nan(nanValue, x)));
    EXPECT_EQ(__hmaxx2_nan(half2{nanValue, y}, x2).y, y);
    EXPECT_EQ(__hminx2_nan(half2{nanValue, y}, x2).y, y);
    EXPECT_EQ(static_cast<half>(3.0), __hfma_relu(x, y, x));
    EXPECT_EQ(static_cast<half>(0.0), __hfma_relu(-x, y, x));
    EXPECT_EQ(__hmaxx2(x2, y2).x, y);
    EXPECT_EQ(__hminx2(x2, y2).x, x);
    EXPECT_EQ(static_cast<half>(5.0), __hfmax2(x2, y2, z2).x);
    EXPECT_EQ(static_cast<half>(1.0), __hfmax2_relu(-x2, y2, z2).x);
    EXPECT_EQ(static_cast<half>(3.0), __hcmadd(x2, y2, z2).x);
    EXPECT_EQ(static_cast<half>(3.0), __hadd(x, y));
    EXPECT_EQ(static_cast<half>(-1.0), __hsub(x, y));
    EXPECT_EQ(y, __hmul(x, y));
    EXPECT_FLOAT_EQ(static_cast<half>(2.0), __hdiv(y, x));
    EXPECT_EQ(static_cast<half>(-1.0), __hneg(x));
#endif
}
