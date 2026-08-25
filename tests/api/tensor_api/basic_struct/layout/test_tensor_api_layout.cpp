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
#include "c_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class Tensor_Api_Layout_Struct : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromShapeAndStride)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(2, 3, 4), MakeStride(12, 4, 1));

    EXPECT_EQ(AscendC::Std::get<0>(layout.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Shape()), 3);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Shape()), 4);

    EXPECT_EQ(AscendC::Std::get<0>(layout.Stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Stride()), 1);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromFlatShape)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(2, 3, 4));

    EXPECT_EQ(AscendC::Std::get<0>(layout.Stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Stride()), 1);
    EXPECT_EQ(layout.Size(), 24);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromNestedShape)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(MakeShape(2, 3), MakeShape(4, 5));
    auto layout = MakeLayout(shape);

    auto rowStride = AscendC::Std::get<0>(layout.Stride());
    auto colStride = AscendC::Std::get<1>(layout.Stride());

    EXPECT_EQ(AscendC::Std::get<0>(rowStride), 1);
    EXPECT_EQ(AscendC::Std::get<1>(rowStride), 8);
    EXPECT_EQ(AscendC::Std::get<0>(colStride), 2);
    EXPECT_EQ(AscendC::Std::get<1>(colStride), 24);
}

TEST_F(Tensor_Api_Layout_Struct, TestGetLayoutPatternFromMakeLayoutFractal)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{})),
        MakeStride(
            MakeStride(AscendC::Te::_16{}, AscendC::Te::_256{}), MakeStride(AscendC::Te::_1{}, AscendC::Te::_512{})));
    auto znLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{})),
        MakeStride(
            MakeStride(AscendC::Te::_1{}, AscendC::Te::_512{}), MakeStride(AscendC::Te::_16{}, AscendC::Te::_256{})));
    auto zzLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{})),
        MakeStride(
            MakeStride(AscendC::Te::_16{}, AscendC::Te::_512{}), MakeStride(AscendC::Te::_1{}, AscendC::Te::_256{})));
    auto nnLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_2{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{})),
        MakeStride(
            MakeStride(AscendC::Te::_1{}, AscendC::Te::_32{}), MakeStride(AscendC::Te::_2{}, AscendC::Te::_128{})));
    auto ndExtLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_1{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_1{}, AscendC::Te::_5{})),
        MakeStride(MakeStride(AscendC::Te::_0{}, AscendC::Te::_5{}), MakeStride(AscendC::Te::_0{}, AscendC::Te::_1{})));
    auto dnExtLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_1{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_1{}, AscendC::Te::_5{})),
        MakeStride(MakeStride(AscendC::Te::_0{}, AscendC::Te::_1{}), MakeStride(AscendC::Te::_0{}, AscendC::Te::_4{})));
    auto ndLayout =
        MakeLayout(MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{}), MakeStride(AscendC::Te::_5{}, AscendC::Te::_1{}));
    auto dnLayout =
        MakeLayout(MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{}), MakeStride(AscendC::Te::_1{}, AscendC::Te::_4{}));
    auto batchedNzLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{}))),
        MakeStride(
            AscendC::Std::Int<1536>{}, MakeStride(
                                           MakeStride(AscendC::Te::_16{}, AscendC::Te::_256{}),
                                           MakeStride(AscendC::Te::_1{}, AscendC::Te::_512{}))));
    auto batchedZnLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{}))),
        MakeStride(
            AscendC::Std::Int<1536>{}, MakeStride(
                                           MakeStride(AscendC::Te::_1{}, AscendC::Te::_512{}),
                                           MakeStride(AscendC::Te::_16{}, AscendC::Te::_256{}))));
    auto batchedZzLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_16{}, AscendC::Te::_2{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{}))),
        MakeStride(
            AscendC::Std::Int<1536>{}, MakeStride(
                                           MakeStride(AscendC::Te::_16{}, AscendC::Te::_512{}),
                                           MakeStride(AscendC::Te::_1{}, AscendC::Te::_256{}))));
    auto batchedNnLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_2{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_16{}, AscendC::Te::_3{}))),
        MakeStride(
            AscendC::Std::Int<384>{}, MakeStride(
                                          MakeStride(AscendC::Te::_1{}, AscendC::Te::_32{}),
                                          MakeStride(AscendC::Te::_2{}, AscendC::Te::_128{}))));
    auto batchedNdExtLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_1{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_1{}, AscendC::Te::_5{}))),
        MakeStride(
            AscendC::Std::Int<20>{},
            MakeStride(
                MakeStride(AscendC::Te::_0{}, AscendC::Te::_5{}), MakeStride(AscendC::Te::_0{}, AscendC::Te::_1{}))));
    auto batchedDnExtLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_1{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_1{}, AscendC::Te::_5{}))),
        MakeStride(
            AscendC::Std::Int<20>{},
            MakeStride(
                MakeStride(AscendC::Te::_0{}, AscendC::Te::_1{}), MakeStride(AscendC::Te::_0{}, AscendC::Te::_4{}))));
    auto batchedNdLayout = MakeLayout(
        MakeShape(AscendC::Te::_2{}, MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{})),
        MakeStride(AscendC::Std::Int<20>{}, MakeStride(AscendC::Te::_5{}, AscendC::Te::_1{})));
    auto batchedDnLayout = MakeLayout(
        MakeShape(AscendC::Te::_2{}, MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{})),
        MakeStride(AscendC::Std::Int<20>{}, MakeStride(AscendC::Te::_1{}, AscendC::Te::_4{})));
    auto batchNdUnContinuousLayout = MakeLayout(
        MakeShape(AscendC::Te::_3{}, MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{})),
        MakeStride(AscendC::Te::_5{}, MakeStride(AscendC::Std::Int<15>{}, AscendC::Te::_1{})));
    auto batchDnUnContinuousLayout = MakeLayout(
        MakeShape(AscendC::Te::_3{}, MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{})),
        MakeStride(AscendC::Te::_4{}, MakeStride(AscendC::Te::_1{}, AscendC::Std::Int<12>{})));
    auto invalidFlatLayout =
        MakeLayout(MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{}), MakeStride(AscendC::Te::_4{}, AscendC::Te::_5{}));
    auto invalidFractalLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{}), MakeShape(AscendC::Te::_8{}, AscendC::Te::_9{})),
        MakeStride(MakeStride(AscendC::Te::_2{}, AscendC::Te::_3{}), MakeStride(AscendC::Te::_4{}, AscendC::Te::_5{})));
    auto invalidScaleADNLayout = MakeLayout(
        MakeShape(MakeShape(AscendC::Te::_1{}, AscendC::Te::_4{}), MakeShape(AscendC::Te::_1{}, AscendC::Te::_5{})),
        MakeStride(MakeStride(AscendC::Te::_0{}, AscendC::Te::_2{}), MakeStride(AscendC::Te::_1{}, AscendC::Te::_8{})));
    auto invalidBatchedFractalLayout = MakeLayout(
        MakeShape(
            AscendC::Te::_2{},
            MakeShape(
                MakeShape(AscendC::Te::_4{}, AscendC::Te::_5{}), MakeShape(AscendC::Te::_8{}, AscendC::Te::_9{}))),
        MakeStride(
            AscendC::Std::Int<180>{},
            MakeStride(
                MakeStride(AscendC::Te::_2{}, AscendC::Te::_3{}), MakeStride(AscendC::Te::_4{}, AscendC::Te::_5{}))));

    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(nzLayout)>, NZLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(znLayout)>, ZNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(zzLayout)>, ZZLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(nnLayout)>, NNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(ndExtLayout)>, NDExtLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(dnExtLayout)>, DNExtLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(ndLayout)>, NDLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(dnLayout)>, DNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedNzLayout)>, NZLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedZnLayout)>, ZNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedZzLayout)>, ZZLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedNnLayout)>, NNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedNdExtLayout)>, NDExtLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedDnExtLayout)>, DNExtLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedNdLayout)>, NDLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchedDnLayout)>, DNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchNdUnContinuousLayout)>, NDLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(batchDnUnContinuousLayout)>, DNLayoutPtn>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(invalidFlatLayout)>, AscendC::Std::ignore_t>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(invalidFractalLayout)>, AscendC::Std::ignore_t>);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(invalidScaleADNLayout)>, AscendC::Std::ignore_t>);
    static_assert(
        AscendC::Std::is_same_v<GetLayoutPattern<decltype(invalidBatchedFractalLayout)>, AscendC::Std::ignore_t>);
}

