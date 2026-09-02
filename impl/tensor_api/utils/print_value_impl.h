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
    "impl/tensor_api/utils/print_value_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/utils/print.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_VALUE_IMPL_H
#endif

/*!
 * \file print_value_impl.h
 * \brief Tensor API value formatting and print session implementation.
 */
#ifndef IMPL_TENSOR_API_UTILS_PRINT_VALUE_IMPL_H
#define IMPL_TENSOR_API_UTILS_PRINT_VALUE_IMPL_H

#include "include/tensor_api/utils/utils.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "utils/debug/asc_printf.h"

namespace asc {
namespace te {
namespace detail {

template <typename T>
struct is_print_char_pointer : Std::false_type {};

template <>
struct is_print_char_pointer<char*> : Std::true_type {};

template <>
struct is_print_char_pointer<const char*> : Std::true_type {};

template <typename T>
constexpr bool is_print_string_v = is_print_char_pointer<Std::decay_t<T>>::value;

__aicore__ inline int num_digits(long long x)
{
    unsigned long long value = x < 0 ? 0ULL - static_cast<unsigned long long>(x) : static_cast<unsigned long long>(x);
    int digits = x < 0 ? 2 : 1;
    while (value >= 10) {
        value /= 10;
        ++digits;
    }
    return digits;
}

template <typename... Args>
__aicore__ inline void print_format(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    __asc_aicore::printf(fmt, static_cast<Args&&>(args)...);
#else
    (void)fmt;
    ((void)args, ...);
#endif
}

struct print_session {
    bool first_fragment = true;
};

__aicore__ static __attribute__((noinline)) void print_fragment(print_session& session, __gm__ const char* fmt)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    if (session.first_fragment) {
        session.first_fragment = false;
        __asc_aicore::printf(fmt);
    } else {
        __asc_aicore::scalar_printf_impl(__asc_aicore::DumpType::DUMP_SCALAR, fmt, (__gm__ const char*)"");
    }
#else
    (void)session;
    (void)fmt;
#endif
}

template <typename Arg>
__aicore__ static __attribute__((noinline)) void print_fragment(print_session& session, __gm__ const char* fmt,
                                                                Arg arg)
{
#if !(defined(ASCENDC_DUMP) && (ASCENDC_DUMP == 0))
    if (session.first_fragment) {
        session.first_fragment = false;
        __asc_aicore::printf(fmt, arg);
    } else {
        __asc_aicore::scalar_printf_impl(__asc_aicore::DumpType::DUMP_SCALAR, fmt, (__gm__ const char*)"", arg);
    }
#else
    (void)session;
    (void)fmt;
    (void)arg;
#endif
}

__aicore__ inline void emit_spaces(print_session& session, int count)
{
    for (int i = 0; i < count; ++i) {
        print_fragment(session, (__gm__ const char*)" ");
    }
}

__aicore__ inline void emit_padded_int(print_session& session, long long value, int width)
{
    emit_spaces(session, width - num_digits(value));
    print_fragment(session, (__gm__ const char*)"%lld", value);
}

template <typename T>
__aicore__ inline void emit_tuple_element(print_session& session, const T& value);

template <typename Tuple, size_t I, size_t N>
struct tuple_printer {
    __aicore__ inline static void print(print_session& session, const Tuple& tuple)
    {
        emit_tuple_element(session, Std::get<I>(tuple));
        if constexpr (I + 1 < N) {
            print_fragment(session, (__gm__ const char*)", ");
            tuple_printer<Tuple, I + 1, N>::print(session, tuple);
        }
    }
};

template <typename Tuple, size_t N>
struct tuple_printer<Tuple, N, N> {
    __aicore__ inline static void print(print_session&, const Tuple&) {}
};

template <typename T>
__aicore__ inline void emit_tuple_element(print_session& session, const T& value)
{
    using value_type = Std::remove_cvref_t<T>;
    if constexpr (Std::is_tuple_v<value_type>) {
        print_fragment(session, (__gm__ const char*)"(");
        tuple_printer<value_type, 0, Std::tuple_size_v<value_type>>::print(session, value);
        print_fragment(session, (__gm__ const char*)")");
    } else if constexpr (is_print_string_v<T>) {
        print_fragment(session, (__gm__ const char*)"%s", value);
    } else if constexpr (is_one_of_attr_v<value_type, half, bfloat16_t>) {
        print_fragment(session, (__gm__ const char*)"%f", static_cast<float>(value));
    } else if constexpr (is_one_of_attr_v<value_type, float>) {
        print_fragment(session, (__gm__ const char*)"%f", static_cast<float>(value));
    } else if constexpr (Std::is_floating_point_v<value_type>) {
        print_fragment(session, (__gm__ const char*)"%f", static_cast<double>(value));
    } else if constexpr (is_one_of_attr_v<value_type, uint8_t, uint16_t, uint32_t, uint64_t>) {
        print_fragment(session, (__gm__ const char*)"%llu", static_cast<unsigned long long>(value));
    } else if constexpr (Std::is_integral_v<value_type>) {
        print_fragment(session, (__gm__ const char*)"%lld", static_cast<long long>(value));
    } else {
        print_fragment(session, (__gm__ const char*)"%lld", static_cast<long long>(value));
    }
}

template <typename T>
__aicore__ inline void emit_value(print_session& session, const T& value)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<T>>) {
        emit_tuple_element(session, value);
    } else if constexpr (is_layout_v<T>) {
        print_fragment(session, (__gm__ const char*)"layout(shape=");
        emit_tuple_element(session, value.shape());
        print_fragment(session, (__gm__ const char*)", stride=");
        emit_tuple_element(session, value.stride());
        print_fragment(session, (__gm__ const char*)")");
    } else if constexpr (is_attr_tensor_v<T>) {
        using element_type = typename Std::remove_cvref_t<T>::element_type;
        using data_type = get_attribute_element_type<element_type*>;
        constexpr uint32_t ELEMENT_BITS = is_b4_type<data_type> ? 4U : sizeof(data_type) * 8U;
        print_fragment(session, (__gm__ const char*)"ptr[%ub](", ELEMENT_BITS);
        print_fragment(session, (__gm__ const char*)"%p", value.data().get());
        print_fragment(session, (__gm__ const char*)") o ");
        emit_value(session, value.layout());
    } else {
        emit_tuple_element(session, value);
    }
}

