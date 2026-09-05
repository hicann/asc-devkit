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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE
#endif

/*!
 * \file reduction_compute.h
 * \brief Register reduction compute interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_H

#include "tensor_api/experimental/utils/reg_enum.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @brief Reduces register elements by summation according to the selected reduction scope.
 * @param src Source register tensor.
 * @return The reduced register tensor with the source mask preserved.
 * @note Element and datablock reduction support int16_t to int32_t, int32_t, uint16_t to uint32_t, uint32_t,
 * half, and float. Pair reduction supports matching half or float types.
 */
template <typename DstType, reduce_scope scope = reduce_scope::element, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_sum(const reg_tensor<SrcType>& src);

/**
 * @brief Reduces register elements by maximum according to the selected reduction scope.
 * @param src Source register tensor.
 * @return The reduced register tensor with the source mask preserved.
 * @note Supported scopes are element and datablock. Source and destination types must match and be one of
 * int16_t, uint16_t, half, int32_t, uint32_t, or float.
 */
template <typename DstType, reduce_scope scope = reduce_scope::element, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_max(const reg_tensor<SrcType>& src);

/**
 * @brief Reduces register elements by minimum according to the selected reduction scope.
 * @param src Source register tensor.
 * @return The reduced register tensor with the source mask preserved.
 * @note Supported scopes are element and datablock. Source and destination types must match and be one of
 * int16_t, uint16_t, half, int32_t, uint32_t, or float.
 */
template <typename DstType, reduce_scope scope = reduce_scope::element, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_min(const reg_tensor<SrcType>& src);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reduction_compute_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE
#endif
