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

class Tensor_Api_Layout_Select : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Layout_Select, TestSelectDimensions)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(7, 8, 9), MakeStride(72, 9, 1));
    auto selected = Select<0, 2>(layout);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(selected)), 7);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(selected)), 9);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(selected)), 72);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(selected)), 1);
}

TEST_F(Tensor_Api_Layout_Select, TestSelectNestedLayouts)
{
    using namespace AscendC::Te;

    auto layout =
        MakeLayout(MakeShape(MakeShape(2, 3), MakeShape(4, 5)), MakeStride(MakeStride(1, 8), MakeStride(2, 24)));
    auto selected = Select<1>(layout);

    auto shape = GetShape(selected);
    auto stride = GetStride(selected);

    EXPECT_EQ(AscendC::Std::get<0>(shape), 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape), 5);
    EXPECT_EQ(AscendC::Std::get<0>(stride), 2);
    EXPECT_EQ(AscendC::Std::get<1>(stride), 24);
}
