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
#warning                                                                                                               \
    "impl/tensor_api/arch/vector/experimental/reduction_compute_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL
#endif

/*!
 * \file reduction_compute_impl.h
 * \brief Register reduction compute implementations.
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_IMPL_H

#include "tensor_api/utils/utils.h"
#include "tensor_api/arch/vector/experimental/reg_tensor.h"

namespace asc {
namespace te {

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_sum(reg_tensor<SrcType> src)
{
    reg_tensor<DstType> dst;
    if constexpr (scope == reduce_scope::vector) {
        asc_reduce_sum(dst.reg, src.reg, src.mask);
    } else if constexpr (scope == reduce_scope::datablock) {
        asc_reduce_sum_datablock(dst.reg, src.reg, src.mask);
    } else {
        asc_pair_reduce_sum(dst.reg, src.reg, src.mask);
    }
    dst.mask = src.mask;
    return dst;
}

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_max(reg_tensor<SrcType> src)
{
    static_assert(scope == reduce_scope::vector || scope == reduce_scope::datablock,
                  "reduce_max only supports reduce_scope::vector and reduce_scope::datablock");
    reg_tensor<DstType> dst;
    if constexpr (scope == reduce_scope::vector) {
        asc_reduce_max(dst.reg, src.reg, src.mask);
    } else if constexpr (scope == reduce_scope::datablock) {
        asc_reduce_max_datablock(dst.reg, src.reg, src.mask);
    }
    dst.mask = src.mask;
    return dst;
}

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline decltype(auto) reduce_min(reg_tensor<SrcType> src)
{
    static_assert(scope == reduce_scope::vector || scope == reduce_scope::datablock,
                  "reduce_min only supports reduce_scope::vector and reduce_scope::datablock");
    reg_tensor<DstType> dst;
    if constexpr (scope == reduce_scope::vector) {
        asc_reduce_min(dst.reg, src.reg, src.mask);
    } else if constexpr (scope == reduce_scope::datablock) {
        asc_reduce_min_datablock(dst.reg, src.reg, src.mask);
    }
    dst.mask = src.mask;
    return dst;
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REDUCTION_COMPUTE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL
#endif
