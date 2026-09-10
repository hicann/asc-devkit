/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file tuple_impl.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_TUPLE_IMPL__H
#define IMPL_STD_ASCENDC_STD_TUPLE_IMPL__H

#include <cstdint>
#include <type_traits>
#include "ignore.h"
#include "../utility/move.h"
#include "../utility/forward.h"
#include "../type_traits/decay.h"
#include "../type_traits/enable_if.h"
#include "../type_traits/integral_constant.h"
#include "../type_traits/is_same.h"
#include "../type_traits/remove_const.h"
#include "../type_traits/remove_cv.h"
#include "../type_traits/remove_reference.h"
#include "../type_traits/remove_volatile.h"

namespace AscendC {
namespace Std {

constexpr uint32_t ASCENDC_STD_TUPLE_STACK_DEPTH = 64;

template <size_t N = 0, typename... Tps>
ASCENDC_HOST_AICORE inline constexpr void tuple_static_assert()
{
    static_assert(N < ASCENDC_STD_TUPLE_STACK_DEPTH, "Index overflow. The index must be smaller than 64!");
    static_assert(sizeof...(Tps) <= ASCENDC_STD_TUPLE_STACK_DEPTH, "The number of template elements must be <= 64!");
}

template <typename... Tps>
class tuple;

template <>
class tuple<> {};

template <typename Tp, typename... Tps>
struct tuple_constraints {
    using removeType = typename remove_reference<Tp>::type;
    static constexpr bool variadic_copy_constructible = !is_same_v<Tp, removeType&&>;
};

template <typename Tp, typename... Tps>
class tuple<Tp, Tps...> : public tuple<Tps...> {
public:
    ASCENDC_HOST_AICORE inline constexpr tuple() : tuple<Tps...>(), value() { tuple_static_assert<0, Tp, Tps...>(); }

    template <
        typename Constraints = tuple_constraints<Tp, Tps...>,
        enable_if_t<Constraints::variadic_copy_constructible, int> = 0>
    ASCENDC_HOST_AICORE inline constexpr tuple(const Tp& val, const Tps&... params)
        : tuple<Tps...>(params...), value(val)
    {
        tuple_static_assert<0, Tp, Tps...>();
    }

    template <
        typename Constraints = tuple_constraints<Tp, Tps...>,
        enable_if_t<!Constraints::variadic_copy_constructible, int> = 0>
    ASCENDC_HOST_AICORE inline constexpr tuple(Tp&& val, Tps&&... params)
        : tuple<Tps...>(forward<Tps>(params)...), value(forward<Tp>(val))
    {
        tuple_static_assert<0, Tp, Tps...>();
    }

    ASCENDC_HOST_AICORE inline constexpr Tp& GetValue() noexcept { return value; }

    ASCENDC_HOST_AICORE inline constexpr const Tp& GetValue() const noexcept { return value; }

