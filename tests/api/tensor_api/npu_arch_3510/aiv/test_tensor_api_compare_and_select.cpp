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

class tensor_api_vector_compare_and_select_3510 : public testing::Test {};

template <typename T>
__aicore__ inline void test_compare_reg_tensor()
{
    asc::te::experimental::reg_tensor<T> src0{};
    asc::te::experimental::reg_tensor<T> src1{};
    auto all = asc::te::experimental::all_mask<T>();
    src0.mask = all.reg;

    auto eq = src0 == src1;
    auto ne = src0 != src1;
    auto lt = src0 < src1;
    auto le = src0 <= src1;
    auto gt = src0 > src1;
    auto ge = src0 >= src1;
    EXPECT_EQ(eq.mask, src0.mask);
    EXPECT_EQ(ne.mask, src0.mask);
    EXPECT_EQ(lt.mask, src0.mask);
    EXPECT_EQ(le.mask, src0.mask);
    EXPECT_EQ(gt.mask, src0.mask);
    EXPECT_EQ(ge.mask, src0.mask);
}

template <typename T>
__aicore__ inline void test_compare_reg_tensor_scalar()
{
    asc::te::experimental::reg_tensor<T> src0{};
    T src1{};
    auto all = asc::te::experimental::all_mask<T>();
    src0.mask = all.reg;

    auto eq = src0 == src1;
    auto ne = src0 != src1;
    auto lt = src0 < src1;
    auto le = src0 <= src1;
    auto gt = src0 > src1;
    auto ge = src0 >= src1;

    EXPECT_EQ(eq.mask, src0.mask);
    EXPECT_EQ(ne.mask, src0.mask);
    EXPECT_EQ(lt.mask, src0.mask);
    EXPECT_EQ(le.mask, src0.mask);
    EXPECT_EQ(gt.mask, src0.mask);
    EXPECT_EQ(ge.mask, src0.mask);
}

template <typename T>
__aicore__ inline void test_compare_scalar_reg_tensor()
{
    T src0{};
    asc::te::experimental::reg_tensor<T> src1{};
    auto all = asc::te::experimental::all_mask<T>();
    src1.mask = all.reg;

    auto eq = src0 == src1;
    auto ne = src0 != src1;
    auto lt = src0 < src1;
    auto le = src0 <= src1;
    auto gt = src0 > src1;
    auto ge = src0 >= src1;
    EXPECT_EQ(eq.mask, src1.mask);
    EXPECT_EQ(ne.mask, src1.mask);
    EXPECT_EQ(lt.mask, src1.mask);
    EXPECT_EQ(le.mask, src1.mask);
    EXPECT_EQ(gt.mask, src1.mask);
    EXPECT_EQ(ge.mask, src1.mask);
}

template <typename T>
__aicore__ inline void test_select()
{
    asc::te::experimental::reg_tensor<bool> condition{};
    asc::te::experimental::reg_tensor<T> src0{};
    asc::te::experimental::reg_tensor<T> src1{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto dst = asc::te::experimental::select(condition, src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

#define COMPARE_AND_SELECT_TEST(Function, DataType)                          \
    TEST_F(tensor_api_vector_compare_and_select_3510, Function##_##DataType) \
    {                                                                        \
        test_##Function<DataType>();                                         \
        SUCCEED();                                                           \
    }

#define SELECT_TYPE_TEST(DataType) COMPARE_AND_SELECT_TEST(select, DataType)

#define COMPARE_TYPE_TESTS(DataType)                             \
    COMPARE_AND_SELECT_TEST(compare_reg_tensor, DataType)        \
    COMPARE_AND_SELECT_TEST(compare_reg_tensor_scalar, DataType) \
    COMPARE_AND_SELECT_TEST(compare_scalar_reg_tensor, DataType)

COMPARE_TYPE_TESTS(uint8_t)
COMPARE_TYPE_TESTS(int8_t)
COMPARE_TYPE_TESTS(uint16_t)
COMPARE_TYPE_TESTS(int16_t)
COMPARE_TYPE_TESTS(half)
COMPARE_TYPE_TESTS(bfloat16_t)
COMPARE_TYPE_TESTS(uint32_t)
COMPARE_TYPE_TESTS(int32_t)
COMPARE_TYPE_TESTS(float)

SELECT_TYPE_TEST(bool)
SELECT_TYPE_TEST(uint8_t)
SELECT_TYPE_TEST(int8_t)
SELECT_TYPE_TEST(fp8_e4m3fn_t)
SELECT_TYPE_TEST(hifloat8_t)
SELECT_TYPE_TEST(fp8_e5m2_t)
SELECT_TYPE_TEST(uint16_t)
SELECT_TYPE_TEST(int16_t)
SELECT_TYPE_TEST(half)
SELECT_TYPE_TEST(bfloat16_t)
SELECT_TYPE_TEST(uint32_t)
SELECT_TYPE_TEST(int32_t)
SELECT_TYPE_TEST(float)
