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
#warning \
    "impl/tensor_api/tensor/layout_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/layout_method.h"
#include "impl/tensor_api/tensor/coord_index.h"
#include "impl/tensor_api/tensor/layout_fractal.h"
#include "impl/tensor_api/tensor/layout_frame.h"
#include "impl/tensor_api/tensor/layout_infer.h"

namespace AscendC {
namespace Te {

struct MinOp {
    template <typename T, typename U>
    __aicore__ inline constexpr auto operator()(const T& src, const U& dst) const
    {
        return Std::min(src, dst);
    }
};

struct DiffOp {
    template <typename T, typename U>
    __aicore__ inline constexpr auto operator()(const T& shape, const U& coord) const
    {
        return shape - coord;
    }
};

template <typename Coord, typename LayoutType>
__aicore__ inline decltype(auto) MakeCoordLayout(const Coord& coord, const LayoutType& layout)
{
    using ShapeType = Std::remove_cvref_t<decltype(layout.Shape())>;
    using CoordType = Std::remove_cvref_t<Coord>;
    static_assert(IsLayoutV<LayoutType> && Std::is_tuple_v<CoordType>, "LayoutType must be Layout");
    static_assert(
        NestingDepthV<ShapeType> == NestingDepthV<CoordType> &&
            Std::tuple_size_v<ShapeType> == Std::tuple_size_v<CoordType>,
        "Shape and coord must have same tuple structure");
    auto coordShape = TransformTupleApply(layout.Shape(), coord, DiffOp{});
    using TraitType = GetLayoutTrait<LayoutType>;
    using PatternType = GetLayoutPattern<LayoutType>;
    return MakePatternLayout<PatternType, TraitType>(coordShape, layout.Stride());
}

template <typename LayoutType, typename ShapeType>
__aicore__ inline decltype(auto) MakeSlicePatternLayout(const LayoutType& layout, const ShapeType& shape)
{
    using TraitType = GetLayoutTrait<LayoutType>;
    using PatternType = GetLayoutPattern<LayoutType>;
    return MakePatternLayout<PatternType, TraitType>(shape, layout.Stride());
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeSameShapeSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    auto coordLayout = MakeCoordLayout(coord, layout);
    auto realShape = TransformTupleApply(coordLayout.Shape(), sliceShape, MinOp{});
    return MakeSlicePatternLayout(layout, realShape);
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeFourDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == TWO_DIM_DATA, "SliceShape must be Two Dim when layout is Four Dim");
    auto innerRow = Get<0, 0>(layout.Shape());
    auto innerCol = Get<1, 0>(layout.Shape());

    auto srcRow = innerRow * Get<0, 1>(layout.Shape()) - Get<0>(coord);
    auto srcCol = innerCol * Get<1, 1>(layout.Shape()) - Get<1>(coord);

    auto realRow = Std::min(srcRow, Get<0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1>(sliceShape));
    return MakeSlicePatternLayout(layout, MakeFractalShape(MakeShape(realRow, realCol), MakeShape(innerRow, innerCol)));
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeFiveDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == THREE_DIM_DATA, "SliceShape must be Three Dim when layout is Five Dim");
    auto innerRow = Get<1, 0, 0>(layout.Shape());
    auto innerCol = Get<1, 1, 0>(layout.Shape());

    auto srcBatch = Get<0>(layout.Shape()) - Get<0>(coord);
    auto srcRow = innerRow * Get<1, 0, 1>(layout.Shape()) - Get<1, 0>(coord);
    auto srcCol = innerCol * Get<1, 1, 1>(layout.Shape()) - Get<1, 1>(coord);

    auto realBatch = Std::min(srcBatch, Get<0>(sliceShape));
    auto realRow = Std::min(srcRow, Get<1, 0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1, 1>(sliceShape));
    auto fractalShape = MakeFractalShape(MakeShape(realRow, realCol), MakeShape(innerRow, innerCol));
    return MakeSlicePatternLayout(layout, MakeShape(realBatch, fractalShape));
}

template <typename Coord, typename LayoutType, typename SliceShape, Std::enable_if_t<!IsLayoutV<SliceShape>, int> = 0>
__aicore__ inline decltype(auto) MakeSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(IsLayoutV<LayoutType>, "LayoutType must be Layout");
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<SliceShape>>, "SliceShape must be a tuple");
    using OriginShape = Std::remove_cvref_t<decltype(layout.Shape())>;
    using SliceShapeType = Std::remove_cvref_t<SliceShape>;
    constexpr auto originShapeDepth = NestingDepthV<OriginShape>;
    constexpr auto sliceShapeDepth = NestingDepthV<SliceShapeType>;
    constexpr bool isSameShape =
        originShapeDepth == sliceShapeDepth && Std::tuple_size_v<OriginShape> == Std::tuple_size_v<SliceShapeType>;

    if constexpr (isSameShape) {
        return MakeSameShapeSliceLayout(coord, layout, sliceShape);
    } else if constexpr (originShapeDepth == FOUR_DIM_DATA && sliceShapeDepth == TWO_DIM_DATA) {
        return MakeFourDimSliceLayout(coord, layout, sliceShape);
    } else if constexpr (originShapeDepth == FIVE_DIM_DATA && sliceShapeDepth == THREE_DIM_DATA) {
        return MakeFiveDimSliceLayout(coord, layout, sliceShape);
    } else {
        static_assert(
            isSameShape || (originShapeDepth == FOUR_DIM_DATA && sliceShapeDepth == TWO_DIM_DATA) ||
                (originShapeDepth == FIVE_DIM_DATA && sliceShapeDepth == THREE_DIM_DATA),
            "SliceShape must be same structure as Layout shape, or logical Two Dim Shape for Four Dim Layout, "
            "or logical Three Dim Shape for Five Dim Layout.");
    }
}

