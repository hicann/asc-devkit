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
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class tensor_api_layout_squeeze : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

// Mode 1: squeeze a single axis by index when its compile-time size is 1.
TEST_F(tensor_api_layout_squeeze, test_squeeze_single_dim)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _1{}, _5{}));
    auto squeezed = squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.stride()).value, 5);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.stride()).value, 1);
}

// Mode 1: squeeze multiple axes by index.
TEST_F(tensor_api_layout_squeeze, test_squeeze_multiple_dims)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_1{}, _4{}, _1{}));
    auto squeezed = squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank_size == 1);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
}

// Mode 1: a named axis whose size is not 1 is kept (no error).
TEST_F(tensor_api_layout_squeeze, test_squeeze_no_op_when_size_not_one)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _5{}));
    auto squeezed = squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 5);
}

// Mode 2: pattern tuple marks squeeze positions with _1, keep with _.
TEST_F(tensor_api_layout_squeeze, test_squeeze_pattern_flat)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_1{}, _4{}, _1{}));
    auto squeezed = squeeze(layout, make_coord(_1{}, _, _1{}));

    static_assert(decltype(squeezed)::rank_size == 1);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
}

// Mode 2: all positions kept.
TEST_F(tensor_api_layout_squeeze, test_squeeze_pattern_keep_all)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _5{}));
    auto squeezed = squeeze(layout, make_coord(_, _));

    static_assert(decltype(squeezed)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 5);
}

// Mode 2: a position marked _1 but whose size is not 1 is kept (no error).
TEST_F(tensor_api_layout_squeeze, test_squeeze_pattern_mark_non_one)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _5{}));
    auto squeezed = squeeze(layout, make_coord(_1{}, _));

    static_assert(decltype(squeezed)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 5);
}

// squeeze reduces the layout rank.
TEST_F(tensor_api_layout_squeeze, test_squeeze_rank_change)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_1{}, _4{}, _1{}, _5{}));
    auto squeezed = squeeze<0, 2>(layout);
    static_assert(decltype(squeezed)::rank_size == 2, "rank should drop by 2");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 5);
}

// Mode 1 (by index) and Mode 2 (by pattern) give the same result.
TEST_F(tensor_api_layout_squeeze, test_squeeze_mode_equivalence)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_1{}, _4{}, _1{}));
    auto by_dims = squeeze<0, 2>(layout);
    auto by_pattern = squeeze(layout, make_coord(_1{}, _, _1{}));

    static_assert(decltype(by_dims)::rank_size == decltype(by_pattern)::rank_size, "modes should give same rank");
    EXPECT_EQ(AscendC::Std::get<0>(by_dims.shape()).value, AscendC::Std::get<0>(by_pattern.shape()).value);
}

// Mode 1: dims may be given out of order; result matches the sorted-dims squeeze.
TEST_F(tensor_api_layout_squeeze, test_squeeze_unordered_dims)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_1{}, _4{}, _1{}, _5{}));
    auto ordered = squeeze<0, 2>(layout);
    auto unordered = squeeze<2, 0>(layout);

    static_assert(decltype(ordered)::rank_size == decltype(unordered)::rank_size, "order must not affect rank");
    static_assert(decltype(unordered)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(unordered.shape()).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(unordered.shape()).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(ordered.shape()).value, AscendC::Std::get<0>(unordered.shape()).value);
    EXPECT_EQ(AscendC::Std::get<1>(ordered.shape()).value, AscendC::Std::get<1>(unordered.shape()).value);
}

