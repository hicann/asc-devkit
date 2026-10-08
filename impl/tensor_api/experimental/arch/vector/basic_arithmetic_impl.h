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
__simd_callee__ inline reg_tensor<T> ln(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "ln does not support this element type");
    reg_tensor<T> dst;
    asc_ln(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log(const reg_tensor<T>& src)
{
    return ln(src);
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
__simd_callee__ inline reg_tensor<T> neg(const reg_tensor<T>& src)
{
    static_assert(detail::supports_neg_v<T>, "neg does not support this element type");
    reg_tensor<T> dst;
    asc_neg(dst.reg, src.reg, src.mask);
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
    static_assert(detail::supports_float_math_v<T>, "log2 does not support this element type");
    const T ln2_reciprocal = static_cast<T>(1.4426950408889634);
    reg_tensor<T> natural_log;
    asc_ln(natural_log.reg, src.reg, src.mask);
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, natural_log.reg, ln2_reciprocal, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log10(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "log10 does not support this element type");
    const T ln10_reciprocal = static_cast<T>(0.43429448190325176);
    reg_tensor<T> natural_log;
    asc_ln(natural_log.reg, src.reg, src.mask);
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, natural_log.reg, ln10_reciprocal, src.mask);
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

template <typename T>
__simd_callee__ inline reg_tensor<T> leaky_relu(const reg_tensor<T>& src, const T& slope)
{
    static_assert(detail::supports_float_math_v<T>, "leaky_relu does not support this element type");
    reg_tensor<T> dst;
    asc_leakyrelu(dst.reg, src.reg, slope, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "add_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_add(result.second.reg, result.first.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src)
{
    static_assert(detail::supports_carry_v<T>, "add_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_addc(result.second.reg, result.first.reg, src0.reg, src1.reg, carry_src.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "sub_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_sub(result.second.reg, result.first.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& borrow_src)
{
    static_assert(detail::supports_carry_v<T>, "sub_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_subc(result.second.reg, result.first.reg, src0.reg, src1.reg, borrow_src.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T> mull(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mull_v<T>, "mull does not support this element type");
    reg_pair<T> result;
    asc_mull(result.first.reg, result.second.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator/(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_div_v<T>, "operator/ does not support this element type");
    reg_tensor<T> dst;
    asc_div(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
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