template <
    typename Coord, typename SrcLayoutType, typename DstLayoutType, Std::enable_if_t<IsLayoutV<DstLayoutType>, int> = 0>
__aicore__ inline decltype(auto) MakeSliceLayout(
    const Coord& coord, const SrcLayoutType& srcLayout, const DstLayoutType& dstLayout)
{
    static_assert(IsLayoutV<SrcLayoutType>, "SrcLayoutType must be Layout");
    static_assert(SrcLayoutType::rank == DstLayoutType::rank, "SrcLayout Rank must be equal to DstLayout Rank");

    auto sliceLayout = MakeCoordLayout(coord, srcLayout);
    auto sliceShape = TransformTupleApply(sliceLayout.Shape(), dstLayout.Shape(), MinOp{});
    using TraitType = GetLayoutTrait<SrcLayoutType>;
    using PatternType = GetLayoutPattern<SrcLayoutType>;
    return MakePatternLayout<PatternType, TraitType>(sliceShape, srcLayout.Stride());
}

template <typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr decltype(auto) RemoveBatchDim(const LayoutType& layout)
{
    constexpr auto layoutDepth = LayoutType::depth;
    static_assert(
        layoutDepth == THREE_DIM_DATA || layoutDepth == FIVE_DIM_DATA,
        "RemoveBatchDim only supports Three Dim or Five Dim Layout.");
    using TraitType = GetLayoutTrait<LayoutType>;
    using PatternType = GetLayoutPattern<LayoutType>;
    return MakePatternLayout<PatternType, TraitType>(Get<1>(layout.Shape()), Get<1>(layout.Stride()));
}

// ===== Squeeze =====

// IsInPack: true if I is in the pack.
template <size_t I, size_t... Dims>
struct IsInPack : Std::false_type {};
template <size_t I, size_t Head, size_t... Tail>
struct IsInPack<I, Head, Tail...> : Std::conditional_t<Head == I, Std::true_type, IsInPack<I, Tail...>> {};
template <size_t I, size_t... Dims>
constexpr bool IsInPackV = IsInPack<I, Dims...>::value;

// IsSqueezeMark: pattern position marked _1.
template <typename T>
struct IsSqueezeMark : Std::is_same<Std::remove_cvref_t<T>, Std::Int<1>> {};
template <typename T>
constexpr bool IsSqueezeMarkV = IsSqueezeMark<Std::remove_cvref_t<T>>::value;

// SqueezableSize: true if axis size is Int<1> or a runtime int (trust caller).
template <typename T, bool IsConst = IsIntegralConstantV<Std::remove_cvref_t<T>>>
struct SqueezableSize : Std::true_type {};
template <typename T>
struct SqueezableSize<T, true> : Std::bool_constant<(Std::remove_cvref_t<T>::value == 1)> {};
template <typename T>
constexpr bool SqueezableSizeV = SqueezableSize<Std::remove_cvref_t<T>>::value;

// DimsMask: keep mask. Drop axis I iff I in SqueezeDims && SqueezableSize.
template <typename Shape, typename DimsSeq, typename IdxSeq>
struct DimsMaskImpl;
template <typename Shape, size_t... Dims, size_t... Is>
struct DimsMaskImpl<Shape, Std::index_sequence<Dims...>, Std::index_sequence<Is...>> {
    using type = Std::IntegerSequence<
        bool, (!(IsInPackV<Is, Dims...> && SqueezableSizeV<typename Std::tuple_element<Is, Shape>::type>))...>;
};
template <typename Shape, size_t... Dims>
using DimsMask = typename DimsMaskImpl<
    Shape, Std::index_sequence<Dims...>, Std::make_index_sequence<Std::tuple_size_v<Shape>>>::type;

// PatternMask: keep mask. Drop axis I iff pattern[I] is _1 && SqueezableSize.
template <typename Shape, typename Pattern, typename IdxSeq>
struct PatternMaskImpl;
template <typename Shape, typename Pattern, size_t... Is>
struct PatternMaskImpl<Shape, Pattern, Std::index_sequence<Is...>> {
    using type = Std::IntegerSequence<
        bool, (!(IsSqueezeMarkV<typename Std::tuple_element<Is, Pattern>::type> &&
                 SqueezableSizeV<typename Std::tuple_element<Is, Shape>::type>))...>;
};
template <typename Shape, typename Pattern>
using PatternMask = typename PatternMaskImpl<Shape, Pattern, Std::make_index_sequence<Std::tuple_size_v<Shape>>>::type;

