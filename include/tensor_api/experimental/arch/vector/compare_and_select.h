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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_H
#endif

#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {
/**
 * @brief Selects elements from two register tensors according to a condition tensor.
 * @param condition Per-element selection condition.
 * @param src0 Source tensor selected when the corresponding condition is true.
 * @param src1 Source tensor selected when the corresponding condition is false.
 * @return A register tensor containing the selected elements.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> select(
    const reg_tensor<bool>& condition, const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares two register tensors for element-wise equality.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the equality results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares two register tensors for element-wise inequality.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the inequality results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares whether elements in one register tensor are less than those in another.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the less-than results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares whether elements in one register tensor are less than or equal to those in another.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the less-than-or-equal results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares whether elements in one register tensor are greater than those in another.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the greater-than results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares whether elements in one register tensor are greater than or equal to those in another.
 * @param src0 First source tensor.
 * @param src1 Second source tensor.
 * @return A boolean register tensor containing the greater-than-or-equal results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @brief Compares register tensor elements with a scalar for equality.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the equality results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(const reg_tensor<T>& src0, const T& src1);

/**
 * @brief Compares register tensor elements with a scalar for inequality.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the inequality results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(const reg_tensor<T>& src0, const T& src1);

/**
 * @brief Compares whether register tensor elements are less than a scalar.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the less-than results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(const reg_tensor<T>& src0, const T& src1);

/**
 * @brief Compares whether register tensor elements are less than or equal to a scalar.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the less-than-or-equal results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(const reg_tensor<T>& src0, const T& src1);

/**
 * @brief Compares whether register tensor elements are greater than a scalar.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the greater-than results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(const reg_tensor<T>& src0, const T& src1);

/**
 * @brief Compares whether register tensor elements are greater than or equal to a scalar.
 * @param src0 Source tensor.
 * @param src1 Scalar value to compare with each tensor element.
 * @return A boolean register tensor containing the greater-than-or-equal results.
 */
template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(const reg_tensor<T>& src0, const T& src1);
} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/compare_and_select_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_H
#endif
