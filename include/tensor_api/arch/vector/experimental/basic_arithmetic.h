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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC
#endif

#ifndef INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_BASIC_ARITHMETIC_H
#define INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_BASIC_ARITHMETIC_H

#include "tensor_api/arch/vector/experimental/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {

template <typename T>
__simd_callee__ inline decltype(auto) abs(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) exp(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) sqrt(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) log(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) log2(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) log10(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) operator-(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) relu(const reg_tensor<T>& src_reg);

template <typename T>
__simd_callee__ inline decltype(auto) prelu(const reg_tensor<T>& src_reg, const reg_tensor<T>& slope);

template <typename T, typename scalar_type>
__simd_callee__ inline decltype(auto) leaky_relu(const reg_tensor<T>& src_reg, const scalar_type& slope);

template <typename T>
__simd_callee__ inline decltype(auto) operator+(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T>
__simd_callee__ inline decltype(auto) operator+(const reg_tensor<T>& src, const T& scalar);

template <typename T>
__simd_callee__ inline decltype(auto) operator+(const T& scalar, const reg_tensor<T>& src);

template <typename T>
__simd_callee__ inline decltype(auto) add(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T>
__simd_callee__ inline decltype(auto) add_c(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1,
    const reg_tensor<bool>& carry_src);

template <typename T, typename U>
__simd_callee__ inline decltype(auto) operator-(const T& a, const U& b);

template <typename T>
__simd_callee__ inline decltype(auto) sub(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T>
__simd_callee__ inline decltype(auto) sub_c(
    reg_tensor<bool>& carry, const reg_tensor<T>& src0, const reg_tensor<T>& src1,
    const reg_tensor<bool>& carry_src);

template <typename T, typename U>
__simd_callee__ inline decltype(auto) operator*(const T& a, const U& b);

template <typename T>
__simd_callee__ inline decltype(auto) mull(
    reg_tensor<T>& high, const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T, typename U>
__simd_callee__ inline decltype(auto) operator/(const T& a, const U& b);

template <typename T, typename U>
__simd_callee__ inline decltype(auto) max(const T& a, const U& b);

template <typename T, typename U>
__simd_callee__ inline decltype(auto) min(const T& a, const U& b);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/arch/vector/experimental/basic_arithmetic_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_BASIC_ARITHMETIC_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC
#endif
