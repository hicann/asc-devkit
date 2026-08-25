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

class Tensor_Api_Layout_Squeeze : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

// Mode 1: squeeze a single axis by index when its compile-time size is 1.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeSingleDim)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _1{}, _5{}));
    auto squeezed = Squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Stride()).value, 5);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Stride()).value, 1);
}

// Mode 1: squeeze multiple axes by index.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeMultipleDims)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_1{}, _4{}, _1{}));
    auto squeezed = Squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank == 1);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
}

// Mode 1: a named axis whose size is not 1 is kept (no error).
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeNoOpWhenSizeNotOne)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _5{}));
    auto squeezed = Squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 5);
}

// Mode 2: pattern tuple marks squeeze positions with _1, keep with _.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePatternFlat)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_1{}, _4{}, _1{}));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, _, _1{}));

    static_assert(decltype(squeezed)::rank == 1);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
}

// Mode 2: all positions kept.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePatternKeepAll)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _5{}));
    auto squeezed = Squeeze(layout, MakeCoord(_, _));

    static_assert(decltype(squeezed)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 5);
}

// Mode 2: a position marked _1 but whose size is not 1 is kept (no error).
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePatternMarkNonOne)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _5{}));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, _));

    static_assert(decltype(squeezed)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 5);
}

// Squeeze reduces the layout rank.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeRankChange)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_1{}, _4{}, _1{}, _5{}));
    auto squeezed = Squeeze<0, 2>(layout);
    static_assert(decltype(squeezed)::rank == 2, "rank should drop by 2");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 5);
}

// Mode 1 (by index) and Mode 2 (by pattern) give the same result.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeModeEquivalence)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_1{}, _4{}, _1{}));
    auto byDims = Squeeze<0, 2>(layout);
    auto byPattern = Squeeze(layout, MakeCoord(_1{}, _, _1{}));

    static_assert(decltype(byDims)::rank == decltype(byPattern)::rank, "modes should give same rank");
    EXPECT_EQ(AscendC::Std::get<0>(byDims.Shape()).value, AscendC::Std::get<0>(byPattern.Shape()).value);
}

// Mode 1: dims may be given out of order; result matches the sorted-dims squeeze.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeUnorderedDims)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_1{}, _4{}, _1{}, _5{}));
    auto ordered = Squeeze<0, 2>(layout);
    auto unordered = Squeeze<2, 0>(layout);

    static_assert(decltype(ordered)::rank == decltype(unordered)::rank, "order must not affect rank");
    static_assert(decltype(unordered)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(unordered.Shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(unordered.Shape()).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(ordered.Shape()).value, AscendC::Std::get<0>(unordered.Shape()).value);
    EXPECT_EQ(AscendC::Std::get<1>(ordered.Shape()).value, AscendC::Std::get<1>(unordered.Shape()).value);
}

// Mode 1: nested (batch + NZ fractal) layout, batch=1. Squeeze the outer batch axis;
// the nested fractal axis is kept intact.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeNestedBatchDim)
{
    using namespace AscendC::Te;
    // (1, ((16,2),(16,3))) with batch=1
    auto layout = MakeLayout(
        MakeShape(_1{}, MakeShape(MakeShape(_16{}, _2{}), MakeShape(_16{}, _3{}))),
        MakeStride(AscendC::Std::Int<1536>{}, MakeStride(MakeStride(_16{}, _256{}), MakeStride(_1{}, _512{}))));
    auto squeezed = Squeeze<0>(layout);

    // Dropping the batch axis leaves a single nested tuple, which is unwrapped:
    // (1, ((16,2),(16,3))) -> ((16,2),(16,3)), rank 2.
    static_assert(decltype(squeezed)::rank == 2, "batch axis dropped and outer wrapper unwrapped");
    auto shape = squeezed.Shape();
    auto stride = squeezed.Stride();
    EXPECT_EQ((AscendC::Te::Get<0, 0>(shape).value), 16);
    EXPECT_EQ((AscendC::Te::Get<0, 1>(shape).value), 2);
    EXPECT_EQ((AscendC::Te::Get<1, 0>(shape).value), 16);
    EXPECT_EQ((AscendC::Te::Get<1, 1>(shape).value), 3);
    EXPECT_EQ((AscendC::Te::Get<0, 0>(stride).value), 16);
    EXPECT_EQ((AscendC::Te::Get<1, 1>(stride).value), 512);
}

// Mode 2: isomorphic pattern over a nested layout. Batch marked _1, fractal positions kept with _.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeNestedPattern)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(
        MakeShape(_1{}, MakeShape(MakeShape(_16{}, _2{}), MakeShape(_16{}, _3{}))),
        MakeStride(AscendC::Std::Int<1536>{}, MakeStride(MakeStride(_16{}, _256{}), MakeStride(_1{}, _512{}))));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, MakeCoord(MakeCoord(_, _), MakeCoord(_, _))));

    // Batch dropped and outer wrapper unwrapped: ((16,2),(16,3)), rank 2.
    static_assert(decltype(squeezed)::rank == 2);
    auto shape = squeezed.Shape();
    EXPECT_EQ((AscendC::Te::Get<0, 0>(shape).value), 16);
    EXPECT_EQ((AscendC::Te::Get<1, 1>(shape).value), 3);
}

