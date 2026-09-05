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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"

namespace asc {
namespace te {
namespace experimental {
template <typename T>
__simd_callee__ inline reg_tensor<T> log(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "log does not support this element type");
    reg_tensor<T> dst;
    asc_ln(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_add_sub_v<T>, "operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_add_sub_v<T>, "operator- does not support this element type");
    reg_tensor<T> dst;
    asc_sub(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator- does not support this element type");
    auto scalar_reg = detail::make_reg_operand<T>(scalar, src.mask);
    reg_tensor<T> dst;
    asc_sub(dst.reg, src.reg, scalar_reg.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator- does not support this element type");
    auto scalar_reg = detail::make_reg_operand<T>(scalar, src.mask);
    reg_tensor<T> dst;
    asc_sub(dst.reg, scalar_reg.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mul_v<T>, "operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_mul_scalar_v<T>, "scalar operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_mul_scalar_v<T>, "scalar operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_min_max_v<T>, "max does not support this element type");
    reg_tensor<T> dst;
    asc_max(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_min_max_v<T>, "max does not support this element type");
    reg_tensor<T> dst;
    asc_max_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const T& scalar, const reg_tensor<T>& src)
{
    return max(src, scalar);
}

template <typename T>
__simd_callee__ inline reg_tensor<T> abs(const reg_tensor<T>& src)
{
    static_assert(detail::supports_abs_v<T>, "abs does not support this element type");
    reg_tensor<T> dst;
    asc_abs(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> exp(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "exp supports half and float");
    reg_tensor<T> dst;
    asc_exp(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> sqrt(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "sqrt supports half and float");
    reg_tensor<T> dst;
    asc_sqrt(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log2(const reg_tensor<T>& src)
{
    auto natural_log = log(src);
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, natural_log.reg, static_cast<T>(1.4426950408889634), src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log10(const reg_tensor<T>& src)
{
    auto natural_log = log(src);
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, natural_log.reg, static_cast<T>(0.4342944819032518), src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src)
{
    static_assert(detail::supports_neg_v<T>, "unary operator- does not support this element type");
    reg_tensor<T> dst;
    asc_neg(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> relu(const reg_tensor<T>& src)
{
    static_assert(detail::supports_relu_v<T>, "relu supports half, int32_t, and float");
    reg_tensor<T> dst;
    asc_relu(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> prelu(const reg_tensor<T>& src, const reg_tensor<T>& slope)
{
    static_assert(detail::supports_float_math_v<T>, "prelu supports half and float");
    reg_tensor<T> dst;
    asc_prelu(dst.reg, src.reg, slope.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T, typename ScalarType>
__simd_callee__ inline reg_tensor<T> leaky_relu(const reg_tensor<T>& src, const ScalarType& slope)
{
    static_assert(detail::supports_float_math_v<T>, "leaky_relu supports half and float");
    static_assert(!is_reg_tensor_v<ScalarType> &&
            Std::is_convertible_v<Std::remove_cvref_t<ScalarType>, T>,
        "leaky_relu requires a scalar convertible to the reg_tensor element type");
    reg_tensor<T> dst;
    asc_leakyrelu(dst.reg, src.reg, static_cast<T>(slope), src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> add(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "add with carry supports int32_t and uint32_t");
    reg_tensor<T> dst;
    asc_add(carry.reg, dst.reg, src0.reg, src1.reg, src0.mask);
    carry.mask = src0.mask;
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> add_c(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1,
    const reg_tensor<bool>& carry_src)
{
    static_assert(detail::supports_carry_v<T>, "add_c supports int32_t and uint32_t");
    reg_tensor<T> dst;
    asc_addc(carry.reg, dst.reg, src0.reg, src1.reg, carry_src.reg, src0.mask);
    carry.mask = src0.mask;
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> sub(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "sub with carry supports int32_t and uint32_t");
    reg_tensor<T> dst;
    asc_sub(carry.reg, dst.reg, src0.reg, src1.reg, src0.mask);
    carry.mask = src0.mask;
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> sub_c(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1,
    const reg_tensor<bool>& carry_src)
{
    static_assert(detail::supports_carry_v<T>, "sub_c supports int32_t and uint32_t");
    reg_tensor<T> dst;
    asc_subc(carry.reg, dst.reg, src0.reg, src1.reg, carry_src.reg, src0.mask);
    carry.mask = src0.mask;
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> mull(
    reg_tensor<T>& high, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mull_v<T>, "mull supports int32_t and uint32_t");
    reg_tensor<T> low;
    asc_mull(low.reg, high.reg, src0.reg, src1.reg, src0.mask);
    low.mask = src0.mask;
    high.mask = src0.mask;
    return low;
}

template <typename T, typename U>
__simd_callee__ inline decltype(auto) operator/(const T& a, const U& b)
{
    using elem_type = detail::binary_elem_t<T, U>;
    detail::check_binary_reg_types<T, U>();
    static_assert(detail::supports_div_v<elem_type>, "operator/ does not support this element type");

    vector_bool mask = detail::binary_mask(a, b);
    auto left = detail::make_reg_operand<elem_type>(a, mask);
    auto right = detail::make_reg_operand<elem_type>(b, mask);
    reg_tensor<elem_type> dst;
    asc_div(dst.reg, left.reg, right.reg, mask);
    dst.mask = mask;
    return dst;
}

template <typename T, typename U>
__simd_callee__ inline decltype(auto) min(const T& a, const U& b)
{
    using elem_type = detail::binary_elem_t<T, U>;
    detail::check_binary_reg_types<T, U>();
    static_assert(detail::supports_min_max_v<elem_type>, "min does not support this element type");

    vector_bool mask = detail::binary_mask(a, b);
    auto left = detail::make_reg_operand<elem_type>(a, mask);
    auto right = detail::make_reg_operand<elem_type>(b, mask);
    reg_tensor<elem_type> dst;
    asc_min(dst.reg, left.reg, right.reg, mask);
    dst.mask = mask;
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H
#endif
