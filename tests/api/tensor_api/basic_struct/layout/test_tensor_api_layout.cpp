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


class tensor_api_layout_struct : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_layout_struct, test_make_layout_from_shape_and_stride)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(2, 3, 4), make_stride(12, 4, 1));

    EXPECT_EQ(AscendC::Std::get<0>(layout.shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(layout.shape()), 3);
    EXPECT_EQ(AscendC::Std::get<2>(layout.shape()), 4);

    EXPECT_EQ(AscendC::Std::get<0>(layout.stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.stride()), 1);
}

TEST_F(tensor_api_layout_struct, test_make_layout_from_flat_shape)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(2, 3, 4));

    EXPECT_EQ(AscendC::Std::get<0>(layout.stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.stride()), 1);
    EXPECT_EQ(layout.size(), 24);
}

TEST_F(tensor_api_layout_struct, test_make_layout_from_nested_shape)
{
    using namespace asc::te;

    auto shape = make_shape(make_shape(2, 3), make_shape(4, 5));
    auto layout = make_layout(shape);

    auto row_stride = AscendC::Std::get<0>(layout.stride());
    auto col_stride = AscendC::Std::get<1>(layout.stride());

    EXPECT_EQ(AscendC::Std::get<0>(row_stride), 1);
    EXPECT_EQ(AscendC::Std::get<1>(row_stride), 8);
    EXPECT_EQ(AscendC::Std::get<0>(col_stride), 2);
    EXPECT_EQ(AscendC::Std::get<1>(col_stride), 24);
}

TEST_F(tensor_api_layout_struct, test_get_layout_pattern_from_make_layout_fractal)
{
    using namespace asc::te;

    auto nz_layout = make_layout(
        make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{})),
        make_stride(make_stride(_16{}, _256{}),
            make_stride(_1{}, _512{})));
    auto zn_layout = make_layout(
        make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{})),
        make_stride(make_stride(_1{}, _512{}),
            make_stride(_16{}, _256{})));
    auto zz_layout = make_layout(
        make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{})),
        make_stride(make_stride(_16{}, _512{}),
            make_stride(_1{}, _256{})));
    auto nn_layout = make_layout(
        make_shape(make_shape(_2{}, _4{}),
            make_shape(_16{}, _3{})),
        make_stride(make_stride(_1{}, _32{}),
            make_stride(_2{}, _128{})));
    auto nd_ext_layout = make_layout(
        make_shape(make_shape(_1{}, _4{}),
            make_shape(_1{}, _5{})),
        make_stride(make_stride(_0{}, _5{}),
            make_stride(_0{}, _1{})));
    auto dn_ext_layout = make_layout(
        make_shape(make_shape(_1{}, _4{}),
            make_shape(_1{}, _5{})),
        make_stride(make_stride(_0{}, _1{}),
            make_stride(_0{}, _4{})));
    auto nd_layout = make_layout(make_shape(_4{}, _5{}),
        make_stride(_5{}, _1{}));
    auto dn_layout = make_layout(make_shape(_4{}, _5{}),
        make_stride(_1{}, _4{}));
    auto batched_nz_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<1536>{}, make_stride(make_stride(_16{}, _256{}),
            make_stride(_1{}, _512{}))));
    auto batched_zn_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<1536>{}, make_stride(make_stride(_1{}, _512{}),
            make_stride(_16{}, _256{}))));
    auto batched_zz_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_16{}, _2{}),
            make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<1536>{}, make_stride(make_stride(_16{}, _512{}),
            make_stride(_1{}, _256{}))));
    auto batched_nn_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_2{}, _4{}),
            make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<384>{}, make_stride(make_stride(_1{}, _32{}),
            make_stride(_2{}, _128{}))));
    auto batched_nd_ext_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_1{}, _4{}),
            make_shape(_1{}, _5{}))),
        make_stride(AscendC::Std::Int<20>{}, make_stride(make_stride(_0{}, _5{}),
            make_stride(_0{}, _1{}))));
    auto batched_dn_ext_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_1{}, _4{}),
            make_shape(_1{}, _5{}))),
        make_stride(AscendC::Std::Int<20>{}, make_stride(make_stride(_0{}, _1{}),
            make_stride(_0{}, _4{}))));
    auto batched_nd_layout = make_layout(
        make_shape(_2{}, make_shape(_4{}, _5{})),
        make_stride(AscendC::Std::Int<20>{}, make_stride(_5{}, _1{})));
    auto batched_dn_layout = make_layout(
        make_shape(_2{}, make_shape(_4{}, _5{})),
        make_stride(AscendC::Std::Int<20>{}, make_stride(_1{}, _4{})));
    auto batch_nd_un_continuous_layout = make_layout(
        make_shape(_3{}, make_shape(_4{}, _5{})),
        make_stride(_5{}, make_stride(AscendC::Std::Int<15>{}, _1{})));
    auto batch_dn_un_continuous_layout = make_layout(
        make_shape(_3{}, make_shape(_4{}, _5{})),
        make_stride(_4{}, make_stride(_1{}, AscendC::Std::Int<12>{})));
    auto invalid_flat_layout = make_layout(make_shape(_4{}, _5{}),
        make_stride(_4{}, _5{}));
    auto invalid_fractal_layout = make_layout(
        make_shape(make_shape(_4{}, _5{}),
            make_shape(_8{}, _9{})),
        make_stride(make_stride(_2{}, _3{}),
            make_stride(_4{}, _5{})));
    auto invalid_scaleadn_layout = make_layout(
        make_shape(make_shape(_1{}, _4{}),
            make_shape(_1{}, _5{})),
        make_stride(make_stride(_0{}, _2{}),
            make_stride(_1{}, _8{})));
    auto invalid_batched_fractal_layout = make_layout(
        make_shape(_2{}, make_shape(make_shape(_4{}, _5{}),
            make_shape(_8{}, _9{}))),
        make_stride(AscendC::Std::Int<180>{}, make_stride(make_stride(_2{}, _3{}),
            make_stride(_4{}, _5{}))));

    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nz_layout)>, nz_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(zn_layout)>, zn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(zz_layout)>, zz_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nn_layout)>, nn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nd_ext_layout)>, nd_ext_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(dn_ext_layout)>, dn_ext_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(nd_layout)>, nd_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(dn_layout)>, dn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_nz_layout)>, nz_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_zn_layout)>, zn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_zz_layout)>, zz_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_nn_layout)>, nn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_nd_ext_layout)>, nd_ext_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_dn_ext_layout)>, dn_ext_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_nd_layout)>, nd_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batched_dn_layout)>, dn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batch_nd_un_continuous_layout)>, nd_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(batch_dn_un_continuous_layout)>, dn_layout_ptn>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(invalid_flat_layout)>, AscendC::Std::ignore_t>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(invalid_fractal_layout)>, AscendC::Std::ignore_t>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(invalid_scaleadn_layout)>, AscendC::Std::ignore_t>);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(invalid_batched_fractal_layout)>,
        AscendC::Std::ignore_t>);
}