// Mode 1: nested (batch + NZ fractal) layout, batch=1. squeeze the outer batch axis;
// the nested fractal axis is kept intact.
TEST_F(tensor_api_layout_squeeze, test_squeeze_nested_batch_dim)
{
    using namespace asc::te;
    // (1, ((16,2),(16,3))) with batch=1
    auto layout = make_layout(
        make_shape(_1{}, make_shape(make_shape(_16{}, _2{}), make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<1536>{}, make_stride(make_stride(_16{}, _256{}), make_stride(_1{}, _512{}))));
    auto squeezed = squeeze<0>(layout);

    // Dropping the batch axis leaves a single nested tuple, which is unwrapped:
    // (1, ((16,2),(16,3))) -> ((16,2),(16,3)), rank 2.
    static_assert(decltype(squeezed)::rank_size == 2, "batch axis dropped and outer wrapper unwrapped");
    auto shape = squeezed.shape();
    auto stride = squeezed.stride();
    EXPECT_EQ((asc::te::get<0, 0>(shape).value), 16);
    EXPECT_EQ((asc::te::get<0, 1>(shape).value), 2);
    EXPECT_EQ((asc::te::get<1, 0>(shape).value), 16);
    EXPECT_EQ((asc::te::get<1, 1>(shape).value), 3);
    EXPECT_EQ((asc::te::get<0, 0>(stride).value), 16);
    EXPECT_EQ((asc::te::get<1, 1>(stride).value), 512);
}

// Mode 2: isomorphic pattern over a nested layout. Batch marked _1, fractal positions kept with _.
TEST_F(tensor_api_layout_squeeze, test_squeeze_nested_pattern)
{
    using namespace asc::te;
    auto layout = make_layout(
        make_shape(_1{}, make_shape(make_shape(_16{}, _2{}), make_shape(_16{}, _3{}))),
        make_stride(AscendC::Std::Int<1536>{}, make_stride(make_stride(_16{}, _256{}), make_stride(_1{}, _512{}))));
    auto squeezed = squeeze(layout, make_coord(_1{}, make_coord(make_coord(_, _), make_coord(_, _))));

    // Batch dropped and outer wrapper unwrapped: ((16,2),(16,3)), rank 2.
    static_assert(decltype(squeezed)::rank_size == 2);
    auto shape = squeezed.shape();
    EXPECT_EQ((asc::te::get<0, 0>(shape).value), 16);
    EXPECT_EQ((asc::te::get<1, 1>(shape).value), 3);
}

// Squeezing a batch=1 axis unwraps the lone remaining nested tuple: (1, (m_value, K)) -> (m_value, K).
TEST_F(tensor_api_layout_squeeze, test_squeeze_unwrap_batch)
{
    using namespace asc::te;
    // (1, (8, 16)) flat inner, batch stride 128.
    auto layout = make_layout(
        make_shape(_1{}, make_shape(_8{}, _16{})), make_stride(AscendC::Std::Int<128>{}, make_stride(_16{}, _1{})));
    auto squeezed = squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank_size == 2, "(1,(m_value,K)) -> (m_value,K)");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 8);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.shape()).value, 16);
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.stride()).value, 16);
    EXPECT_EQ(AscendC::Std::get<1>(squeezed.stride()).value, 1);
}

// A lone scalar axis is left as-is (no unwrap): squeezing (4, 1) by index 1 -> (4), rank 1.
TEST_F(tensor_api_layout_squeeze, test_squeeze_scalar_not_unwrapped)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _1{}));
    auto squeezed = squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank_size == 1, "lone scalar axis stays rank 1");
    EXPECT_EQ(AscendC::Std::get<0>(squeezed.shape()).value, 4);
}

// Probe: pattern squeeze only acts on the outer level (not recursive).
// (1, (1, (2, 3))) with pattern (_1, (_1, (_,_))): only outer batch removed, then unwrapped.
// Expect result (1, (2, 3)) -- the INNER 1 is NOT removed.
TEST_F(tensor_api_layout_squeeze, test_squeeze_double_nested_probe)
{
    using namespace asc::te;
    auto layout = make_layout(
        make_shape(_1{}, make_shape(_1{}, make_shape(_2{}, _3{}))),
        make_stride(_6{}, make_stride(_6{}, make_stride(_3{}, _1{}))));
    auto squeezed = squeeze(layout, make_coord(_1{}, make_coord(_1{}, make_coord(_, _))));

    // Recursive squeeze removes both the outer and the inner 1: (1,(1,(2,3))) -> (2,3).
    static_assert(decltype(squeezed)::rank_size == 2, "outer and inner 1 removed -> (2,3)");
    auto shape = squeezed.shape();
    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 2);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 3);
}

