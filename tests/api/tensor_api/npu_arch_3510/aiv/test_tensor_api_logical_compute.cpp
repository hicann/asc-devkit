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
__aicore__ inline void TestLogicalNot()
{
    reg_type<T> src{};
    src.mask = all_mask<T>().reg;

    auto dst = !src;
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestBitwiseAnd()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    src0.mask = all_mask<T>().reg;

    auto dst = src0 & src1;
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestBitwiseXor()
{
    reg_type<T> src0{};
    reg_type<T> src1{};
    src0.mask = all_mask<T>().reg;

    auto dst = src0 ^ src1;
    static_assert(AscendC::Std::is_same_v<decltype(dst), reg_type<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

template <typename T, typename ShiftType>
__aicore__ inline void TestShiftLeft()
{
    reg_type<T> src{};
    reg_type<ShiftType> shift{};
    int16_t scalar_shift{};
    src.mask = all_mask<T>().reg;

    auto reg_dst = src << shift;
    auto scalar_dst = src << scalar_shift;
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), reg_type<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(scalar_dst), reg_type<T>>);
    EXPECT_EQ(reg_dst.mask, src.mask);
    EXPECT_EQ(scalar_dst.mask, src.mask);
}

template <typename T, typename ShiftType>
__aicore__ inline void TestShiftRight()
{
    reg_type<T> src{};
    reg_type<ShiftType> shift{};
    int16_t scalar_shift{};
    src.mask = all_mask<T>().reg;

    auto reg_dst = src >> shift;
    auto scalar_dst = src >> scalar_shift;
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), reg_type<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(scalar_dst), reg_type<T>>);
    EXPECT_EQ(reg_dst.mask, src.mask);
    EXPECT_EQ(scalar_dst.mask, src.mask);
}

#define LOGICAL_COMPUTE_TEST(Function, DataType)                 \
    TEST(test_tensor_api_logical_compute, Function##_##DataType) \
    {                                                            \
        Test##Function<DataType>();                              \
        SUCCEED();                                               \
    }

#define SHIFT_TEST(Function, DataType, ShiftType)                              \
    TEST(test_tensor_api_logical_compute, Function##_##DataType##_##ShiftType) \
    {                                                                          \
        Test##Function<DataType, ShiftType>();                                 \
        SUCCEED();                                                             \
    }

LOGICAL_COMPUTE_TEST(LogicalNot, int8_t)
LOGICAL_COMPUTE_TEST(LogicalNot, uint8_t)
LOGICAL_COMPUTE_TEST(LogicalNot, int16_t)
LOGICAL_COMPUTE_TEST(LogicalNot, uint16_t)
LOGICAL_COMPUTE_TEST(LogicalNot, half)
LOGICAL_COMPUTE_TEST(LogicalNot, int32_t)
LOGICAL_COMPUTE_TEST(LogicalNot, uint32_t)
LOGICAL_COMPUTE_TEST(LogicalNot, float)
LOGICAL_COMPUTE_TEST(LogicalNot, bool)

LOGICAL_COMPUTE_TEST(BitwiseAnd, int8_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, uint8_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, int16_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, uint16_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, int32_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, uint32_t)
LOGICAL_COMPUTE_TEST(BitwiseAnd, bool)

LOGICAL_COMPUTE_TEST(BitwiseXor, int8_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, uint8_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, int16_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, uint16_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, int32_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, uint32_t)
LOGICAL_COMPUTE_TEST(BitwiseXor, bool)

SHIFT_TEST(ShiftLeft, int8_t, int8_t)
SHIFT_TEST(ShiftLeft, uint8_t, int8_t)
SHIFT_TEST(ShiftLeft, int16_t, int16_t)
SHIFT_TEST(ShiftLeft, uint16_t, int16_t)
SHIFT_TEST(ShiftLeft, int32_t, int32_t)
SHIFT_TEST(ShiftLeft, uint32_t, int32_t)

SHIFT_TEST(ShiftRight, int8_t, int8_t)
SHIFT_TEST(ShiftRight, uint8_t, int8_t)
SHIFT_TEST(ShiftRight, int16_t, int16_t)
SHIFT_TEST(ShiftRight, uint16_t, int16_t)
SHIFT_TEST(ShiftRight, int32_t, int32_t)
SHIFT_TEST(ShiftRight, uint32_t, int32_t)

} // namespace
