/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/


#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/tensor/layout_frame.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
* \file layout_frame.h
* \brief
*/
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H

#include "impl/tensor_api/tensor/layout_pattern.h"

namespace AscendC {
namespace Te {

using LayoutFormatSet = TupleMap<
    Std::tuple<NZLayoutPtn, MakeNzFrameLayout>,
    Std::tuple<NDLayoutPtn, MakeNDFrameLayout>,
    Std::tuple<DNLayoutPtn, MakeDNFrameLayout>,
    Std::tuple<NDExtLayoutPtn, MakeNDExtFrameLayout>,
    Std::tuple<DNExtLayoutPtn, MakeDNExtFrameLayout>,
    Std::tuple<NNLayoutPtn, MakeNnFrameLayout>,
    Std::tuple<ZZLayoutPtn, MakeZzFrameLayout>,
    Std::tuple<ZNLayoutPtn, MakeZnFrameLayout>,
    Std::tuple<ScaleANDLayoutPtn, MakeScaleANDFrameLayout>,
    Std::tuple<ScaleADNLayoutPtn, MakeScaleADNFrameLayout>,
    Std::tuple<ScaleBNDLayoutPtn, MakeScaleBNDFrameLayout>,
    Std::tuple<ScaleBDNLayoutPtn, MakeScaleBDNFrameLayout>,
    Std::tuple<NCHWLayoutPtn, MakeNCHWFrameLayout>,
    Std::tuple<NHWCLayoutPtn, MakeNHWCFrameLayout>,
    Std::tuple<NC1HWC0LayoutPtn, MakeNC1HWC0FrameLayout>,
    Std::tuple<NCDHWLayoutPtn, MakeNCDHWFrameLayout>,
    Std::tuple<NDC1HWC0LayoutPtn, MakeNDC1HWC0FrameLayout>>;

template <typename T, typename C0>
struct LayoutTrait {
    static constexpr auto C0_ELEMENT = C0{};
};

template <typename T = uint16_t, typename C0 = Std::Int<C0_ELEMENT<T>>>
struct LayoutTraitDefault : LayoutTrait<T, C0> {};

struct LayoutTraitScale : LayoutTraitDefault<fp8_e8m0_t, Std::Int<MX_SCALE_K0 / sizeof(fp8_e8m0_t)>> {};

struct LayoutTraitFP4 : LayoutTraitDefault<fp4x2_e2m1_t, Std::Int<C0_ELEMENT<fp4x2_e2m1_t>>> {};

using FormatTraitSet = TupleMap<
    Std::tuple<NZLayoutPtn, LayoutTraitDefault<>>,
    Std::tuple<NDLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<DNLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NDExtLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<DNExtLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ZZLayoutPtn, LayoutTraitDefault<>>,
    Std::tuple<ZNLayoutPtn, LayoutTraitDefault<>>,
    Std::tuple<ScaleANDLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleADNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleBNDLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleBDNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<NCHWLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NHWCLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NC1HWC0LayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NCDHWLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NDC1HWC0LayoutPtn, LayoutTrait<Std::ignore_t, _1>>>;
    
template <typename T, typename = void>
struct IsFrameLayoutTrait : Std::false_type {};

template <typename T>
struct IsFrameLayoutTrait<T, void_t<decltype(T::C0_ELEMENT)>> : Std::true_type {};

template <typename T>
constexpr bool IsFrameLayoutTraitV = IsFrameLayoutTrait<T>::value;

template <typename LayoutPattern, typename TraitType>
struct TraitConversion {
private:
    using RawTrait = Std::remove_cvref_t<TraitType>;

    static_assert(
        Std::is_same_v<RawTrait, Std::ignore_t> ||
        IsIntegralConstantV<RawTrait> ||
        IsFrameLayoutTraitV<RawTrait> ||
        IsDataType<RawTrait>,
        "TraitType must be ignore_t, integral constant, frame layout trait, or data type."
    );

    using FromPattern = typename FormatTraitSet::template Get<LayoutPattern>;
    using FromInt = LayoutTrait<Std::ignore_t, RawTrait>;
    using FromDataType = LayoutTraitDefault<RawTrait>;
    using FromTrait = RawTrait;

