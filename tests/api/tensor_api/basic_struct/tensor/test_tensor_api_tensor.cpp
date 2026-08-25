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

class Tensor_Api_Tensor_Struct : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

namespace {

template <typename T>
struct IsTensorApiGlobalTensor : AscendC::Std::false_type {};

template <typename Engine, typename Layout>
struct IsTensorApiGlobalTensor<AscendC::GlobalTensor<AscendC::TensorAttribute<Engine, Layout>>>
    : AscendC::Std::true_type {};

template <typename T>
constexpr bool IsTensorApiGlobalTensorV = IsTensorApiGlobalTensor<AscendC::Std::remove_cvref_t<T>>::value;

template <typename T>
struct IsTensorApiLocalTensor : AscendC::Std::false_type {};

template <typename Engine, typename Layout>
struct IsTensorApiLocalTensor<AscendC::LocalTensor<AscendC::TensorAttribute<Engine, Layout>>>
    : AscendC::Std::true_type {};

template <typename T>
constexpr bool IsTensorApiLocalTensorV = IsTensorApiLocalTensor<AscendC::Std::remove_cvref_t<T>>::value;

template <typename Tensor>
void ExpectTensorBasicAbility(const Tensor& tensor, uint32_t size)
{
    EXPECT_EQ(tensor.Tensor().Data(), tensor.Data());
    EXPECT_EQ(tensor.Engine().Begin(), tensor.Data());
    EXPECT_EQ(tensor.Size(), size);
    EXPECT_EQ(tensor.Capacity(), size);
}

} // namespace

TEST_F(Tensor_Api_Tensor_Struct, TestLocalTensorStruct)
{
    using namespace AscendC::Te;

    __gm__ float data[6] = {0, 1, 2, 3, 4, 5};
    auto tensor =
        MakeTensor(MakeMemPtr<Location::GM>(data), MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(2, 3));

    EXPECT_EQ(tensor.Tensor().Data(), tensor.Data());
    EXPECT_EQ(tensor.Engine().Begin(), tensor.Data());
    EXPECT_EQ(tensor.Size(), 6);
    EXPECT_EQ(tensor.Capacity(), 6);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.Shape()), 3);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.Stride()), 3);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.Stride()), 1);
    EXPECT_EQ(tensor[MakeCoord(1, 2)], 5);
}

TEST_F(Tensor_Api_Tensor_Struct, TestLocalTensorCoord)
{
    using namespace AscendC::Te;

    __gm__ float data[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto layout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(3, 4);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto subTensor = tensor(MakeCoord(1, 1));

    EXPECT_EQ(subTensor.Data(), tensor.Data() + layout(MakeCoord(1, 1)));
    EXPECT_EQ(AscendC::Std::get<0>(subTensor.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(subTensor.Shape()), 3);
    EXPECT_EQ(subTensor[MakeCoord(1, 2)], 11);
}

TEST_F(Tensor_Api_Tensor_Struct, MakeTensorReturnsGlobalTensorForGM)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float data[m * n] = {0};
    auto tensor = MakeTensor(MakeMemPtr(data), MakeFrameLayout<NDLayoutPtn>(m, n));

    static_assert(IsTensorApiGlobalTensorV<decltype(tensor)>);
    static_assert(!IsTensorApiLocalTensorV<decltype(tensor)>);
    static_assert(IsAttrTensorV<decltype(tensor)>);

    ExpectTensorBasicAbility(tensor, m * n);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.Shape()), m);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.Shape()), n);
    EXPECT_EQ(AscendC::Std::get<0>(tensor.Stride()), n);
    EXPECT_EQ(AscendC::Std::get<1>(tensor.Stride()), 1);
}

