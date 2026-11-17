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
#include "tensor_api/experimental/vector_ops.h"
#include "tensor_api/tensor.h"

class TensorApiVectorDataPadding3510 : public testing::Test {};

template <typename T>
__aicore__ inline void TestFillScalar()
{
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto dst = asc::te::experimental::fill(T{}, mask);
    static_assert(AscendC::Std::is_same_v<decltype(dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(dst.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestFillRegTensor()
{
    asc::te::experimental::reg_tensor<T> src{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto dst = asc::te::experimental::fill(src, mask);
    static_assert(AscendC::Std::is_same_v<decltype(dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(dst.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestFillRegTensorPart()
{
    asc::te::experimental::reg_tensor<T> src{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto low_dst = asc::te::experimental::fill<asc::te::experimental::high_low_part::low>(src, mask);
    auto high_dst = asc::te::experimental::fill<asc::te::experimental::high_low_part::high>(src, mask);
    auto low_default_dst = asc::te::experimental::fill<asc::te::experimental::high_low_part::low>(src);
    auto high_default_dst = asc::te::experimental::fill<asc::te::experimental::high_low_part::high>(src);
    static_assert(AscendC::Std::is_same_v<decltype(low_dst), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(high_dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(low_dst.mask, mask.reg);
    EXPECT_EQ(high_dst.mask, mask.reg);
    auto default_mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(low_default_dst.mask, default_mask.reg);
    EXPECT_EQ(high_default_dst.mask, default_mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDefaultMask()
{
    asc::te::experimental::reg_tensor<T> src{};

    auto scalar_dst = asc::te::experimental::fill(T{});
    auto reg_tensor_dst = asc::te::experimental::fill(src);
    static_assert(AscendC::Std::is_same_v<decltype(scalar_dst), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(reg_tensor_dst), asc::te::experimental::reg_tensor<T>>);
    auto mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(scalar_dst.mask, mask.reg);
    EXPECT_EQ(reg_tensor_dst.mask, mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDstScalar()
{
    asc::te::experimental::reg_tensor<T> dst{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every4, T>();
    dst.mask = dst_mask.reg;

    auto zeroing_dst = asc::te::experimental::fill(dst, T{}, mask);
    auto merging_dst = asc::te::experimental::fill<asc::te::experimental::merge_mode::merging>(dst, T{}, mask);
    static_assert(AscendC::Std::is_same_v<decltype(zeroing_dst), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(merging_dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(zeroing_dst.mask, dst_mask.reg);
    EXPECT_EQ(merging_dst.mask, dst_mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDstRegTensor()
{
    asc::te::experimental::reg_tensor<T> dst{};
    asc::te::experimental::reg_tensor<T> src{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every4, T>();
    dst.mask = dst_mask.reg;

    auto zeroing_dst = asc::te::experimental::fill(dst, src, mask);
    auto merging_dst = asc::te::experimental::fill<
        asc::te::experimental::high_low_part::low, asc::te::experimental::merge_mode::merging>(dst, src, mask);
    static_assert(AscendC::Std::is_same_v<decltype(zeroing_dst), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(merging_dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(zeroing_dst.mask, dst_mask.reg);
    EXPECT_EQ(merging_dst.mask, dst_mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDstRegTensorHighest()
{
    asc::te::experimental::reg_tensor<T> dst{};
    asc::te::experimental::reg_tensor<T> src{};
    auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, T>();
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every4, T>();
    dst.mask = dst_mask.reg;

    auto zeroing_dst = asc::te::experimental::fill<asc::te::experimental::high_low_part::high>(dst, src, mask);
    auto merging_dst = asc::te::experimental::fill<
        asc::te::experimental::high_low_part::high, asc::te::experimental::merge_mode::merging>(dst, src, mask);
    static_assert(AscendC::Std::is_same_v<decltype(zeroing_dst), asc::te::experimental::reg_tensor<T>>);
    static_assert(AscendC::Std::is_same_v<decltype(merging_dst), asc::te::experimental::reg_tensor<T>>);
    EXPECT_EQ(zeroing_dst.mask, dst_mask.reg);
    EXPECT_EQ(merging_dst.mask, dst_mask.reg);
}

template <typename T>
__aicore__ inline void TestFillDstDefaultMask()
{
    asc::te::experimental::reg_tensor<T> dst{};
    asc::te::experimental::reg_tensor<T> src{};
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every4, T>();
    dst.mask = dst_mask.reg;

    auto scalar_dst = asc::te::experimental::fill(dst, T{});
    auto reg_tensor_dst = asc::te::experimental::fill(dst, src);
    EXPECT_EQ(scalar_dst.mask, dst_mask.reg);
    EXPECT_EQ(reg_tensor_dst.mask, dst_mask.reg);
}

#define DATA_PADDING_TEST(Function, DataType)                     \
    TEST_F(TensorApiVectorDataPadding3510, Function##_##DataType) \
    {                                                             \
        Test##Function<DataType>();                               \
        SUCCEED();                                                \
    }

#define DATA_PADDING_TYPE_TESTS(DataType)                \
    DATA_PADDING_TEST(FillScalar, DataType)              \
    DATA_PADDING_TEST(FillRegTensor, DataType)           \
    DATA_PADDING_TEST(FillRegTensorPart, DataType)       \
    DATA_PADDING_TEST(FillDefaultMask, DataType)         \
    DATA_PADDING_TEST(FillDstScalar, DataType)           \
    DATA_PADDING_TEST(FillDstRegTensor, DataType)        \
    DATA_PADDING_TEST(FillDstRegTensorHighest, DataType) \
    DATA_PADDING_TEST(FillDstDefaultMask, DataType)

DATA_PADDING_TYPE_TESTS(uint8_t)
DATA_PADDING_TYPE_TESTS(int8_t)
DATA_PADDING_TYPE_TESTS(fp8_e4m3fn_t)
DATA_PADDING_TYPE_TESTS(fp8_e8m0_t)
DATA_PADDING_TYPE_TESTS(fp8_e5m2_t)
DATA_PADDING_TYPE_TESTS(uint16_t)
DATA_PADDING_TYPE_TESTS(int16_t)
DATA_PADDING_TYPE_TESTS(half)
DATA_PADDING_TYPE_TESTS(bfloat16_t)
DATA_PADDING_TYPE_TESTS(uint32_t)
DATA_PADDING_TYPE_TESTS(int32_t)
DATA_PADDING_TYPE_TESTS(float)
