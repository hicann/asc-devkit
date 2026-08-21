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
#ifndef INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_H
#define INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_H

#include "tensor_api/utils/utils.h"
#include "tensor_api/arch/vector/experimental/reg_tensor.h"

namespace asc {
namespace te {

template <typename DstType, reduce_scope scope = reduce_scope::vector, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_sum(reg_tensor<SrcType> src);

template <typename DstType, reduce_scope scope = reduce_scope::vector, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_max(reg_tensor<SrcType> src);

template <typename DstType, reduce_scope scope = reduce_scope::vector, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_min(reg_tensor<SrcType> src);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/vector/experimental/reduction_compute_impl.h"

#endif // INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE
#endif
