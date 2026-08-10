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

class tensor_api_layout_metrics : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_layout_metrics, test_layout_metrics)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(3, 4), make_stride(7, 1));

    EXPECT_EQ(rank(layout), 2);
    EXPECT_EQ(size(layout), 12);
    EXPECT_EQ(capacity(layout), 21);
    EXPECT_EQ(coshape(layout), 18);
    EXPECT_EQ(cosize(layout), 18);
}

TEST_F(tensor_api_layout_metrics, test_crd_to_idx)
{
    using namespace asc::te;

    auto shape = make_shape(make_shape(2, 3), make_shape(4, 5));
    auto stride = make_stride(make_stride(1, 8), make_stride(2, 24));
    auto layout = make_layout(shape, stride);
    auto coord = make_coord(make_coord(1, 2), make_coord(3, 4));

    EXPECT_EQ(crd2idx(coord, shape, stride), layout(coord));
    EXPECT_EQ(layout(coord), 119);
}
