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
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class Tensor_Api_Cube_Copy_L12L0ScaleA_3510 : public testing::Test {
protected:
    void SetUp() override { AscendC::SetGCoreType(1); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

template <typename LocationTag, typename Pointer, typename Layout>
auto MakeTensorAt(Pointer ptr, const Layout& layout)
{
    return AscendC::Te::MakeTensor(
        AscendC::Te::MakeMemPtr<LocationTag, fp8_e8m0_t>(reinterpret_cast<uint64_t>(ptr) / 16), layout);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyCallPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}, Trait{});
    atom.Call(dst, src);

    CopyAtom<CopyTraits<CopyOp, Trait>>{}.Call(dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}, dst, src);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyWithPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with();
    atom.Call(dst, src);
    Copy(atom, dst, src);
}

uint64_t gExpectedMxDstAddr = 0;
__cbuf__ void* gExpectedMxSrc = nullptr;

// Batched scale L1->L0 now copies one matrix per batch in a loop (see commit "scale batch layout"),
// so the instruction fires once per batch with the single-matrix step values, and the dst/src
// addresses advance by the batch layout's per-batch stride each call. With the compact
// MakeBatchPatternLayout below, that stride is the single-matrix Capacity (ZZ m=16,n=32,c0=2 -> 512),
// applied equally to dst and src. Single-matrix mStep(xStep)=ceil(m/16)=1, yStep=ceil(n/2)=16.
constexpr uint32_t kScaleABatch = 3;
constexpr uint32_t kScaleAM = 16;
constexpr uint32_t kScaleAN = 32;
constexpr uint32_t kScaleABatchStrideElem = 512;
uint32_t gScaleACallIdx = 0;

void load_cbuf_to_ca_mx_batch_stub(
    uint64_t dst, __cbuf__ void* src, uint16_t xStartPos, uint16_t yStartPos, uint8_t xStep, uint8_t yStep,
    uint16_t srcStride, uint16_t dstStride)
{
    EXPECT_EQ(dst, gExpectedMxDstAddr + gScaleACallIdx * kScaleABatchStrideElem);
    EXPECT_EQ(
        src, reinterpret_cast<__cbuf__ void*>(
                 reinterpret_cast<fp8_e8m0_t*>(gExpectedMxSrc) + gScaleACallIdx * kScaleABatchStrideElem));
    EXPECT_EQ(xStartPos, 0);
    EXPECT_EQ(yStartPos, 0);
    EXPECT_EQ(xStep, 1);
    EXPECT_EQ(yStep, 16);
    EXPECT_EQ(srcStride, 16);
    EXPECT_EQ(dstStride, 16);
    ++gScaleACallIdx;
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_L12L0ScaleA_3510, CopyL12L0ScaleARoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 16;
    constexpr uint32_t n = 32;
    __cbuf__ fp8_e8m0_t src[m * n] = {0};
    __ca__ fp8_e8m0_t dst[m * n] = {0};

    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(src), MakeFrameLayout<ZZLayoutPtn, AscendC::Std::Int<2>>(m, n));
    auto dstPtr = MakeMemPtr<Location::L0ScaleA, fp8_e8m0_t>((reinterpret_cast<uint64_t>(dst)) / 16);
    auto l0aTensor = MakeTensor(dstPtr, MakeFrameLayout<ZZLayoutPtn, AscendC::Std::Int<2>>(m, n));

    RunCopyCallPaths<CopyL12L0ScaleA, CopyL12L0ScaleATraitDefault>(l0aTensor, l1Tensor);
    RunCopyWithPaths<CopyL12L0ScaleA, CopyL12L0ScaleATraitDefault>(l0aTensor, l1Tensor);

    EXPECT_EQ(dst[0], static_cast<fp8_e8m0_t>(0));
}

TEST_F(Tensor_Api_Cube_Copy_L12L0ScaleA_3510, CopyL12L0ScaleABatch)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = kScaleABatch;
    constexpr uint32_t m = kScaleAM;
    constexpr uint32_t n = kScaleAN;
    __cbuf__ fp8_e8m0_t src[batch * m * n] = {0};
    __ca__ fp8_e8m0_t dst[batch * m * n] = {0};

    auto baseLayout = MakeFrameLayout<ZZLayoutPtn, AscendC::Std::Int<2>>(m, n);
    using LayoutTraitType = GetLayoutTrait<decltype(baseLayout)>;
    auto batchLayout = MakeBatchPatternLayout<ZZLayoutPtn, LayoutTraitType>(batch, baseLayout);
    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(src), batchLayout);
    auto dstPtr = MakeMemPtr<Location::L0ScaleA, fp8_e8m0_t>((reinterpret_cast<uint64_t>(dst)) / 16);
    auto l0aTensor = MakeTensor(dstPtr, batchLayout);

    gExpectedMxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
    gExpectedMxSrc = reinterpret_cast<__cbuf__ void*>(src);
    gScaleACallIdx = 0;

    MOCKER_CPP(
        load_cbuf_to_ca_mx, void(uint64_t, __cbuf__ void*, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint16_t))
        .times(batch)
        .will(invoke(&load_cbuf_to_ca_mx_batch_stub));

    Copy(CopyAtom<CopyTraits<CopyL12L0ScaleA, CopyL12L0ScaleATraitDefault>>{}, l0aTensor, l1Tensor);

    EXPECT_EQ(gScaleACallIdx, batch);
    mockcpp::GlobalMockObject::verify();
}
