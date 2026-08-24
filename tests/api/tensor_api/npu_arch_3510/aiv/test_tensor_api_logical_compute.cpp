/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the license.
 */

#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

namespace {

using namespace asc::te::experimental;

template <typename T>
using reg_type = asc::te::reg_tensor<T>;

template <typename T>
void check_logical_not()
{
    static_assert(std::is_same_v<decltype(!std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_bitwise_and()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() & std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() & std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() & std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_bitwise_or()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() | std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() | std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() | std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_bitwise_xor()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() ^ std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() ^ std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() ^ std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T, typename shift_type>
void check_shift()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() << std::declval<const reg_type<shift_type>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() << std::declval<const int16_t&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() >> std::declval<const reg_type<shift_type>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() >> std::declval<const int16_t&>()), reg_type<T>>);
}

TEST(test_tensor_api_logical_compute, compile_public_apis_and_supported_types)
{
    check_logical_not<int8_t>();
    check_logical_not<uint8_t>();
    check_logical_not<int16_t>();
    check_logical_not<uint16_t>();
    check_logical_not<half>();
    check_logical_not<bfloat16_t>();
    check_logical_not<int32_t>();
    check_logical_not<uint32_t>();
    check_logical_not<float>();
    check_logical_not<bool>();

    check_bitwise_and<int8_t>();
    check_bitwise_and<uint8_t>();
    check_bitwise_and<fp8_e5m2_t>();
    check_bitwise_and<fp8_e4m3fn_t>();
    check_bitwise_and<int16_t>();
    check_bitwise_and<uint16_t>();
    check_bitwise_and<half>();
    check_bitwise_and<bfloat16_t>();
    check_bitwise_and<int32_t>();
    check_bitwise_and<uint32_t>();
    check_bitwise_and<float>();

    check_bitwise_or<int8_t>();
    check_bitwise_or<uint8_t>();
    check_bitwise_or<int16_t>();
    check_bitwise_or<uint16_t>();
    check_bitwise_or<half>();
    check_bitwise_or<int32_t>();
    check_bitwise_or<uint32_t>();
    check_bitwise_or<float>();

    check_bitwise_xor<int8_t>();
    check_bitwise_xor<uint8_t>();
    check_bitwise_xor<int16_t>();
    check_bitwise_xor<uint16_t>();
    check_bitwise_xor<int32_t>();
    check_bitwise_xor<uint32_t>();

    using predicate_type = reg_type<bool>;
    static_assert(std::is_same_v<decltype(
        std::declval<const predicate_type&>() & std::declval<const predicate_type&>()), predicate_type>);
    static_assert(std::is_same_v<decltype(
        std::declval<const predicate_type&>() | std::declval<const predicate_type&>()), predicate_type>);
    static_assert(std::is_same_v<decltype(
        std::declval<const predicate_type&>() ^ std::declval<const predicate_type&>()), predicate_type>);

    check_shift<int8_t, int8_t>();
    check_shift<uint8_t, int8_t>();
    check_shift<int16_t, int16_t>();
    check_shift<uint16_t, int16_t>();
    check_shift<int32_t, int32_t>();
    check_shift<uint32_t, int32_t>();
}

} // namespace