// Promote single nested tuple after squeeze; preserve Pattern/Trait.
template <typename Pattern, typename Trait, typename ShapeT, typename StrideT>
__aicore__ inline constexpr auto MakeSqueezedLayout(const ShapeT& shape, const StrideT& stride)
{
    if constexpr (
        Std::tuple_size_v<ShapeT> == 1 &&
        Std::is_tuple_v<Std::remove_cvref_t<typename Std::tuple_element<0, ShapeT>::type>>) {
        return MakePatternLayout<Pattern, Trait>(Get<0>(shape), Get<0>(stride));
    } else {
        return MakePatternLayout<Pattern, Trait>(shape, stride);
    }
}

// Squeeze mode 1: drop selected axes by index (if squeezable). Preserves Pattern/Trait.
template <
    size_t... SqueezeDims, typename LayoutType,
    typename = Std::enable_if_t<IsLayoutV<LayoutType> && (sizeof...(SqueezeDims) > 0)>>
__aicore__ inline constexpr auto SqueezeLayout(const LayoutType& layout)
{
    using Shape = Std::remove_cvref_t<decltype(layout.Shape())>;
    using Mask = DimsMask<Shape, SqueezeDims...>;
    using KeepSeq = typename KeepIndexSeqFromSeq<Mask>::type;
    using Pattern = GetLayoutPattern<LayoutType>;
    using Trait = GetLayoutTrait<LayoutType>;
    return MakeSqueezedLayout<Pattern, Trait>(
        SelectBySeq<KeepSeq>(layout.Shape()), SelectBySeq<KeepSeq>(layout.Stride()));
}

// ---- Recursive pattern squeeze (mode 2) ----

template <typename S, typename D, typename P>
__aicore__ inline constexpr auto SqueezeRecTupleLevel(const S& s, const D& d, const P& p);

// Recurse into tuple pattern nodes; keep leaves as-is.
template <typename SNode, typename DNode, typename PNode>
__aicore__ inline constexpr auto SqueezeRecNode(const SNode& s, const DNode& d, const PNode& p)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<PNode>>) {
        return SqueezeRecTupleLevel(s, d, p);
    } else {
        return Std::make_tuple(s, d);
    }
}

// Unwrap single-element tuple pair one layer.
template <typename S, typename D>
__aicore__ inline constexpr auto SqueezeUnwrap(const S& s, const D& d)
{
    if constexpr (Std::tuple_size_v<Std::remove_cvref_t<S>> == 1) {
        return Std::make_tuple(Get<0>(s), Get<0>(d));
    } else {
        return Std::make_tuple(s, d);
    }
}

template <typename S, typename D, typename P, size_t... Is>
__aicore__ inline constexpr auto SqueezeRecTupleLevelImpl(
    const S& s, const D& d, const P& p, Std::index_sequence<Is...>)
{
    auto tShape = Std::make_tuple(Get<0>(SqueezeRecNode(Get<Is>(s), Get<Is>(d), Get<Is>(p)))...);
    auto tStride = Std::make_tuple(Get<1>(SqueezeRecNode(Get<Is>(s), Get<Is>(d), Get<Is>(p)))...);
    using Mask = PatternMask<Std::remove_cvref_t<S>, Std::remove_cvref_t<P>>;
    using KeepSeq = typename KeepIndexSeqFromSeq<Mask>::type;
    return SqueezeUnwrap(SelectBySeq<KeepSeq>(tShape), SelectBySeq<KeepSeq>(tStride));
}

template <typename S, typename D, typename P>
__aicore__ inline constexpr auto SqueezeRecTupleLevel(const S& s, const D& d, const P& p)
{
    return SqueezeRecTupleLevelImpl(s, d, p, Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<S>>>{});
}

// Squeeze mode 2: recursively drop _1-marked positions (if squeezable). Preserves Pattern/Trait.
template <
    typename Pattern, typename LayoutType,
    typename = Std::enable_if_t<IsLayoutV<LayoutType> && Std::is_tuple_v<Std::remove_cvref_t<Pattern>>>>
__aicore__ inline constexpr auto SqueezeLayout(const LayoutType& layout, const Pattern& pattern)
{
    using Shape = Std::remove_cvref_t<decltype(layout.Shape())>;
    static_assert(IsSameStructureV<Pattern, Shape>, "Squeeze pattern must be isomorphic to the layout shape.");
    using LayoutPatternType = GetLayoutPattern<LayoutType>;
    using LayoutTraitType = GetLayoutTrait<LayoutType>;
    auto pr = SqueezeRecTupleLevel(layout.Shape(), layout.Stride(), pattern);
    auto outShape = Get<0>(pr);
    auto outStride = Get<1>(pr);
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<decltype(outShape)>>) {
        return MakePatternLayout<LayoutPatternType, LayoutTraitType>(outShape, outStride);
    } else {
        return MakePatternLayout<LayoutPatternType, LayoutTraitType>(
            Std::make_tuple(outShape), Std::make_tuple(outStride));
    }
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
