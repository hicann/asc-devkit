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

class TEST_ALLOC_AIV : public testing::Test {
protected:
    void SetUp() { g_coreType = AscendC::AIV_TYPE; }
    void TearDown()
    {
        ConstDefiner::Instance().allocatorUsed.clear();
        GlobalMockObject::verify();
        g_coreType = AscendC::MIX_TYPE;
    }
};

TEST_F(TEST_ALLOC_AIV, TestCustmizedGen)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

    uint32_t addr = 128;
    uint32_t tileSize = 32;
    AscendC::LocalTensor<float> tensor1 = AscendC::LocalTensor<float>(AscendC::TPosition::VECIN, addr, tileSize);
    uint32_t tensorSize = tensor1.GetSize();
    uint32_t tensorLen = tensor1.GetLength();
    EXPECT_EQ(tensorLen, tileSize * sizeof(float));
    EXPECT_EQ(tensorSize, tileSize);

    g_coreType = tmp;
}

TEST_F(TEST_ALLOC_AIV, TestCustmizedGenIllegal)
{
    EXPECT_DEATH({ AscendC::LocalTensor<float> tensor(AscendC::TPosition::VECIN, 127, 31); }, "");
}

TEST_F(TEST_ALLOC_AIV, TestCustmizedGenInt4)
{
    constexpr uint32_t tileSize = 64;
    AscendC::LocalTensor<int4b_t> tensor2 = AscendC::LocalTensor<int4b_t>(AscendC::TPosition::VECIN, 128, tileSize);
    uint32_t tensorSize2 = tensor2.GetSize();
    uint32_t tensorLen2 = tensor2.GetLength();
    EXPECT_EQ(tensorLen2, tileSize / 2);
    EXPECT_EQ(tensorSize2, tileSize);
}

TEST_F(TEST_ALLOC_AIV, TestCustmizedGenIllegalInt4Size)
{
    EXPECT_DEATH({ AscendC::LocalTensor<int4b_t> tensor(AscendC::TPosition::VECIN, 128, 16); }, "");
}

template <uint32_t v>
using UIntImm = Std::integral_constant<uint32_t, v>;

TEST_F(TEST_ALLOC_AIV, TestTensorTrait)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

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

    g_coreType = tmp;
}

TEST_F(TEST_ALLOC_AIV, TestLocalMemAllocatorTensorTrait)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

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
    g_coreType = tmp;
}

TEST_F(TEST_ALLOC_AIV, TestLocalMemAllocatorAlloc1)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

    LocalMemAllocator<Hardware::L1> allocator;
    auto tensor1 = allocator.Alloc<TPosition::A1, float, 1024>();
    auto tensor2 = allocator.Alloc<TPosition::A1, int4b_t, 1024>();

    EXPECT_EQ(tensor1.GetSize(), 1024);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(tensor2.GetSize(), 1024);
    EXPECT_EQ(tensor2.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(allocator.GetCurAddr(), 1024 * sizeof(float) + 1024 / 2);
    g_coreType = tmp;
}

TEST_F(TEST_ALLOC_AIV, TestLocalMemAllocatorAlloc2)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStubCreateTensor));

    LocalMemAllocator<Hardware::L1> allocator;
    auto tensor1 = allocator.Alloc<TPosition::A1, float>(2048);
    EXPECT_EQ(tensor1.GetSize(), 2048);
    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(allocator.GetCurAddr(), 2048 * sizeof(float));
    g_coreType = tmp;
}

TEST_F(TEST_ALLOC_AIV, TestLocalMemAllocatorDefaultPos)
{
    LocalMemAllocator<> allocatorUB;
    auto tensor1 = allocatorUB.Alloc<float, 1024>();
    auto tensor2 = allocatorUB.Alloc<int4b_t, 1024>();

    LocalMemAllocator<Hardware::L1> allocatorL1;
    auto tensor3 = allocatorL1.Alloc<float, 1024>();
    auto tensor4 = allocatorL1.Alloc<int4b_t, 1024>();

    LocalMemAllocator<Hardware::L0A> allocatorL0A;
    auto tensor5 = allocatorL0A.Alloc<float, 1024>();
    auto tensor6 = allocatorL0A.Alloc<float, 1024>();

    LocalMemAllocator<Hardware::L0B> allocatorL0B;
    auto tensor7 = allocatorL0B.Alloc<float, 1024>();
    auto tensor8 = allocatorL0B.Alloc<float, 1024>();

    LocalMemAllocator<Hardware::L0C> allocatorL0C;
    auto tensor9 = allocatorL0C.Alloc<float, 1024>();
    auto tensor10 = allocatorL0C.Alloc<float, 1024>();

    EXPECT_EQ(tensor1.GetSize(), 1024);
    EXPECT_EQ(tensor2.GetSize(), 1024);
    EXPECT_EQ(tensor3.GetSize(), 1024);
    EXPECT_EQ(tensor4.GetSize(), 1024);
    EXPECT_EQ(tensor5.GetSize(), 1024);
    EXPECT_EQ(tensor6.GetSize(), 1024);
    EXPECT_EQ(tensor7.GetSize(), 1024);
    EXPECT_EQ(tensor8.GetSize(), 1024);
    EXPECT_EQ(tensor9.GetSize(), 1024);
    EXPECT_EQ(tensor10.GetSize(), 1024);

    EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECCALC));
    EXPECT_EQ(tensor2.GetPosition(), static_cast<int32_t>(AscendC::TPosition::VECCALC));
    EXPECT_EQ(tensor3.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(tensor4.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A1));
    EXPECT_EQ(tensor5.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A2));
    EXPECT_EQ(tensor6.GetPosition(), static_cast<int32_t>(AscendC::TPosition::A2));
    EXPECT_EQ(tensor7.GetPosition(), static_cast<int32_t>(AscendC::TPosition::B2));
    EXPECT_EQ(tensor8.GetPosition(), static_cast<int32_t>(AscendC::TPosition::B2));
    EXPECT_EQ(tensor9.GetPosition(), static_cast<int32_t>(AscendC::TPosition::CO1));
    EXPECT_EQ(tensor10.GetPosition(), static_cast<int32_t>(AscendC::TPosition::CO1));

    EXPECT_EQ(allocatorUB.GetCurAddr(), 1024 * sizeof(float) + 512);
    EXPECT_EQ(allocatorL1.GetCurAddr(), 1024 * sizeof(float) + 512);
    EXPECT_EQ(allocatorL0A.GetCurAddr(), 2048 * sizeof(float));
    EXPECT_EQ(allocatorL0B.GetCurAddr(), 2048 * sizeof(float));
    EXPECT_EQ(allocatorL0C.GetCurAddr(), 2048 * sizeof(float));
}
