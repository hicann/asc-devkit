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
    "impl/tensor_api/tensor/layout_method.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_method.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_METHOD_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_METHOD_H

#include "impl/tensor_api/tensor/layout_definition.h"

namespace AscendC {
namespace Te {

template <typename... Ts>
struct HasZeroIntegralConstant : Std::bool_constant<(... || Std::is_same_v<Std::remove_cvref_t<Ts>, Std::Int<0>>)> {};

template <typename T, typename... Ts>
__aicore__ inline constexpr Shape<T, Ts...> MakeShape(const T& t, const Ts&... ts)
{
    static_assert(!HasZeroIntegralConstant<T, Ts...>::value, "MakeShape does not accept Int<0> arguments.");
    return {t, ts...};
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Stride<T, Ts...> MakeStride(const T& t, const Ts&... ts)
{
    return {t, ts...};
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Tile<T, Ts...> MakeTile(const T& t, const Ts&... ts)
{
    return {t, ts...};
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Coord<T, Ts...> MakeCoord(const T& t, const Ts&... ts)
{
    return {t, ts...};
}

template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride)
{
    static_assert(Std::is_tuple_v<T> && Std::is_tuple_v<U>, "Shape or Stride is not tuple!");
    static_assert(
        NestingDepthV<T> == NestingDepthV<U> && Std::tuple_size_v<T> == Std::tuple_size_v<U>,
        "Shape and Stride structure are not compatible.");
    return Layout<T, U>(shape, stride);
}

// shape = ((x1, x2, ..., xn), (y1, y2, ..., yn))
// stride[0][0] = 1; stride[0][i] = shape[0][i-1]*shape[1][i-1]*stride[0][i-1]; stride[1][i] = shape[0][i]*stride[0][i]
template <size_t I, typename Row, typename Col>
struct StrideRowElem {
    __aicore__ static inline constexpr auto value(const Row& row, const Col& col)
    {
        if constexpr (I == 0) {
            return _1{};
        } else {
            return Std::get<I - 1>(row) * Std::get<I - 1>(col) * StrideRowElem<I - 1, Row, Col>::value(row, col);
        }
    }
};

template <size_t I, typename Row, typename Col>
struct StrideColElem {
    __aicore__ static inline constexpr auto value(const Row& row, const Col& col)
    {
        return Std::get<I>(row) * StrideRowElem<I, Row, Col>::value(row, col);
    }
};

template <typename Row, typename Col, size_t... Is>
__aicore__ inline constexpr auto BuildStrideRowImpl(const Row& row, const Col& col, Std::index_sequence<Is...>)
{
    return MakeStride(StrideRowElem<Is, Row, Col>::value(row, col)...);
}

template <typename Row, typename Col, size_t... Is>
__aicore__ inline constexpr auto BuildStrideColImpl(const Row& row, const Col& col, Std::index_sequence<Is...>)
{
    return MakeStride(StrideColElem<Is, Row, Col>::value(row, col)...);
}

template <typename ShapeType>
__aicore__ inline constexpr auto ComputeStride(const ShapeType& shape)
{
    static_assert(
        Std::is_tuple_v<ShapeType> && Std::tuple_size_v<ShapeType> == 2, "ShapeType must be tuple of two tuples");
    const auto& row = Std::get<0>(shape);
    const auto& col = Std::get<1>(shape);
    static_assert(
        Std::tuple_size_v<Std::remove_cvref_t<decltype(row)>> == Std::tuple_size_v<Std::remove_cvref_t<decltype(col)>>,
        "ShapeType rows must have same length");
    constexpr size_t N = Std::tuple_size_v<Std::remove_cvref_t<decltype(row)>>;
    using Row = Std::remove_cvref_t<decltype(row)>;
    using Col = Std::remove_cvref_t<decltype(col)>;
    auto stride0 = BuildStrideRowImpl(row, col, Std::make_index_sequence<N>{});
    auto stride1 = BuildStrideColImpl(row, col, Std::make_index_sequence<N>{});
    return MakeStride(stride0, stride1);
}

// shape = (x1, x2, x3, ..., xn) -> stride = (x2*x3*...*xn, ..., x_{n-1}*xn, xn, 1)
template <size_t I, typename ShapeType>
struct FlatStrideElem {
    __aicore__ static inline constexpr auto value(const ShapeType& shape)
    {
        constexpr size_t N = Std::tuple_size_v<ShapeType>;
        static_assert(N > 0, "ShapeType must not be empty");
        if constexpr (I == N - 1) {
            return _1{};
        } else {
            return FlatStrideElem<I + 1, ShapeType>::value(shape) * Std::get<I + 1>(shape);
        }
    }
};

template <typename ShapeType, size_t... Is>
__aicore__ inline constexpr auto BuildFlatStrideImpl(const ShapeType& shape, Std::index_sequence<Is...>)
{
    return MakeStride(FlatStrideElem<Is, ShapeType>::value(shape)...);
}

template <typename ShapeType>
__aicore__ inline constexpr auto ComputeFlatStride(const ShapeType& shape)
{
    static_assert(Std::is_tuple_v<ShapeType>, "ShapeType must be tuple");
    constexpr size_t N = Std::tuple_size_v<ShapeType>;
    return BuildFlatStrideImpl(shape, Std::make_index_sequence<N>{});
}

template <typename ShapeType>
__aicore__ inline constexpr auto MakeLayout(const ShapeType& shape)
{
    static_assert(Std::is_tuple_v<ShapeType>, "ShapeType is not tuple!");
    using ElemT = Std::remove_cvref_t<decltype(Std::get<0>(shape))>;
    if constexpr (Std::is_tuple_v<ElemT>) {
        return MakeLayout(shape, ComputeStride(shape));
    } else {
        return MakeLayout(shape, ComputeFlatStride(shape));
    }
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto GetShape(const LayoutType& layout)
{
    return layout.template Shape<Is...>();
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto GetShape(LayoutType& layout)
{
    return layout.template Shape<Is...>();
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto GetStride(const LayoutType& layout)
{
    return layout.template Stride<Is...>();
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto GetStride(LayoutType& layout)
{
    return layout.template Stride<Is...>();
}

struct CoshapeSum {
    template <typename... Args>
    __aicore__ inline constexpr auto operator()(const Args&... args) const
    {
        return (_0{} + ... + args);
    }
};

struct CoshapeCompute {
    template <typename T, typename U>
    __aicore__ inline constexpr auto operator()(const T& shape, const U& stride) const
    {
        if constexpr (Std::is_tuple_v<T> && Std::is_tuple_v<U>) {
            static_assert(Std::tuple_size_v<T> == Std::tuple_size_v<U>, "Mismatched ranks");
            return TransformApply(shape, stride, CoshapeCompute{}, CoshapeSum{});
        } else {
            auto m1Shape = shape - _1{};
            auto absStride = stride < 0 ? -stride : stride;
            return m1Shape * absStride;
        }
    }
};

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Coshape(const LayoutType& layout)
{
    auto shape = GetShape<Is...>(layout);
    auto stride = GetStride<Is...>(layout);
    auto coCoord = CoshapeCompute{}(shape, stride);
    return coCoord + _1{};
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Cosize(const LayoutType& layout)
{
    return TupleSize(Coshape<Is...>(layout));
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Rank(const LayoutType& layout)
{
    return layout.template Rank<Is...>();
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Select(const LayoutType& layout)
{
    return MakeLayout(SelectTuple<Is...>(layout.Shape()), SelectTuple<Is...>(layout.Stride()));
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Get(const LayoutType& layout)
{
    return MakeLayout(GetTuple<Is...>(layout.Shape()), GetTuple<Is...>(layout.Stride()));
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Size(const LayoutType& layout)
{
    return layout.template Size<Is...>();
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<IsLayoutV<LayoutType>>>
__aicore__ inline constexpr auto Capacity(const LayoutType& layout)
{
    return layout.template Capacity<Is...>();
}

template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const Coord& coord, const Info& info)
{
    return static_cast<Tensor&&>(tensor).Slice(coord, info);
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_METHOD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
