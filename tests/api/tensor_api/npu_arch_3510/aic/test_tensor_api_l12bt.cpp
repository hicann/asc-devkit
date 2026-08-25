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

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12BTRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __biasbuf__ float dst[m * n] = {0};

    auto l1Tensor = MakeTensorAt<Location::L1>(src, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto btTensor = MakeTensorAt<Location::BIAS>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL12BT, CopyL12BTTraitDefault>(btTensor, l1Tensor);
    RunCopyWithPaths<CopyL12BT, CopyL12BTTraitDefault>(btTensor, l1Tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12BTNDLayoutRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ float src[m * n] = {0};
    __biasbuf__ float dst[m * n] = {0};

    auto l1Tensor = MakeTensorAt<Location::L1>(src, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto btTensor = MakeTensorAt<Location::BIAS>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL12BT, CopyL12BTTraitDefault>(btTensor, l1Tensor);
    RunCopyWithPaths<CopyL12BT, CopyL12BTTraitDefault>(btTensor, l1Tensor);

    EXPECT_EQ(dst[0], 0);
}

enum class CubeLayout { RowMajor, NZ, ColumnMajor, ZN, ND };

enum class Prefix {
    gm,
    cbuf,
    biasbuf,
    fbuf,
};

constexpr int TEST_FRACTAL_FIXED = 16;
constexpr int TEST_C0_SIZE = 32;
constexpr int TEST_L12BT_UNIT = TEST_C0_SIZE;         // 64
constexpr int TEST_C2PIPE2GM_UNIT = TEST_C0_SIZE * 2; // 128

constexpr int TestCeilDivision(int value, int divisor) { return (value + divisor - 1) / divisor; }

// L1 -> BIAS: CopyCbufToBT3501
template <typename DTYPE, int SRC_SIZE1, int SRC_SIZE2, int DST_SIZE1, int DST_SIZE2>
__aicore__ inline void copy_cbuf_to_bt_stub(
    uint64_t dst, __cbuf__ DTYPE* src, uint16_t convControl, uint16_t blockCount, uint16_t blockLen, uint16_t srcStride,
    uint16_t dstStride)
{
    EXPECT_EQ(convControl, 0);
    EXPECT_EQ(blockCount, DST_SIZE1);
    EXPECT_EQ(blockLen, DST_SIZE2 * sizeof(DTYPE) / TEST_L12BT_UNIT);
    EXPECT_EQ(srcStride, (SRC_SIZE2 - DST_SIZE2) * sizeof(DTYPE) / TEST_C0_SIZE);
    EXPECT_EQ(dstStride, (DST_SIZE2 - DST_SIZE2) * sizeof(DTYPE) / TEST_L12BT_UNIT);
}

// L1 -> BIAS two type: CopyCbufToBT3501
template <typename SRC_DTYPE, typename DST_DTYPE, int SRC_SIZE1, int SRC_SIZE2, int DST_SIZE1, int DST_SIZE2>
__aicore__ inline void copy_cbuf_to_bt_two_type_stub(
    uint64_t dst, __cbuf__ SRC_DTYPE* src, uint16_t convControl, uint16_t blockCount, uint16_t blockLen,
    uint16_t srcStride, uint16_t dstStride)
{
    if constexpr (std::is_same_v<SRC_DTYPE, half>) {
        EXPECT_EQ(convControl, 1);
    } else {
        EXPECT_EQ(convControl, 0);
    }
    EXPECT_EQ(blockCount, DST_SIZE1);
    EXPECT_EQ(blockLen, DST_SIZE2 * sizeof(SRC_DTYPE) / TEST_L12BT_UNIT);
    EXPECT_EQ(srcStride, (SRC_SIZE2 - DST_SIZE2) * sizeof(SRC_DTYPE) / TEST_C0_SIZE);
    EXPECT_EQ(dstStride, (DST_SIZE2 - DST_SIZE2) * sizeof(DST_DTYPE) / TEST_L12BT_UNIT);
}

#define MAKE_LAYOUT_TYPE(fmt) fmt##LayoutPtn

// create tensor
#define CREATE_TENSOR(                                                                                                 \
    DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2, SRC_PREFIX, SRC_LOCATION, SRC_LAYOUT, DST_PREFIX, DST_LOCATION, \
    DST_LAYOUT)                                                                                                        \
    using namespace AscendC::Te;                                                                                       \
    __##SRC_PREFIX##__ DTYPE srcData[SRC_SIZE1 * SRC_SIZE2 * sizeof(DTYPE)];                                           \
    __##DST_PREFIX##__ DTYPE dstData[DST_SIZE1 * DST_SIZE2 * sizeof(DTYPE)];                                           \
                                                                                                                       \
    auto srcIterator = MakeMemPtr<Location::SRC_LOCATION>(srcData);                                                    \
    auto srcLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(SRC_LAYOUT), LayoutTraitDefault<DTYPE>>(SRC_SIZE1, SRC_SIZE2);   \
    auto srcTensor = MakeTensor(srcIterator, srcLayout);                                                               \
                                                                                                                       \
    auto dstIterator = MakeMemPtr<Location::DST_LOCATION>(dstData);                                                    \
    auto dstLayout = MakeFrameLayout<MAKE_LAYOUT_TYPE(DST_LAYOUT), LayoutTraitDefault<DTYPE>>(DST_SIZE1, DST_SIZE2);   \
    auto dstTensor = MakeTensor(dstIterator, dstLayout);

// create tensor
#define CREATE_TENSOR_TWO_TYPE(                                                                             \
    SRC_DTYPE, SRC_SIZE1, SRC_SIZE2, DST_DTYPE, DST_SIZE1, DST_SIZE2, SRC_PREFIX, SRC_LOCATION, SRC_LAYOUT, \
    DST_PREFIX, DST_LOCATION, DST_LAYOUT)                                                                   \
    using namespace AscendC::Te;                                                                            \
    __##SRC_PREFIX##__ SRC_DTYPE srcData[SRC_SIZE1 * SRC_SIZE2 * sizeof(SRC_DTYPE)];                        \
    __##DST_PREFIX##__ DST_DTYPE dstData[DST_SIZE1 * DST_SIZE2 * sizeof(DST_DTYPE)];                        \
                                                                                                            \
    auto srcIterator = MakeMemPtr<Location::SRC_LOCATION>(srcData);                                         \
    auto srcLayout =                                                                                        \
        MakeFrameLayout<MAKE_LAYOUT_TYPE(SRC_LAYOUT), LayoutTraitDefault<SRC_DTYPE>>(SRC_SIZE1, SRC_SIZE2); \
    auto srcTensor = MakeTensor(srcIterator, srcLayout);                                                    \
                                                                                                            \
    auto dstIterator = MakeMemPtr<Location::DST_LOCATION>(dstData);                                         \
    auto dstLayout =                                                                                        \
        MakeFrameLayout<MAKE_LAYOUT_TYPE(DST_LAYOUT), LayoutTraitDefault<DST_DTYPE>>(DST_SIZE1, DST_SIZE2); \
    auto dstTensor = MakeTensor(dstIterator, dstLayout);

