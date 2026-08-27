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
    "impl/tensor_api/experimental/arch/vector/reduction_compute_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL
#endif

/*!
 * \file reduction_compute_impl.h
 * \brief Register reduction compute implementations.
 */
#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_IMPL_H

#include "tensor_api/experimental/utils/reg_enum.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "utils/std/type_traits.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <typename DstType, typename SrcType>
inline constexpr bool is_reduce_sum_support_type_v =
    (AscendC::Std::is_same_v<DstType, int32_t>
        && AscendC::Std::is_one_of_v<SrcType, int16_t, int32_t>)
    || (AscendC::Std::is_same_v<DstType, uint32_t>
        && AscendC::Std::is_one_of_v<SrcType, uint16_t, uint32_t>)
    || (AscendC::Std::is_same_v<DstType, half> && AscendC::Std::is_same_v<SrcType, half>)
    || (AscendC::Std::is_same_v<DstType, float> && AscendC::Std::is_same_v<SrcType, float>);

template <typename DstType, typename SrcType>
inline constexpr bool is_pair_reduce_sum_support_type_v =
    AscendC::Std::is_same_v<DstType, SrcType> && AscendC::Std::is_one_of_v<SrcType, half, float>;

template <typename DstType, typename SrcType>
inline constexpr bool is_reduce_min_max_support_type_v =
    AscendC::Std::is_same_v<DstType, SrcType>
    && AscendC::Std::is_one_of_v<SrcType, int16_t, uint16_t, half, int32_t, uint32_t, float>;

} // namespace detail

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_sum(const reg_tensor<SrcType>& src)
{
    static_assert(scope == reduce_scope::element || scope == reduce_scope::datablock || scope == reduce_scope::pair,
        "reduce_sum only supports reduce_scope::element, reduce_scope::datablock and reduce_scope::pair");
    reg_tensor<DstType> dst;
    if constexpr (scope == reduce_scope::element) {
        static_assert(detail::is_reduce_sum_support_type_v<DstType, SrcType>,
            "reduce_sum does not support this source and destination type combination");
        if constexpr (detail::is_reduce_sum_support_type_v<DstType, SrcType>) {
            asc_reduce_sum(dst.reg, src.reg, src.mask);
        }
    } else if constexpr (scope == reduce_scope::datablock) {
        static_assert(detail::is_reduce_sum_support_type_v<DstType, SrcType>,
            "datablock reduce_sum does not support this source and destination type combination");
        if constexpr (detail::is_reduce_sum_support_type_v<DstType, SrcType>) {
            asc_reduce_sum_datablock(dst.reg, src.reg, src.mask);
        }
    } else if constexpr (scope == reduce_scope::pair) {
        static_assert(detail::is_pair_reduce_sum_support_type_v<DstType, SrcType>,
            "pair reduce_sum only supports matching half or float source and destination types");
        if constexpr (detail::is_pair_reduce_sum_support_type_v<DstType, SrcType>) {
            asc_pair_reduce_sum(dst.reg, src.reg, src.mask);
        }
    }
    dst.mask = src.mask;
    return dst;
}

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_max(const reg_tensor<SrcType>& src)
{
    static_assert(scope == reduce_scope::element || scope == reduce_scope::datablock,
                  "reduce_max only supports reduce_scope::element and reduce_scope::datablock");
    static_assert(detail::is_reduce_min_max_support_type_v<DstType, SrcType>,
        "reduce_max only supports matching int16_t, uint16_t, half, int32_t, uint32_t or float types");
    reg_tensor<DstType> dst;
    if constexpr (detail::is_reduce_min_max_support_type_v<DstType, SrcType>) {
        if constexpr (scope == reduce_scope::element) {
            asc_reduce_max(dst.reg, src.reg, src.mask);
        } else if constexpr (scope == reduce_scope::datablock) {
            asc_reduce_max_datablock(dst.reg, src.reg, src.mask);
        }
    }
    dst.mask = src.mask;
    return dst;
}

template <typename DstType, reduce_scope scope, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> reduce_min(const reg_tensor<SrcType>& src)
{
    static_assert(scope == reduce_scope::element || scope == reduce_scope::datablock,
                  "reduce_min only supports reduce_scope::element and reduce_scope::datablock");
    static_assert(detail::is_reduce_min_max_support_type_v<DstType, SrcType>,
        "reduce_min only supports matching int16_t, uint16_t, half, int32_t, uint32_t or float types");
    reg_tensor<DstType> dst;
    if constexpr (detail::is_reduce_min_max_support_type_v<DstType, SrcType>) {
        if constexpr (scope == reduce_scope::element) {
            asc_reduce_min(dst.reg, src.reg, src.mask);
        } else if constexpr (scope == reduce_scope::datablock) {
            asc_reduce_min_datablock(dst.reg, src.reg, src.mask);
        }
    }
    dst.mask = src.mask;
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REDUCTION_COMPUTE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REDUCTION_COMPUTE_IMPL
#endif
