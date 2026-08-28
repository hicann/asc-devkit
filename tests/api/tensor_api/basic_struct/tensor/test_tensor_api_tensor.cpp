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
#include "include/tensor_api/tensor.h"

class tensor_api_tensor_struct : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

namespace {

template <typename data_type>
struct is_tensor_api_global_tensor : AscendC::Std::false_type {};

template <typename engine_type, typename layout_type>
struct is_tensor_api_global_tensor<asc::te::global_tensor<engine_type, layout_type>> : AscendC::Std::true_type {};

template <typename data_type>
constexpr bool is_tensor_api_global_tensor_v =
    is_tensor_api_global_tensor<AscendC::Std::remove_cvref_t<data_type>>::value;

template <typename data_type>
struct is_tensor_api_local_tensor : AscendC::Std::false_type {};

template <typename engine_type, typename layout_type>
struct is_tensor_api_local_tensor<asc::te::local_tensor<engine_type, layout_type>> : AscendC::Std::true_type {};

template <typename data_type>
constexpr bool is_tensor_api_local_tensor_v =
    is_tensor_api_local_tensor<AscendC::Std::remove_cvref_t<data_type>>::value;

template <typename Dst, typename Src>
using dispatched_copy_operation =
    typename asc::te::copy_operation_dispatch_map::template get<AscendC::Std::tuple<Dst, Src>>;

template <typename tensor_type>
void expect_tensor_basic_ability(const tensor_type& tensor, uint32_t size)
{
    EXPECT_EQ(tensor.tensor().data(), tensor.data());
    EXPECT_EQ(tensor.engine().begin(), tensor.data());
    EXPECT_EQ(tensor.size(), size);
    EXPECT_EQ(tensor.capacity(), size);
}

} // namespace

TEST_F(tensor_api_tensor_struct, test_local_tensor_struct)
{
    using namespace asc::te;

    __gm__ float data[6] = {0, 1, 2, 3, 4, 5};
    auto tensor = make_tensor(
        make_mem_ptr<location::gm>(data), make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(2, 3));

    EXPECT_EQ(tensor.tensor().data(), tensor.data());
    EXPECT_EQ(tensor.engine().begin(), tensor.data());
    EXPECT_EQ(tensor.size(), 6);
    EXPECT_EQ(tensor.capacity(), 6);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.shape()), 3);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.stride()), 3);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.stride()), 1);
    EXPECT_EQ(tensor[make_coord(1, 2)], 5);
}

TEST_F(tensor_api_tensor_struct, test_local_tensor_coord)
{
    using namespace asc::te;

    __gm__ float data[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto layout = make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(3, 4);
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto sub_tensor = tensor(make_coord(1, 1));

    EXPECT_EQ(sub_tensor.data(), tensor.data() + layout(make_coord(1, 1)));
    EXPECT_EQ(AscendC::Std::get<0>(sub_tensor.shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(sub_tensor.shape()), 3);
    EXPECT_EQ(sub_tensor[make_coord(1, 2)], 11);
}

TEST_F(tensor_api_tensor_struct, make_tensor_returns_global_tensor_for_gm)
{
    using namespace asc::te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float data[m * n] = {0};
    auto tensor = make_tensor(make_mem_ptr(data), make_frame_layout<nd_layout_ptn>(m, n));

    static_assert(is_tensor_api_global_tensor_v<decltype(tensor)>);
    static_assert(!is_tensor_api_local_tensor_v<decltype(tensor)>);
    static_assert(is_attr_tensor_v<decltype(tensor)>);

    expect_tensor_basic_ability(tensor, m * n);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.shape()), m);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.shape()), n);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.stride()), n);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.stride()), 1);
}

TEST_F(tensor_api_tensor_struct, make_tensor_returns_local_tensor_for_non_gm)
{
    using namespace asc::te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    constexpr uint32_t size = m * n;
    __ubuf__ float ub_data[size] = {0};
    __cbuf__ float l1_data[size] = {0};
    __ca__ float l0a_data[size] = {0};
    __cb__ float l0b_data[size] = {0};
    __cc__ float l0c_data[size] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), layout);
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_data), layout);
    auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(l0a_data), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(l0b_data), layout);
    auto l0c_tensor = make_tensor(make_mem_ptr<location::l0c>(l0c_data), layout);

    static_assert(is_tensor_api_local_tensor_v<decltype(ub_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l1_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0a_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0b_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0c_tensor)>);
    static_assert(!is_tensor_api_global_tensor_v<decltype(ub_tensor)>);

    expect_tensor_basic_ability(ub_tensor, size);
    expect_tensor_basic_ability(l1_tensor, size);
    expect_tensor_basic_ability(l0a_tensor, size);
    expect_tensor_basic_ability(l0b_tensor, size);
    expect_tensor_basic_ability(l0c_tensor, size);
}