    using Converted = typename Std::conditional<
        Std::is_same_v<RawTrait, Std::ignore_t>,
        FromPattern,
        typename Std::conditional<
            IsIntegralConstantV<RawTrait>,
            FromInt,
            typename Std::conditional<
                IsFrameLayoutTraitV<RawTrait>,
                FromTrait,
                FromDataType
            >::type
        >::type
    >::type;

public:
    using type = Converted;
};

// ===== Multi-batch frame layout: MakeFrameLayout<Ptn>(batch0, ..., batchN, row, col) =====

// SupportsRowColMake: true when the pattern's Make accepts a bare (row, column) pair, i.e. it is one
// of the matrix layouts that also offer the (batch, row, column) form. The conv feature-map patterns
// (NCHW/NHWC/NC1HWC0/NCDHW/NDC1HWC0) take fixed positional dimensions and have no 2-arg Make, so they
// never take the multi-batch path below.
template <typename Maker, typename Trait, typename = void>
struct SupportsRowColMake : Std::false_type {};

template <typename Maker, typename Trait>
struct SupportsRowColMake<
    Maker, Trait, void_t<decltype(Maker::template Make<Trait>(Std::declval<int>(), Std::declval<int>()))>>
    : Std::true_type {};

template <typename Maker, typename Trait>
constexpr bool SupportsRowColMakeV = SupportsRowColMake<Maker, Trait>::value;

// BatchStrideAt: stride of batch axis I, given the batch extents tuple and the base layout capacity.
// Batch axes are flat and row-major over the base block, so the stride of an axis is the product of
// all batch extents to its right times the base capacity:
//   stride[n-1] = capacity, stride[n-2] = batch[n-1]*capacity, ...
template <size_t I, size_t BatchNum, typename BatchTuple, typename Capacity>
__aicore__ inline constexpr auto BatchStrideAt(const BatchTuple& batches, const Capacity& capacity)
{
    if constexpr (I + 1 >= BatchNum) {
        return capacity;
    } else {
        return Get<I + 1>(batches) * BatchStrideAt<I + 1, BatchNum, BatchTuple, Capacity>(batches, capacity);
    }
}

// Builds the flat multi-batch layout: all batch axes sit side by side in the outermost tuple and the
// base (row, col) block is appended as the last element, e.g.
//   (batch0, batch1, ((row0, row1), (col0, col1)))
// so rank is batchNum + 1 (not a chain of nested single-batch layouts).
template <typename LayoutPattern, typename Trait, typename BatchTuple, typename BaseLayout, size_t... BatchIs>
__aicore__ inline constexpr auto MakeFlatBatchLayout(
    const BatchTuple& batches, const BaseLayout& base, Std::index_sequence<BatchIs...>)
{
    constexpr size_t batchNum = sizeof...(BatchIs);
    auto capacity = base.Capacity();
    return MakePatternLayout<LayoutPattern, Trait>(
        MakeShape(Get<BatchIs>(batches)..., base.Shape()),
        MakeStride(BatchStrideAt<BatchIs, batchNum, BatchTuple, decltype(capacity)>(batches, capacity)...,
                   base.Stride()));
}

// Peels the trailing (row, column) off the argument pack, builds the 2D base layout from them, then
// lays the leading arguments out as flat batch axes.
template <typename LayoutPattern, typename Trait, typename Maker, typename ArgsTuple, size_t... BatchIs>
__aicore__ inline constexpr auto MakeMultiBatchFrameLayoutImpl(
    const ArgsTuple& args, Std::index_sequence<BatchIs...>)
{
    constexpr size_t argNum = Std::tuple_size_v<ArgsTuple>;
    auto base = Maker::template Make<Trait>(Get<argNum - 2>(args), Get<argNum - 1>(args));
    auto batches = Std::make_tuple(Get<BatchIs>(args)...);
    return MakeFlatBatchLayout<LayoutPattern, Trait>(batches, base, Std::index_sequence<BatchIs...>{});
}

template <typename LayoutPattern, typename Trait, typename Maker, typename... Args>
__aicore__ inline constexpr auto MakeMultiBatchFrameLayout(const Args&... args)
{
    return MakeMultiBatchFrameLayoutImpl<LayoutPattern, Trait, Maker>(
        Std::make_tuple(args...), Std::make_index_sequence<sizeof...(Args) - 2>{});
}

template <typename LayoutPattern, typename TraitType = Std::ignore_t, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args) {
    using Trait = typename TraitConversion<LayoutPattern, TraitType>::type;
    using LayoutMaker = typename LayoutFormatSet::template Get<LayoutPattern>;
    static_assert(!Std::is_same_v<LayoutMaker, Std::ignore_t>, "Unsupported layout pattern.");
    // 2 args = (row, col) and 3 args = (batch, row, col) are handled by the pattern's own Make. Four
    // or more args on a row/col-style pattern means multiple batch axes: (batch0, ..., batchN, row, col).
    if constexpr (sizeof...(Args) > 3 && SupportsRowColMakeV<LayoutMaker, Trait>) {
        return MakeMultiBatchFrameLayout<LayoutPattern, Trait, LayoutMaker>(args...);
    } else {
        return LayoutMaker::template Make<Trait>(args...);
    }
}

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args) {
    return MakeFrameLayout<LayoutPattern, Std::Int<C0Element>>(args...);
}

template <typename LayoutPattern, typename TraitType = LayoutTraitDefault<>>
struct FrameLayoutFormat {
    template <typename... Args>
    __aicore__ inline constexpr decltype(auto) operator()(const Args&... args) {
        return MakeFrameLayout<LayoutPattern, TraitType>(args...);
    }
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
