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

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0BRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __cb__ float dst[m * n] = {0};

    auto l1Tensor = MakeTensorAt<Location::L1>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto l0bTensor = MakeTensorAt<Location::L0B>(dst, MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL12L0B, CopyL12L0BTraitDefault>(l0bTensor, l1Tensor);
    RunCopyWithPaths<CopyL12L0B, CopyL12L0BTraitDefault>(l0bTensor, l1Tensor);

    EXPECT_EQ(dst[0], 0);
}

template <bool transpose, typename T, int M_STEP, int K_STEP>
void load_cbuf_to_cb_stub(
    __cb__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(transposed, transpose);
}

namespace {

__cb__ fp4x2_e1m2_t* gBatchB4L0BDstBase = nullptr;
__cbuf__ fp4x2_e1m2_t* gBatchB4L0BSrcBase = nullptr;
uint32_t gBatchB4L0BSrcBatchStride = 0;
uint32_t gBatchB4L0BDstBatchStride = 0;
uint32_t gBatchB4L0BDstSplitStride = 0;
uint32_t gBatchB4L0BCallIndex = 0;

template <typename T, int M_STEP, int K_STEP, int SRC_STRIDE, int DST_STRIDE, uint32_t SPLIT_NUM>
void load_cbuf_to_cb_batch_b4_split_stub(
    __cb__ T* dst, __cbuf__ T* src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
    int16_t srcStride, uint16_t dstStride, bool transposed)
{
    const auto batchIdx = gBatchB4L0BCallIndex / SPLIT_NUM;
    const auto splitIdx = gBatchB4L0BCallIndex % SPLIT_NUM;
    EXPECT_EQ(dst, gBatchB4L0BDstBase + batchIdx * gBatchB4L0BDstBatchStride + splitIdx * gBatchB4L0BDstSplitStride);
    EXPECT_EQ(src, gBatchB4L0BSrcBase + batchIdx * gBatchB4L0BSrcBatchStride);
    EXPECT_EQ(mStartPosition, splitIdx * M_STEP);
    EXPECT_EQ(kStartPosition, 0);
    EXPECT_EQ(mStep, M_STEP);
    EXPECT_EQ(kStep, K_STEP);
    EXPECT_EQ(srcStride, SRC_STRIDE);
    EXPECT_EQ(dstStride, DST_STRIDE);
    EXPECT_TRUE(transposed);
    ++gBatchB4L0BCallIndex;
}

} // namespace

template <typename T, uint32_t BATCH, uint32_t M, uint32_t N>
auto MakeBatchZNLayout()
{
    using namespace AscendC::Te;
    constexpr uint32_t C0 = C0_ELEMENT<T>;

    auto shape = MakeShape(
        AscendC::Std::Int<BATCH>{},
        MakeShape(
            MakeShape(
                AscendC::Std::Int<C0>{}, AscendC::Std::ceil_division(AscendC::Std::Int<M>{}, AscendC::Std::Int<C0>{})),
            MakeShape(
                AscendC::Std::Int<FRACTAL_FIXED>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<N>{}, AscendC::Std::Int<FRACTAL_FIXED>{}))));
    auto stride = MakeStride(
        AscendC::Std::Int<M * N>{},
        MakeStride(
            MakeStride(
                AscendC::Std::Int<1>{},
                AscendC::Std::Int<C0>{} *
                    AscendC::Std::ceil_align(AscendC::Std::Int<N>{}, AscendC::Std::Int<FRACTAL_FIXED>{})),
            MakeStride(AscendC::Std::Int<C0>{}, AscendC::Std::Int<C0 * FRACTAL_FIXED>{})));
    return MakePatternLayout<ZNLayoutPtn, LayoutTraitDefault<T>>(shape, stride);
}

