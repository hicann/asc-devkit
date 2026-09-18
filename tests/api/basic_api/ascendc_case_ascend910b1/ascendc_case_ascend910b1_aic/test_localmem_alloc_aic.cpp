/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include "kernel_operator.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "mockcpp/mockcpp.hpp"

using namespace AscendC;
using namespace std;

namespace {
int32_t RaiseStubCreateTensor(int32_t i) { return 0; }
} // namespace

class TEST_ALLOC_AIC : public testing::Test {
protected:
    void SetUp() { g_coreType = AscendC::AIC_TYPE; }
    void TearDown()
    {
        ConstDefiner::Instance().allocatorUsed.clear();
        GlobalMockObject::verify();
        g_coreType = AscendC::MIX_TYPE;
    }
};
/* **************************** LocalTensor Operator[] ****************************** */
TEST_F(TEST_ALLOC_AIC, TestCustmizedGen)
{
    uint32_t addr = 128;
    uint32_t tileSize = 32;
    AscendC::LocalTensor<float> tensor1 = AscendC::LocalTensor<float>(AscendC::TPosition::VECIN, addr, tileSize);
    uint32_t tensorSize = tensor1.GetSize();
    uint32_t tensorLen = tensor1.GetLength();
    EXPECT_EQ(tensorLen, tileSize * sizeof(float));
    EXPECT_EQ(tensorSize, tileSize);
}

TEST_F(TEST_ALLOC_AIC, TestCustmizedGenIllegal)
{
    EXPECT_DEATH({ AscendC::LocalTensor<float> tensor(AscendC::TPosition::VECIN, 127, 31); }, "");
}

TEST_F(TEST_ALLOC_AIC, TestCustmizedGenInt4)
{
    constexpr uint32_t tileSize = 64;
    AscendC::LocalTensor<int4b_t> tensor2 = AscendC::LocalTensor<int4b_t>(AscendC::TPosition::VECIN, 128, tileSize);
    uint32_t tensorSize2 = tensor2.GetSize();
    uint32_t tensorLen2 = tensor2.GetLength();
    EXPECT_EQ(tensorLen2, tileSize / 2);
    EXPECT_EQ(tensorSize2, tileSize);
}

TEST_F(TEST_ALLOC_AIC, TestCustmizedGenIllegalInt4Size)
{
    EXPECT_DEATH({ AscendC::LocalTensor<int4b_t> tensor(AscendC::TPosition::VECIN, 128, 16); }, "");
}

template <uint32_t v>
using UIntImm = Std::integral_constant<uint32_t, v>;

