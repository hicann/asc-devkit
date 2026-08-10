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
    "impl/tensor_api/tensor/tuple_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file tuple_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_TUPLE_IMPL_H
#define IMPL_TENSOR_API_TENSOR_TUPLE_IMPL_H

#include "impl/tensor_api/utils/utils_impl.h"

namespace asc {
namespace te {

template <typename T, typename F, typename G, size_t... I>
__aicore__ inline constexpr auto tuple_apply(T&& t, F&& f, G&& g, Std::index_sequence<I...>)
{
    return g(f(Std::get<I>(static_cast<T&&>(t)))...);
}

template <typename T0, typename T1, typename F, typename G, size_t... I>
__aicore__ inline constexpr auto tuple_apply(T0&& t0, T1&& t1, F&& f, G&& g, Std::index_sequence<I...>)
{
    return g(f(Std::get<I>(static_cast<T0&&>(t0)), Std::get<I>(static_cast<T1&&>(t1)))...);
}

template <typename T, typename F, typename G>
__aicore__ inline constexpr auto transform_apply(T&& t, F&& f, G&& g)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<T>>) {
        return tuple_apply(static_cast<T&&>(t), f, g, tuple_sequence<T>{});
    } else {
        return g(f(static_cast<T&&>(t)));
    }
}

template <typename T0, typename T1, typename F, typename G>
__aicore__ inline constexpr auto transform_apply(T0&& t0, T1&& t1, F&& f, G&& g)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<T0>>) {
        return tuple_apply(static_cast<T0&&>(t0), static_cast<T1&&>(t1), f, g, tuple_sequence<T0>{});
    } else {
        return g(f(static_cast<T0&&>(t0), static_cast<T1&&>(t1)));
    }
}

template <typename T, typename U, typename F>
__aicore__ inline constexpr auto transform_tuple_apply(const T& t, const U& u, const F& f);

template <typename T, typename U, typename F, size_t... Is>
__aicore__ inline constexpr auto transform_tuple_apply_impl(const T& t, const U& u, const F& f,
                                                            Std::index_sequence<Is...>)
{
    return Std::make_tuple(transform_tuple_apply(Std::get<Is>(t), Std::get<Is>(u), f)...);
}

template <typename T, typename U, typename F>
__aicore__ inline constexpr auto transform_tuple_apply(const T& t, const U& u, const F& f)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<T>>) {
        static_assert(Std::tuple_size_v<Std::remove_cvref_t<T>> == Std::tuple_size_v<Std::remove_cvref_t<U>>,
                      "Two tuple needs to be the same tuple size");
        return transform_tuple_apply_impl(t, u, f,
                                          Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<T>>>{});
    } else {
        static_assert(!Std::is_tuple_v<Std::remove_cvref_t<U>>, "Two tuple needs to be the same tuple size");
        return f(t, u);
    }
}

struct multiplies_unary_left_fold {
    template <typename... T>
    __aicore__ inline constexpr auto operator()(T&&... t) const
    {
        return (... * t);
    }
};

struct product {
    template <typename T>
    __aicore__ inline constexpr auto operator()(const T& int_t) const
    {
        if constexpr (Std::is_tuple_v<T>) {
            if constexpr (Std::tuple_size_v<T> == 0) {
                return _1{};
            } else {
                return transform_apply(int_t, product{}, multiplies_unary_left_fold{});
            }
        } else if constexpr (Std::is_integral<T>::value) {
            return int_t;
        } else {
            static_assert(sizeof(T) == 0, "Invalid Product parameters");
        }
    }
};

template <size_t I, typename Tuple, typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto get(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    return Std::get<I>(static_cast<Tuple&&>(t));
}

template <size_t I0, size_t I1, size_t... Is, typename Tuple,
          typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto get(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    return get<I1, Is...>(get<I0>(static_cast<Tuple&&>(t)));
}

template <typename Tuple, typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto get(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    return static_cast<Tuple&&>(t);
}

template <size_t... Is, typename Tuple>
__aicore__ inline constexpr auto get_tuple(Tuple&& t)
{
    auto element = get<Is...>(static_cast<Tuple&&>(t));
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<decltype(element)>>) {
        return element;
    } else {
        return Std::make_tuple(element);
    }
}

template <size_t... Is, typename Tuple>
__aicore__ inline constexpr auto get_rank(const Tuple& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple!");
    if constexpr (sizeof...(Is) == 0) {
        return Std::Int<Std::tuple_size_v<Tuple>>{};
    } else {
        return get_rank(get_tuple<Is...>(t));
    }
}

template <size_t... Is, typename Tuple>
__aicore__ inline constexpr auto tuple_size(const Tuple& t)
{
    if constexpr (sizeof...(Is) == 0) {
        return product{}(t);
    } else {
        return tuple_size(get_tuple<Is...>(t));
    }
}

template <size_t I, typename Tuple>
__aicore__ inline constexpr auto select_tuple(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    auto&& tt = Std::get<I>(static_cast<Tuple&&>(t));
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<decltype(tt)>>) {
        return tt;
    } else {
        return Std::make_tuple(tt);
    }
}

template <size_t I0, size_t I1, size_t... Is, typename Tuple>
__aicore__ inline constexpr auto select_tuple(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    return Std::make_tuple(Std::get<I0>(static_cast<Tuple&&>(t)), Std::get<I1>(static_cast<Tuple&&>(t)),
                           Std::get<Is>(static_cast<Tuple&&>(t))...);
}

