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

class Tensor_Api_Vector_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

struct CopyGM2UBCapture {
    uint8_t leftPaddingCount = 0;
    uint8_t rightPaddingCount = 0;
    bool enableConstantPad = true;
};

CopyGM2UBCapture gCopyGM2UBCapture;

void CopyGM2UBStub(__ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t leftPaddingCount,
                   uint8_t rightPaddingCount, bool enableConstantPad, uint8_t, uint64_t, uint32_t)
{
    gCopyGM2UBCapture = {leftPaddingCount, rightPaddingCount, enableConstantPad};
}

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

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBND2ND)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBWithPaddingParams)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 2;
    constexpr uint32_t n = 16;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    MOCKER_CPP(copy_gm_to_ubuf_align_v2, void(__ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t,
                                              uint8_t, bool, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(CopyGM2UBStub));

    constexpr CopyGM2UBParams params{3, 5, false};
    Copy(MakeCopy(CopyGM2UB{}).with(params), ubTensor, gmTensor);

    GlobalMockObject::verify();
    EXPECT_EQ(gCopyGM2UBCapture.leftPaddingCount, params.leftPaddingCount);
    EXPECT_EQ(gCopyGM2UBCapture.rightPaddingCount, params.rightPaddingCount);
    EXPECT_EQ(gCopyGM2UBCapture.enableConstantPad, params.enableConstantPad);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBDefaultPaddingParams)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 2;
    constexpr uint32_t n = 16;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    MOCKER_CPP(copy_gm_to_ubuf_align_v2, void(__ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t,
                                              uint8_t, bool, uint8_t, uint64_t, uint32_t))
        .times(1)
        .will(invoke(CopyGM2UBStub));

    gCopyGM2UBCapture = {1, 1, false};
    Copy(MakeCopy(CopyGM2UB{}), ubTensor, gmTensor);

    GlobalMockObject::verify();
    EXPECT_EQ(gCopyGM2UBCapture.leftPaddingCount, 0);
    EXPECT_EQ(gCopyGM2UBCapture.rightPaddingCount, 0);
    EXPECT_TRUE(gCopyGM2UBCapture.enableConstantPad);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBNDLayout2NDLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_PERSISTENT);

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBDN2DN)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBDNLayout2DNLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBNZ2NZ)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Vector_Copy_3510, CopyGM2UBZN2ZN)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

    RunCopyCallPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);
    RunCopyWithPaths<CopyGM2UB, CopyGM2UBTraitDefault>(ubTensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}