// L1 to BIAS ND2ND test case
#define DATA_COPY_TEST_L12BIAS_ND2ND(DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2)                                               \
    TEST_F(                                                                                                                           \
        Tensor_Api_Cube_Copy_3510,                                                                                                    \
        TEST_TENSOR_API_DATACOPY_L12BIAS_ND2ND_##DTYPE##_SRC_SIZE##_##SRC_SIZE1##x##SRC_SIZE2##_DST_SIZE##_##DST_SIZE1##x##DST_SIZE2) \
    {                                                                                                                                 \
        using namespace AscendC::Te;                                                                                                  \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ DTYPE*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t))                \
            .times(1)                                                                                                                 \
            .will(invoke(&copy_cbuf_to_bt_stub<DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2>));                                  \
        CREATE_TENSOR(DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2, cbuf, L1, ND, biasbuf, BIAS, ND)                             \
        Copy(CopyAtom<CopyTraits<CopyL12BT, CopyL12BTTraitDefault>>{}, dstTensor, srcTensor);                                         \
        GlobalMockObject::verify();                                                                                                   \
    }

DATA_COPY_TEST_L12BIAS_ND2ND(float, 1, 64, 1, 64)
DATA_COPY_TEST_L12BIAS_ND2ND(int32_t, 1, 64, 1, 64)

