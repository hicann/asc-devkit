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

class tensor_api_tensor_slice : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_by_shape)
{
    using namespace asc::te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = make_frame_layout<scalea_nd_layout_ptn>(6, 8);
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto sliced = slice(tensor, make_coord(1, 2), make_shape(3, 3));

    EXPECT_EQ(sliced.data(), tensor.data() + layout(make_coord(1, 2)));
    EXPECT_EQ(sliced[make_coord(make_coord(0, 0), make_coord(0, 0))], 10);
    EXPECT_EQ(sliced[make_coord(make_coord(0, 2), make_coord(0, 1))], 27);
}

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_by_layout)
{
    using namespace asc::te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = make_frame_layout<scalea_nd_layout_ptn>(6, 8);
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto info_layout = make_frame_layout<scalea_nd_layout_ptn>(2, 4);
    auto sliced = slice(tensor, make_coord(make_coord(0, 1), make_coord(0, 2)), info_layout);

    EXPECT_EQ(sliced[make_coord(make_coord(0, 0), make_coord(0, 0))], 10);
    EXPECT_EQ(sliced[make_coord(make_coord(0, 1), make_coord(0, 3))], 21);
}

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_three_dim_layout_by_shape)
{
    using namespace asc::te;

    __gm__ float data[48] = {};
    for (int32_t i = 0; i < 48; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto layout = make_layout(make_shape(2, make_shape(6, 4)), make_stride(24, make_stride(4, 1)));
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto coord = make_coord(0, make_coord(2, 1));
    auto sliced = slice(tensor, coord, make_shape(2, make_shape(3, 2)));

    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ(AscendC::Std::get<0>(sliced.shape()), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.shape())), 3);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.shape())), 2);
    EXPECT_EQ(sliced[make_coord(0, make_coord(0, 0))], data[layout(make_coord(0, make_coord(2, 1)))]);
    EXPECT_EQ(sliced[make_coord(1, make_coord(2, 1))], data[layout(make_coord(1, make_coord(4, 2)))]);
}

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_five_dim_layout_by_shape)
{
    using namespace asc::te;

    __gm__ float data[240] = {};
    for (int32_t i = 0; i < 240; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto layout = make_layout(
        make_shape(2, make_shape(make_shape(2, 3), make_shape(4, 5))),
        make_stride(120, make_stride(make_stride(1, 8), make_stride(2, 24))));
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto coord = make_coord(0, make_coord(2, 4));
    auto sliced = slice(tensor, coord, make_shape(2, make_shape(4, 12)));

    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ(AscendC::Std::get<0>(sliced.shape()), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.shape()))), 2);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.shape()))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.shape()))), 4);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.shape()))), 3);
    EXPECT_EQ(sliced[make_coord(0, make_coord(0, 0))], data[layout(make_coord(0, make_coord(2, 4)))]);
    EXPECT_EQ(sliced[make_coord(1, make_coord(3, 11))], data[layout(make_coord(1, make_coord(5, 15)))]);
}

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_four_dim_layout_by_same_shape)
{
    using namespace asc::te;

    __gm__ float data[120] = {};
    for (int32_t i = 0; i < 120; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto shape = make_shape(make_shape(2, 4), make_shape(3, 5));
    auto stride = make_stride(make_stride(1, 6), make_stride(2, 24));
    auto layout = make_pattern_layout<nz_layout_ptn, layout_trait_default<>>(shape, stride);
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto coord = make_coord(make_coord(1, 2), make_coord(1, 3));
    auto slice_shape = make_shape(make_shape(2, 1), make_shape(2, 3));
    auto sliced = slice(tensor, coord, slice_shape);

    using slice_layout = AscendC::Std::remove_cvref_t<decltype(sliced.layout())>;
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<slice_layout>, nz_layout_ptn>);
    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ((get<0, 0>(sliced.shape())), 1);
    EXPECT_EQ((get<0, 1>(sliced.shape())), 1);
    EXPECT_EQ((get<1, 0>(sliced.shape())), 2);
    EXPECT_EQ((get<1, 1>(sliced.shape())), 2);
    EXPECT_EQ(sliced[make_coord(make_coord(0, 0), make_coord(0, 0))], data[layout(coord)]);
    EXPECT_EQ(sliced[make_coord(make_coord(0, 0), make_coord(1, 1))],
        data[layout(make_coord(make_coord(1, 2), make_coord(2, 4)))]);
}

