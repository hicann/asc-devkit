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
using namespace AscendC::Te;
constexpr bool enableRelu = false;
constexpr bool enableChannelSplit = true;
constexpr CopyL0C2L1Trait l0c2l1Trait = {RoundMode::DEFAULT, enableRelu, enableChannelSplit};

struct CopyL0C2L1TraitCustom {
    using TraitType = CopyL0C2L1Trait;
    static constexpr const TraitType value = l0c2l1Trait;
};

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

template <typename CopyOp, typename Trait, typename Param, typename DstTensor, typename SrcTensor>
void RunCopyWithParamPaths(const DstTensor& dst, const SrcTensor& src, const Param& param)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}).with(param);
    atom.Call(dst, src);

    auto copyAtom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param);
    copyAtom.Call(dst, src);

    Copy(copyAtom, dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param), dst, src);
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2L1NZ2NZFloat)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, float>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NZLayoutPtn, float>(m, n));

    RunCopyCallPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2L1NZ2NZInt32)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ int32_t src[m * n] = {0};
    __cbuf__ int32_t dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, int32_t>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NZLayoutPtn, int32_t>(m, n));

    RunCopyCallPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2L1NZ2NZF322F16)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ half dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, float>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NZLayoutPtn, half>(m, n));

    RunCopyCallPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], static_cast<half>(0));
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2L1NZ2NZF322BF16)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ bfloat16_t dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, float>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NZLayoutPtn, bfloat16_t>(m, n));

    RunCopyCallPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2L1, CopyL0C2L1TraitDefault>(l1Tensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], static_cast<bfloat16_t>(0));
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2L1NZ2NZWithChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __cbuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, float>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NZLayoutPtn, float>(m, n));

    RunCopyCallPaths<CopyL0C2L1, CopyL0C2L1TraitCustom>(l1Tensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2L1, CopyL0C2L1TraitCustom>(l1Tensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}
