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
#include "tensor_api/tensor.h"

class TensorApiVectorDataPadding3510 : public testing::Test {};

template <typename T>
__aicore__ inline void TestFillScalar()
{
    auto mask  = asc::te::make_mask<asc::te::mask_pattern::every3, T>();
    auto dst = asc::te::fill(T {}, mask);
    EXPECT_EQ(dst.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestFillRegTensor()
{
    asc::te::reg_tensor<T> src {};
    auto mask  = asc::te::make_mask<asc::te::mask_pattern::every3, T>();
    auto dst = asc::te::fill(src, mask);
    EXPECT_EQ(dst.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDefaultMask()
{
    asc::te::reg_tensor<T> src {};

    auto scalar_dst = asc::te::fill(T {});
    auto reg_tensor_dst = asc::te::fill(src);
    auto mask = asc::te::all_mask<T>();
    EXPECT_EQ(scalar_dst.mask, mask.reg);
    EXPECT_EQ(reg_tensor_dst.mask, mask.reg);
}

#define DATA_PADDING_TEST(Function, DataType)                       \
    TEST_F(TensorApiVectorDataPadding3510, Function##_##DataType)   \
    {                                                               \
        Test##Function<DataType>();                                 \
        SUCCEED();                                                   \
    }

#define DATA_PADDING_TYPE_TESTS(DataType)       \
    DATA_PADDING_TEST(FillScalar, DataType)     \
    DATA_PADDING_TEST(FillRegTensor, DataType)  \
    DATA_PADDING_TEST(FillDefaultMask, DataType)

DATA_PADDING_TYPE_TESTS(uint8_t)
DATA_PADDING_TYPE_TESTS(int8_t)
DATA_PADDING_TYPE_TESTS(fp8_e4m3fn_t)
//DATA_PADDING_TYPE_TESTS(fp8_e8m0_t)
DATA_PADDING_TYPE_TESTS(fp8_e5m2_t)
DATA_PADDING_TYPE_TESTS(uint16_t)
DATA_PADDING_TYPE_TESTS(int16_t)
DATA_PADDING_TYPE_TESTS(half)
DATA_PADDING_TYPE_TESTS(bfloat16_t)
DATA_PADDING_TYPE_TESTS(uint32_t)
DATA_PADDING_TYPE_TESTS(int32_t)
DATA_PADDING_TYPE_TESTS(float)
