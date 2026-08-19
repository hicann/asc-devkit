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


class tensor_api_layout_get : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_layout_get, test_get_layout_slices)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(10, 20, 30), make_stride(600, 30, 1));

    auto dim1 = get<1>(layout);
    auto dim2 = get<2>(make_shape(10, 20, 30));

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(dim1)), 20);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(dim1)), 30);
    EXPECT_EQ(dim2, 30);
}

TEST_F(tensor_api_layout_get, test_get_static_layouts)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(make_shape(_2{}, _3{}), make_shape(_4{}, _5{})),
                             make_stride(make_stride(_1{}, _8{}), make_stride(_2{}, _24{})));

    auto inner = get<1, 0>(layout);

    EXPECT_EQ(AscendC::Std::get<0>(get_shape(inner)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(inner)), 2);
}
