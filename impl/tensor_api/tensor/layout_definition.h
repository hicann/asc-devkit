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

namespace asc {
namespace te {

template <typename T, typename U, typename S>
__aicore__ inline constexpr auto crd2idx(const T& coord, const U& shape, const S& stride);

// struct layout
template <typename... Shapes>
using shape_type = Std::tuple<Shapes...>;

template <typename... Strides>
using stride_type = Std::tuple<Strides...>;

template <typename... Layouts>
using tile_type = Std::tuple<Layouts...>;

template <typename... Coords>
using coord_type = Std::tuple<Coords...>;

template <typename T, typename U, typename Info = Std::ignore_t>
struct layout_type : private Std::tuple<T, U> {
public:
    static constexpr auto depth = nesting_depth_v<T>;
    static constexpr auto rank = Std::tuple_size_v<T>;

    __aicore__ inline constexpr layout_type(const T& shape = {}, const U& stride = {}) : Std::tuple<T, U>(shape, stride)
    {
        static_assert(Std::is_tuple_v<T> && Std::is_tuple_v<U>, "Shape or Stride is not tuple!");
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) capacity() const
    {
        return get_capacity(shape<I...>(), stride<I...>());
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
    __aicore__ inline constexpr decltype(auto) shape()
    {
        return get_value<0, I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) shape() const
    {
        return get_value<0, I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) stride()
    {
        return get_value<1, I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) stride() const
    {
        return get_value<1, I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }

    template <typename S>
    __aicore__ inline constexpr auto operator()(const S& coord) const
    {
        return crd2idx(coord, shape(), stride());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) get_rank() const
    {
        static_assert(Std::tuple_size_v<T> == Std::tuple_size_v<U>,
                      "The dimensions of the Shape and Stride are not the same.");
        return asc::te::get_rank<I...>(shape());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) size() const
    {
        return tuple_size<I...>(shape());
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) get()
    {
        return asc::te::get<I...>(static_cast<Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) get() const
    {
        return asc::te::get<I...>(static_cast<const Std::tuple<T, U>&>(*this));
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Capacity() const
    {
        return capacity<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape()
    {
        return shape<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape() const
    {
        return shape<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride()
    {
        return stride<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride() const
    {
        return stride<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Rank() const
    {
        return get_rank<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Size() const
    {
        return size<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get()
    {
        return get<I...>();
    }

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get() const
    {
        return get<I...>();
    }

private:
    using tag = Info;
};

template <typename T, typename U, typename Info = Std::ignore_t>
using layout = layout_type<T, U, Info>;

template <typename T>
struct get_layout_info {
    using type = Std::tuple<Std::ignore_t, Std::ignore_t>;
};

template <typename T, typename U, typename LayoutPattern, typename TraitType>
struct get_layout_info<layout_type<T, U, Std::tuple<LayoutPattern, TraitType>>> {
    using type = Std::tuple<LayoutPattern, TraitType>;
};

template <typename T>
using get_layout_info_t = typename get_layout_info<T>::type;

template <typename T>
struct get_pattern {
    using type = typename Std::tuple_element<0, get_layout_info_t<T>>::type;
};

template <typename T>
struct get_trait_type {
    using type = typename Std::tuple_element<1, get_layout_info_t<T>>::type;
};

template <typename T>
using get_layout_trait = typename get_trait_type<Std::remove_cvref_t<T>>::type;

template <typename T>
using get_layout_pattern = typename get_pattern<Std::remove_cvref_t<T>>::type;

template <typename T>
struct is_layout : Std::false_type {};

template <typename T, typename U, typename S>
struct is_layout<layout_type<T, U, S>> : Std::true_type {};

template <typename T>
constexpr bool is_layout_v = is_layout<Std::remove_cvref_t<T>>::value;

} // namespace te
} // namespace asc



#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_DEFINITION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
