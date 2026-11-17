/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \\
    "impl/tensor_api/arch/atomic/scalar_atomic_op_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\"#include \\"tensor_api/arch/atomic/scalar_atomic_op.h\\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_IMPL_H
#endif

/*!
 * \file scalar_atomic_op_impl.h
 * \brief Scalar atomic operation implementation.
 */
#ifndef IMPL_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_IMPL_H
#define IMPL_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_IMPL_H

#include "c_api/asc_simd.h"
#include "impl/tensor_api/utils/macro_impl.h"
#include "utils/std/type_traits.h"

namespace asc {
namespace te {
namespace detail {

template <typename T>
constexpr bool is_scalar_atomic_type_v =
    AscendC::Std::is_same_v<T, int32_t> || AscendC::Std::is_same_v<T, uint32_t> || AscendC::Std::is_same_v<T, float> ||
    AscendC::Std::is_same_v<T, int64_t> || AscendC::Std::is_same_v<T, uint64_t>;

template <typename T>
constexpr bool is_scalar_atomic_bitwise_type_v =
    AscendC::Std::is_same_v<T, int32_t> || AscendC::Std::is_same_v<T, uint32_t> ||
    AscendC::Std::is_same_v<T, int64_t> || AscendC::Std::is_same_v<T, uint64_t>;

template <typename T>
constexpr bool is_scalar_atomic_inc_dec_type_v =
    AscendC::Std::is_same_v<T, uint32_t> || AscendC::Std::is_same_v<T, uint64_t>;

} // namespace detail

template <typename T>
__aicore__ inline T atomic_add(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_add(address, val);
}

template <typename T>
__aicore__ inline T atomic_sub(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_sub(address, val);
}

template <typename T>
__aicore__ inline T atomic_min(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_min(address, val);
}

template <typename T>
__aicore__ inline T atomic_max(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_max(address, val);
}

template <typename T>
__aicore__ inline T atomic_and(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_bitwise_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_and(address, val);
}

template <typename T>
__aicore__ inline T atomic_or(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_bitwise_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_or(address, val);
}

template <typename T>
__aicore__ inline T atomic_xor(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_bitwise_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_xor(address, val);
}

template <typename T>
__aicore__ inline T atomic_inc(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_inc_dec_type_v<T>, "Unsupported scalar atomic element type");
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return static_cast<T>(
            __asc_aicore::asc_atomic_inc(reinterpret_cast<__gm__ uint32_t*>(address), static_cast<uint32_t>(val)));
    }
    return static_cast<T>(
        __asc_aicore::asc_atomic_inc(reinterpret_cast<__gm__ uint64_t*>(address), static_cast<uint64_t>(val)));
}

template <typename T>
__aicore__ inline T atomic_dec(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_inc_dec_type_v<T>, "Unsupported scalar atomic element type");
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return static_cast<T>(
            __asc_aicore::asc_atomic_dec(reinterpret_cast<__gm__ uint32_t*>(address), static_cast<uint32_t>(val)));
    }
    return static_cast<T>(
        __asc_aicore::asc_atomic_dec(reinterpret_cast<__gm__ uint64_t*>(address), static_cast<uint64_t>(val)));
}

template <typename T>
__aicore__ inline T atomic_exch(__gm__ T* address, T val)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_exch(address, val);
}

template <typename T>
__aicore__ inline T atomic_cas(__gm__ T* address, T compare, T desired)
{
    static_assert(detail::is_scalar_atomic_type_v<T>, "Unsupported scalar atomic element type");
    return __asc_aicore::asc_atomic_cas(address, compare, desired);
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_IMPL_H
#endif
