/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstddef>

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include "tensor_api/stub/cce_stub.h"

#include "tensor_api/experimental/vector_ops.h"
#include "tensor_api/tensor.h"

namespace {

using asc::te::experimental::cast_layout;
using asc::te::experimental::cast_options;
using asc::te::experimental::cast_round_mode;
using asc::te::experimental::cast_sat_mode;

constexpr cast_options value_initialized_options{};
static_assert(
    value_initialized_options.layout == cast_layout::unknown &&
    value_initialized_options.round == cast_round_mode::unknown &&
    value_initialized_options.sat == cast_sat_mode::unknown);
constexpr cast_options float_to_half_options = {cast_layout::one, cast_round_mode::ceil, cast_sat_mode::sat};
constexpr cast_options half_to_float_options = {cast_layout::zero, cast_round_mode::unknown, cast_sat_mode::unknown};
constexpr cast_options int16_to_float_options = {cast_layout::zero, cast_round_mode::unknown, cast_sat_mode::unknown};
constexpr cast_options int32_to_float_options = {cast_layout::unknown, cast_round_mode::floor, cast_sat_mode::unknown};

template <typename DstType, typename SrcType, cast_round_mode Round, cast_layout... Layouts>
constexpr bool supports_saturated_and_unsaturated_casts()
{
    return (
        (asc::te::experimental::detail::reg_cast_op<
             DstType, SrcType, Layouts, Round, cast_sat_mode::not_sat>::supported &&
         asc::te::experimental::detail::reg_cast_op<DstType, SrcType, Layouts, Round, cast_sat_mode::sat>::supported) &&
        ...);
}

static_assert(
    supports_saturated_and_unsaturated_casts<half, float, cast_round_mode::odd, cast_layout::zero, cast_layout::one>());
static_assert(!asc::te::experimental::detail::reg_cast_op<
              half, float, cast_layout::two, cast_round_mode::odd, cast_sat_mode::not_sat>::supported);
static_assert(!asc::te::experimental::detail::reg_cast_op<
              float, half, cast_layout::zero, cast_round_mode::odd, cast_sat_mode::not_sat>::supported);
static_assert(!asc::te::experimental::detail::reg_cast_op<
              half, float, cast_layout::zero, cast_round_mode::hybrid, cast_sat_mode::not_sat>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              float, half, cast_layout::zero, cast_round_mode::unknown, cast_sat_mode::unknown>::supported);
static_assert(!asc::te::experimental::detail::reg_cast_op<
              float, half, cast_layout::zero, cast_round_mode::rint, cast_sat_mode::not_sat>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              float, int16_t, cast_layout::one, cast_round_mode::unknown, cast_sat_mode::unknown>::supported);
static_assert(!asc::te::experimental::detail::reg_cast_op<
              float, int16_t, cast_layout::one, cast_round_mode::rint, cast_sat_mode::not_sat>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              int32_t, int16_t, cast_layout::zero, cast_round_mode::unknown, cast_sat_mode::unknown>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              float, int32_t, cast_layout::unknown, cast_round_mode::floor, cast_sat_mode::unknown>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              half, float, cast_layout::one, cast_round_mode::ceil, cast_sat_mode::sat>::supported);
static_assert(asc::te::experimental::detail::reg_cast_op<
              uint8_t, int32_t, cast_layout::two, cast_round_mode::unknown, cast_sat_mode::sat>::supported);
static_assert(!asc::te::experimental::detail::reg_cast_op<
              float, half, cast_layout::two, cast_round_mode::unknown, cast_sat_mode::unknown>::supported);

using half_reg = asc::te::experimental::reg_tensor<half>;
static_assert(AscendC::Std::is_same_v<
              decltype(asc::te::experimental::cast<float, half_to_float_options>(AscendC::Std::declval<half_reg>())),
              asc::te::experimental::reg_tensor<float>>);

class tensor_api_reg_cast_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

template <typename To, const cast_options& opt, typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_reg_loop(const SrcTensor src, DstTensor dst, uint32_t data_size, uint32_t column_count)
{
    using From = typename SrcTensor::data_type;
    using src_reg_type = typename asc::te::experimental::reg_tensor<From>::reg_type;

    constexpr uint32_t elements_per_reg = sizeof(src_reg_type) / sizeof(From);
    const auto full_mask = asc::te::experimental::all_mask<From>();
    const uint32_t repeat = data_size / elements_per_reg;

    for (uint32_t i = 0; i < repeat; ++i) {
        const uint32_t offset = i * elements_per_reg;
        const auto coord = asc::te::make_coord(offset / column_count, offset % column_count);
        auto src_reg = asc::te::experimental::load(src, coord);
        src_reg.with_mask(full_mask);

        auto dst_reg = asc::te::experimental::cast<To, opt>(src_reg);
        asc::te::experimental::store(dst, coord, dst_reg);
    }
}

template <typename To, const cast_options& opt, typename From, size_t N>
void cast_one_register(From input_value, To (&dst_data)[N])
{
    using src_reg_type = typename asc::te::experimental::reg_tensor<From>::reg_type;
    using dst_reg_type = typename asc::te::experimental::reg_tensor<To>::reg_type;

    constexpr uint32_t src_elements_per_reg = sizeof(src_reg_type) / sizeof(From);
    constexpr uint32_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(To);
    static_assert(N == dst_elements_per_reg, "destination buffer must hold one register");

    __ubuf__ From src_data[src_elements_per_reg]{};
    for (uint32_t i = 0; i < src_elements_per_reg; ++i) {
        src_data[i] = input_value;
    }

    const auto src_layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn>(1, src_elements_per_reg);
    const auto dst_layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn>(1, dst_elements_per_reg);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(src_data), src_layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(dst_data), dst_layout);
    cast_reg_loop<To, opt>(src, dst, src_elements_per_reg, src_elements_per_reg);
}

