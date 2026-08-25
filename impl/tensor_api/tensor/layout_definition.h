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
    "impl/tensor_api/tensor/layout_definition.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_definition.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_DEFINITION_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_DEFINITION_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/layout_size.h"
#include "impl/tensor_api/tensor/tuple_impl.h"

namespace AscendC {
namespace Te {

template <typename T, typename U, typename S>
__aicore__ inline constexpr auto Crd2Idx(const T& coord, const U& shape, const S& stride);

// struct layout
template <typename... Shapes>
using Shape = Std::tuple<Shapes...>;

template <typename... Strides>
using Stride = Std::tuple<Strides...>;

template <typename... Layouts>
using Tile = Std::tuple<Layouts...>;

template <typename... Coords>
using Coord = Std::tuple<Coords...>;

template <typename T, typename U, typename Info = Std::ignore_t>
struct Layout : private Std::tuple<T, U>
{
public:
    static constexpr auto depth = NestingDepthV<T>;
    static constexpr auto rank = Std::tuple_size_v<T>;

    __aicore__ inline constexpr Layout(const T& shape  = {}, const U& stride = {})
        : Std::tuple<T, U>(shape, stride)
    {
        static_assert(Std::is_tuple_v<T> && Std::is_tuple_v<U>, "Shape or Stride is not tuple!");
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Capacity() const
    {
        return GetCapacity(Shape<I...>(), Stride<I...>());
    }

    __aicore__ inline constexpr decltype(auto) layout()
    {
        return *this;
    }

    __aicore__ inline constexpr decltype(auto) layout() const
    {
        return *this;
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape()
    {
        return GetValue<0, I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape() const
    {
        return GetValue<0, I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride()
    {
        return GetValue<1, I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride() const
    {
        return GetValue<1, I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }

    template <typename S>
    __aicore__ inline constexpr auto operator()(const S& coord) const
    {
        return Crd2Idx(coord, Shape(), Stride());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Rank() const
    {
        static_assert(Std::tuple_size_v<T> == Std::tuple_size_v<U>, "The dimensions of the Shape and Stride are not the same.");
        return GetRank<I...>(Shape());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Size() const
    {
        return TupleSize<I...>(Shape());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get()
    {
        return Te::Get<I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get() const
    {
        return Te::Get<I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }
private:
    using tag = Info;
};

template <typename T>
struct GetLayoutInfo {
    using type = Std::tuple<Std::ignore_t, Std::ignore_t>;
};

template <typename T, typename U, typename LayoutPattern, typename TraitType>
struct GetLayoutInfo<Layout<T, U, Std::tuple<LayoutPattern, TraitType>>> {
    using type = Std::tuple<LayoutPattern, TraitType>;
};

template <typename T>
using GetLayoutInfoT = typename GetLayoutInfo<T>::type;

template <typename T>
struct GetPattern {
    using type = typename Std::tuple_element<0, GetLayoutInfoT<T>>::type;
};

template <typename T>
struct GetTraitType {
    using type = typename Std::tuple_element<1, GetLayoutInfoT<T>>::type;
};

template <typename T>
using GetLayoutTrait = typename GetTraitType<Std::remove_cvref_t<T>>::type;

template <typename T>
using GetLayoutPattern = typename GetPattern<Std::remove_cvref_t<T>>::type;

template <typename T>
struct IsLayout : Std::false_type {};

template <typename T, typename U, typename S>
struct IsLayout<Layout<T, U, S>> : Std::true_type {};

template <typename T>
constexpr bool IsLayoutV = IsLayout<Std::remove_cvref_t<T>>::value;

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_DEFINITION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