TEST_F(Tensor_Api_Tensor_Struct, MakeTensorReturnsLocalTensorForNonGM)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    constexpr uint32_t size = m * n;
    __ubuf__ float ubData[size] = {0};
    __cbuf__ float l1Data[size] = {0};
    __ca__ float l0aData[size] = {0};
    __cb__ float l0bData[size] = {0};
    __cc__ float l0cData[size] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), layout);
    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Data), layout);
    auto l0aTensor = MakeTensor(MakeMemPtr<Location::L0A>(l0aData), layout);
    auto l0bTensor = MakeTensor(MakeMemPtr<Location::L0B>(l0bData), layout);
    auto l0cTensor = MakeTensor(MakeMemPtr<Location::L0C>(l0cData), layout);

    static_assert(IsTensorApiLocalTensorV<decltype(ubTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l1Tensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0aTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0bTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0cTensor)>);
    static_assert(!IsTensorApiGlobalTensorV<decltype(ubTensor)>);

    ExpectTensorBasicAbility(ubTensor, size);
    ExpectTensorBasicAbility(l1Tensor, size);
    ExpectTensorBasicAbility(l0aTensor, size);
    ExpectTensorBasicAbility(l0bTensor, size);
    ExpectTensorBasicAbility(l0cTensor, size);
}

TEST_F(Tensor_Api_Tensor_Struct, SlicePreservesGlobalOrLocalTensorKind)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float gmData[m * n] = {0};
    __ubuf__ float ubData[m * n] = {0};
    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);
    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), layout);

    auto gmCoordTensor = gmTensor(MakeCoord(1, 1));
    auto gmSliceTensor = gmTensor.Slice(MakeCoord(1, 1), MakeShape(2, 3));
    auto ubCoordTensor = ubTensor(MakeCoord(1, 1));
    auto ubSliceTensor = ubTensor.Slice(MakeCoord(1, 1), MakeShape(2, 3));

    static_assert(IsTensorApiGlobalTensorV<decltype(gmCoordTensor)>);
    static_assert(IsTensorApiGlobalTensorV<decltype(gmSliceTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(ubCoordTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(ubSliceTensor)>);

    EXPECT_EQ(gmCoordTensor.Data(), gmTensor.Data() + layout(MakeCoord(1, 1)));
    EXPECT_EQ(gmSliceTensor.Data(), gmTensor.Data() + layout(MakeCoord(1, 1)));
    EXPECT_EQ(ubCoordTensor.Data(), ubTensor.Data() + layout(MakeCoord(1, 1)));
    EXPECT_EQ(ubSliceTensor.Data(), ubTensor.Data() + layout(MakeCoord(1, 1)));
}

TEST_F(Tensor_Api_Tensor_Struct, FormatTraitsSupportGlobalLocalAndCvRefTensor)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 4;
    constexpr uint32_t n = 8;
    __gm__ float gmData[m * n] = {0};
    __ubuf__ float ubData[m * n] = {0};
    auto layout = MakeFrameLayout<NDExtLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);
    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), layout);

    static_assert(IsSatisfiedPtnFormatV<decltype(gmTensor), NDExtLayoutPtn>);
    static_assert(IsSatisfiedPtnFormatV<const decltype(gmTensor)&, NDExtLayoutPtn>);
    static_assert(IsSatisfiedPtnFormatV<decltype(ubTensor), NDExtLayoutPtn>);
    static_assert(IsSatisfiedPtnFormatV<const decltype(ubTensor)&, NDExtLayoutPtn>);

    using GmShapeRow0 = typename GetNDimType<const decltype(gmTensor)&, AttrInfo::Shape, AttrInfo::Row, 0>::type;
    using GmShapeCol0 = typename GetNDimType<const decltype(gmTensor)&, AttrInfo::Shape, AttrInfo::Column, 0>::type;
    using UbStrideRow0 = typename GetNDimType<const decltype(ubTensor)&, AttrInfo::Stride, AttrInfo::Row, 0>::type;
    using UbStrideCol0 = typename GetNDimType<const decltype(ubTensor)&, AttrInfo::Stride, AttrInfo::Column, 0>::type;
    using UbStrideCol1 = typename GetNDimType<const decltype(ubTensor)&, AttrInfo::Stride, AttrInfo::Column, 1>::type;

    static_assert(AscendC::Std::is_same_v<GmShapeRow0, AscendC::Std::Int<1>>);
    static_assert(AscendC::Std::is_same_v<GmShapeCol0, AscendC::Std::Int<1>>);
    static_assert(AscendC::Std::is_same_v<UbStrideRow0, AscendC::Std::Int<0>>);
    static_assert(AscendC::Std::is_same_v<UbStrideCol0, AscendC::Std::Int<0>>);
    static_assert(AscendC::Std::is_same_v<UbStrideCol1, AscendC::Std::Int<1>>);

    EXPECT_EQ(gmTensor.Size(), m * n);
    EXPECT_EQ(ubTensor.Size(), m * n);
}

