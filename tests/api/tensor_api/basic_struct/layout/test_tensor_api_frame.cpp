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

class Tensor_Api_Frame_Layout : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupDefaultTrait)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, BatchFrameLayoutDefaultTrait)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn>(2, 8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn>(2, 8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn>(2, 8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn>(2, 8, 16);
    auto nzLayout = MakeFrameLayout<NZLayoutPtn>(2, 32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn>(2, 32, 64);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride<1>(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride<1>(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride<1>(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride<1>(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndExtLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndExtLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride<1>(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride<1>(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnExtLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnExtLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride<1>(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride<1>(dnExtLayout))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nzLayout)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape<1>(nzLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(nzLayout))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(nzLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(nzLayout))), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetStride<1>(nzLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride<1>(nzLayout))), 256);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetStride<1>(nzLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride<1>(nzLayout))), 512);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(znLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(znLayout)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape<1>(znLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(znLayout))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(znLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(znLayout))), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetStride<1>(znLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride<1>(znLayout))), 1024);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetStride<1>(znLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride<1>(znLayout))), 256);
}

TEST_F(Tensor_Api_Frame_Layout, BatchFrameLayoutTraitForms)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(2, 8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, float>(2, 8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, 8>(2, 8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(2, 8, 16);
    auto nzLayoutWithTrait = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(2, 32, 64);
    auto nzLayoutWithDataType = MakeFrameLayout<NZLayoutPtn, float>(2, 32, 64);
    auto nzLayoutWithCompatTrait = MakeFrameLayout<NZLayoutPtn, LayoutTrait<float, _8>>(2, 32, 64);
    auto znLayoutWithC0 = MakeFrameLayout<ZNLayoutPtn, 8>(2, 32, 64);
    using NzTraitWithDefault = GetLayoutTrait<decltype(nzLayoutWithTrait)>;
    using NzTraitWithDataType = GetLayoutTrait<decltype(nzLayoutWithDataType)>;
    using NzCompatTrait = GetLayoutTrait<decltype(nzLayoutWithCompatTrait)>;
    static_assert(NzTraitWithDefault::C0_ELEMENT == _8{}, "LayoutTraitDefault<T> should only be used to infer C0.");
    static_assert(NzTraitWithDataType::C0_ELEMENT == _8{}, "Data type argument should only be used to infer C0.");
    static_assert(
        AscendC::Std::is_same_v<NzCompatTrait, LayoutTrait<float, _8>>,
        "LayoutTrait<T, C0> should remain source-compatible.");
    static_assert(NzCompatTrait::C0_ELEMENT == _8{}, "LayoutTrait<T, C0> should use C0 as layout trait.");

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndExtLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndExtLayout)), 128);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnExtLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnExtLayout)), 128);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nzLayoutWithTrait)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nzLayoutWithTrait)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithTrait))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithTrait))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nzLayoutWithDataType)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nzLayoutWithDataType)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithDataType))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithDataType))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nzLayoutWithCompatTrait)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nzLayoutWithCompatTrait)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithCompatTrait))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(nzLayoutWithCompatTrait))), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(znLayoutWithC0)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(znLayoutWithC0)), 2048);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape<1>(znLayoutWithC0))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(znLayoutWithC0))), 4);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupTraitWithType)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, AscendC::Std::Int<16>>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, AscendC::Std::Int<16>>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, AscendC::Std::Int<16>>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, AscendC::Std::Int<16>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, ScaleGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto andLayout = MakeFrameLayout<ScaleANDLayoutPtn>(32, 16);
    auto adnLayout = MakeFrameLayout<ScaleADNLayoutPtn>(32, 16);
    auto bndLayout = MakeFrameLayout<ScaleBNDLayoutPtn>(16, 32);
    auto bdnLayout = MakeFrameLayout<ScaleBDNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(andLayout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(andLayout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(adnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(adnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(bndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bndLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bdnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(bdnLayout)), 32);
}

TEST_F(Tensor_Api_Frame_Layout, ScaleGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto andLayout = MakeFrameLayout<ScaleANDLayoutPtn>(32, 16);
    auto adnLayout = MakeFrameLayout<ScaleADNLayoutPtn>(32, 16);
    auto bndLayout = MakeFrameLayout<ScaleBNDLayoutPtn>(16, 32);
    auto bdnLayout = MakeFrameLayout<ScaleBDNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(andLayout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(andLayout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(adnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(adnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(bndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bndLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bdnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(bdnLayout)), 32);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupDefaultTrait)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, LayoutTrait<AscendC::Std::ignore_t, _2>>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupTraitWithType)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupWithDataType)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, float>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, float>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, float>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupWithC0Element)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, 8>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, 8>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, 8>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, 2>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, AscendC::Std::Int<16>>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, AscendC::Std::Int<16>>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, AscendC::Std::Int<16>>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, AscendC::Std::Int<2>>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, ScaleAndZzNnBatchFrameLayout)
{
    using namespace AscendC::Te;

    constexpr int batch = 2;
    auto zz = MakeFrameLayout<ZZLayoutPtn, 2>(batch, 32, 64);
    auto nn = MakeFrameLayout<NNLayoutPtn, 2>(batch, 16, 32);
    auto sand = MakeFrameLayout<ScaleANDLayoutPtn, 2>(batch, 32, 16);
    auto sadn = MakeFrameLayout<ScaleADNLayoutPtn, 2>(batch, 32, 16);
    auto sbnd = MakeFrameLayout<ScaleBNDLayoutPtn, 2>(batch, 16, 32);
    auto sbdn = MakeFrameLayout<ScaleBDNLayoutPtn, 2>(batch, 16, 32);

    auto zzBase = MakeFrameLayout<ZZLayoutPtn, 2>(32, 64);
    auto nnBase = MakeFrameLayout<NNLayoutPtn, 2>(16, 32);
    auto sandBase = MakeFrameLayout<ScaleANDLayoutPtn, 2>(32, 16);
    auto sadnBase = MakeFrameLayout<ScaleADNLayoutPtn, 2>(32, 16);
    auto sbndBase = MakeFrameLayout<ScaleBNDLayoutPtn, 2>(16, 32);
    auto sbdnBase = MakeFrameLayout<ScaleBDNLayoutPtn, 2>(16, 32);

    // Batch dim shape == batch, batch stride == single-matrix Capacity, for every pattern.
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(zz)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(zz)), Capacity(zzBase));
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nn)), Capacity(nnBase));
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(sand)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(sand)), Capacity(sandBase));
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(sadn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(sadn)), Capacity(sadnBase));
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(sbnd)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(sbnd)), Capacity(sbndBase));
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(sbdn)), batch);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(sbdn)), Capacity(sbdnBase));

    // Inner fractal shape unchanged vs the non-batch ZZ(32,64,c0=2) layout: ((16,2),(2,32)).
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape<1>(zz))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape<1>(zz))), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape<1>(zz))), 2);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape<1>(zz))), 32);
}

