/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the license.
 */

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/experimental/vector_compute.h"
#include "tensor_api/tensor.h"

namespace {

using namespace asc::te::experimental;

template <typename T>
using reg_type = asc::te::experimental::reg_tensor<T>;

template <typename T>
__aicore__ inline void TestAbs()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::abs(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestExp()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::exp(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestSqrt()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::sqrt(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestLog2()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::log2(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestLn()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::ln(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestNeg()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::neg(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestLog10()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::log10(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestRelu()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::relu(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestPrelu()
{
    reg_type<T> src{};
    reg_type<T> slope{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::prelu(src, slope);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestLeakyRelu()
{
    reg_type<T> src{};
    T slope{};
    src.mask = all_mask<T>().reg;

    auto dst = asc::te::experimental::leaky_relu(src, slope);
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestAddCarry()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    reg_type<bool> carry_src{};
    src0.mask = all_mask<T>().reg;

    auto result_without_carry = asc::te::experimental::add_carry(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(result_without_carry), reg_pair<T, bool>>);
    EXPECT_EQ(result_without_carry.first.mask, src0.mask);
    EXPECT_EQ(result_without_carry.second.mask, src0.mask);

    auto result = asc::te::experimental::add_carry(src0, src1, carry_src);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_pair<T, bool>>);
    EXPECT_EQ(result.first.mask, src0.mask);
    EXPECT_EQ(result.second.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestSubCarry()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    reg_type<bool> borrow_src{};
    src0.mask = all_mask<T>().reg;

    auto result_without_borrow = asc::te::experimental::sub_carry(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(result_without_borrow), reg_pair<T, bool>>);
    EXPECT_EQ(result_without_borrow.first.mask, src0.mask);
    EXPECT_EQ(result_without_borrow.second.mask, src0.mask);

    auto result = asc::te::experimental::sub_carry(src0, src1, borrow_src);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_pair<T, bool>>);
    EXPECT_EQ(result.first.mask, src0.mask);
    EXPECT_EQ(result.second.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestMull()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    src0.mask = all_mask<T>().reg;

    auto result = asc::te::experimental::mull(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(result), reg_pair<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(result.first), reg_type<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(result.second), reg_type<T>>);
    EXPECT_EQ(result.first.mask, src0.mask);
    EXPECT_EQ(result.second.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestDiv()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    src0.mask = all_mask<T>().reg;

    auto dst = src0 / src1;
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestMin()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    T scalar{};
    src0.mask = all_mask<T>().reg;

    auto reg_dst = asc::te::experimental::min(src0, src1);
    auto right_scalar_dst = asc::te::experimental::min(src0, scalar);
    auto left_scalar_dst = asc::te::experimental::min(scalar, src0);
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), reg_type<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(right_scalar_dst), reg_type<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(left_scalar_dst), reg_type<T>>);
    EXPECT_EQ(reg_dst.mask, src0.mask);
    EXPECT_EQ(right_scalar_dst.mask, src0.mask);
    EXPECT_EQ(left_scalar_dst.mask, src0.mask);
}

#define BASIC_ARITHMETIC_TEST(Function, DataType)                 \
    TEST(test_tensor_api_basic_arithmetic, Function##_##DataType) \
    {                                                             \
        Test##Function<DataType>();                               \
        SUCCEED();                                                \
    }

BASIC_ARITHMETIC_TEST(Abs, int8_t)
BASIC_ARITHMETIC_TEST(Abs, int16_t)
BASIC_ARITHMETIC_TEST(Abs, half)
BASIC_ARITHMETIC_TEST(Abs, int32_t)
BASIC_ARITHMETIC_TEST(Abs, float)
BASIC_ARITHMETIC_TEST(Exp, half)
BASIC_ARITHMETIC_TEST(Exp, float)
BASIC_ARITHMETIC_TEST(Sqrt, half)
BASIC_ARITHMETIC_TEST(Sqrt, float)
BASIC_ARITHMETIC_TEST(Ln, half)
BASIC_ARITHMETIC_TEST(Ln, float)
BASIC_ARITHMETIC_TEST(Neg, int8_t)
BASIC_ARITHMETIC_TEST(Neg, int16_t)
BASIC_ARITHMETIC_TEST(Neg, half)
BASIC_ARITHMETIC_TEST(Neg, int32_t)
BASIC_ARITHMETIC_TEST(Neg, float)
BASIC_ARITHMETIC_TEST(Log2, half)
BASIC_ARITHMETIC_TEST(Log2, float)
BASIC_ARITHMETIC_TEST(Log10, half)
BASIC_ARITHMETIC_TEST(Log10, float)
BASIC_ARITHMETIC_TEST(Relu, half)
BASIC_ARITHMETIC_TEST(Relu, int32_t)
BASIC_ARITHMETIC_TEST(Relu, float)
BASIC_ARITHMETIC_TEST(Prelu, half)
BASIC_ARITHMETIC_TEST(Prelu, float)
BASIC_ARITHMETIC_TEST(LeakyRelu, half)
BASIC_ARITHMETIC_TEST(LeakyRelu, float)
BASIC_ARITHMETIC_TEST(AddCarry, int32_t)
BASIC_ARITHMETIC_TEST(AddCarry, uint32_t)
BASIC_ARITHMETIC_TEST(SubCarry, int32_t)
BASIC_ARITHMETIC_TEST(SubCarry, uint32_t)
BASIC_ARITHMETIC_TEST(Mull, int32_t)
BASIC_ARITHMETIC_TEST(Mull, uint32_t)
BASIC_ARITHMETIC_TEST(Div, int16_t)
BASIC_ARITHMETIC_TEST(Div, uint16_t)
BASIC_ARITHMETIC_TEST(Div, half)
BASIC_ARITHMETIC_TEST(Div, int32_t)
BASIC_ARITHMETIC_TEST(Div, uint32_t)
BASIC_ARITHMETIC_TEST(Div, float)
BASIC_ARITHMETIC_TEST(Min, int8_t)
BASIC_ARITHMETIC_TEST(Min, uint8_t)
BASIC_ARITHMETIC_TEST(Min, int16_t)
BASIC_ARITHMETIC_TEST(Min, uint16_t)
BASIC_ARITHMETIC_TEST(Min, half)
BASIC_ARITHMETIC_TEST(Min, bfloat16_t)
BASIC_ARITHMETIC_TEST(Min, int32_t)
BASIC_ARITHMETIC_TEST(Min, uint32_t)
BASIC_ARITHMETIC_TEST(Min, float)

} // namespace
