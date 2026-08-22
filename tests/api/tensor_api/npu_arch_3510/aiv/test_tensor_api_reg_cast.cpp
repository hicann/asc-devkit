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

using asc::te::cast_layout;
using asc::te::cast_options;
using asc::te::cast_round_mode;
using asc::te::cast_sat_mode;

constexpr cast_options float_to_half_options = {
    cast_layout::one, cast_round_mode::ceil, cast_sat_mode::sat};
constexpr cast_options int32_to_float_options = {
    cast_layout::zero, cast_round_mode::floor, cast_sat_mode::not_sat};

static_assert(asc::te::detail::reg_cast_op<float, half, cast_layout::zero,
    cast_round_mode::rint, cast_sat_mode::not_sat>::supported);
static_assert(asc::te::detail::reg_cast_op<half, float, cast_layout::one,
    cast_round_mode::ceil, cast_sat_mode::sat>::supported);
static_assert(asc::te::detail::reg_cast_op<uint8_t, int32_t, cast_layout::two,
    cast_round_mode::rint, cast_sat_mode::sat>::supported);
static_assert(!asc::te::detail::reg_cast_op<float, half, cast_layout::two,
    cast_round_mode::rint, cast_sat_mode::not_sat>::supported);

using half_reg = asc::te::reg_tensor<half>;
static_assert(AscendC::Std::is_same_v<decltype(asc::te::cast<float>(AscendC::Std::declval<half_reg>())),
    asc::te::reg_tensor<float>>);

class tensor_api_reg_cast_3510 : public testing::Test {
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

template <typename To, const cast_options& opt, typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_reg_loop(const SrcTensor src, DstTensor dst, uint32_t data_size,
    uint32_t column_count)
{
    using From = typename SrcTensor::data_type;
    using src_reg_type = typename asc::te::reg_tensor<From>::reg_type;

    constexpr uint32_t elements_per_reg = sizeof(src_reg_type) / sizeof(From);
    const auto full_mask = asc::te::all_mask<From>();
    const uint32_t repeat = data_size / elements_per_reg;

    for (uint32_t i = 0; i < repeat; ++i) {
        const uint32_t offset = i * elements_per_reg;
        const auto coord = asc::te::make_coord(offset / column_count, offset % column_count);
        auto src_reg = src.load(coord);
        src_reg.with_mask(full_mask);

        auto dst_reg = asc::te::cast<To, opt>(src_reg);
        dst.store(coord, dst_reg);
    }
}

template <typename To, const cast_options& opt = asc::te::default_cast_ops, typename From, size_t N>
void cast_one_register(From input_value, To (&dst_data)[N])
{
    using src_reg_type = typename asc::te::reg_tensor<From>::reg_type;
    using dst_reg_type = typename asc::te::reg_tensor<To>::reg_type;

    constexpr uint32_t src_elements_per_reg = sizeof(src_reg_type) / sizeof(From);
    constexpr uint32_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(To);
    static_assert(N == dst_elements_per_reg, "destination buffer must hold one register");

    __ubuf__ From src_data[src_elements_per_reg] {};
    for (uint32_t i = 0; i < src_elements_per_reg; ++i) {
        src_data[i] = input_value;
    }

    const auto src_layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn>(1, src_elements_per_reg);
    const auto dst_layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn>(1, dst_elements_per_reg);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src_data), src_layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst_data), dst_layout);
    cast_reg_loop<To, opt>(src, dst, src_elements_per_reg, src_elements_per_reg);
}

TEST_F(tensor_api_reg_cast_3510, default_policy_converts_half_to_float)
{
    using dst_reg_type = typename asc::te::reg_tensor<float>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(float);
    __ubuf__ float dst_data[dst_elements_per_reg] {};

    cast_one_register<float>(static_cast<half>(0.0F), dst_data);

    for (float value : dst_data) {
        EXPECT_EQ(value, 0.0F);
    }
}

TEST_F(tensor_api_reg_cast_3510, explicit_policy_converts_float_to_half_with_layout_one_and_saturation)
{
    using dst_reg_type = typename asc::te::reg_tensor<half>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(half);
    __ubuf__ half dst_data[dst_elements_per_reg] {};

    cast_one_register<half, float_to_half_options>(0.0F, dst_data);

    for (half value : dst_data) {
        const float actual = static_cast<float>(value);
        EXPECT_EQ(actual, 0.0F);
    }
}

TEST_F(tensor_api_reg_cast_3510, converts_int32_to_float)
{
    using dst_reg_type = typename asc::te::reg_tensor<float>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(float);
    __ubuf__ float dst_data[dst_elements_per_reg] {};

    cast_one_register<float, int32_to_float_options>(0, dst_data);

    for (float value : dst_data) {
        EXPECT_EQ(value, 0.0F);
    }
}

} // namespace