TEST_F(Tensor_Api_Layout_Struct, TestRemoveBatchDimFromThreeDimLayout)
{
    using namespace AscendC::Te;

    auto layout = MakePatternLayout<NDLayoutPtn, LayoutTraitDefault<>>(
        MakeShape(2, MakeShape(8, 16)), MakeStride(128, MakeStride(16, 1)));
    auto noBatchLayout = RemoveBatchDim(layout);

    static_assert(decltype(noBatchLayout)::depth == TWO_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(noBatchLayout)>, NDLayoutPtn>);
    EXPECT_EQ(AscendC::Std::get<0>(noBatchLayout.Shape()), 8);
    EXPECT_EQ(AscendC::Std::get<1>(noBatchLayout.Shape()), 16);
    EXPECT_EQ(AscendC::Std::get<0>(noBatchLayout.Stride()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(noBatchLayout.Stride()), 1);
}

TEST_F(Tensor_Api_Layout_Struct, TestRemoveBatchDimFromFiveDimLayout)
{
    using namespace AscendC::Te;

    auto layout = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(
        MakeShape(2, MakeShape(MakeShape(16, 2), MakeShape(16, 4))),
        MakeStride(2048, MakeStride(MakeStride(16, 256), MakeStride(1, 512))));
    auto noBatchLayout = RemoveBatchDim(layout);
    auto shape = noBatchLayout.Shape();
    auto stride = noBatchLayout.Stride();

    static_assert(decltype(noBatchLayout)::depth == FOUR_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(noBatchLayout)>, NZLayoutPtn>);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(shape)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(shape)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(stride)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(stride)), 256);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(stride)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(stride)), 512);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeTuple)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(2, 3, 4);
    auto stride = MakeStride(12, 4, 1);
    auto tile = MakeTile(8, 16);
    auto coord = MakeCoord(1, 2, 3);

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

TEST_F(Tensor_Api_Layout_Struct, TestMakeIntTuple)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(AscendC::Std::Int<4>{}, AscendC::Std::Int<5>{});
    auto stride = MakeStride(AscendC::Std::Int<5>{}, AscendC::Std::Int<1>{});
    auto coord = MakeCoord(AscendC::Std::Int<1>{}, AscendC::Std::Int<3>{});

    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(stride).value, 5);
    EXPECT_EQ(AscendC::Std::get<1>(stride).value, 1);
    EXPECT_EQ(AscendC::Std::get<0>(coord).value, 1);
    EXPECT_EQ(AscendC::Std::get<1>(coord).value, 3);
}
