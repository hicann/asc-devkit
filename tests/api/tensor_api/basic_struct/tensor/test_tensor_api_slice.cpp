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

class Tensor_Api_Tensor_Slice : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceByShape)
{
    using namespace AscendC::Te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = MakeFrameLayout<ScaleANDLayoutPtn>(6, 8);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto sliced = Slice(tensor, MakeCoord(1, 2), MakeShape(3, 3));

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(MakeCoord(1, 2)));
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0))], 10);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 2), MakeCoord(0, 1))], 27);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceByLayout)
{
    using namespace AscendC::Te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = MakeFrameLayout<ScaleANDLayoutPtn>(6, 8);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto infoLayout = MakeFrameLayout<ScaleANDLayoutPtn>(2, 4);
    auto sliced = Slice(tensor, MakeCoord(MakeCoord(0, 1), MakeCoord(0, 2)), infoLayout);

    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0))], 10);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 1), MakeCoord(0, 3))], 21);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceThreeDimLayoutByShape)
{
    using namespace AscendC::Te;

    __gm__ float data[48] = {};
    for (int32_t i = 0; i < 48; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto layout = MakeLayout(MakeShape(2, MakeShape(6, 4)), MakeStride(24, MakeStride(4, 1)));
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto coord = MakeCoord(0, MakeCoord(2, 1));
    auto sliced = Slice(tensor, coord, MakeShape(2, MakeShape(3, 2)));

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));
    EXPECT_EQ(AscendC::Std::get<0>(sliced.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.Shape())), 3);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.Shape())), 2);
    EXPECT_EQ(sliced[MakeCoord(0, MakeCoord(0, 0))], data[layout(MakeCoord(0, MakeCoord(2, 1)))]);
    EXPECT_EQ(sliced[MakeCoord(1, MakeCoord(2, 1))], data[layout(MakeCoord(1, MakeCoord(4, 2)))]);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceFiveDimLayoutByShape)
{
    using namespace AscendC::Te;

    __gm__ float data[240] = {};
    for (int32_t i = 0; i < 240; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto layout = MakeLayout(
        MakeShape(2, MakeShape(MakeShape(2, 3), MakeShape(4, 5))),
        MakeStride(120, MakeStride(MakeStride(1, 8), MakeStride(2, 24))));
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto coord = MakeCoord(0, MakeCoord(2, 4));
    auto sliced = Slice(tensor, coord, MakeShape(2, MakeShape(4, 12)));

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));
    EXPECT_EQ(AscendC::Std::get<0>(sliced.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.Shape()))), 2);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(AscendC::Std::get<1>(sliced.Shape()))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.Shape()))), 4);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(AscendC::Std::get<1>(sliced.Shape()))), 3);
    EXPECT_EQ(sliced[MakeCoord(0, MakeCoord(0, 0))], data[layout(MakeCoord(0, MakeCoord(2, 4)))]);
    EXPECT_EQ(sliced[MakeCoord(1, MakeCoord(3, 11))], data[layout(MakeCoord(1, MakeCoord(5, 15)))]);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceFourDimLayoutBySameShape)
{
    using namespace AscendC::Te;

    __gm__ float data[120] = {};
    for (int32_t i = 0; i < 120; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto shape = MakeShape(MakeShape(2, 4), MakeShape(3, 5));
    auto stride = MakeStride(MakeStride(1, 6), MakeStride(2, 24));
    auto layout = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(shape, stride);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto coord = MakeCoord(MakeCoord(1, 2), MakeCoord(1, 3));
    auto sliceShape = MakeShape(MakeShape(2, 1), MakeShape(2, 3));
    auto sliced = Slice(tensor, coord, sliceShape);

    using SliceLayout = AscendC::Std::remove_cvref_t<decltype(sliced.Layout())>;
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<SliceLayout>, NZLayoutPtn>);
    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));
    EXPECT_EQ((Get<0, 0>(sliced.Shape())), 1);
    EXPECT_EQ((Get<0, 1>(sliced.Shape())), 1);
    EXPECT_EQ((Get<1, 0>(sliced.Shape())), 2);
    EXPECT_EQ((Get<1, 1>(sliced.Shape())), 2);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0))], data[layout(coord)]);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(1, 1))],
        data[layout(MakeCoord(MakeCoord(1, 2), MakeCoord(2, 4)))]);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceFiveDimLayoutBySameShape)
{
    using namespace AscendC::Te;

    __gm__ float data[240] = {};
    for (int32_t i = 0; i < 240; ++i) {
        data[i] = static_cast<float>(i);
    }
    auto shape = MakeShape(2, MakeShape(MakeShape(2, 4), MakeShape(3, 5)));
    auto stride = MakeStride(120, MakeStride(MakeStride(1, 6), MakeStride(2, 24)));
    auto layout = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(shape, stride);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto coord = MakeCoord(1, MakeCoord(MakeCoord(1, 2), MakeCoord(1, 3)));
    auto sliceShape = MakeShape(2, MakeShape(MakeShape(2, 1), MakeShape(2, 3)));
    auto sliced = Slice(tensor, coord, sliceShape);

    using SliceLayout = AscendC::Std::remove_cvref_t<decltype(sliced.Layout())>;
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<SliceLayout>, NZLayoutPtn>);
    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));
    EXPECT_EQ(Get<0>(sliced.Shape()), 1);
    EXPECT_EQ((Get<1, 0, 0>(sliced.Shape())), 1);
    EXPECT_EQ((Get<1, 0, 1>(sliced.Shape())), 1);
    EXPECT_EQ((Get<1, 1, 0>(sliced.Shape())), 2);
    EXPECT_EQ((Get<1, 1, 1>(sliced.Shape())), 2);
    EXPECT_EQ(sliced[MakeCoord(0, MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)))], data[layout(coord)]);
    EXPECT_EQ(sliced[MakeCoord(0, MakeCoord(MakeCoord(0, 0), MakeCoord(1, 1)))],
        data[layout(MakeCoord(1, MakeCoord(MakeCoord(1, 2), MakeCoord(2, 4))))]);
}

