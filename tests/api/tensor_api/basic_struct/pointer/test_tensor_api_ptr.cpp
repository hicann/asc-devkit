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

class Tensor_Api_Pointer : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Pointer, TestMakeMemPtr)
{
    using namespace AscendC::Te;

    __gm__ float gmData[4] = {0};
    __ubuf__ float ubData[4] = {0};
    __cbuf__ float l1Data[4] = {0};
    __ca__ float l0aData[4] = {0};
    __cb__ float l0bData[4] = {0};
    __cc__ float l0cData[4] = {0};
    __biasbuf__ float biasData[4] = {0};
    __fbuf__ float fixbufData[4] = {0};

    auto gmPtr = MakeMemPtr<Location::GM>(gmData);
    auto ubPtr = MakeMemPtr<Location::UB>(ubData);
    auto l1Ptr = MakeMemPtr<Location::L1>(l1Data);
    auto l0aPtr = MakeMemPtr<Location::L0A>(l0aData);
    auto l0bPtr = MakeMemPtr<Location::L0B>(l0bData);
    auto l0cPtr = MakeMemPtr<Location::L0C>(l0cData);
    auto biasPtr = MakeMemPtr<Location::BIAS>(biasData);
    auto fixbufPtr = MakeMemPtr<Location::FIXBUF>(fixbufData);

    EXPECT_EQ(gmPtr.Get(), gmData);
    EXPECT_EQ(ubPtr.Get(), ubData);
    EXPECT_EQ(l1Ptr.Get(), l1Data);
    EXPECT_EQ(l0aPtr.Get(), l0aData);
    EXPECT_EQ(l0bPtr.Get(), l0bData);
    EXPECT_EQ(l0cPtr.Get(), l0cData);
    EXPECT_EQ(biasPtr.Get(), biasData);
    EXPECT_EQ(fixbufPtr.Get(), fixbufData);
}

TEST_F(Tensor_Api_Pointer, TestPointerAdaptor)
{
    using namespace AscendC::Te;

    __gm__ float data[4] = {1, 2, 3, 4};
    auto ptr = MakeMemPtr<Location::GM>(data);

    EXPECT_EQ(*ptr, 1);
    EXPECT_EQ(ptr[2], 3);
    EXPECT_EQ((ptr + 3).Get(), data + 3);

    *ptr = 10;
    (ptr + 1)[1] = 20;

    EXPECT_EQ(data[0], 10);
    EXPECT_EQ(data[2], 20);
}

TEST_F(Tensor_Api_Pointer, TestPointerOperators)
{
    using namespace AscendC::Te;

    __gm__ float data[4] = {0};
    auto ptr = MakeMemPtr<Location::GM>(data);
    auto next = ptr + 1;
    auto far = ptr + 3;

    EXPECT_TRUE(ptr == ptr);
    EXPECT_TRUE(ptr != next);
    EXPECT_TRUE(ptr < next);
    EXPECT_TRUE(next > ptr);
    EXPECT_TRUE(ptr <= next);
    EXPECT_TRUE(far >= next);
}
