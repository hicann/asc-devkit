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

class Tensor_Api_Pointer_Offset : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Pointer_Offset, TestMakeMemPtrByteOffset)
{
    using namespace AscendC::Te;

    constexpr uint64_t byteOffset = 128;

    auto ubPtr = MakeMemPtr<Location::UB, float>(byteOffset);
    auto l1Ptr = MakeMemPtr<Location::L1, float>(byteOffset);
    auto l0cPtr = MakeMemPtr<Location::L0C, float>(byteOffset);

    EXPECT_EQ(ubPtr.Get(), reinterpret_cast<__ubuf__ float*>(0 + byteOffset));
    EXPECT_EQ(l1Ptr.Get(), reinterpret_cast<__cbuf__ float*>(0 + byteOffset));
    EXPECT_EQ(l0cPtr.Get(), reinterpret_cast<__cc__ float*>(0 + byteOffset));
}