// L1 to BIAS two data type  test case
#define DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(SRC_DTYPE, SRC_SIZE1, SRC_SIZE2, DST_DTYPE, DST_SIZE1, DST_SIZE2)                                               \
    TEST_F(                                                                                                                                                   \
        Tensor_Api_Cube_Copy_3510,                                                                                                                            \
        TEST_TENSOR_API_DATACOPY_L12BIAS_TWO_TYPE_ND2ND_##SRC_DTYPE##_SRC_SIZE##_##SRC_SIZE1##x##SRC_SIZE2##DST_DTYPE##_DST_SIZE##_##DST_SIZE1##x##DST_SIZE2) \
    {                                                                                                                                                         \
        using namespace AscendC::Te;                                                                                                                          \
        MOCKER_CPP(                                                                                                                                           \
            copy_cbuf_to_bt, void(uint64_t, __cbuf__ SRC_DTYPE*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t))                                           \
            .times(1)                                                                                                                                         \
            .will(invoke(                                                                                                                                     \
                &copy_cbuf_to_bt_two_type_stub<SRC_DTYPE, DST_DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2>));                                           \
        CREATE_TENSOR_TWO_TYPE(                                                                                                                               \
            SRC_DTYPE, SRC_SIZE1, SRC_SIZE2, DST_DTYPE, DST_SIZE1, DST_SIZE2, cbuf, L1, ND, biasbuf, BIAS, ND)                                                \
        Copy(CopyAtom<CopyTraits<CopyL12BT, CopyL12BTTraitDefault>>{}, dstTensor, srcTensor);                                                                 \
        GlobalMockObject::verify();                                                                                                                           \
    }

DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(bfloat16_t, 1, 64, float, 1, 64)
DATA_COPY_TEST_L12BIAS_TWO_TYPE_ND2ND(half, 1, 64, float, 1, 64)

// =========================================================================
// Batch ND2ND (l1_to_bt)
//
// Layout (B, (M, N)) constructed via MakeBatchPatternLayout always emits
// sB == M*N. The implementation only keeps the batched branch:
//   convControl = 0 (same-type) or 1 (half->float)
//   blockCount  = B
//   blockLen    = ceil_division(M*N, C0_ELEMENT<srcType>), rounded up to 2 for fp32/int32
//   srcStride   = (sB - M*N) / C0_ELEMENT<srcType>
//   dstStride   = ceil_align((sB - M*N) / C0_ELEMENT<dstType>, 2)
// With sB == M*N both strides are 0.
// =========================================================================
template <typename DTYPE, int B, int M, int N>
__aicore__ inline void copy_cbuf_to_bt_batch_compact_stub(
    uint64_t dst, __cbuf__ DTYPE* src, uint16_t convControl, uint16_t blockCount, uint16_t blockLen, uint16_t srcStride,
    uint16_t dstStride)
{
    EXPECT_EQ(convControl, 0);
    EXPECT_EQ(blockCount, B);
    uint16_t expectBlockLen = TestCeilDivision(M * N * sizeof(DTYPE), TEST_C0_SIZE);
    if constexpr (std::is_same_v<DTYPE, float> || std::is_same_v<DTYPE, int32_t>) {
        expectBlockLen = (expectBlockLen + 1) & ~1;
    }
    EXPECT_EQ(blockLen, expectBlockLen);
    EXPECT_EQ(srcStride, 0);
    EXPECT_EQ(dstStride, 0);
}

#define DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(DTYPE, B, M, N)                                                     \
    TEST_F(Tensor_Api_Cube_Copy_3510, TEST_TENSOR_API_DATACOPY_L12BIAS_BATCH_ND2ND_##DTYPE##_##B##x##M##x##N)          \
    {                                                                                                                  \
        using namespace AscendC::Te;                                                                                   \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ DTYPE*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1)                                                                                                  \
            .will(invoke(&copy_cbuf_to_bt_batch_compact_stub<DTYPE, B, M, N>));                                        \
        __cbuf__ DTYPE srcData[B * M * N];                                                                             \
        __biasbuf__ DTYPE dstData[B * M * N];                                                                          \
        auto srcLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<DTYPE>>(                                      \
            static_cast<uint32_t>(B), static_cast<uint32_t>(M), static_cast<uint32_t>(N));                             \
        auto dstLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<DTYPE>>(                                      \
            static_cast<uint32_t>(B), static_cast<uint32_t>(M), static_cast<uint32_t>(N));                             \
        auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(srcData), srcLayout);                                     \
        auto dstTensor = MakeTensor(MakeMemPtr<Location::BIAS>(dstData), dstLayout);                                   \
        Copy(CopyAtom<CopyTraits<CopyL12BT, CopyL12BTTraitDefault>>{}, dstTensor, srcTensor);                          \
        GlobalMockObject::verify();                                                                                    \
    }

DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(float, 2, 4, 64)
DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(int32_t, 2, 4, 64)
DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_COMPACT(float, 4, 1, 64)

// half -> float convControl = 1 path
template <int B, int M, int N>
__aicore__ inline void copy_cbuf_to_bt_batch_compact_half2float_stub(
    uint64_t dst, __cbuf__ half* src, uint16_t convControl, uint16_t blockCount, uint16_t blockLen, uint16_t srcStride,
    uint16_t dstStride)
{
    EXPECT_EQ(convControl, 1);
    EXPECT_EQ(blockCount, B);
    EXPECT_EQ(blockLen, TestCeilDivision(M * N * sizeof(half), TEST_C0_SIZE));
    EXPECT_EQ(srcStride, 0);
    EXPECT_EQ(dstStride, 0);
}

#define DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_HALF2FLOAT(B, M, N)                                                        \
    TEST_F(Tensor_Api_Cube_Copy_3510, TEST_TENSOR_API_DATACOPY_L12BIAS_BATCH_ND2ND_half2float_##B##x##M##x##N)        \
    {                                                                                                                 \
        using namespace AscendC::Te;                                                                                  \
        MOCKER_CPP(copy_cbuf_to_bt, void(uint64_t, __cbuf__ half*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1)                                                                                                 \
            .will(invoke(&copy_cbuf_to_bt_batch_compact_half2float_stub<B, M, N>));                                   \
        __cbuf__ half srcData[B * M * N];                                                                             \
        __biasbuf__ float dstData[B * M * N];                                                                         \
        auto srcLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<half>>(                                      \
            static_cast<uint32_t>(B), static_cast<uint32_t>(M), static_cast<uint32_t>(N));                            \
        auto dstLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(                                     \
            static_cast<uint32_t>(B), static_cast<uint32_t>(M), static_cast<uint32_t>(N));                            \
        auto srcTensor = MakeTensor(MakeMemPtr<Location::L1>(srcData), srcLayout);                                    \
        auto dstTensor = MakeTensor(MakeMemPtr<Location::BIAS>(dstData), dstLayout);                                  \
        Copy(CopyAtom<CopyTraits<CopyL12BT, CopyL12BTTraitDefault>>{}, dstTensor, srcTensor);                         \
        GlobalMockObject::verify();                                                                                   \
    }

DATA_COPY_TEST_L12BIAS_BATCH_ND2ND_HALF2FLOAT(2, 4, 64)
