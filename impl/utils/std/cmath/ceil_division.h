/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ceil_division.h
 * \brief
 */
#ifndef IMPL_STD_CEIL_DIVISION_H
#define IMPL_STD_CEIL_DIVISION_H

#include "../algorithm/min.h"
#include "../type_traits/is_integral.h"

namespace AscendC {
namespace Std {

template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_division(const T& num1, const U& num2)
{
    return (num1 + num2 - 1) / num2;
}

template <typename T>
struct integer_traits {
    static constexpr bool is_signed = false;
    using unsigned_type = T;
};

template <>
struct integer_traits<signed char> {
    static constexpr bool is_signed = true;
    using unsigned_type = unsigned char;
};

template <>
struct integer_traits<short> {
    static constexpr bool is_signed = true;
    using unsigned_type = unsigned short;
};

template <>
struct integer_traits<int> {
    static constexpr bool is_signed = true;
    using unsigned_type = unsigned int;
};

template <>
struct integer_traits<long> {
    static constexpr bool is_signed = true;
    using unsigned_type = unsigned long;
};

template <>
struct integer_traits<long long> {
    static constexpr bool is_signed = true;
    using unsigned_type = unsigned long long;
};

template <typename T>
static constexpr bool is_signed_impl_v = integer_traits<T>::is_signed;

template <typename T>
using make_unsigned_t = typename integer_traits<T>::unsigned_type;

/*!
 * \brief Divide a by b and round the result up.
 *
 * \note For signed inputs, callers must ensure a >= 0 and b > 0.
 */
template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_div_generic(const T& a, const U& b)
{
    static_assert(is_integral_v<T>, "AscendC::Std::ceil_div: T must be an integral type");
    static_assert(is_integral_v<U>, "AscendC::Std::ceil_div: U must be an integral type");
    using Prom = decltype(T{} / U{});
    using UProm = make_unsigned_t<Prom>;

    auto a1 = static_cast<UProm>(a);
    auto b1 = static_cast<UProm>(b);

    // When _Prom is signed, (a + b - 1) / b is safe because:
    // a <= INT_MAX, b <= INT_MAX, INT_MAX + INT_MAX fits in unsigned
    if constexpr (is_signed_impl_v<Prom>) {
        return static_cast<Prom>((a1 + b1 - 1) / b1);
    } else {
        // When _Prom is unsigned, a could be near UINT_MAX, so (a + b - 1) could overflow/wrap.
        // Use min-based branchless form: min(a, 1 + (a - 1) / b) avoids overflow risk.
        return min(a1, static_cast<UProm>(1) + ((a1 - 1) / b1));
    }
}

template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_div(const T& a, const U& b)
{
    using ReType = decltype(ceil_div_generic(a, b));
    if (b == 0) {
        return ReType(0);
    }
#if defined(__NPU_HOST__)
    return ceil_div_generic(a, b);
#elif defined(__NPU_DEVICE__)
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#if (defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1) || !defined(SPLIT_CORE_VEC)
    return ceil_div_generic(a, b);
#else
    return ReType(get_repeat_ceiling(a, b));
#endif
#else
    return ceil_div_generic(a, b);
#endif
#else
    return ceil_div_generic(a, b);
#endif
}
} // namespace Std
} // namespace AscendC
#endif