TEST_F(tensor_api_layout_struct, test_remove_batch_dim_from_three_dim_layout)
{
    using namespace asc::te;

    auto layout = make_pattern_layout<nd_layout_ptn, layout_trait_default<>>(
        make_shape(2, make_shape(8, 16)), make_stride(128, make_stride(16, 1)));
    auto no_batch_layout = remove_batch_dim(layout);

    static_assert(decltype(no_batch_layout)::depth == TWO_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(no_batch_layout)>, nd_layout_ptn>);
    EXPECT_EQ(AscendC::Std::get<0>(no_batch_layout.shape()), 8);
    EXPECT_EQ(AscendC::Std::get<1>(no_batch_layout.shape()), 16);
    EXPECT_EQ(AscendC::Std::get<0>(no_batch_layout.stride()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(no_batch_layout.stride()), 1);
}

TEST_F(tensor_api_layout_struct, test_remove_batch_dim_from_five_dim_layout)
{
    using namespace asc::te;

    auto layout = make_pattern_layout<nz_layout_ptn, layout_trait_default<>>(
        make_shape(2, make_shape(make_shape(16, 2), make_shape(16, 4))),
        make_stride(2048, make_stride(make_stride(16, 256), make_stride(1, 512))));
    auto no_batch_layout = remove_batch_dim(layout);
    auto shape = no_batch_layout.shape();
    auto stride = no_batch_layout.stride();

    static_assert(decltype(no_batch_layout)::depth == FOUR_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(no_batch_layout)>, nz_layout_ptn>);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(shape)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(shape)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(stride)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(stride)), 256);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(stride)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(stride)), 512);
}

TEST_F(tensor_api_layout_struct, test_make_tuple)
{
    using namespace asc::te;

    auto shape = make_shape(2, 3, 4);
    auto stride = make_stride(12, 4, 1);
    auto tile = make_tile(8, 16);
    auto coord = make_coord(1, 2, 3);

    EXPECT_EQ(AscendC::Std::get<0>(shape), 2);
    EXPECT_EQ(AscendC::Std::get<1>(shape), 3);
    EXPECT_EQ(AscendC::Std::get<2>(shape), 4);

    EXPECT_EQ(AscendC::Std::get<0>(stride), 12);
    EXPECT_EQ(AscendC::Std::get<1>(stride), 4);
    EXPECT_EQ(AscendC::Std::get<2>(stride), 1);

    EXPECT_EQ(AscendC::Std::get<0>(tile), 8);
    EXPECT_EQ(AscendC::Std::get<1>(tile), 16);

    EXPECT_EQ(AscendC::Std::get<0>(coord), 1);
    EXPECT_EQ(AscendC::Std::get<1>(coord), 2);
    EXPECT_EQ(AscendC::Std::get<2>(coord), 3);
}

TEST_F(tensor_api_layout_struct, test_make_int_tuple)
{
    using namespace asc::te;

    auto shape = make_shape(_4{}, _5{});
    auto stride = make_stride(_5{}, _1{});
    auto coord = make_coord(_1{}, _3{});

    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(stride).value, 5);
    EXPECT_EQ(AscendC::Std::get<1>(stride).value, 1);
    EXPECT_EQ(AscendC::Std::get<0>(coord).value, 1);
    EXPECT_EQ(AscendC::Std::get<1>(coord).value, 3);
}
