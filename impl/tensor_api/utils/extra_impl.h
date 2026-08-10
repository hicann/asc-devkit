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
    "impl/tensor_api/utils/extra_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file extra_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_EXTRA_IMPL_H
#define IMPL_TENSOR_API_UTILS_EXTRA_IMPL_H

#include <cstdint>
#include <utility>
#include <type_traits>
#include "impl/tensor_api/utils/macro_impl.h"
#include "utils/std/tuple.h"
#include "utils/std/type_traits.h"
#include "utils/std/utility.h"
#include "utils/std/algorithm.h"
#include "utils/std/cmath.h"
#include "impl/tensor_api/utils/int_impl.h"

namespace asc {
namespace te {
template <typename... Ts>
using void_t = void;

template <typename T, typename = void>
struct get_trait_member_type {
    using type = typename T::TraitType;
};

template <typename T>
struct get_trait_member_type<T, void_t<typename T::trait_type>> {
    using type = typename T::trait_type;
};

template <typename T>
using get_trait_member_type_t = typename get_trait_member_type<T>::type;

template <typename T, typename = void>
struct iter_ref {
    using type = decltype(*Std::declval<T&>()); // type = T&
};

template <typename T>
struct iter_ref<T, void_t<typename T::reference>> {
    using type = typename T::reference;
};

template <typename T, typename = void>
struct iter_ele {
    using type = Std::remove_reference_t<typename iter_ref<T>::type>;
};

template <typename T>
struct iter_ele<T, void_t<typename T::element_type>> {
    using type = typename T::element_type;
};

template <typename T, typename = void>
struct iter_val {
    using type = Std::remove_cv_t<typename iter_ele<T>::type>;
};

template <typename T>
struct iter_val<T, void_t<typename T::value_type>> {
    using type = typename T::value_type;
};
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_EXTRA_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