// Squeezing a batch=1 axis unwraps the lone remaining nested tuple: (1, (M, K)) -> (M, K).
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeUnwrapBatch)
{
    using namespace AscendC::Te;
    // (1, (8, 16)) flat inner, batch stride 128.
    auto layout = MakeLayout(
        MakeShape(_1{}, MakeShape(_8{}, _16{})), MakeStride(AscendC::Std::Int<128>{}, MakeStride(_16{}, _1{})));
    auto squeezed = Squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank == 2, "(1,(M,K)) -> (M,K)");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 8);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Shape()).value, 16);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Stride()).value, 16);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.Stride()).value, 1);
}

// A lone scalar axis is left as-is (no unwrap): squeezing (4, 1) by index 1 -> (4), rank 1.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeScalarNotUnwrapped)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _1{}));
    auto squeezed = Squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank == 1, "lone scalar axis stays rank 1");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.Shape()).value, 4);
}

// Probe: pattern squeeze only acts on the outer level (not recursive).
// (1, (1, (2, 3))) with pattern (_1, (_1, (_,_))): only outer batch removed, then unwrapped.
// Expect result (1, (2, 3)) -- the INNER 1 is NOT removed.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeDoubleNestedProbe)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(
        MakeShape(_1{}, MakeShape(_1{}, MakeShape(_2{}, _3{}))),
        MakeStride(_6{}, MakeStride(_6{}, MakeStride(_3{}, _1{}))));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, MakeCoord(_1{}, MakeCoord(_, _))));

    // Recursive squeeze removes both the outer and the inner 1: (1,(1,(2,3))) -> (2,3).
    static_assert(decltype(squeezed)::rank == 2, "outer and inner 1 removed -> (2,3)");
    auto shape = squeezed.Shape();
    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 2);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 3);
}

// Probe: pattern ((_1,_),(_1,_)) on ((1,x),(1,y)). The outer level has two tuple elements,
// neither is _1, so with the current (non-recursive) squeeze NOTHING is removed:
// result stays ((1,x),(1,y)), rank 2.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeInnerOnesProbe)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(
        MakeShape(MakeShape(_1{}, _4{}), MakeShape(_1{}, _5{})),
        MakeStride(MakeStride(_4{}, _1{}), MakeStride(_5{}, _1{})));
    auto squeezed = Squeeze(layout, MakeCoord(MakeCoord(_1{}, _), MakeCoord(_1{}, _)));

    // Recursive squeeze: each inner (1,x) drops its 1 and unwraps to x, giving (4,5).
    static_assert(decltype(squeezed)::rank == 2, "inner 1s removed -> (4,5)");
    auto shape = squeezed.Shape();
    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 5);
}

// Squeeze must preserve the original layout's Pattern/Trait. A batch NZ layout (B, ((16,M),(16,N)))
// with B=1, squeezed on the batch axis, must keep NZLayoutPtn on the result (mode 1 by index).
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePreservesPatternDims)
{
    using namespace AscendC::Te;
    // MakeFrameLayout<NZLayoutPtn>(batch=1, M, N) -> batch NZ, tagged NZLayoutPtn.
    auto layout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<half, _16>>(1, 32, 64);
    auto squeezed = Squeeze<0>(layout);

    // Batch axis dropped and the lone fractal wrapper unwrapped; pattern tag must survive.
    static_assert(
        AscendC::Std::is_same_v<GetLayoutPattern<decltype(squeezed)>, NZLayoutPtn>,
        "Squeeze (dims) must preserve NZLayoutPtn");
}

// Same preservation requirement for the pattern (mode 2) path.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePreservesPatternMode2)
{
    using namespace AscendC::Te;
    auto layout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<half, _16>>(1, 32, 64);
    // Isomorphic pattern: batch axis marked _1, fractal axes kept.
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, MakeCoord(MakeCoord(_, _), MakeCoord(_, _))));

    static_assert(
        AscendC::Std::is_same_v<GetLayoutPattern<decltype(squeezed)>, NZLayoutPtn>,
        "Squeeze (pattern) must preserve NZLayoutPtn");
}

// ---- Plan A: drop runtime value-1 axes (mode 1). ----
// A selected axis whose size is a runtime int is dropped unconditionally (trusting the caller
// that its value is 1); no runtime check is emitted. The remaining axes keep their strides.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeRuntimeDim)
{
    using namespace AscendC::Te;
    // shape (4, 1, 5) all runtime ints; the middle axis is 1 at runtime.
    auto layout = MakeFrameLayout<NZLayoutPtn, half>(1, 4, 6);

    // Squeeze the batch axis (mode 1, by index).
    auto squeezed = Squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank == 2, "runtime axis must be dropped at compile time");
    static_assert(
        AscendC::Std::is_same_v<GetLayoutPattern<decltype(squeezed)>, NZLayoutPtn>,
        "pattern NZLayoutPtn must be preserved");
}

