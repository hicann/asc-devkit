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

class Tensor_Api_Layout_Metrics : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Layout_Metrics, TetsLayoutMetrics)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(3, 4), MakeStride(7, 1));

    EXPECT_EQ(Rank(layout), 2);
    EXPECT_EQ(Size(layout), 12);
    EXPECT_EQ(Capacity(layout), 21);
    EXPECT_EQ(Coshape(layout), 18);
    EXPECT_EQ(Cosize(layout), 18);
}

TEST_F(Tensor_Api_Layout_Metrics, TestCrd2Idx)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(MakeShape(2, 3), MakeShape(4, 5));
    auto stride = MakeStride(MakeStride(1, 8), MakeStride(2, 24));
    auto layout = MakeLayout(shape, stride);
    auto coord = MakeCoord(MakeCoord(1, 2), MakeCoord(3, 4));

    EXPECT_EQ(Crd2Idx(coord, shape, stride), layout(coord));
    EXPECT_EQ(layout(coord), 119);
}
