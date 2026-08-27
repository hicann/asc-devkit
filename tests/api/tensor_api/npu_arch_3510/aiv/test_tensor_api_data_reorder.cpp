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
#include "tensor_api/experimental/vector_compute.h"
#include "tensor_api/tensor.h"

class TensorApiVectorDataReorder3510 : public testing::Test {};

template <typename T>
__aicore__ inline void TestInterleave()
{
    asc::te::experimental::reg_tensor<T> src0 {};
    asc::te::experimental::reg_tensor<T> src1 {};

    auto [dst0, dst1] = asc::te::experimental::interleave<T>(src0, src1);
    auto mask = asc::te::experimental::all_mask<uint8_t>();
    EXPECT_EQ(dst0.mask, mask.reg);
    EXPECT_EQ(dst1.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestDeinterleave()
{
    asc::te::experimental::reg_tensor<T> src0 {};
    asc::te::experimental::reg_tensor<T> src1 {};
    
    auto mask = asc::te::experimental::all_mask<uint8_t>();
    auto [dst0, dst1] = asc::te::experimental::deinterleave<T>(src0, src1);
    EXPECT_EQ(dst0.mask, mask.reg);
    EXPECT_EQ(dst1.mask, mask.reg);
}

#define DATA_REORDER_TEST(Function, DataType)                                   \
    TEST_F(TensorApiVectorDataReorder3510, Function##_##DataType)               \
    {                                                                            \
        Test##Function<DataType>();                                              \
        SUCCEED();                                                               \
    }

#define DATA_REORDER_TYPE_TESTS(DataType) \
    DATA_REORDER_TEST(Interleave, DataType) \
    DATA_REORDER_TEST(Deinterleave, DataType)

DATA_REORDER_TYPE_TESTS(uint8_t)
DATA_REORDER_TYPE_TESTS(int8_t)
DATA_REORDER_TYPE_TESTS(fp8_e4m3fn_t)
DATA_REORDER_TYPE_TESTS(fp8_e8m0_t)
DATA_REORDER_TYPE_TESTS(fp8_e5m2_t)
DATA_REORDER_TYPE_TESTS(uint16_t)
DATA_REORDER_TYPE_TESTS(int16_t)
DATA_REORDER_TYPE_TESTS(half)
DATA_REORDER_TYPE_TESTS(bfloat16_t)
DATA_REORDER_TYPE_TESTS(uint32_t)
DATA_REORDER_TYPE_TESTS(int32_t)
DATA_REORDER_TYPE_TESTS(float)
