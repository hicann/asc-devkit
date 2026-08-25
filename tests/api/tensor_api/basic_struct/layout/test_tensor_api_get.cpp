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

class Tensor_Api_Layout_Get : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Layout_Get, TestGetLayoutSlices)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(10, 20, 30), MakeStride(600, 30, 1));

    auto all = Get(layout);
    auto dim1 = Get<1>(layout);
    auto dim2 = Get<2>(MakeShape(10, 20, 30));

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(all)), 10);
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(all)), 1);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dim1)), 20);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dim1)), 30);
    EXPECT_EQ(dim2, 30);
}

TEST_F(Tensor_Api_Layout_Get, TestGetStaticLayouts)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(
        MakeShape(
            MakeShape(AscendC::Std::Int<2>{}, AscendC::Std::Int<3>{}),
            MakeShape(AscendC::Std::Int<4>{}, AscendC::Std::Int<5>{})),
        MakeStride(
            MakeStride(AscendC::Std::Int<1>{}, AscendC::Std::Int<8>{}),
            MakeStride(AscendC::Std::Int<2>{}, AscendC::Std::Int<24>{})));

    auto inner = Get<1, 0>(layout);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(inner)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(inner)), 2);
}