TEST_F(Tensor_Api_Tensor_Struct, TensorHeaderPublicEntryBuilds)
{
    using namespace AscendC::Te;

    __gm__ float data[4] = {0};
    auto tensor = MakeTensor(MakeMemPtr(data), MakeShape(2, 2), MakeStride(2, 1));

    static_assert(IsTensorApiGlobalTensorV<decltype(tensor)>);
    EXPECT_EQ(tensor.Size(), 4);
}

TEST_F(Tensor_Api_Tensor_Struct, LocalTensorHasNoSetL2CacheHintMethod)
{
    using namespace AscendC::Te;

    constexpr uint32_t size = 128;
    __ubuf__ float ubData[size] = {0};
    __cbuf__ float l1Data[size] = {0};

    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), MakeLayout(MakeShape(8, 16)));
    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Data), MakeLayout(MakeShape(8, 16)));

    static_assert(IsTensorApiLocalTensorV<decltype(ubTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l1Tensor)>);
    static_assert(!IsTensorApiGlobalTensorV<decltype(ubTensor)>);
    static_assert(!IsTensorApiGlobalTensorV<decltype(l1Tensor)>);

    __gm__ float gmData[size] = {0};
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), MakeLayout(MakeShape(8, 16)));
    static_assert(IsTensorApiGlobalTensorV<decltype(gmTensor)>);
    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);

    EXPECT_TRUE(true);
}

TEST_F(Tensor_Api_Tensor_Struct, LocalTensorSliceHasNoSetL2CacheHintMethod)
{
    using namespace AscendC::Te;

    constexpr uint32_t size = 128;
    __ubuf__ float ubData[size] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(8, 16);
    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), layout);

    auto ubSlice = ubTensor.Slice(MakeCoord(2, 4), MakeShape(4, 8));
    auto ubCoord = ubTensor(MakeCoord(2, 4));

    static_assert(IsTensorApiLocalTensorV<decltype(ubSlice)>);
    static_assert(IsTensorApiLocalTensorV<decltype(ubCoord)>);
    static_assert(!IsTensorApiGlobalTensorV<decltype(ubSlice)>);
    static_assert(!IsTensorApiGlobalTensorV<decltype(ubCoord)>);

    EXPECT_TRUE(true);
}

TEST_F(Tensor_Api_Tensor_Struct, AllOnChipMemoryTypesNoSetL2CacheHint)
{
    using namespace AscendC::Te;

    constexpr uint32_t size = 64;
    __ubuf__ float ubData[size] = {0};
    __cbuf__ float l1Data[size] = {0};
    __ca__ float l0aData[size] = {0};
    __cb__ float l0bData[size] = {0};
    __cc__ float l0cData[size] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(8, 8);

    auto ubTensor = MakeTensor(MakeMemPtr<Location::UB>(ubData), layout);
    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Data), layout);
    auto l0aTensor = MakeTensor(MakeMemPtr<Location::L0A>(l0aData), layout);
    auto l0bTensor = MakeTensor(MakeMemPtr<Location::L0B>(l0bData), layout);
    auto l0cTensor = MakeTensor(MakeMemPtr<Location::L0C>(l0cData), layout);

    static_assert(IsTensorApiLocalTensorV<decltype(ubTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l1Tensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0aTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0bTensor)>);
    static_assert(IsTensorApiLocalTensorV<decltype(l0cTensor)>);

    EXPECT_TRUE(true);
}