// Flat multi-batch layout from MakeFrameLayout(batch0, batch1, row, col):
//   shape (2, 3, ((16, 2), (16, 2)))
// sliced with the logical form (batch0, batch1, (x, y)): the batch axes are clamped elementwise and
// the trailing (x, y) is refractalized against the layout's inner row/col.
TEST_F(Tensor_Api_Tensor_Slice, TestSliceFlatTwoBatchLayoutByLogicalShape)
{
    using namespace AscendC::Te;

    constexpr int batch0 = 2;
    constexpr int batch1 = 3;
    constexpr int row = 32;
    constexpr int col = 32;

    __gm__ half data[batch0 * batch1 * row * col] = {};
    auto layout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<half>>(batch0, batch1, row, col);
    static_assert(decltype(layout)::rank == 3, "two flat batch axes + fractal block");

    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);

    // coord: batch (1, 1) and logical (row, col) offset (16, 16) inside the block.
    auto coord = MakeCoord(1, 1, MakeCoord(16, 16));
    // slice shape: keep 1 of each batch axis, take a logical 16x16 tile of the block.
    auto sliceShape = MakeShape(1, 1, MakeShape(16, 16));
    auto sliced = Slice(tensor, coord, sliceShape);

    using SliceLayout = AscendC::Std::remove_cvref_t<decltype(sliced.Layout())>;
    static_assert(SliceLayout::rank == 3, "slice keeps the flat multi-batch rank");
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<SliceLayout>, NZLayoutPtn>,
        "Slice must preserve NZLayoutPtn");

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));

    // Both batch axes clamped to 1.
    EXPECT_EQ(Get<0>(sliced.Shape()), 1);
    EXPECT_EQ(Get<1>(sliced.Shape()), 1);

    // Trailing block: logical 16x16 refractalized against inner (16, 16) -> ((16, 1), (16, 1)).
    EXPECT_EQ((Get<2, 0, 0>(sliced.Shape())), 16);
    EXPECT_EQ((Get<2, 0, 1>(sliced.Shape())), 1);
    EXPECT_EQ((Get<2, 1, 0>(sliced.Shape())), 16);
    EXPECT_EQ((Get<2, 1, 1>(sliced.Shape())), 1);

    // Strides are inherited untouched from the source layout.
    EXPECT_EQ(Get<0>(sliced.Stride()), Get<0>(layout.Stride()));
    EXPECT_EQ(Get<1>(sliced.Stride()), Get<1>(layout.Stride()));
}

// Same flat-batch slicing with three batch axes, and with the slice shape larger than what remains on
// a batch axis so the elementwise clamp is exercised (batch0 has 3 left from coord 1, ask for 5).
TEST_F(Tensor_Api_Tensor_Slice, TestSliceFlatThreeBatchLayoutClamped)
{
    using namespace AscendC::Te;

    constexpr int batch0 = 4;
    constexpr int batch1 = 2;
    constexpr int batch2 = 2;
    constexpr int row = 16;
    constexpr int col = 16;

    __gm__ half data[batch0 * batch1 * batch2 * row * col] = {};
    auto layout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<half>>(batch0, batch1, batch2, row, col);
    static_assert(decltype(layout)::rank == 4, "three flat batch axes + fractal block");

    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);

    auto coord = MakeCoord(1, 0, 1, MakeCoord(0, 0));
    // batch0: 4 - 1 = 3 remain but 5 requested -> clamped to 3. batch1: min(2, 2) = 2. batch2: min(1, 1) = 1.
    auto sliceShape = MakeShape(5, 2, 1, MakeShape(16, 16));
    auto sliced = Slice(tensor, coord, sliceShape);

    using SliceLayout = AscendC::Std::remove_cvref_t<decltype(sliced.Layout())>;
    static_assert(SliceLayout::rank == 4);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<SliceLayout>, NZLayoutPtn>);

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(coord));
    EXPECT_EQ(Get<0>(sliced.Shape()), 3); // clamped by what remains
    EXPECT_EQ(Get<1>(sliced.Shape()), 2);
    EXPECT_EQ(Get<2>(sliced.Shape()), 1);
    EXPECT_EQ((Get<3, 0, 0>(sliced.Shape())), 16);
    EXPECT_EQ((Get<3, 0, 1>(sliced.Shape())), 1);
    EXPECT_EQ((Get<3, 1, 0>(sliced.Shape())), 16);
    EXPECT_EQ((Get<3, 1, 1>(sliced.Shape())), 1);
}
