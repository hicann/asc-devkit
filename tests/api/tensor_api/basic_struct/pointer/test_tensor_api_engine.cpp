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

class Tensor_Api_Engine : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Engine, TestViewEngineBeginAndOffset)
{
    using namespace AscendC::Te;

    __gm__ float data[4] = {1, 2, 3, 4};
    auto ptr = MakeMemPtr<Location::GM>(data);

    ViewEngine<decltype(ptr)> engine(ptr);
    EXPECT_EQ(engine.Begin(), ptr);
    EXPECT_EQ(engine.Begin()[1], 2);
    EXPECT_EQ((engine + 2).Begin(), ptr + 2);
}

TEST_F(Tensor_Api_Engine, TestConstViewEngine)
{
    using namespace AscendC::Te;

    ConstViewEngine<decltype(MakeMemPtr<Location::GM>((__gm__ float*)nullptr))> constEngine;
    EXPECT_EQ(constEngine.Begin().Get(), nullptr);
}
