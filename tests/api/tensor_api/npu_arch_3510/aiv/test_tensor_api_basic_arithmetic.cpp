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
void check_unary_arithmetic()
{
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::abs(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(-std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_float_math()
{
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::exp(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::sqrt(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::log(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::log2(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::log10(std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::prelu(
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::leaky_relu(
        std::declval<const reg_type<T>&>(), std::declval<const T&>())), reg_type<T>>);
}

template <typename T>
void check_relu()
{
    static_assert(std::is_same_v<decltype(
        asc::te::experimental::relu(std::declval<const reg_type<T>&>())), reg_type<T>>);
}

template <typename T>
void check_add_sub()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() + std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() + std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() + std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() - std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() - std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() - std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_register_mul()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() * std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_scalar_mul_and_div()
{
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() * std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() * std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() / std::declval<const reg_type<T>&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const reg_type<T>&>() / std::declval<const T&>()), reg_type<T>>);
    static_assert(std::is_same_v<decltype(
        std::declval<const T&>() / std::declval<const reg_type<T>&>()), reg_type<T>>);
}

template <typename T>
void check_min_max()
{
    static_assert(std::is_same_v<decltype(asc::te::experimental::max(
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::max(
        std::declval<const reg_type<T>&>(), std::declval<const T&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::max(
        std::declval<const T&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::min(
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::min(
        std::declval<const reg_type<T>&>(), std::declval<const T&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::min(
        std::declval<const T&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
}

template <typename T>
void check_carry()
{
    static_assert(std::is_same_v<decltype(asc::te::experimental::add(std::declval<reg_type<bool>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::add_c(std::declval<reg_type<bool>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>(),
        std::declval<const reg_type<bool>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::sub(std::declval<reg_type<bool>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::sub_c(std::declval<reg_type<bool>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>(),
        std::declval<const reg_type<bool>&>())), reg_type<T>>);
}

template <typename T>
void check_mull()
{
    static_assert(std::is_same_v<decltype(asc::te::experimental::mull(std::declval<reg_type<T>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
}

TEST(test_tensor_api_basic_arithmetic, compile_public_apis_and_supported_types)
{
    check_unary_arithmetic<int8_t>();
    check_unary_arithmetic<int16_t>();
    check_unary_arithmetic<half>();
    check_unary_arithmetic<int32_t>();
    check_unary_arithmetic<float>();
    check_float_math<half>();
    check_float_math<float>();
    check_relu<half>();
    check_relu<int32_t>();
    check_relu<float>();

    check_add_sub<int8_t>();
    check_add_sub<uint8_t>();
    check_add_sub<int16_t>();
    check_add_sub<uint16_t>();
    check_add_sub<half>();
    check_add_sub<bfloat16_t>();
    check_add_sub<int32_t>();
    check_add_sub<uint32_t>();
    check_add_sub<float>();

    check_register_mul<int16_t>();
    check_register_mul<uint16_t>();
    check_register_mul<half>();
    check_register_mul<bfloat16_t>();
    check_register_mul<int32_t>();
    check_register_mul<uint32_t>();
    check_register_mul<float>();

    check_scalar_mul_and_div<int16_t>();
    check_scalar_mul_and_div<uint16_t>();
    check_scalar_mul_and_div<half>();
    check_scalar_mul_and_div<int32_t>();
    check_scalar_mul_and_div<uint32_t>();
    check_scalar_mul_and_div<float>();

    check_min_max<int8_t>();
    check_min_max<uint8_t>();
    check_min_max<int16_t>();
    check_min_max<uint16_t>();
    check_min_max<half>();
    check_min_max<bfloat16_t>();
    check_min_max<int32_t>();
    check_min_max<uint32_t>();
    check_min_max<float>();
    check_carry<int32_t>();
    check_carry<uint32_t>();
    check_mull<int32_t>();
    check_mull<uint32_t>();
}

} // namespace