TEST_F(tensor_api_tensor_slice, test_local_tensor_slice_five_dim_layout_by_same_shape)
{
    using namespace asc::te;

    __gm__ float data[240] = {};
    for (int32_t i = 0; i < 240; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto shape = make_shape(2, make_shape(make_shape(2, 4), make_shape(3, 5)));
    auto stride = make_stride(120, make_stride(make_stride(1, 6), make_stride(2, 24)));
    auto layout = make_pattern_layout<nz_layout_ptn, layout_trait_default<>>(shape, stride);
    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);
    auto coord = make_coord(1, make_coord(make_coord(1, 2), make_coord(1, 3)));
    auto slice_shape = make_shape(2, make_shape(make_shape(2, 1), make_shape(2, 3)));
    auto sliced = slice(tensor, coord, slice_shape);

    using slice_layout = AscendC::Std::remove_cvref_t<decltype(sliced.layout())>;
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<slice_layout>, nz_layout_ptn>);
    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ(get<0>(sliced.shape()), 1);
    EXPECT_EQ((get<1, 0, 0>(sliced.shape())), 1);
    EXPECT_EQ((get<1, 0, 1>(sliced.shape())), 1);
    EXPECT_EQ((get<1, 1, 0>(sliced.shape())), 2);
    EXPECT_EQ((get<1, 1, 1>(sliced.shape())), 2);
    EXPECT_EQ(sliced[make_coord(0, make_coord(make_coord(0, 0), make_coord(0, 0)))], data[layout(coord)]);
    EXPECT_EQ(sliced[make_coord(0, make_coord(make_coord(0, 0), make_coord(1, 1)))],
        data[layout(make_coord(1, make_coord(make_coord(1, 2), make_coord(2, 4))))]);
}

// Flat multi-batch layout from make_frame_layout(batch0, batch1, row, col):
//   shape (2, 3, ((16, 2), (16, 2)))
// sliced with the logical form (batch0, batch1, (x, y)): the batch axes are clamped elementwise and
// the trailing (x, y) is refractalized against the layout's inner row/col.
TEST_F(tensor_api_tensor_slice, test_slice_flat_two_batch_layout_by_logical_shape)
{
    using namespace asc::te;

    constexpr int batch0 = 2;
    constexpr int batch1 = 3;
    constexpr int row = 32;
    constexpr int col = 32;

    __gm__ half data[batch0 * batch1 * row * col] = {};
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<half>>(batch0, batch1, row, col);
    static_assert(decltype(layout)::rank_size == 3, "two flat batch axes + fractal block");

    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);

    // coord: batch (1, 1) and logical (row, col) offset (16, 16) inside the block.
    auto coord = make_coord(1, 1, make_coord(16, 16));
    // slice shape: keep 1 of each batch axis, take a logical 16x16 tile of the block.
    auto slice_shape = make_shape(1, 1, make_shape(16, 16));
    auto sliced = slice(tensor, coord, slice_shape);

    using slice_layout = AscendC::Std::remove_cvref_t<decltype(sliced.layout())>;
    static_assert(slice_layout::rank_size == 3, "slice keeps the flat multi-batch rank");
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<slice_layout>, nz_layout_ptn>,
        "Slice must preserve nz_layout_ptn");

    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));

    // Both batch axes clamped to 1.
    EXPECT_EQ(get<0>(sliced.shape()), 1);
    EXPECT_EQ(get<1>(sliced.shape()), 1);

    // Trailing block: logical 16x16 refractalized against inner (16, 16) -> ((16, 1), (16, 1)).
    EXPECT_EQ((get<2, 0, 0>(sliced.shape())), 16);
    EXPECT_EQ((get<2, 0, 1>(sliced.shape())), 1);
    EXPECT_EQ((get<2, 1, 0>(sliced.shape())), 16);
    EXPECT_EQ((get<2, 1, 1>(sliced.shape())), 1);

    // Strides are inherited untouched from the source layout.
    EXPECT_EQ(get<0>(sliced.stride()), get<0>(layout.stride()));
    EXPECT_EQ(get<1>(sliced.stride()), get<1>(layout.stride()));
}

// Same flat-batch slicing with three batch axes, and with the slice shape larger than what remains on
// a batch axis so the elementwise clamp is exercised (batch0 has 3 left from coord 1, ask for 5).
TEST_F(tensor_api_tensor_slice, test_slice_flat_three_batch_layout_clamped)
{
    using namespace asc::te;

    constexpr int batch0 = 4;
    constexpr int batch1 = 2;
    constexpr int batch2 = 2;
    constexpr int row = 16;
    constexpr int col = 16;

    __gm__ half data[batch0 * batch1 * batch2 * row * col] = {};
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<half>>(batch0, batch1, batch2, row, col);
    static_assert(decltype(layout)::rank_size == 4, "three flat batch axes + fractal block");

    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);

    auto coord = make_coord(1, 0, 1, make_coord(0, 0));
    // batch0: 4 - 1 = 3 remain but 5 requested -> clamped to 3. batch1: min(2, 2) = 2. batch2: min(1, 1) = 1.
    auto slice_shape = make_shape(5, 2, 1, make_shape(16, 16));
    auto sliced = slice(tensor, coord, slice_shape);

    using slice_layout = AscendC::Std::remove_cvref_t<decltype(sliced.layout())>;
    static_assert(slice_layout::rank_size == 4);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<slice_layout>, nz_layout_ptn>);

    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ(get<0>(sliced.shape()), 3); // clamped by what remains
    EXPECT_EQ(get<1>(sliced.shape()), 2);
    EXPECT_EQ(get<2>(sliced.shape()), 1);
    EXPECT_EQ((get<3, 0, 0>(sliced.shape())), 16);
    EXPECT_EQ((get<3, 0, 1>(sliced.shape())), 1);
    EXPECT_EQ((get<3, 1, 0>(sliced.shape())), 16);
    EXPECT_EQ((get<3, 1, 1>(sliced.shape())), 1);
}
