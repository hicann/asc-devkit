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
#include <mockcpp/mockcpp.hpp>

class Tensor_Api_Cube_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(1); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

template <typename LocationTag, typename Pointer, typename Layout>
auto MakeTensorAt(Pointer ptr, const Layout& layout)
{
    return AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr<LocationTag>(ptr), layout);
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

__ca__ float* gBatchDstBase = nullptr;
__cbuf__ float* gBatchSrcBase = nullptr;
uint32_t gBatchCallIndex = 0;
__ca__ fp4x2_e1m2_t* gBatchB4DstBase = nullptr;
__cbuf__ fp4x2_e1m2_t* gBatchB4SrcBase = nullptr;
uint32_t gBatchB4CallIndex = 0;

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0ARoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __ca__ float dst[m * n] = {0};

    auto l1Tensor = MakeTensorAt<Location::L1>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto l0aTensor = MakeTensorAt<Location::L0A>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL12L0A, CopyL12L0ATraitDefault>(l0aTensor, l1Tensor);
    RunCopyWithPaths<CopyL12L0A, CopyL12L0ATraitDefault>(l0aTensor, l1Tensor);

    EXPECT_EQ(dst[0], 0);
}

template <bool transpose, typename T, int M_STEP, int K_STEP>
void load_cbuf_to_ca_stub(
    __ca__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(transposed, transpose);
}

template <typename T, int M_STEP, int K_STEP, int SRC_STRIDE, int DST_STRIDE>
void load_cbuf_to_ca_batch_stub(
    __ca__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    EXPECT_EQ(dst, gBatchDstBase);
    EXPECT_EQ(src, gBatchSrcBase);
    EXPECT_EQ(mStartPosition, 0);
    EXPECT_EQ(kStartPosition, 0);
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(srcStride, SRC_STRIDE);
    EXPECT_EQ(dstStride, DST_STRIDE);
    EXPECT_FALSE(transposed);
    ++gBatchCallIndex;
}

template <typename T, int M_STEP, int K_STEP, int SRC_STRIDE, int DST_STRIDE, uint32_t BATCH_STRIDE, bool TRANSPOSE>
void load_cbuf_to_ca_batch_offset_stub(
    __ca__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    const auto expectedOffset = gBatchCallIndex * BATCH_STRIDE;
    EXPECT_EQ(dst, gBatchDstBase + expectedOffset);
    EXPECT_EQ(src, gBatchSrcBase + expectedOffset);
    EXPECT_EQ(mStartPosition, 0);
    EXPECT_EQ(kStartPosition, 0);
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(srcStride, SRC_STRIDE);
    EXPECT_EQ(dstStride, DST_STRIDE);
    EXPECT_EQ(transposed, TRANSPOSE);
    ++gBatchCallIndex;
}

template <
    typename T, int M_STEP, int K_STEP, int SRC_STRIDE, int DST_STRIDE, uint32_t SRC_BATCH_STRIDE,
    uint32_t DST_BATCH_STRIDE, uint32_t DST_SPLIT_STRIDE, uint32_t SPLIT_NUM>
