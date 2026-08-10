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

class tensor_api_pointer_offset : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};


TEST_F(tensor_api_pointer_offset, test_make_mem_ptr_byte_offset)
{
    using namespace asc::te;

    constexpr uint64_t byte_offset = 128;

    auto ub_ptr = make_mem_ptr<location::ub, float>(byte_offset);
    auto l1_ptr = make_mem_ptr<location::l1, float>(byte_offset);
    auto l0c_ptr = make_mem_ptr<location::l0c, float>(byte_offset);

    EXPECT_EQ(ub_ptr.get(), reinterpret_cast<__ubuf__ float*>(0 + byte_offset));
    EXPECT_EQ(l1_ptr.get(), reinterpret_cast<__cbuf__ float*>(0 + byte_offset));
    EXPECT_EQ(l0c_ptr.get(), reinterpret_cast<__cc__ float*>(0 + byte_offset));
}
