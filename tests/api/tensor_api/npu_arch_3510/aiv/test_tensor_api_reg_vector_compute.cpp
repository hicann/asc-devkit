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

class TensorApiRegVectorCompute3510 : public testing::Test {};

template <typename T>
using RegTensor = asc::te::experimental::reg_tensor<T>;

template <typename T>
__aicore__ inline void TestLog()
{
    RegTensor<T> src{};
    src.mask = asc::te::experimental::all_mask<T>().reg;

    auto dst = asc::te::experimental::log(src);
    static_assert(AscendC::Std::is_same_v<decltype(dst), RegTensor<T>>);
    EXPECT_EQ(dst.mask, src.mask);
}

template <typename T>
__aicore__ inline void TestAdd()
{
    RegTensor<T> src0{};
    RegTensor<T> src1{};
    T scalar{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto reg_dst = src0 + src1;
    auto right_scalar_dst = src0 + scalar;
    auto left_scalar_dst = scalar + src0;
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(right_scalar_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(left_scalar_dst), RegTensor<T>>);
    EXPECT_EQ(reg_dst.mask, src0.mask);
    EXPECT_EQ(right_scalar_dst.mask, src0.mask);
    EXPECT_EQ(left_scalar_dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestSub()
{
    RegTensor<T> src0{};
    RegTensor<T> src1{};
    T scalar{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto reg_dst = src0 - src1;
    auto right_scalar_dst = src0 - scalar;
    auto left_scalar_dst = scalar - src0;
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(right_scalar_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(left_scalar_dst), RegTensor<T>>);
    EXPECT_EQ(reg_dst.mask, src0.mask);
    EXPECT_EQ(right_scalar_dst.mask, src0.mask);
    EXPECT_EQ(left_scalar_dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestMul()
{
    RegTensor<T> src0{};
    RegTensor<T> src1{};
    T scalar{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto reg_dst = src0 * src1;
    auto right_scalar_dst = src0 * scalar;
    auto left_scalar_dst = scalar * src0;
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(right_scalar_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(left_scalar_dst), RegTensor<T>>);
    EXPECT_EQ(reg_dst.mask, src0.mask);
    EXPECT_EQ(right_scalar_dst.mask, src0.mask);
    EXPECT_EQ(left_scalar_dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestMax()
{
    RegTensor<T> src0{};
    RegTensor<T> src1{};
    T scalar{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto reg_dst = asc::te::experimental::max(src0, src1);
    auto right_scalar_dst = asc::te::experimental::max(src0, scalar);
    auto left_scalar_dst = asc::te::experimental::max(scalar, src0);
    static_assert(AscendC::Std::is_same_v<decltype(reg_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(right_scalar_dst), RegTensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(left_scalar_dst), RegTensor<T>>);
    EXPECT_EQ(reg_dst.mask, src0.mask);
    EXPECT_EQ(right_scalar_dst.mask, src0.mask);
    EXPECT_EQ(left_scalar_dst.mask, src0.mask);
}

template <typename T>
__aicore__ inline void TestBitOr()
{
    RegTensor<T> src0{};
    RegTensor<T> src1{};
    src0.mask = asc::te::experimental::all_mask<T>().reg;

    auto dst = src0 | src1;
    static_assert(AscendC::Std::is_same_v<decltype(dst), RegTensor<T>>);
    EXPECT_EQ(dst.mask, src0.mask);
}

#define REG_VECTOR_COMPUTE_TEST(Function, DataType)              \
    TEST_F(TensorApiRegVectorCompute3510, Function##_##DataType) \
    {                                                            \
        Test##Function<DataType>();                              \
        SUCCEED();                                               \
    }

REG_VECTOR_COMPUTE_TEST(Log, float)
REG_VECTOR_COMPUTE_TEST(Add, float)
REG_VECTOR_COMPUTE_TEST(Sub, float)
REG_VECTOR_COMPUTE_TEST(Mul, int16_t)
REG_VECTOR_COMPUTE_TEST(Max, float)
REG_VECTOR_COMPUTE_TEST(BitOr, int32_t)
REG_VECTOR_COMPUTE_TEST(BitOr, half)
REG_VECTOR_COMPUTE_TEST(BitOr, float)
REG_VECTOR_COMPUTE_TEST(BitOr, bool)
