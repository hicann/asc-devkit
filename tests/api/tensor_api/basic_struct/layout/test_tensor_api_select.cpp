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
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class tensor_api_layout_select : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_layout_select, test_select_dimensions)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(7, 8, 9), make_stride(72, 9, 1));
    auto selected = select<0, 2>(layout);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(selected)), 7);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(selected)), 9);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(selected)), 72);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(selected)), 1);
}

TEST_F(tensor_api_layout_select, test_select_nested_layouts)
{
    using namespace asc::te;

    auto layout =
        make_layout(make_shape(make_shape(2, 3), make_shape(4, 5)), make_stride(make_stride(1, 8), make_stride(2, 24)));
    auto selected = select<1>(layout);

    auto shape = get_shape(selected);
    auto stride = get_stride(selected);

    EXPECT_EQ(AscendC::Std::get<0>(shape), 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape), 5);
    EXPECT_EQ(AscendC::Std::get<0>(stride), 2);
    EXPECT_EQ(AscendC::Std::get<1>(stride), 24);
}