// Probe: pattern ((_1,_),(_1,_)) on ((1,x),(1,y)). The outer level has two tuple elements,
// neither is _1, so with the current (non-recursive) squeeze NOTHING is removed:
// result stays ((1,x),(1,y)), rank 2.
TEST_F(tensor_api_layout_squeeze, test_squeeze_inner_ones_probe)
{
    using namespace asc::te;
    auto layout = make_layout(
        make_shape(make_shape(_1{}, _4{}), make_shape(_1{}, _5{})),
        make_stride(make_stride(_4{}, _1{}), make_stride(_5{}, _1{})));
    auto squeezed = squeeze(layout, make_coord(make_coord(_1{}, _), make_coord(_1{}, _)));

    // Recursive squeeze: each inner (1,x) drops its 1 and unwraps to x, giving (4,5).
    static_assert(decltype(squeezed)::rank_size == 2, "inner 1s removed -> (4,5)");
    auto shape = squeezed.shape();
    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 5);
}

// squeeze must preserve the original layout's Pattern/trait_type. A batch NZ layout (batch_value, ((16,m_value),(16,n_value)))
// with batch_value=1, squeezed on the batch axis, must keep nz_layout_ptn on the result (mode 1 by index).
TEST_F(tensor_api_layout_squeeze, test_squeeze_preserves_pattern_dims)
{
    using namespace asc::te;
    // make_frame_layout<nz_layout_ptn>(batch=1, m_value, n_value) -> batch NZ, tagged nz_layout_ptn.
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<half, _16>>(1, 32, 64);
    auto squeezed = squeeze<0>(layout);

    // Batch axis dropped and the lone fractal wrapper unwrapped; pattern tag must survive.
    static_assert(
        AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed)>, nz_layout_ptn>,
        "squeeze (dims) must preserve nz_layout_ptn");
}

// Same preservation requirement for the pattern (mode 2) path.
TEST_F(tensor_api_layout_squeeze, test_squeeze_preserves_pattern_mode2)
{
    using namespace asc::te;
    auto layout = make_frame_layout<nz_layout_ptn, layout_trait_default<half, _16>>(1, 32, 64);
    // Isomorphic pattern: batch axis marked _1, fractal axes kept.
    auto squeezed = squeeze(layout, make_coord(_1{}, make_coord(make_coord(_, _), make_coord(_, _))));

    static_assert(
        AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed)>, nz_layout_ptn>,
        "squeeze (pattern) must preserve nz_layout_ptn");
}

// ---- Plan A: drop runtime value-1 axes (mode 1). ----
// A selected axis whose size is a runtime int is dropped unconditionally (trusting the caller
// that its value is 1); no runtime check is emitted. The remaining axes keep their strides.
TEST_F(tensor_api_layout_squeeze, test_squeeze_runtime_dim)
{
    using namespace asc::te;
    // shape (4, 1, 5) all runtime ints; the middle axis is 1 at runtime.
    auto layout = make_frame_layout<nz_layout_ptn, half>(1, 4, 6);

    // squeeze the batch axis (mode 1, by index).
    auto squeezed = squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank_size == 2, "runtime axis must be dropped at compile time");
    static_assert(
        AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed)>, nz_layout_ptn>,
        "pattern nz_layout_ptn must be preserved");
}

// Mode 1: drop several runtime value-1 axes at once.
TEST_F(tensor_api_layout_squeeze, test_squeeze_runtime_multiple_dims)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(1, 4, 1), make_stride(4, 1, 1));
    auto squeezed = squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank_size == 1);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(squeezed)), 1);
}

// Mode 1: mixed compile-time Int<1> and runtime-1 axes are both dropped.
TEST_F(tensor_api_layout_squeeze, test_squeeze_mixed_static_and_runtime)
{
    using namespace asc::te;
    // axis0 = static _1{}, axis2 = runtime 1.
    auto layout = make_layout(make_shape(_1{}, 4, 1), make_stride(4, 1, 1));
    auto squeezed = squeeze<0, 2>(layout);

    static_assert(decltype(squeezed)::rank_size == 1);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)), 4);
}

// Mode 1: a runtime axis is dropped even when the caller did not verify it is 1.
// Plan A trusts the caller: no runtime assertion. Here the runtime value happens to be 1,
// so addressing of the kept axes stays correct.
TEST_F(tensor_api_layout_squeeze, test_squeeze_runtime_no_validation)
{
    using namespace asc::te;
    int32_t batch = 1;
    auto layout = make_layout(make_shape(batch, 32, 64), make_stride(32 * 64, 64, 1));
    auto squeezed = squeeze<0>(layout);

    static_assert(decltype(squeezed)::rank_size == 2);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(squeezed)), 64);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(squeezed)), 64);
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(squeezed)), 1);
}

