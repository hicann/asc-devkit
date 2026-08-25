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

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12FBRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ uint64_t src[m * n] = {0};
    __fbuf__ uint64_t dst[m * n] = {0};

    auto l1Tensor =
        MakeTensorAt<Location::L1>(src, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<uint64_t>>(m, n));
    auto fbTensor =
        MakeTensorAt<Location::FIXBUF>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<uint64_t>>(m, n));

    RunCopyCallPaths<CopyL12FB, CopyL12FBTraitDefault>(fbTensor, l1Tensor);
    RunCopyWithPaths<CopyL12FB, CopyL12FBTraitDefault>(fbTensor, l1Tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL12FBNDLayoutRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cbuf__ uint64_t src[m * n] = {0};
    __fbuf__ uint64_t dst[m * n] = {0};

    auto l1Tensor = MakeTensorAt<Location::L1>(src, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<uint64_t>>(m, n));
    auto fbTensor =
        MakeTensorAt<Location::FIXBUF>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<uint64_t>>(m, n));

    RunCopyCallPaths<CopyL12FB, CopyL12FBTraitDefault>(fbTensor, l1Tensor);
    RunCopyWithPaths<CopyL12FB, CopyL12FBTraitDefault>(fbTensor, l1Tensor);

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

// L1 -> FIXBUF: CopyCbufToFB3501
template <typename DTYPE, int SRC_SIZE1, int SRC_SIZE2, int DST_SIZE1, int DST_SIZE2>
__aicore__ inline void copy_cbuf_to_fbuf_stub(
    __fbuf__ void* dst, __cbuf__ void* src, uint16_t blockCount, uint16_t blockLen, uint16_t srcStride,
    uint16_t dstStride)
{
    EXPECT_EQ(blockCount, DST_SIZE1);
    EXPECT_EQ(blockLen, TestCeilDivision(DST_SIZE2 * sizeof(DTYPE), TEST_C2PIPE2GM_UNIT));
    EXPECT_EQ(srcStride, TestCeilDivision(SRC_SIZE2 * sizeof(DTYPE), TEST_C0_SIZE));
    EXPECT_EQ(dstStride, TestCeilDivision(DST_SIZE2 * sizeof(DTYPE), TEST_C2PIPE2GM_UNIT));
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

// L1 to FP ND2ND test case
#define DATA_COPY_TEST_L12FB_ND2ND(DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2)                                               \
    TEST_F(                                                                                                                         \
        Tensor_Api_Cube_Copy_3510,                                                                                                  \
        TEST_TENSOR_API_DATACOPY_L12FB_ND2ND_##DTYPE##_SRC_SIZE##_##SRC_SIZE1##x##SRC_SIZE2##_DST_SIZE##_##DST_SIZE1##x##DST_SIZE2) \
    {                                                                                                                               \
        using namespace AscendC::Te;                                                                                                \
        MOCKER_CPP(copy_cbuf_to_fbuf, void(__fbuf__ void*, __cbuf__ void*, uint16_t, uint16_t, uint16_t, uint16_t))                 \
            .times(1)                                                                                                               \
            .will(invoke(&copy_cbuf_to_fbuf_stub<DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2>));                              \
        CREATE_TENSOR(DTYPE, SRC_SIZE1, SRC_SIZE2, DST_SIZE1, DST_SIZE2, cbuf, L1, ND, fbuf, FIXBUF, ND)                            \
        Copy(CopyAtom<CopyTraits<CopyL12FB, CopyL12FBTraitDefault>>{}, dstTensor, srcTensor);                                       \
        GlobalMockObject::verify();                                                                                                 \
    }

DATA_COPY_TEST_L12FB_ND2ND(uint64_t, 1, 64, 1, 64)