TEST_F(tensor_api_tensor_struct, slice_preserves_global_or_local_tensor_kind)
{
    using namespace asc::te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float gm_data[m * n] = {0};
    __ubuf__ float ub_data[m * n] = {0};
    auto layout = make_frame_layout<nd_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);
    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), layout);

    auto gm_coord_tensor = gm_tensor(make_coord(1, 1));
    auto gm_slice_tensor = gm_tensor.slice(make_coord(1, 1), make_shape(2, 3));
    auto ub_coord_tensor = ub_tensor(make_coord(1, 1));
    auto ub_slice_tensor = ub_tensor.slice(make_coord(1, 1), make_shape(2, 3));

    static_assert(is_tensor_api_global_tensor_v<decltype(gm_coord_tensor)>);
    static_assert(is_tensor_api_global_tensor_v<decltype(gm_slice_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(ub_coord_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(ub_slice_tensor)>);

    EXPECT_EQ(gm_coord_tensor.data(), gm_tensor.data() + layout(make_coord(1, 1)));
    EXPECT_EQ(gm_slice_tensor.data(), gm_tensor.data() + layout(make_coord(1, 1)));
    EXPECT_EQ(ub_coord_tensor.data(), ub_tensor.data() + layout(make_coord(1, 1)));
    EXPECT_EQ(ub_slice_tensor.data(), ub_tensor.data() + layout(make_coord(1, 1)));
}

TEST_F(tensor_api_tensor_struct, format_traits_support_global_local_and_cv_ref_tensor)
{
    using namespace asc::te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float gm_data[m * n] = {0};
    __ubuf__ float ub_data[m * n] = {0};
    auto layout = make_frame_layout<nd_ext_layout_ptn>(m, n);
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), layout);
    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), layout);

    static_assert(is_satisfied_ptn_format_v<decltype(gm_tensor), nd_ext_layout_ptn>);
    static_assert(is_satisfied_ptn_format_v<const decltype(gm_tensor)&, nd_ext_layout_ptn>);
    static_assert(is_satisfied_ptn_format_v<decltype(ub_tensor), nd_ext_layout_ptn>);
    static_assert(is_satisfied_ptn_format_v<const decltype(ub_tensor)&, nd_ext_layout_ptn>);

    using gm_shape_row0 =
        typename get_n_dim_type<const decltype(gm_tensor)&, attr_info::shape, attr_info::row, 0>::type;
    using gm_shape_col0 =
        typename get_n_dim_type<const decltype(gm_tensor)&, attr_info::shape, attr_info::column, 0>::type;
    using ub_stride_row0 =
        typename get_n_dim_type<const decltype(ub_tensor)&, attr_info::stride, attr_info::row, 0>::type;
    using ub_stride_col0 =
        typename get_n_dim_type<const decltype(ub_tensor)&, attr_info::stride, attr_info::column, 0>::type;
    using ub_stride_col1 =
        typename get_n_dim_type<const decltype(ub_tensor)&, attr_info::stride, attr_info::column, 1>::type;

    static_assert(AscendC::Std::is_same_v<gm_shape_row0, _1>);
    static_assert(AscendC::Std::is_same_v<gm_shape_col0, _1>);
    static_assert(AscendC::Std::is_same_v<ub_stride_row0, _0>);
    static_assert(AscendC::Std::is_same_v<ub_stride_col0, _0>);
    static_assert(AscendC::Std::is_same_v<ub_stride_col1, _1>);

    EXPECT_EQ(gm_tensor.size(), m * n);
    EXPECT_EQ(ub_tensor.size(), m * n);
}

TEST_F(tensor_api_tensor_struct, tensor_header_public_entry_builds)
{
    using namespace asc::te;

    __gm__ float data[4] = {0};
    auto tensor = make_tensor(make_mem_ptr(data), make_shape(2, 2), make_stride(2, 1));

    static_assert(is_tensor_api_global_tensor_v<decltype(tensor)>);
    EXPECT_EQ(tensor.size(), 4);
}

