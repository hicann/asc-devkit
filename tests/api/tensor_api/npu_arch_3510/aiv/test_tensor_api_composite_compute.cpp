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

class tensor_api_vector_composite_compute_3510 : public testing::Test {};

template <typename T>
using reg_type = asc::te::experimental::reg_tensor<T>;

template <typename T>
__aicore__ inline void test_axpy()
{
    reg_type<T> dst{};
    reg_type<T> src{};
    T scalar{};
    dst.mask = asc::te::experimental::all_mask<T>().reg;

    auto result = asc::te::experimental::axpy(dst, src, scalar);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_type<T>>);
    EXPECT_EQ(result.mask, dst.mask);
}

template <typename T>
__aicore__ inline void test_abs_diff()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto dst = asc::te::experimental::abs_diff(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void test_madd()
{
    reg_type<T> dst{};
    reg_type<T> src0{};
    reg_type<T> src1{};
    dst.mask = asc::te::experimental::all_mask<T>().reg;

    auto result = asc::te::experimental::madd(dst, src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_type<T>>);
    EXPECT_EQ(result.mask, dst.mask);
}

template <typename T>
__aicore__ inline void test_mula()
{
    reg_type<T> dst{};
    reg_type<T> src0{};
    reg_type<T> src1{};
    dst.mask = asc::te::experimental::all_mask<T>().reg;

    auto result = asc::te::experimental::mula(dst, src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_type<T>>);
    EXPECT_EQ(result.mask, dst.mask);
}

template <typename T>
__aicore__ inline void test_fma()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    reg_type<T> src2{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto dst = asc::te::experimental::fma(src0, src1, src2);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

__aicore__ inline void test_exp_diff_float()
{
    reg_type<float> src0{};
    reg_type<float> src1{};
    src0.mask = asc::te::experimental::all_mask<float>().reg;

    auto dst = asc::te::experimental::exp_diff<float, float>(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<float>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

__aicore__ inline void test_exp_diff_half()
{
    reg_type<half> src0{};
    reg_type<half> src1{};
    auto output_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::vl3, float>();
    src0.mask = output_mask.reg;

    auto even_dst = asc::te::experimental::exp_diff<float, half>(src0, src1, ASC_POSITION_EVEN);
    auto odd_dst = asc::te::experimental::exp_diff<float, half>(src0, src1, ASC_POSITION_ODD);
    static_assert(AscendC::Std::is_same_v<decltype(even_dst), reg_type<float>>);
    static_assert(AscendC::Std::is_same_v<decltype(odd_dst), reg_type<float>>);
    EXPECT_EQ(even_dst.mask, src0.mask);
    EXPECT_EQ(odd_dst.mask, src0.mask);
}

__aicore__ inline void test_muls_cast()
{
    reg_type<float> src{};
    src.mask = asc::te::experimental::all_mask<float>().reg;

    auto even_dst = asc::te::experimental::muls_cast<asc::te::experimental::cast_layout::zero, half>(src, 1.0f);
    auto odd_dst = asc::te::experimental::muls_cast<asc::te::experimental::cast_layout::one, half>(src, 1.0f);
    static_assert(AscendC::Std::is_same_v<decltype(even_dst), reg_type<half>>);
    static_assert(AscendC::Std::is_same_v<decltype(odd_dst), reg_type<half>>);
    EXPECT_EQ(even_dst.mask, src.mask);
    EXPECT_EQ(odd_dst.mask, src.mask);
}

#define COMPOSITE_COMPUTE_TEST(Function, DataType)                          \
    TEST_F(tensor_api_vector_composite_compute_3510, Function##_##DataType) \
    {                                                                       \
        test_##Function<DataType>();                                        \
        SUCCEED();                                                          \
    }

COMPOSITE_COMPUTE_TEST(axpy, half)
COMPOSITE_COMPUTE_TEST(axpy, float)
COMPOSITE_COMPUTE_TEST(abs_diff, half)
COMPOSITE_COMPUTE_TEST(abs_diff, float)
COMPOSITE_COMPUTE_TEST(madd, half)
COMPOSITE_COMPUTE_TEST(madd, bfloat16_t)
COMPOSITE_COMPUTE_TEST(madd, float)
COMPOSITE_COMPUTE_TEST(mula, int16_t)
COMPOSITE_COMPUTE_TEST(mula, uint16_t)
COMPOSITE_COMPUTE_TEST(mula, half)
COMPOSITE_COMPUTE_TEST(mula, bfloat16_t)
COMPOSITE_COMPUTE_TEST(mula, int32_t)
COMPOSITE_COMPUTE_TEST(mula, uint32_t)
COMPOSITE_COMPUTE_TEST(mula, float)
COMPOSITE_COMPUTE_TEST(fma, half)
COMPOSITE_COMPUTE_TEST(fma, bfloat16_t)
COMPOSITE_COMPUTE_TEST(fma, float)

TEST_F(tensor_api_vector_composite_compute_3510, exp_diff_float)
{
    test_exp_diff_float();
    SUCCEED();
}

TEST_F(tensor_api_vector_composite_compute_3510, exp_diff_half)
{
    test_exp_diff_half();
    SUCCEED();
}

TEST_F(tensor_api_vector_composite_compute_3510, muls_cast)
{
    test_muls_cast();
    SUCCEED();
}
