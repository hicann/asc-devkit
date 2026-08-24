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

template <typename T>
using reg_type = asc::te::reg_tensor<T>;

template <typename T>
void check_composite_compute()
{
    static_assert(std::is_same_v<decltype(asc::te::experimental::axpy(std::declval<const reg_type<T>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const T&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::absdiff(
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::exp_sub(
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<float>>);
    static_assert(std::is_same_v<decltype(asc::te::experimental::fma(std::declval<const reg_type<T>&>(),
        std::declval<const reg_type<T>&>(), std::declval<const reg_type<T>&>())), reg_type<T>>);
}

TEST(test_tensor_api_composite_compute, compile_public_apis_and_supported_types)
{
    check_composite_compute<half>();
    check_composite_compute<float>();
    static_assert(std::is_same_v<decltype(asc::te::experimental::muls_cast(
        std::declval<const reg_type<float>&>(), std::declval<const float&>())), reg_type<half>>);
}

} // namespace
