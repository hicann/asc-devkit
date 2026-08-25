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

} // namespace

class Tensor_Api_Tensor_CacheMode : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Tensor_CacheMode, SetL2CacheHint)
{
    using namespace AscendC::Te;

    constexpr uint32_t TILE_LENGTH = 8;
    __gm__ float data[TILE_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7};
    auto ptr = MakeMemPtr<Location::GM>(data);
    auto tensor = MakeTensor(
        ptr, MakeShape(AscendC::Std::Int<2>{}, AscendC::Std::Int<2>{}, AscendC::Std::Int<2>{}),
        MakeStride(AscendC::Std::Int<4>{}, AscendC::Std::Int<2>{}, AscendC::Std::Int<1>{}));

    tensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
    EXPECT_EQ(tensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));

    tensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);
    EXPECT_EQ(tensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_NORMAL));

    tensor.SetL2CacheHint(CacheMode::CACHE_MODE_LAST);
    EXPECT_EQ(tensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_LAST));

    tensor.SetL2CacheHint(CacheMode::CACHE_MODE_PERSISTENT);
    EXPECT_EQ(tensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_PERSISTENT));
}

TEST_F(Tensor_Api_Tensor_CacheMode, SliceCoordTensorCanSetL2CacheHint)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gmData[m * n] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);

    auto coordTensor = gmTensor(MakeCoord(2, 4));

    static_assert(IsTensorApiGlobalTensorV<decltype(coordTensor)>);

    coordTensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
    EXPECT_EQ(coordTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));

    coordTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);
    EXPECT_EQ(coordTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_NORMAL));
}

TEST_F(Tensor_Api_Tensor_CacheMode, SliceTensorCanSetL2CacheHint)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gmData[m * n] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);

    auto sliceTensor = gmTensor.Slice(MakeCoord(2, 4), MakeShape(4, 8));

    static_assert(IsTensorApiGlobalTensorV<decltype(sliceTensor)>);

    sliceTensor.SetL2CacheHint(CacheMode::CACHE_MODE_LAST);
    EXPECT_EQ(sliceTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_LAST));

    sliceTensor.SetL2CacheHint(CacheMode::CACHE_MODE_PERSISTENT);
    EXPECT_EQ(sliceTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_PERSISTENT));
}

TEST_F(Tensor_Api_Tensor_CacheMode, SliceTensorInheritsCacheMode)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gmData[m * n] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);

    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
    EXPECT_EQ(gmTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));

    auto coordTensor = gmTensor(MakeCoord(2, 4));
    EXPECT_EQ(coordTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));

    auto sliceTensor = gmTensor.Slice(MakeCoord(2, 4), MakeShape(4, 8));
    EXPECT_EQ(sliceTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));

    coordTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);
    EXPECT_EQ(coordTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_NORMAL));

    EXPECT_EQ(gmTensor.Engine().GetCacheMode(), static_cast<uint8_t>(CacheMode::CACHE_MODE_DISABLE));
}

TEST_F(Tensor_Api_Tensor_CacheMode, SliceTensorStillGlobalTensor)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 8;
    constexpr uint32_t n = 16;
    __gm__ float gmData[m * n] = {0};

    auto layout = MakeFrameLayout<NDLayoutPtn>(m, n);
    auto gmTensor = MakeTensor(MakeMemPtr(gmData), layout);

    static_assert(IsTensorApiGlobalTensorV<decltype(gmTensor)>);

    auto coordTensor = gmTensor(MakeCoord(2, 4));
    static_assert(IsTensorApiGlobalTensorV<decltype(coordTensor)>);
    static_assert(!IsTensorApiLocalTensorV<decltype(coordTensor)>);

    auto sliceTensor = gmTensor.Slice(MakeCoord(2, 4), MakeShape(4, 8));
    static_assert(IsTensorApiGlobalTensorV<decltype(sliceTensor)>);
    static_assert(!IsTensorApiLocalTensorV<decltype(sliceTensor)>);

    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
    coordTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);
    sliceTensor.SetL2CacheHint(CacheMode::CACHE_MODE_PERSISTENT);

    EXPECT_TRUE(true);
}
