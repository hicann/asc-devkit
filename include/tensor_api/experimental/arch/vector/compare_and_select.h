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
