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

class tensor_api_pointer : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_pointer, test_make_mem_ptr)
{
    using namespace asc::te;

    __gm__ float gm_data[4] = {0};
    __ubuf__ float ub_data[4] = {0};
    __cbuf__ float l1_data[4] = {0};
    __ca__ float l0a_data[4] = {0};
    __cb__ float l0b_data[4] = {0};
    __cc__ float l0c_data[4] = {0};
    __biasbuf__ float bias_data[4] = {0};
    __fbuf__ float fixbuf_data[4] = {0};

    auto gm_ptr = make_mem_ptr<location::gm>(gm_data);
    auto ub_ptr = make_mem_ptr<location::ub>(ub_data);
    auto l1_ptr = make_mem_ptr<location::l1>(l1_data);
    auto l0a_ptr = make_mem_ptr<location::l0a>(l0a_data);
    auto l0b_ptr = make_mem_ptr<location::l0b>(l0b_data);
    auto l0c_ptr = make_mem_ptr<location::l0c>(l0c_data);
    auto bias_ptr = make_mem_ptr<location::bias>(bias_data);
    auto fixbuf_ptr = make_mem_ptr<location::fixbuf>(fixbuf_data);

    EXPECT_EQ(gm_ptr.get(), gm_data);
    EXPECT_EQ(ub_ptr.get(), ub_data);
    EXPECT_EQ(l1_ptr.get(), l1_data);
    EXPECT_EQ(l0a_ptr.get(), l0a_data);
    EXPECT_EQ(l0b_ptr.get(), l0b_data);
    EXPECT_EQ(l0c_ptr.get(), l0c_data);
    EXPECT_EQ(bias_ptr.get(), bias_data);
    EXPECT_EQ(fixbuf_ptr.get(), fixbuf_data);
}

TEST_F(tensor_api_pointer, test_pointer_adaptor)
{
    using namespace asc::te;

    __gm__ float data[4] = {1, 2, 3, 4};
    auto ptr = make_mem_ptr<location::gm>(data);

    EXPECT_EQ(*ptr, 1);
    EXPECT_EQ(ptr[2], 3);
    EXPECT_EQ((ptr + 3).get(), data + 3);

    *ptr = 10;
    (ptr + 1)[1] = 20;

    EXPECT_EQ(data[0], 10);
    EXPECT_EQ(data[2], 20);
}

TEST_F(tensor_api_pointer, test_pointer_operators)
{
    using namespace asc::te;

    __gm__ float data[4] = {0};
    auto ptr = make_mem_ptr<location::gm>(data);
    auto next = ptr + 1;
    auto far = ptr + 3;

    EXPECT_TRUE(ptr == ptr);
    EXPECT_TRUE(ptr != next);
    EXPECT_TRUE(ptr < next);
    EXPECT_TRUE(next > ptr);
    EXPECT_TRUE(ptr <= next);
    EXPECT_TRUE(far >= next);
}