template <typename Value, typename T>
__aicore__ inline void emit_element_as(print_session& session, const T& value)
{
    if constexpr (Std::is_same_v<Value, float> || Std::is_same_v<Value, half>
                  || Std::is_same_v<Value, bfloat16_t>) {
        print_fragment(session, (__gm__ const char*)"%f  ", static_cast<float>(value));
    } else if constexpr (Std::is_same_v<Value, int8_t> || Std::is_same_v<Value, int16_t>
                         || Std::is_same_v<Value, int32_t>) {
        print_fragment(session, (__gm__ const char*)"%d  ", static_cast<int32_t>(value));
    } else if constexpr (Std::is_same_v<Value, int64_t>) {
        print_fragment(session, (__gm__ const char*)"%lld  ", static_cast<long long>(value));
    } else if constexpr (Std::is_same_v<Value, uint8_t> || Std::is_same_v<Value, uint16_t>
                         || Std::is_same_v<Value, uint32_t>) {
        print_fragment(session, (__gm__ const char*)"%u  ", static_cast<uint32_t>(value));
    } else if constexpr (Std::is_same_v<Value, uint64_t>) {
        print_fragment(session, (__gm__ const char*)"%llu  ", static_cast<unsigned long long>(value));
    } else {
        print_fragment(session, (__gm__ const char*)"  ");
        emit_value(session, value);
    }
}

} // namespace detail
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_PRINT_VALUE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_VALUE_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_VALUE_IMPL_H
#endif