TEST_F(tensor_api_tensor_struct, row_and_column_stride_support_batch_layouts)
{
    using namespace asc::te;

    auto plain_layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(32, 64);
    auto plain_batch_layout =
        make_batch_pattern_layout<nd_ext_layout_ptn, get_layout_trait<decltype(plain_layout)>>(3, plain_layout);
    EXPECT_EQ(get_row_stride(plain_layout), get_row_stride(plain_batch_layout));
    EXPECT_EQ(get_column_stride(plain_layout), get_column_stride(plain_batch_layout));

    auto fractal_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(32, 64);
    auto fractal_batch_layout =
        make_batch_pattern_layout<nz_layout_ptn, get_layout_trait<decltype(fractal_layout)>>(3, fractal_layout);
    EXPECT_EQ(get_row_stride(fractal_layout), get_row_stride(fractal_batch_layout));
    EXPECT_EQ(get_column_stride(fractal_layout), get_column_stride(fractal_batch_layout));
}

TEST_F(tensor_api_tensor_struct, local_tensor_has_no_set_l2_cache_hint_method)
{
    using namespace asc::te;

    constexpr uint32_t size = 128;
    __ubuf__ float ub_data[size] = {0};
    __cbuf__ float l1_data[size] = {0};

    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), make_layout(make_shape(8, 16)));
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_data), make_layout(make_shape(8, 16)));

    static_assert(is_tensor_api_local_tensor_v<decltype(ub_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l1_tensor)>);
    static_assert(!is_tensor_api_global_tensor_v<decltype(ub_tensor)>);
    static_assert(!is_tensor_api_global_tensor_v<decltype(l1_tensor)>);

    __gm__ float gm_data[size] = {0};
    auto gm_tensor = make_tensor(make_mem_ptr(gm_data), make_layout(make_shape(8, 16)));
    static_assert(is_tensor_api_global_tensor_v<decltype(gm_tensor)>);
    gm_tensor.set_l2_cache_hint(cache_mode::disable);

    EXPECT_TRUE(true);
}

TEST_F(tensor_api_tensor_struct, local_tensor_slice_has_no_set_l2_cache_hint_method)
{
    using namespace asc::te;

    constexpr uint32_t size = 128;
    __ubuf__ float ub_data[size] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(8, 16);
    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), layout);

    auto ub_slice = ub_tensor.slice(make_coord(2, 4), make_shape(4, 8));
    auto ub_coord = ub_tensor(make_coord(2, 4));

    static_assert(is_tensor_api_local_tensor_v<decltype(ub_slice)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(ub_coord)>);
    static_assert(!is_tensor_api_global_tensor_v<decltype(ub_slice)>);
    static_assert(!is_tensor_api_global_tensor_v<decltype(ub_coord)>);

    EXPECT_TRUE(true);
}

TEST_F(tensor_api_tensor_struct, all_on_chip_memory_types_no_set_l2_cache_hint)
{
    using namespace asc::te;

    constexpr uint32_t size = 64;
    __ubuf__ float ub_data[size] = {0};
    __cbuf__ float l1_data[size] = {0};
    __ca__ float l0a_data[size] = {0};
    __cb__ float l0b_data[size] = {0};
    __cc__ float l0c_data[size] = {0};

    auto layout = make_frame_layout<nd_layout_ptn>(8, 8);

    auto ub_tensor = make_tensor(make_mem_ptr<location::ub>(ub_data), layout);
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_data), layout);
    auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(l0a_data), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(l0b_data), layout);
    auto l0c_tensor = make_tensor(make_mem_ptr<location::l0c>(l0c_data), layout);

    static_assert(is_tensor_api_local_tensor_v<decltype(ub_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l1_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0a_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0b_tensor)>);
    static_assert(is_tensor_api_local_tensor_v<decltype(l0c_tensor)>);

    EXPECT_TRUE(true);
}

TEST_F(tensor_api_tensor_struct, copy_operation_dispatch)
{
    using namespace asc::te;

    static_assert(Std::is_same_v<dispatched_copy_operation<location::l1, location::gm>, copy_gm_to_l1>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::ub, location::gm>, copy_gm_to_ub>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::gm, location::ub>, copy_ub_to_gm>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l1, location::ub>, copy_ub_to_l1>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::ub, location::ub>, copy_ub_to_ub>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::ub, location::l1>, copy_l1_to_ub>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::bias, location::l1>, copy_l1_to_biastable>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::fixbuf, location::l1>, copy_l1_to_fixbuf>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l0a, location::l1>, copy_l1_to_l0a>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l0b, location::l1>, copy_l1_to_l0b>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l0scalea, location::l1>, copy_l1_to_l0scalea>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l0scaleb, location::l1>, copy_l1_to_l0scaleb>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::gm, location::l0c>, copy_l0c_to_gm>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::ub, location::l0c>, copy_l0c_to_ub>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::l1, location::l0c>, copy_l0c_to_l1>);
    static_assert(Std::is_same_v<dispatched_copy_operation<location::gm, location::gm>, Std::ignore_t>);

    EXPECT_TRUE(true);
}