// Mode 1: a selected axis whose compile-time size is a constant != 1 is KEPT (safety).
// Only Int<1> and runtime ints are droppable; a static Int<5> is never dropped.
TEST_F(tensor_api_layout_squeeze, test_squeeze_static_non_one_kept)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _5{}));
    auto squeezed = squeeze<1>(layout);

    static_assert(decltype(squeezed)::rank_size == 2, "static non-one axis must be kept");
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(squeezed)).value, 5);
}

// ---- Plan A: drop runtime value-1 axes (mode 2, pattern). ----
// A _1-marked position whose size is a runtime int is dropped unconditionally.
TEST_F(tensor_api_layout_squeeze, test_squeeze_runtime_pattern)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(1, 4, 1), make_stride(4, 1, 1));
    auto squeezed = squeeze(layout, make_coord(_1{}, _, _1{}));

    static_assert(decltype(squeezed)::rank_size == 1);
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(squeezed)), 1);
}

// Mode 2: a _1-marked position whose compile-time size is a constant != 1 is kept.
TEST_F(tensor_api_layout_squeeze, test_squeeze_pattern_static_non_one_kept)
{
    using namespace asc::te;
    auto layout = make_layout(make_shape(_4{}, _5{}));
    auto squeezed = squeeze(layout, make_coord(_1{}, _));

    static_assert(decltype(squeezed)::rank_size == 2, "static non-one _1-marked axis must be kept");
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(squeezed)).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(squeezed)).value, 5);
}

// make_frame_layout batch=1: create a batch NZ layout via make_frame_layout, squeeze the batch axis.
// Verify shape, stride and pattern are all correct after squeezing.
TEST_F(tensor_api_layout_squeeze, test_squeeze_make_frame_layout_batch_one)
{
    using namespace asc::te;
    // make_frame_layout<nz_layout_ptn>(batch_value=1, m_value=32, n_value=64) -> batch NZ layout with runtime batch=1.
    auto batch_layout = make_frame_layout<nz_layout_ptn>(1, 32, 64);

    // Batch layout shape: (1, ((16,2),(16,4))), rank 2 (outer batch + inner fractal).
    static_assert(decltype(batch_layout)::rank_size == 2, "batch NZ layout rank should be 2");

    // squeeze the batch axis (mode 1, by index).
    auto squeezed = squeeze<0>(batch_layout);

    // After squeeze: batch removed, inner fractal unwrapped -> rank 2 (the NZ inner).
    static_assert(decltype(squeezed)::rank_size == 2, "squeezed NZ should be rank 2");
    static_assert(
        AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed)>, nz_layout_ptn>,
        "pattern nz_layout_ptn must be preserved");

    // Verify inner fractal shape: ((16,2),(16,4)) where m_value=32->(16,2), n_value=64->(16,4).
    auto shape = squeezed.shape();
    EXPECT_EQ((asc::te::get<0, 0>(shape)), 16);
    EXPECT_EQ((asc::te::get<0, 1>(shape)), 2);
    EXPECT_EQ((asc::te::get<1, 0>(shape)), 16);
    EXPECT_EQ((asc::te::get<1, 1>(shape)), 4);

    // Also verify mode 2 (pattern) gives the same result.
    auto squeezed_by_pattern = squeeze(batch_layout, make_coord(_1{}, make_coord(make_coord(_, _), make_coord(_, _))));
    static_assert(decltype(squeezed_by_pattern)::rank_size == 2);
    EXPECT_EQ((asc::te::get<0, 0>(squeezed_by_pattern.shape())), 16);
    EXPECT_EQ((asc::te::get<1, 1>(squeezed_by_pattern.shape())), 4);
}