TEST_F(Tensor_Api_Frame_Layout, ConvFeatureMapLayouts)
{
    using namespace AscendC::Te;

    // NCHW(N,C,H,W): row-major contiguous stride (C*H*W, H*W, W, 1).
    auto nchw = MakeFrameLayout<NCHWLayoutPtn>(2, 3, 4, 5);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nchw)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(nchw)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(GetShape(nchw)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(GetShape(nchw)), 5);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nchw)), 60);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(nchw)), 20);
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(nchw)), 5);
    EXPECT_EQ(AscendC::Std::get<3>(GetStride(nchw)), 1);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(nchw)>, NCHWLayoutPtn>);

    // NHWC(N,H,W,C): row-major contiguous stride (H*W*C, W*C, C, 1).
    auto nhwc = MakeFrameLayout<NHWCLayoutPtn>(2, 4, 5, 3);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nhwc)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(nhwc)), 4);
    EXPECT_EQ(AscendC::Std::get<2>(GetShape(nhwc)), 5);
    EXPECT_EQ(AscendC::Std::get<3>(GetShape(nhwc)), 3);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nhwc)), 60);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(nhwc)), 15);
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(nhwc)), 3);
    EXPECT_EQ(AscendC::Std::get<3>(GetStride(nhwc)), 1);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(nhwc)>, NHWCLayoutPtn>);

    // NC1HWC0(N,C1,H,W,C0): C0 supplied by caller, row-major contiguous stride.
    auto nc1hwc0 = MakeFrameLayout<NC1HWC0LayoutPtn>(2, 3, 4, 5, 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(nc1hwc0)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(nc1hwc0)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(GetShape(nc1hwc0)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(GetShape(nc1hwc0)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(GetShape(nc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(nc1hwc0)), 960);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(nc1hwc0)), 320);
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(nc1hwc0)), 80);
    EXPECT_EQ(AscendC::Std::get<3>(GetStride(nc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<4>(GetStride(nc1hwc0)), 1);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(nc1hwc0)>, NC1HWC0LayoutPtn>);
}

TEST_F(Tensor_Api_Frame_Layout, Conv3DFeatureMapLayouts)
{
    using namespace AscendC::Te;

    // NCDHW(N,C,D,H,W): row-major contiguous stride (C*D*H*W, D*H*W, H*W, W, 1).
    auto ncdhw = MakeFrameLayout<NCDHWLayoutPtn>(2, 3, 4, 5, 6);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ncdhw)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ncdhw)), 3);
    EXPECT_EQ(AscendC::Std::get<2>(GetShape(ncdhw)), 4);
    EXPECT_EQ(AscendC::Std::get<3>(GetShape(ncdhw)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(GetShape(ncdhw)), 6);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ncdhw)), 360); // 3*4*5*6
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ncdhw)), 120); // 4*5*6
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(ncdhw)), 30);  // 5*6
    EXPECT_EQ(AscendC::Std::get<3>(GetStride(ncdhw)), 6);   // 6
    EXPECT_EQ(AscendC::Std::get<4>(GetStride(ncdhw)), 1);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(ncdhw)>, NCDHWLayoutPtn>);

    // NDC1HWC0(N,D,C1,H,W,C0): C0 supplied by caller, row-major contiguous stride.
    auto ndc1hwc0 = MakeFrameLayout<NDC1HWC0LayoutPtn>(2, 4, 3, 5, 6, 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndc1hwc0)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndc1hwc0)), 4);
    EXPECT_EQ(AscendC::Std::get<2>(GetShape(ndc1hwc0)), 3);
    EXPECT_EQ(AscendC::Std::get<3>(GetShape(ndc1hwc0)), 5);
    EXPECT_EQ(AscendC::Std::get<4>(GetShape(ndc1hwc0)), 6);
    EXPECT_EQ(AscendC::Std::get<5>(GetShape(ndc1hwc0)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndc1hwc0)), 5760); // 4*3*5*6*16
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndc1hwc0)), 1440); // 3*5*6*16
    EXPECT_EQ(AscendC::Std::get<2>(GetStride(ndc1hwc0)), 480);  // 5*6*16
    EXPECT_EQ(AscendC::Std::get<3>(GetStride(ndc1hwc0)), 96);   // 6*16
    EXPECT_EQ(AscendC::Std::get<4>(GetStride(ndc1hwc0)), 16);   // 16
    EXPECT_EQ(AscendC::Std::get<5>(GetStride(ndc1hwc0)), 1);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(ndc1hwc0)>, NDC1HWC0LayoutPtn>);
}
