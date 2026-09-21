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

class tensor_api_vector_mask_reg_compute_3510 : public testing::Test {};

template <asc::te::experimental::mask_pattern Pattern, typename T>
__aicore__ inline void test_make_mask_pattern()
{
    auto mask = asc::te::experimental::make_mask<Pattern, T>();
    static_assert(AscendC::Std::is_same_v<decltype(mask), asc::te::experimental::reg_tensor<bool>>);
    (void)mask;
}

template <typename T>
__aicore__ inline void test_make_mask()
{
    test_make_mask_pattern<asc::te::experimental::mask_pattern::all, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl1, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl2, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl3, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl4, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl8, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl16, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl32, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl64, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::vl128, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::every3, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::every4, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::half, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::quarter, T>();
    test_make_mask_pattern<asc::te::experimental::mask_pattern::none, T>();

    auto all = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::all, T>();
    auto none = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::none, T>();
    EXPECT_EQ(all.reg, asc::te::experimental::all_mask<T>().reg);
    EXPECT_EQ(none.reg, asc::te::experimental::none_mask<T>().reg);
}

template <typename T>
__aicore__ inline void test_all_mask()
{
    auto mask = asc::te::experimental::all_mask<T>();
    static_assert(AscendC::Std::is_same_v<decltype(mask), asc::te::experimental::reg_tensor<bool>>);
    EXPECT_EQ(mask.reg, (asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::all, T>().reg));
}

template <typename T>
__aicore__ inline void test_none_mask()
{
    auto mask = asc::te::experimental::none_mask<T>();
    static_assert(AscendC::Std::is_same_v<decltype(mask), asc::te::experimental::reg_tensor<bool>>);
    EXPECT_EQ(mask.reg, (asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::none, T>().reg));
}

template <typename T>
__aicore__ inline void test_update_mask()
{
    uint32_t total = 256;
    uint32_t remain = total;
    auto mask = asc::te::experimental::update_mask<T>(remain);
    static_assert(AscendC::Std::is_same_v<decltype(mask), asc::te::experimental::reg_tensor<bool>>);
    EXPECT_EQ(mask.reg, asc::te::experimental::all_mask<T>().reg);
}

template <typename T>
__aicore__ inline void test_interleave()
{
    auto src0 = asc::te::experimental::all_mask<T>();
    auto src1 = asc::te::experimental::none_mask<T>();
    auto dst = asc::te::experimental::interleave<T>(src0, src1);
    static_assert(AscendC::Std::is_same_v<decltype(dst), asc::te::experimental::reg_pair<bool>>);
    auto expect_mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(dst.first.mask, expect_mask.reg);
    EXPECT_EQ(dst.second.mask, expect_mask.reg);

    auto restored = asc::te::experimental::deinterleave<T>(dst.first, dst.second);
    (void)restored;
}

template <typename T>
__aicore__ inline void test_deinterleave()
{
    auto src0 = asc::te::experimental::all_mask<T>();
    auto src1 = asc::te::experimental::none_mask<T>();
    auto interleaved = asc::te::experimental::interleave<T>(src0, src1);
    auto dst = asc::te::experimental::deinterleave<T>(interleaved.first, interleaved.second);
    static_assert(AscendC::Std::is_same_v<decltype(dst), asc::te::experimental::reg_pair<bool>>);
    auto expect_mask = asc::te::experimental::all_mask<T>();
    EXPECT_EQ(dst.first.mask, expect_mask.reg);
    EXPECT_EQ(dst.second.mask, expect_mask.reg);
}

#define MASK_REG_COMPUTE_TEST(Function, DataType)                          \
    TEST_F(tensor_api_vector_mask_reg_compute_3510, Function##_##DataType) \
    {                                                                      \
        test_##Function<DataType>();                                       \
        SUCCEED();                                                         \
    }

#define MASK_REG_COMPUTE_TYPE_TESTS(DataType)    \
    MASK_REG_COMPUTE_TEST(make_mask, DataType)   \
    MASK_REG_COMPUTE_TEST(all_mask, DataType)    \
    MASK_REG_COMPUTE_TEST(none_mask, DataType)   \
    MASK_REG_COMPUTE_TEST(update_mask, DataType) \
    MASK_REG_COMPUTE_TEST(interleave, DataType)  \
    MASK_REG_COMPUTE_TEST(deinterleave, DataType)

MASK_REG_COMPUTE_TYPE_TESTS(uint8_t)
MASK_REG_COMPUTE_TYPE_TESTS(uint16_t)
MASK_REG_COMPUTE_TYPE_TESTS(uint32_t)