void load_cbuf_to_ca_batch_b4_split_stub(
    __ca__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    const auto batchIdx = gBatchB4CallIndex / SPLIT_NUM;
    const auto splitIdx = gBatchB4CallIndex % SPLIT_NUM;
    EXPECT_EQ(dst, gBatchB4DstBase + batchIdx * DST_BATCH_STRIDE + splitIdx * DST_SPLIT_STRIDE);
    EXPECT_EQ(src, gBatchB4SrcBase + batchIdx * SRC_BATCH_STRIDE);
    EXPECT_EQ(mStartPosition, splitIdx * M_STEP);
    EXPECT_EQ(kStartPosition, 0);
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(srcStride, SRC_STRIDE);
    EXPECT_EQ(dstStride, DST_STRIDE);
    EXPECT_TRUE(transposed);
    ++gBatchB4CallIndex;
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0ABatchNz2Nz)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[batch * m * n] = {0};
    __ca__ float dst[batch * m * n] = {0};

    auto batchLayout = MakeFrameLayout<NZLayoutPtn, float>(batch, m, n);
    auto l1Tensor = MakeTensorAt<Location::L1>(src, batchLayout);
    auto l0aTensor = MakeTensorAt<Location::L0A>(dst, batchLayout);

    gBatchDstBase = dst;
    gBatchSrcBase = src;
    gBatchCallIndex = 0;

    MOCKER_CPP(
        load_cbuf_to_ca,
        void(__ca__ float*, __cbuf__ float*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(1)
        .will(invoke(&load_cbuf_to_ca_batch_stub<float, 2, 8, 2, 2>));

    Copy(CopyAtom<CopyTraits<CopyL12L0A, CopyL12L0ATraitDefault>>{}, l0aTensor, l1Tensor);

    EXPECT_EQ(gBatchCallIndex, 1);
    mockcpp::GlobalMockObject::verify();
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0ABatchZn2Nz)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    constexpr uint32_t batchStride = m * n;
    __cbuf__ float src[batch * m * n] = {0};
    __ca__ float dst[batch * m * n] = {0};

    auto srcBatchLayout = MakeFrameLayout<ZNLayoutPtn, float>(batch, m, n);
    auto dstBatchLayout = MakeFrameLayout<NZLayoutPtn, float>(batch, m, n);
    auto l1Tensor = MakeTensorAt<Location::L1>(src, srcBatchLayout);
    auto l0aTensor = MakeTensorAt<Location::L0A>(dst, dstBatchLayout);

    gBatchDstBase = dst;
    gBatchSrcBase = src;
    gBatchCallIndex = 0;

    MOCKER_CPP(
        load_cbuf_to_ca,
        void(__ca__ float*, __cbuf__ float*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .times(batch)
        .will(invoke(&load_cbuf_to_ca_batch_offset_stub<float, 2, 4, 2, 2, batchStride, true>));

    Copy(CopyAtom<CopyTraits<CopyL12L0A, CopyL12L0ATraitDefault>>{}, l0aTensor, l1Tensor);

    EXPECT_EQ(gBatchCallIndex, batch);
    mockcpp::GlobalMockObject::verify();
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0ABatchZn2NzB8B4)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 128;
    constexpr uint32_t srcBatchStride = 4096;
    constexpr uint32_t dstBatchStride = 1024;
    constexpr uint32_t dstSplitStride = 512;
    constexpr uint32_t splitNum = 2;
    __cbuf__ fp4x2_e1m2_t src[batch * srcBatchStride];
    __ca__ fp4x2_e1m2_t dst[batch * dstBatchStride];

    auto srcBatchLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitFP4>(batch, m, n);
    auto dstBatchLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitFP4>(batch, m, n);
    auto l1Tensor = MakeTensorAt<Location::L1>(src, srcBatchLayout);
    auto l0aTensor = MakeTensorAt<Location::L0A>(dst, dstBatchLayout);

    gBatchB4DstBase = dst;
    gBatchB4SrcBase = src;
    gBatchB4CallIndex = 0;

    MOCKER_CPP(
        load_cbuf_to_ca_s4, void(
                                __ca__ fp4x2_e1m2_t*, __cbuf__ fp4x2_e1m2_t*, uint16_t, uint16_t, uint8_t, uint8_t,
                                int16_t, uint16_t, bool))
        .times(batch * splitNum)
        .will(invoke(&load_cbuf_to_ca_batch_b4_split_stub<
                     fp4x2_e1m2_t, 4, 1, 8, 1, srcBatchStride, dstBatchStride, dstSplitStride, splitNum>));

    Copy(CopyAtom<CopyTraits<CopyL12L0A, CopyL12L0ATraitDefault>>{}, l0aTensor, l1Tensor);

    EXPECT_EQ(gBatchB4CallIndex, batch * splitNum);
    mockcpp::GlobalMockObject::verify();
}

#define MAKE_LAYOUT_TYPE(fmt) fmt##LayoutPtn

#define TEST_TENSOR_API_LOAD_DATA(TYPE, M, N, SRC_FORMAT, DST_FORMAT, SRC_POS, DST_POS, SRC_TAG, DST_TAG, TRANSPOSE)   \
    TEST_F(                                                                                                            \
        Tensor_Api_Cube_Copy_3510,                                                                                     \
        TestLoadData_##TYPE##M##N##SRC_FORMAT##DST_FORMAT##SRC_POS##DST_POS##SRC_TAG##DST_TAG##TRANSPOSE)              \
    {                                                                                                                  \
        using namespace AscendC::Te;                                                                                   \
        __##DST_TAG##__ TYPE dst[M * N] = {0};                                                                         \
        auto dstIterator = MakeMemPtr<Location::DST_POS>(dst);                                                         \
        auto dstMatrixLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(DST_FORMAT), LayoutTraitDefault<TYPE>>(M, N);          \
        auto dstTensor = MakeTensor(dstIterator, dstMatrixLayout);                                                     \
                                                                                                                       \
        __##SRC_TAG##__ TYPE src[M * N] = {0};                                                                         \
        auto srcIterator = MakeMemPtr<Location::SRC_POS>(src);                                                         \
        auto srcMatrixLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(SRC_FORMAT), LayoutTraitDefault<TYPE>>(M, N);          \
        auto srcTensor = MakeTensor(srcIterator, srcMatrixLayout);                                                     \
                                                                                                                       \
        constexpr int M_STEP = (sizeof(TYPE) == 1 && TRANSPOSE) ? 2 : 1;                                               \
        constexpr int K_STEP = (sizeof(TYPE) == 4 && TRANSPOSE) ? 2 : 1;                                               \
        MOCKER_CPP(                                                                                                    \
            load_cbuf_to_##DST_TAG,                                                                                    \
            void(                                                                                                      \
                __##DST_TAG##__ TYPE*, __cbuf__ TYPE*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool)) \
            .times(1)                                                                                                  \
            .will(invoke(&load_cbuf_to_##DST_TAG##_stub<TRANSPOSE, TYPE, M_STEP, K_STEP>));                            \
        Copy(CopyAtom<CopyTraits<CopyL12##DST_POS, CopyL12##DST_POS##TraitDefault>>{}, dstTensor, srcTensor);          \
                                                                                                                       \
        mockcpp::GlobalMockObject::verify();                                                                           \
    }

#define TEST_TENSOR_API_LOAD_S4_DATA(                                                                         \
    TYPE, M, N, SRC_FORMAT, DST_FORMAT, SRC_POS, DST_POS, SRC_TAG, DST_TAG, TRANSPOSE)                        \
    TEST_F(                                                                                                   \
        Tensor_Api_Cube_Copy_3510,                                                                            \
        TestLoadData_##TYPE##M##N##SRC_FORMAT##DST_FORMAT##SRC_POS##DST_POS##SRC_TAG##DST_TAG##TRANSPOSE)     \
    {                                                                                                         \
        using namespace AscendC::Te;                                                                          \
        __##DST_TAG##__ TYPE dst[M * N];                                                                      \
        auto dstIterator = MakeMemPtr<Location::DST_POS>(dst);                                                \
        auto dstMatrixLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(DST_FORMAT), LayoutTraitFP4>(M, N);           \
        auto dstTensor = MakeTensor(dstIterator, dstMatrixLayout);                                            \
                                                                                                              \
        __##SRC_TAG##__ TYPE src[M * N];                                                                      \
        auto srcIterator = MakeMemPtr<Location::SRC_POS>(src);                                                \
        auto srcMatrixLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(SRC_FORMAT), LayoutTraitFP4>(M, N);           \
        auto srcTensor = MakeTensor(srcIterator, srcMatrixLayout);                                            \
                                                                                                              \
        Copy(CopyAtom<CopyTraits<CopyL12##DST_POS, CopyL12##DST_POS##TraitDefault>>{}, dstTensor, srcTensor); \
                                                                                                              \
        mockcpp::GlobalMockObject::verify();                                                                  \
    }

// l1 -> l0A NZ2NZ非转置，覆盖所有TYPE
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 16, 64, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 16, 64, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(float, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 8, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int8_t, 16, 32, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 16, 32, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, NZ, NZ, L1, L0A, cbuf, ca, false);

// l1 -> l0A ZN2NZ转置，覆盖所有TYPE
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 64, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 16, 64, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(float, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 16, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 32, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, ZN, NZ, L1, L0A, cbuf, ca, true);

// ================= img2col (L1 NC1HWC0 -> L0A NZ) =================
// The img2col instruction has 17 params, above mockcpp's API-hook arity limit, so this drives the
// full copy path against the empty cce stub and checks it routes/compiles without a crash. The dst
// NZ M/K (conv-unfolded, ceil-aligned) and the src NC1HWC0 axes exercise LoadDataL12L0AImg2Col::Run.
namespace {
template <typename T, size_t C0>
auto MakeNc1hwc0Layout(int n, int c1, int h, int w)
{
    using namespace AscendC::Te;
    return MakeFrameLayout<NC1HWC0LayoutPtn>(n, c1, h, w, static_cast<int>(C0));
}
} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0AImg2Col)
{
    using namespace AscendC::Te;

    // src NC1HWC0: N=1, C1=2, H=5, W=5, C0=16 -> channelSize = C1*C0 = 32.
    constexpr int N = 1;
    constexpr int C1 = 2;
    constexpr int H = 5;
    constexpr int W = 5;
    constexpr int C0 = 16;

    // filter 3x3, stride 1, pad 1, dilation 1 -> Ho = Wo = 5, M = 25, K = 3*3*32 = 288.
    constexpr int filterH = 3;
    constexpr int filterW = 3;
    constexpr int M = 25;
    constexpr int K = filterH * filterW * C1 * C0; // 288
    constexpr int mAlign = 32;                     // ceil(25, 16) * 16
    constexpr int kAlign = 288;                    // already a multiple of C0

    __cbuf__ int16_t src[N * C1 * H * W * C0] = {0};
    __ca__ int16_t dst[mAlign * kAlign] = {0};

    auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(src), MakeNc1hwc0Layout<int16_t, C0>(N, C1, H, W));
    auto l0aTensor = MakeTensorAt<Location::L0A>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<int16_t>>(M, K));

    // dst NZ ceil-aligned M/K, now provided by the caller via Img2ColParams.
    Img2ColParams<int16_t> params;
    params.mExtension = static_cast<uint16_t>(mAlign);
    params.kExtension = static_cast<uint16_t>(kAlign);
    params.filterW = filterW;
    params.filterH = filterH;
    params.strideW = 1;
    params.strideH = 1;
    params.padList[0] = params.padList[1] = params.padList[2] = params.padList[3] = 1;

    auto atom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(atom.with(params), l0aTensor, srcTensor);

    EXPECT_EQ(dst[0], 0);
}