    template <typename Head, typename... Args>
    ASCENDC_HOST_AICORE inline tuple<Tp, Tps...>& operator=(const tuple<Head, Args...>& t)
    {
        static_assert(sizeof...(Tps) == sizeof...(Args), "Both tuples must have the same number of elements");
        this->value = t.value;
        tuple<Tps...>(*this) = tuple<Args...>(t);
        return *this;
    }

private:
    template <typename...>
    friend class tuple;
    Tp value;
};

// tuple_size
template <typename... Tps>
struct tuple_size;

template <typename... Tps>
struct tuple_size<tuple<Tps...>> : integral_constant<size_t, sizeof...(Tps)> {};

template <typename T>
struct tuple_size<const T> : public integral_constant<size_t, tuple_size<T>::value> {};

template <typename T>
struct tuple_size<volatile T> : public integral_constant<size_t, tuple_size<T>::value> {};

template <typename T>
struct tuple_size<const volatile T> : public integral_constant<size_t, tuple_size<T>::value> {};

template <typename T>
constexpr size_t tuple_size_v = tuple_size<T>::value;

// tuple_element
template <size_t N, typename... Tps>
struct tuple_element;

template <size_t N>
struct tuple_element<N, tuple<>> {
    static_assert(N < 0, "The index(N) is greater than the number of elements!");
};

template <size_t N, typename Tp, typename... Tps>
struct tuple_element<N, tuple<Tp, Tps...>> : public tuple_element<N - 1, tuple<Tps...>> {};

template <typename Tp, typename... Tps>
struct tuple_element<0, tuple<Tp, Tps...>> {
    using type = Tp;
    using tuple_t = tuple<Tp, Tps...>;
};

template <size_t N, typename T>
struct tuple_element<N, const T> {
    using type = const typename remove_const<typename tuple_element<N, T>::type>::type;
};

template <size_t N, typename T>
struct tuple_element<N, volatile T> {
    using type = volatile typename remove_volatile<typename tuple_element<N, T>::type>::type;
};

template <size_t N, typename T>
struct tuple_element<N, const volatile T> {
    using type = const volatile typename remove_cv<typename tuple_element<N, T>::type>::type;
};

// make_tuple
template <typename T>
struct unwrap_refwrapper {
    using type = T;
};

template <typename T>
struct unwrap_refwrapper<std::reference_wrapper<T>> {
    using type = T&;
};

template <typename T>
using unwrap_decay_t = typename unwrap_refwrapper<decay_t<T>>::type;

template <typename... Tps>
ASCENDC_HOST_AICORE inline constexpr tuple<unwrap_decay_t<Tps>...> make_tuple(Tps&&... args)
{
    tuple_static_assert<0, Tps...>();
    return tuple<unwrap_decay_t<Tps>...>(forward<Tps>(args)...);
}

// tie
template <typename... Tps>
ASCENDC_HOST_AICORE inline constexpr tuple<Tps&...> tie(Tps&... args) noexcept
{
    tuple_static_assert<0, Tps...>();
    return tuple<Tps&...>(args...);
}

// forward_as_tuple
template <typename... Tps>
ASCENDC_HOST_AICORE inline constexpr tuple<Tps&&...> forward_as_tuple(Tps&&... args) noexcept
{
    tuple_static_assert<0, Tps...>();
    return tuple<Tps&&...>(forward<Tps>(args)...);
}

// get
template <size_t N, typename... Tps>
ASCENDC_HOST_AICORE inline constexpr typename tuple_element<N, tuple<Tps...>>::type& get(tuple<Tps...>& t) noexcept
{
    tuple_static_assert<N, Tps...>();
    using type = typename tuple_element<N, tuple<Tps...>>::type;
    using tuple_t = typename tuple_element<N, tuple<Tps...>>::tuple_t;
    return static_cast<type&>(static_cast<tuple_t&>(t).GetValue());
}

template <size_t N, typename... Tps>
ASCENDC_HOST_AICORE inline constexpr const typename tuple_element<N, tuple<Tps...>>::type& get(
    const tuple<Tps...>& t) noexcept
{
    tuple_static_assert<N, Tps...>();
    using type = const typename tuple_element<N, tuple<Tps...>>::type;
    using tuple_t = const typename tuple_element<N, tuple<Tps...>>::tuple_t;
    return static_cast<type&>(static_cast<tuple_t&>(t).GetValue());
}

template <size_t N, typename... Tps>
ASCENDC_HOST_AICORE inline constexpr typename tuple_element<N, tuple<Tps...>>::type&& get(tuple<Tps...>&& t) noexcept
{
    using type = typename tuple_element<N, tuple<Tps...>>::type;
    return static_cast<type&&>(get<N, Tps...>(static_cast<tuple<Tps...>&>(t)));
}

template <size_t N, typename... Tps>
ASCENDC_HOST_AICORE inline constexpr const typename tuple_element<N, tuple<Tps...>>::type&& get(
    const tuple<Tps...>&& t) noexcept
{
    using type = const typename tuple_element<N, tuple<Tps...>>::type;
    return static_cast<type&&>(get<N, Tps...>(static_cast<const tuple<Tps...>&>(t)));
}

} // namespace Std
} // namespace AscendC

#endif // IMPL_STD_ASCENDC_STD_TUPLE_IMPL__H