template <typename T, uint32_t BATCH, uint32_t M, uint32_t N>
auto MakeBatchNZLayout()
{
    using namespace AscendC::Te;
    constexpr uint32_t C0 = C0_ELEMENT<T>;

    auto shape = MakeShape(
        AscendC::Std::Int<BATCH>{},
        MakeShape(
            MakeShape(
                AscendC::Std::Int<FRACTAL_FIXED>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<M>{}, AscendC::Std::Int<FRACTAL_FIXED>{})),
            MakeShape(
                AscendC::Std::Int<C0>{},
                AscendC::Std::ceil_division(AscendC::Std::Int<N>{}, AscendC::Std::Int<C0>{}))));
    auto stride = MakeStride(
        AscendC::Std::Int<M * N>{},
        MakeStride(
            MakeStride(AscendC::Std::Int<C0>{}, AscendC::Std::Int<C0 * FRACTAL_FIXED>{}),
            MakeStride(
                AscendC::Std::Int<1>{},
                AscendC::Std::Int<C0>{} *
                    AscendC::Std::ceil_align(AscendC::Std::Int<M>{}, AscendC::Std::Int<FRACTAL_FIXED>{}))));
    return MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<T>>(shape, stride);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0BZN2ZNBatchRoutesToSingleBatchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 16;
    __cbuf__ half src[batch * m * n] = {0};
    __cb__ half dst[batch * m * n] = {0};

    auto srcLayout = MakeBatchZNLayout<half, batch, m, n>();
    auto dstLayout = MakeBatchZNLayout<half, batch, m, n>();
    auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(src), srcLayout);
    auto dstTensor = MakeTensor(MakeMemPtr<Location::L0B>(dst), dstLayout);

    MOCKER_CPP(
        load_cbuf_to_cb,
        void(__cb__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .expects(once())
        .will(invoke(&load_cbuf_to_cb_stub<false, half, 1, batch>));

    Copy(CopyAtom<CopyTraits<CopyL12L0B, CopyL12L0BTraitDefault>>{}, dstTensor, srcTensor);

    mockcpp::GlobalMockObject::verify();
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0BNZ2ZNBatchRoutesToSingleBatchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t n = 16;
    __cbuf__ half src[batch * m * n] = {0};
    __cb__ half dst[batch * m * n] = {0};

    auto srcLayout = MakeBatchNZLayout<half, batch, m, n>();
    auto dstLayout = MakeBatchZNLayout<half, batch, m, n>();
    auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(src), srcLayout);
    auto dstTensor = MakeTensor(MakeMemPtr<Location::L0B>(dst), dstLayout);

    MOCKER_CPP(
        load_cbuf_to_cb,
        void(__cb__ half*, __cbuf__ half*, uint16_t, uint16_t, uint8_t, uint8_t, int16_t, uint16_t, bool))
        .expects(exactly(batch))
        .will(invoke(&load_cbuf_to_cb_stub<true, half, 1, 1>));

    Copy(CopyAtom<CopyTraits<CopyL12L0B, CopyL12L0BTraitDefault>>{}, dstTensor, srcTensor);

    mockcpp::GlobalMockObject::verify();
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12L0BBatchNz2ZnB8B4)
{
    using namespace AscendC::Te;

    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 128;
    constexpr uint32_t n = 16;
    constexpr uint32_t srcBufferSize = 8192;
    constexpr uint32_t dstBufferSize = 2048;
    constexpr uint32_t splitNum = 2;
    __cbuf__ fp4x2_e1m2_t src[batch * srcBufferSize];
    __cb__ fp4x2_e1m2_t dst[batch * dstBufferSize];

    auto srcLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitFP4>(batch, m, n);
    auto dstLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitFP4>(batch, m, n);
    auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(src), srcLayout);
    auto dstTensor = MakeTensor(MakeMemPtr<Location::L0B>(dst), dstLayout);

    auto firstDst = MakeSingleBatchSubTensor(dstTensor, 0)(MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)));
    auto secondDst = MakeSingleBatchSubTensor(dstTensor, 1)(MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)));
    auto splitDst = MakeSingleBatchSubTensor(dstTensor, 0)(MakeCoord(MakeCoord(0, 1), MakeCoord(0, 0)));
    auto firstSrc = MakeSingleBatchSubTensor(srcTensor, 0);
    auto secondSrc = MakeSingleBatchSubTensor(srcTensor, 1);
    gBatchB4L0BDstBase = firstDst.Data().Get();
    gBatchB4L0BSrcBase = firstSrc.Data().Get();
    gBatchB4L0BDstBatchStride = secondDst.Data().Get() - gBatchB4L0BDstBase;
    gBatchB4L0BDstSplitStride = splitDst.Data().Get() - gBatchB4L0BDstBase;
    gBatchB4L0BSrcBatchStride = secondSrc.Data().Get() - gBatchB4L0BSrcBase;
    gBatchB4L0BCallIndex = 0;

    MOCKER_CPP(
        load_cbuf_to_cb_s4, void(
                                __cb__ fp4x2_e1m2_t*, __cbuf__ fp4x2_e1m2_t*, uint16_t, uint16_t, uint8_t, uint8_t,
                                int16_t, uint16_t, bool))
        .times(batch * splitNum)
        .will(invoke(&load_cbuf_to_cb_batch_b4_split_stub<fp4x2_e1m2_t, 4, 1, 8, 1, splitNum>));

    Copy(CopyAtom<CopyTraits<CopyL12L0B, CopyL12L0BTraitDefault>>{}, dstTensor, srcTensor);

    EXPECT_EQ(gBatchB4L0BCallIndex, batch * splitNum);
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

// l1 -> l0B ZN2ZN非转置，覆盖所有TYPE
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 64, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(float, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int32_t, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 8, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, ZN, ZN, L1, L0B, cbuf, cb, false);

// l1 -> l0B NZ2ZN转置，覆盖所有TYPE
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e1m2_t, 64, 64, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_S4_DATA(fp4x2_e2m1_t, 64, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(bfloat16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(half, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(float, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int32_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint32_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int8_t, 32, 32, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint8_t, 32, 32, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(int16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
TEST_TENSOR_API_LOAD_DATA(uint16_t, 16, 16, NZ, ZN, L1, L0B, cbuf, cb, true);
