/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This file is a part of the CANN Open Software.
 * Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstddef>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"

#include "tensor_api/tensor.h"

namespace {

template <typename T>
using truncate_result = decltype(asc::te::trunc(AscendC::Std::declval<asc::te::reg_tensor<T>>()));

static_assert(asc::te::detail::is_supported<half>::value);
static_assert(asc::te::detail::is_supported<bfloat16_t>::value);
static_assert(asc::te::detail::is_supported<float>::value);
static_assert(!asc::te::detail::is_supported<int32_t>::value);
static_assert(AscendC::Std::is_same_v<truncate_result<half>, asc::te::reg_tensor<half>>);
static_assert(AscendC::Std::is_same_v<truncate_result<bfloat16_t>, asc::te::reg_tensor<bfloat16_t>>);
static_assert(AscendC::Std::is_same_v<truncate_result<float>, asc::te::reg_tensor<float>>);

class tensor_api_reg_truncate_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override
    {
        AscendC::SetGCoreType(2);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
    }
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void truncate_reg_loop(const SrcTensor src, DstTensor dst, uint32_t data_size,
    uint32_t column_count)
{
    using T = typename SrcTensor::data_type;
    using reg_type = typename asc::te::reg_tensor<T>::reg_type;

    constexpr uint32_t elements_per_reg = sizeof(reg_type) / sizeof(T);
    const auto full_mask = asc::te::all_mask<T>();
    const uint32_t repeat = data_size / elements_per_reg;

    for (uint32_t i = 0; i < repeat; ++i) {
        const uint32_t offset = i * elements_per_reg;
        const auto coord = asc::te::make_coord(offset / column_count, offset % column_count);
        auto src_reg = src.load(coord);
        src_reg.with_mask(full_mask);

        auto dst_reg = asc::te::trunc(src_reg);
        dst.store(coord, dst_reg);
    }
}

template <typename T>
void expect_truncate_result()
{
    using reg_type = typename asc::te::reg_tensor<T>::reg_type;
    constexpr uint32_t elements_per_reg = sizeof(reg_type) / sizeof(T);

    __ubuf__ T src_data[elements_per_reg] {};
    __ubuf__ T dst_data[elements_per_reg] {};
    for (uint32_t i = 0; i < elements_per_reg; ++i) {
        src_data[i] = static_cast<T>(0);
    }

    const auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn>(1, elements_per_reg);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src_data), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst_data), layout);
    truncate_reg_loop(src, dst, elements_per_reg, elements_per_reg);

    for (uint32_t i = 0; i < elements_per_reg; ++i) {
        const float expected = 0.0F;
        EXPECT_FLOAT_EQ(static_cast<float>(dst_data[i]), expected);
    }
}

TEST_F(tensor_api_reg_truncate_3510, truncates_half)
{
    expect_truncate_result<half>();
}

TEST_F(tensor_api_reg_truncate_3510, truncates_bfloat16)
{
    expect_truncate_result<bfloat16_t>();
}

TEST_F(tensor_api_reg_truncate_3510, truncates_float)
{
    expect_truncate_result<float>();
}

} // namespace