// End-to-end on a two-batch NN layout: build (b0, b1, ((2,8),(16,2))) with the flat multi-batch
// make_frame_layout, Slice both batch axes down to 1, then squeeze the two now-degenerate batch axes
// away, leaving the bare NN fractal block. NN requires c0_value == 2.
TEST_F(tensor_api_layout_squeeze, test_squeeze_nn_two_batch_after_slice)
{
    using namespace asc::te;

    constexpr int batch0 = 2;
    constexpr int batch1 = 3;
    constexpr int row = 16;
    constexpr int col = 32;
    // NN(16,32) with c0_value=2: shape ((2,8),(16,2)), capacity 512.
    constexpr int base_capacity = row * col;

    __gm__ uint8_t data[batch0 * batch1 * base_capacity] = {};

    // Flat multi-batch NN layout: (2, 3, ((2,8),(16,2))), rank 3.
    auto layout = make_frame_layout<nn_layout_ptn, 2>(batch0, batch1, row, col);
    static_assert(decltype(layout)::rank_size == 3, "two flat batch axes + NN base block");
    EXPECT_EQ(AscendC::Std::get<0>(get_shape(layout)), batch0);
    EXPECT_EQ(AscendC::Std::get<1>(get_shape(layout)), batch1);
    EXPECT_EQ(AscendC::Std::get<0>(get_stride(layout)), batch1 * base_capacity); // 3 * 512
    EXPECT_EQ(AscendC::Std::get<1>(get_stride(layout)), base_capacity);          // 512

    auto tensor = make_tensor(make_mem_ptr<location::gm>(data), layout);

    // Slice both batch axes to 1, keeping the whole NN base block. The slice shape is isomorphic to
    // the layout shape: (1, 1, ((2,8),(16,2))).
    auto coord = make_coord(1, 2, make_coord(make_coord(0, 0), make_coord(0, 0)));
    auto slice_shape = make_shape(_1{}, _1{}, make_shape(make_shape(2, 8), make_shape(16, 2)));
    auto sliced = slice(tensor, coord, slice_shape);

    // Batch axes are now 1; the base block is untouched. Address moved to the (1,2) batch element.
    using sliced_layout = AscendC::Std::remove_cvref_t<decltype(sliced.layout())>;
    static_assert(sliced_layout::rank_size == 3);
    EXPECT_EQ(sliced.data(), tensor.data() + layout(coord));
    EXPECT_EQ(AscendC::Std::get<0>(sliced.shape()), 1);
    EXPECT_EQ(AscendC::Std::get<1>(sliced.shape()), 1);

    // squeeze the two degenerate batch axes; only the NN fractal block remains.
    auto squeezed = squeeze<0, 1>(sliced.layout());

    static_assert(decltype(squeezed)::rank_size == 2, "both batch axes dropped, NN block unwrapped");
    static_assert(
        AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed)>, nn_layout_ptn>,
        "squeeze must preserve nn_layout_ptn");

    // NN(16,32) fractal shape ((c0_value=2, row/c0_value=8), (16, ceil(32/16)=2)).
    auto shape = squeezed.shape();
    EXPECT_EQ((asc::te::get<0, 0>(shape)), 2);
    EXPECT_EQ((asc::te::get<0, 1>(shape)), 8);
    EXPECT_EQ((asc::te::get<1, 0>(shape)), 16);
    EXPECT_EQ((asc::te::get<1, 1>(shape)), 2);

    // Strides of the base block survive the slice + squeeze unchanged.
    auto stride = squeezed.stride();
    EXPECT_EQ((asc::te::get<0, 0>(stride)), 1);
    EXPECT_EQ((asc::te::get<0, 1>(stride)), 32); // c0_value * fractal_fixed = 2 * 16
    EXPECT_EQ((asc::te::get<1, 0>(stride)), 2);  // c0_value
    EXPECT_EQ((asc::te::get<1, 1>(stride)), 256); // row * fractal_fixed = 16 * 16

    // Mode 2 (isomorphic pattern) on the same sliced layout gives the same result.
    auto squeezed_by_pattern =
        squeeze(sliced.layout(), make_coord(_1{}, _1{}, make_coord(make_coord(_, _), make_coord(_, _))));
    static_assert(decltype(squeezed_by_pattern)::rank_size == 2);
    static_assert(AscendC::Std::is_same_v<get_layout_pattern<decltype(squeezed_by_pattern)>, nn_layout_ptn>);
    EXPECT_EQ((asc::te::get<0, 0>(squeezed_by_pattern.shape())), 2);
    EXPECT_EQ((asc::te::get<1, 1>(squeezed_by_pattern.shape())), 2);
}