template <typename Tuple>
__aicore__ inline constexpr auto select_tuple(Tuple&& t)
{
    static_assert(Std::is_tuple_v<Std::remove_cvref_t<Tuple>>, "Shape or Stride is not Tuple");
    return static_cast<Tuple&&>(t);
}

template <size_t index, size_t I, size_t... Is, typename Tuple>
__aicore__ inline constexpr decltype(auto) get_value(const Tuple& t)
{
    decltype(auto) tuple_ele = Std::get<index>(t);
    if constexpr (sizeof...(Is) == 0) {
        return Std::get<I>(tuple_ele);
    } else {
        return Std::make_tuple(Std::get<I>(tuple_ele), Std::get<Is>(tuple_ele)...);
    }
}

template <size_t index, typename Tuple>
__aicore__ inline constexpr decltype(auto) get_value(const Tuple& t)
{
    return Std::get<index>(t);
}

template <typename T0, typename... Ts>
__aicore__ inline constexpr auto get_max(const T0& t0, const Ts&... ts)
{
    if constexpr (sizeof...(Ts) == 0) {
        return t0;
    } else {
        return Std::max(t0, get_max(ts...));
    }
}

template <typename Shape, typename Stride>
__aicore__ inline constexpr auto get_capacity(const Shape& shape, const Stride& stride);

template <typename Shape, typename Stride, size_t... Is>
__aicore__ inline constexpr auto get_capacity_impl(const Shape& shape, const Stride& stride, Std::index_sequence<Is...>)
{
    return get_max(get_capacity(Std::get<Is>(shape), Std::get<Is>(stride))...);
}

template <typename Shape, typename Stride>
__aicore__ inline constexpr auto get_capacity(const Shape& shape, const Stride& stride)
{
    if constexpr (Std::is_tuple_v<Shape> && Std::is_tuple_v<Stride>) {
        static_assert(Std::tuple_size_v<Shape> == Std::tuple_size_v<Stride>, "Mismatched ranks");
        return get_capacity_impl(shape, stride, Std::make_index_sequence<Std::tuple_size_v<Shape>>{});
    } else {
        return shape * stride;
    }
}

// ===== Squeeze helpers =====

// PrependIndex: prepend a size_t to an index_sequence.
template <size_t Idx, typename Seq>
struct prepend_index;
template <size_t Idx, size_t... Is>
struct prepend_index<Idx, Std::index_sequence<Is...>> {
    using type = Std::index_sequence<Idx, Is...>;
};

// KeepIndexSeq: produce the index_sequence of positions whose bool mask is true.
template <size_t Idx, bool... Mask>
struct keep_index_seq {
    using type = Std::index_sequence<>;
};
template <size_t Idx, bool Head, bool... Tail>
struct keep_index_seq<Idx, Head, Tail...> {
    using rest = typename keep_index_seq<Idx + 1, Tail...>::type;
    using type = Std::conditional_t<Head, typename prepend_index<Idx, rest>::type, rest>;
};
template <bool... Mask>
using keep_index_seq_t = typename keep_index_seq<0, Mask...>::type;

// KeepIndexSeqFromSeq: same as KeepIndexSeqT but the bool mask is carried by an IntegerSequence.
template <typename BoolSeq>
struct keep_index_seq_from_seq;
template <bool... Bs>
struct keep_index_seq_from_seq<Std::IntegerSequence<bool, Bs...>> {
    using type = typename keep_index_seq_t<Bs...>::type;
};

// SelectBySeq: pick tuple elements at the indices carried by an index_sequence.
template <typename Tuple, size_t... Is>
__aicore__ inline constexpr auto select_by_seq_impl(const Tuple& t, Std::index_sequence<Is...>)
{
    return Std::make_tuple(Std::get<Is>(t)...);
}
template <typename Seq, typename Tuple>
__aicore__ inline constexpr auto select_by_seq(const Tuple& t)
{
    return select_by_seq_impl(t, Seq{});
}

// IsSameStructure: two types share the same tuple nesting structure (same depth, same size per level).
template <typename T, typename U, typename = void>
struct is_same_structure;

template <typename T, typename U, typename Seq>
struct all_children_same;

template <typename T, typename U, size_t... Is>
struct all_children_same<T, U, Std::index_sequence<Is...>>
    : Std::bool_constant<(is_same_structure<typename Std::tuple_element<Is, Std::remove_cvref_t<T>>::type,
                                            typename Std::tuple_element<Is, Std::remove_cvref_t<U>>::type>::value
                          && ...)> {};

template <typename T, typename U, typename>
struct is_same_structure : Std::false_type {};

template <typename T, typename U>
struct is_same_structure<
    T, U, Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<T>> && Std::is_tuple_v<Std::remove_cvref_t<U>>>>
    : Std::conditional_t<Std::tuple_size_v<Std::remove_cvref_t<T>> == Std::tuple_size_v<Std::remove_cvref_t<U>>,
                         all_children_same<Std::remove_cvref_t<T>, Std::remove_cvref_t<U>,
                                           Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<T>>>>,
                         Std::false_type> {};

template <typename T, typename U>
struct is_same_structure<
    T, U, Std::enable_if_t<!Std::is_tuple_v<Std::remove_cvref_t<T>> && !Std::is_tuple_v<Std::remove_cvref_t<U>>>>
    : Std::true_type {};

template <typename T, typename U>
constexpr bool is_same_structure_v = is_same_structure<Std::remove_cvref_t<T>, Std::remove_cvref_t<U>>::value;

} // namespace te
} // namespace asc



#endif // IMPL_TENSOR_API_TENSOR_TUPLE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