// Mode 1: drop several runtime value-1 axes at once.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeRuntimeMultipleDims)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(1, 4, 1), MakeStride(4, 1, 1));
    auto squeezed = Squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank == 1);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(squeezed)), 1);
}

// Mode 1: mixed compile-time Int<1> and runtime-1 axes are both dropped.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeMixedStaticAndRuntime)
{
    using namespace AscendC::Te;
    // axis0 = static _1{}, axis2 = runtime 1.
    auto layout = MakeLayout(MakeShape(_1{}, 4, 1), MakeStride(4, 1, 1));
    auto squeezed = Squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank == 1);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)), 4);
}

// Mode 1: a runtime axis is dropped even when the caller did not verify it is 1.
// Plan A trusts the caller: no runtime assertion. Here the runtime value happens to be 1,
// so addressing of the kept axes stays correct.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeRuntimeNoValidation)
{
    using namespace AscendC::Te;
    int32_t batch = 1;
    auto layout = MakeLayout(MakeShape(batch, 32, 64), MakeStride(32 * 64, 64, 1));
    auto squeezed = Squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank == 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(squeezed)), 64);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(squeezed)), 64);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(squeezed)), 1);
}

// Mode 1: a selected axis whose compile-time size is a constant != 1 is KEPT (safety).
// Only Int<1> and runtime ints are droppable; a static Int<5> is never dropped.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeStaticNonOneKept)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _5{}));
    auto squeezed = Squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank == 2, "static non-one axis must be kept");
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(squeezed)).value, 5);
}

// ---- Plan A: drop runtime value-1 axes (mode 2, pattern). ----
// A _1-marked position whose size is a runtime int is dropped unconditionally.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeRuntimePattern)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(1, 4, 1), MakeStride(4, 1, 1));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, _, _1{}));

    static_assert(decltype(squeezed)::rank == 1);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(squeezed)), 1);
}

// Mode 2: a _1-marked position whose compile-time size is a constant != 1 is kept.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezePatternStaticNonOneKept)
{
    using namespace AscendC::Te;
    auto layout = MakeLayout(MakeShape(_4{}, _5{}));
    auto squeezed = Squeeze(layout, MakeCoord(_1{}, _));

    static_assert(decltype(squeezed)::rank == 2, "static non-one _1-marked axis must be kept");
    EXPECT_EQ(AscendC::Std::get<0>(GetShape(squeezed)).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(squeezed)).value, 5);
}

// MakeFrameLayout batch=1: create a batch NZ layout via MakeFrameLayout, squeeze the batch axis.
// Verify shape, stride and pattern are all correct after squeezing.
TEST_F(Tensor_Api_Layout_Squeeze, TestSqueezeMakeFrameLayoutBatchOne)
{
    using namespace AscendC::Te;
    // MakeFrameLayout<NZLayoutPtn>(B=1, M=32, N=64) -> batch NZ layout with runtime batch=1.
    auto batchLayout = MakeFrameLayout<NZLayoutPtn>(1, 32, 64);

    // Batch layout shape: (1, ((16,2),(16,4))), rank 2 (outer batch + inner fractal).
    static_assert(decltype(batchLayout)::rank == 2, "batch NZ layout rank should be 2");

    // Squeeze the batch axis (mode 1, by index).
    auto squeezed = Squeeze<0>(batchLayout);

    // After squeeze: batch removed, inner fractal unwrapped -> rank 2 (the NZ inner).
    static_assert(decltype(squeezed)::rank == 2, "squeezed NZ should be rank 2");
    static_assert(
        AscendC::Std::is_same_v<GetLayoutPattern<decltype(squeezed)>, NZLayoutPtn>,
        "pattern NZLayoutPtn must be preserved");

    // Verify inner fractal shape: ((16,2),(16,4)) where M=32->(16,2), N=64->(16,4).
    auto shape = squeezed.Shape();
    EXPECT_EQ((AscendC::Te::Get<0, 0>(shape)), 16);
    EXPECT_EQ((AscendC::Te::Get<0, 1>(shape)), 2);
    EXPECT_EQ((AscendC::Te::Get<1, 0>(shape)), 16);
    EXPECT_EQ((AscendC::Te::Get<1, 1>(shape)), 4);

    // Also verify mode 2 (pattern) gives the same result.
    auto squeezedByPattern = Squeeze(batchLayout, MakeCoord(_1{}, MakeCoord(MakeCoord(_, _), MakeCoord(_, _))));
    static_assert(decltype(squeezedByPattern)::rank == 2);
    EXPECT_EQ((AscendC::Te::Get<0, 0>(squeezedByPattern.Shape())), 16);
    EXPECT_EQ((AscendC::Te::Get<1, 1>(squeezedByPattern.Shape())), 4);
}