template <typename Element, int32_t ExpectedRound>
void round_instruction_stub(
    typename asc::te::experimental::reg_tensor<Element>::reg_type& dst,
    typename asc::te::experimental::reg_tensor<Element>::reg_type src, int32_t round_mode, vector_bool mask,
    int32_t merge_mode)
{
    EXPECT_EQ(round_mode, ExpectedRound);
    EXPECT_EQ(mask, static_cast<vector_bool>(7));
    EXPECT_EQ(merge_mode, static_cast<int32_t>(MODE_ZEROING));
    for (size_t index = 0; index < src.size(); ++index) {
        EXPECT_EQ(static_cast<float>(src[index]), index % 2 == 0 ? 2.5F : -2.5F);
        dst[index] = static_cast<Element>(index % 2 == 0 ? 17.0F : -19.0F);
    }
}

template <cast_round_mode Mode, bool UseDefault, typename Element, int32_t ExpectedRound>
void check_round_mode()
{
    using reg_type = typename asc::te::experimental::reg_tensor<Element>::reg_type;
    constexpr size_t elements_per_reg = sizeof(reg_type) / sizeof(Element);
    SCOPED_TRACE(
        testing::Message() << "Mode=" << static_cast<int>(Mode) << ", element_size=" << sizeof(Element)
                           << ", default=" << UseDefault);
    asc::te::experimental::reg_tensor<Element> src{};
    src.mask = static_cast<vector_bool>(7);
    for (size_t index = 0; index < elements_per_reg; ++index) {
        src.reg[index] = static_cast<Element>(index % 2 == 0 ? 2.5F : -2.5F);
    }

    MOCKER_CPP(vtrc, void(reg_type&, reg_type, int32_t, vector_bool, int32_t))
        .times(1)
        .will(invoke(round_instruction_stub<Element, ExpectedRound>));

    auto dst = [&]() {
        if constexpr (UseDefault) {
            return asc::te::experimental::trunc(src);
        } else {
            return asc::te::experimental::trunc<Mode>(src);
        }
    }();

    EXPECT_EQ(dst.mask, src.mask);
    for (size_t index = 0; index < elements_per_reg; ++index) {
        EXPECT_EQ(static_cast<float>(dst.reg[index]), index % 2 == 0 ? 17.0F : -19.0F);
    }
    GlobalMockObject::verify();
}

template <typename Element>
void check_all_round_modes()
{
    check_round_mode<cast_round_mode::trunc, true, Element, ROUND_Z>();
    check_round_mode<cast_round_mode::trunc, false, Element, ROUND_Z>();
    check_round_mode<cast_round_mode::round, false, Element, ROUND_A>();
    check_round_mode<cast_round_mode::rint, false, Element, ROUND_R>();
    check_round_mode<cast_round_mode::ceil, false, Element, ROUND_C>();
    check_round_mode<cast_round_mode::floor, false, Element, ROUND_F>();
}

TEST_F(tensor_api_reg_cast_3510, trunc_selects_all_supported_round_modes)
{
    check_all_round_modes<float>();
    check_all_round_modes<half>();
    check_all_round_modes<bfloat16_t>();
}

TEST_F(tensor_api_reg_cast_3510, converts_half_to_float_without_rounding_or_saturation)
{
    using dst_reg_type = typename asc::te::experimental::reg_tensor<float>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(float);
    __ubuf__ float dst_data[dst_elements_per_reg]{};

    cast_one_register<float, half_to_float_options>(static_cast<half>(0.0F), dst_data);

    for (float value : dst_data) {
        EXPECT_EQ(value, 0.0F);
    }
}

TEST_F(tensor_api_reg_cast_3510, explicit_policy_converts_float_to_half_with_layout_one_and_saturation)
{
    using dst_reg_type = typename asc::te::experimental::reg_tensor<half>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(half);
    __ubuf__ half dst_data[dst_elements_per_reg]{};

    cast_one_register<half, float_to_half_options>(0.0F, dst_data);

    for (half value : dst_data) {
        const float actual = static_cast<float>(value);
        EXPECT_EQ(actual, 0.0F);
    }
}

TEST_F(tensor_api_reg_cast_3510, converts_int32_to_float)
{
    using dst_reg_type = typename asc::te::experimental::reg_tensor<float>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(float);
    __ubuf__ float dst_data[dst_elements_per_reg]{};

    cast_one_register<float, int32_to_float_options>(0, dst_data);

    for (float value : dst_data) {
        EXPECT_EQ(value, 0.0F);
    }
}

TEST_F(tensor_api_reg_cast_3510, converts_int16_to_float_without_rounding_or_saturation)
{
    using dst_reg_type = typename asc::te::experimental::reg_tensor<float>::reg_type;
    constexpr size_t dst_elements_per_reg = sizeof(dst_reg_type) / sizeof(float);
    __ubuf__ float dst_data[dst_elements_per_reg]{};

    cast_one_register<float, int16_to_float_options>(static_cast<int16_t>(0), dst_data);

    for (float value : dst_data) {
        EXPECT_EQ(value, 0.0F);
    }
}

} // namespace