TEST_F(TEST_ALLOC_AIC, TestTensorTrait1)
{
    auto shape = AscendC::MakeShape(UIntImm<16>{}, UIntImm<16>{});
    auto stride = AscendC::MakeStride(UIntImm<1>{}, UIntImm<16>{});
    auto layoutMake = AscendC::MakeLayout(shape, stride);
    auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
    uint32_t addr = 128;
    auto tensor1 = AscendC::LocalTensor<decltype(tensorTraitMake)>(addr, layoutMake);

    auto tensorTraitMake2 = AscendC::MakeTensorTrait<int4b_t, AscendC::TPosition::VECIN>(layoutMake);
    auto tensor2 = AscendC::LocalTensor<decltype(tensorTraitMake2)>(addr, layoutMake);

    EXPECT_EQ(AscendC::Std::get<0>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(tensor1.GetSize(), 16 * 16);
    EXPECT_EQ(tensor1.GetLength(), 16 * 16 * sizeof(float));
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
    EXPECT_EQ(tensor2.GetSize(), 16 * 16);
    EXPECT_EQ(tensor2.GetLength(), 16 * 16 / 2);
    EXPECT_EQ(tensor2.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
}

TEST_F(TEST_ALLOC_AIC, TestTensorTrait2)
{
    using shape = AscendC::Shape<AscendC::Std::Int<16>, AscendC::Std::Int<16>>;
    using stride = AscendC::Stride<AscendC::Std::Int<1>, AscendC::Std::Int<16>>;
    AscendC::Layout<shape, stride> layoutMake;
    auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
    uint32_t addr = 128;
    auto tensor1 = AscendC::LocalTensor<decltype(tensorTraitMake)>(addr);

    auto tensorTraitMake2 = AscendC::MakeTensorTrait<int4b_t, AscendC::TPosition::VECIN>(layoutMake);
    auto tensor2 = AscendC::LocalTensor<decltype(tensorTraitMake2)>(addr);

    EXPECT_EQ(AscendC::Std::get<0>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(tensor1.GetSize(), 16 * 16);
    EXPECT_EQ(tensor1.GetLength(), 16 * 16 * sizeof(float));
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
    EXPECT_EQ(tensor2.GetSize(), 16 * 16);
    EXPECT_EQ(tensor2.GetLength(), 16 * 16 / 2);
    EXPECT_EQ(tensor2.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
}

TEST_F(TEST_ALLOC_AIC, TestLocalMemAllocatorTensorTrait1)
{
    using shape = AscendC::Shape<AscendC::Std::Int<16>, AscendC::Std::Int<16>>;
    using stride = AscendC::Stride<AscendC::Std::Int<1>, AscendC::Std::Int<16>>;
    AscendC::Layout<shape, stride> layoutMake;
    auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
    LocalMemAllocator allocator;
    auto tensor1 = allocator.Alloc<decltype(tensorTraitMake)>();

    EXPECT_EQ(AscendC::Std::get<0>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(tensor1.GetSize(), 16 * 16);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
    EXPECT_EQ(allocator.GetCurAddr(), 16 * 16 * sizeof(float));
}

TEST_F(TEST_ALLOC_AIC, TestLocalMemAllocatorTensorTrait2)
{
    auto shape = AscendC::MakeShape(UIntImm<16>{}, UIntImm<16>{});
    auto stride = AscendC::MakeStride(UIntImm<1>{}, UIntImm<16>{});
    auto layoutMake = AscendC::MakeLayout(shape, stride);
    auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
    LocalMemAllocator allocator;
    auto tensor1 = allocator.Alloc<decltype(tensorTraitMake)>(layoutMake);

    EXPECT_EQ(AscendC::Std::get<0>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(tensor1.GetTensorTrait().GetLayout().GetShape()), 16);
    EXPECT_EQ(tensor1.GetSize(), 16 * 16);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECIN));
    EXPECT_EQ(allocator.GetCurAddr(), 16 * 16 * sizeof(float));
}

TEST_F(TEST_ALLOC_AIC, TestLocalMemAllocatorAlloc1)
{
    LocalMemAllocator<Hardware::L1> allocator;
    auto tensor1 = allocator.Alloc<TPosition::A1, float, 1024>();
    auto tensor2 = allocator.Alloc<TPosition::A1, int4b_t, 1024>();

    EXPECT_EQ(tensor1.GetSize(), 1024);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(tensor2.GetSize(), 1024);
    EXPECT_EQ(tensor2.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(allocator.GetCurAddr(), 1024 * sizeof(float) + 1024 / 2);
}

TEST_F(TEST_ALLOC_AIC, TestLocalMemAllocatorAlloc2)
{
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStubCreateTensor));

    LocalMemAllocator<Hardware::L1> allocator;
    auto tensor1 = allocator.Alloc<TPosition::A1, float>(2048);
    EXPECT_EQ(tensor1.GetSize(), 2048);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(allocator.GetCurAddr(), 2048 * sizeof(float));
}

TEST_F(TEST_ALLOC_AIC, TestLocalMemAllocatorDefaultPos)
{
    LocalMemAllocator<Hardware::BIAS> allocatorBias;
    auto tensor11 = allocatorBias.Alloc<float, 32>();
    auto tensor12 = allocatorBias.Alloc<float, 32>();

    LocalMemAllocator<Hardware::FIXBUF> allocatorFB;
    auto tensor13 = allocatorFB.Alloc<float, 64>();
    auto tensor14 = allocatorFB.Alloc<float, 64>();

    EXPECT_EQ(tensor11.GetSize(), 32);
    EXPECT_EQ(tensor12.GetSize(), 32);
    EXPECT_EQ(tensor13.GetSize(), 64);
    EXPECT_EQ(tensor14.GetSize(), 64);
    EXPECT_EQ(tensor11.GetPosition(), static_cast<int32_t>(AscendC::TPosition::C2));
    EXPECT_EQ(tensor12.GetPosition(), static_cast<int32_t>(AscendC::TPosition::C2));
    EXPECT_EQ(tensor13.GetPosition(), static_cast<int32_t>(AscendC::TPosition::C2PIPE2GM));
    EXPECT_EQ(tensor14.GetPosition(), static_cast<int32_t>(AscendC::TPosition::C2PIPE2GM));
    EXPECT_EQ(allocatorBias.GetCurAddr(), 64 * sizeof(float));
    EXPECT_EQ(allocatorFB.GetCurAddr(), 128 * sizeof(float));
}
