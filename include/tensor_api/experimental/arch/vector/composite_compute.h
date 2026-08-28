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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE
#endif

#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {

template <typename T, typename scalar_type>
__simd_callee__ inline reg_tensor<T> axpy(
    const reg_tensor<T>& dst, const reg_tensor<T>& src, const scalar_type& scalar);

template <typename T>
__simd_callee__ inline reg_tensor<T> abs_diff(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T>
__simd_callee__ inline reg_tensor<float> exp_diff(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

template <typename T>
__simd_callee__ inline reg_tensor<T> fma(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<T>& src2);

__simd_callee__ inline reg_tensor<half> muls_cast(const reg_tensor<float>& src, const float& scalar);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/composite_compute_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE
#endif
