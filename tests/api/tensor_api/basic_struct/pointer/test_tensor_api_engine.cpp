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

class tensor_api_engine : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_engine, test_view_engine_begin_and_offset)
{
    using namespace asc::te;

    __gm__ float data[4] = {1, 2, 3, 4};
    auto ptr = make_mem_ptr<location::gm>(data);

    view_engine<decltype(ptr)> engine(ptr);
    EXPECT_EQ(engine.begin(), ptr);
    EXPECT_EQ(engine.begin()[1], 2);
    EXPECT_EQ((engine + 2).begin(), ptr + 2);
}

TEST_F(tensor_api_engine, test_const_view_engine)
{
    using namespace asc::te;

    const_view_engine<decltype(make_mem_ptr<location::gm>((__gm__ float*)nullptr))> const_engine;
    EXPECT_EQ(const_engine.begin().get(), nullptr);
}
