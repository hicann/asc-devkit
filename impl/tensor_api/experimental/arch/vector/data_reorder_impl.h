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
    "impl/tensor_api/experimental/arch/vector/data_reorder_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/experimental/arch/vector/data_reorder.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_REORDER_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_REORDER_IMPL_H

#include "tensor_api/experimental/arch/vector/data_reorder.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {
template <typename T>
inline constexpr bool is_data_reorder_support_type = AscendC::Std::is_one_of_v<T, uint8_t, int8_t,
    fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t,
    int32_t, float>;
} // namespace detail

template <typename T>
__simd_callee__ inline reg_pair<T> interleave(reg_tensor<T> src0, reg_tensor<T> src1) {
    static_assert(detail::is_data_reorder_support_type<T>, "interleave with reg_tensor<T> only supports "
        "uint8_t, int8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, "
        "uint32_t, int32_t and float.");
    reg_pair<T> dst;   
    asc_intlv(dst.first.reg, dst.second.reg, src0.reg, src1.reg);
    dst.first.with_mask(all_mask<T>());
    dst.second.mask = dst.first.mask;
    return dst;
}
 
template <typename T>
__simd_callee__ inline reg_pair<T> deinterleave(reg_tensor<T> src0, reg_tensor<T> src1) {
    static_assert(detail::is_data_reorder_support_type<T>, "deinterleave with reg_tensor<T> only supports "
        "uint8_t, int8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, "
        "uint32_t, int32_t and float.");
    reg_pair<T> dst;   
    asc_deintlv(dst.first.reg, dst.second.reg, src0.reg, src1.reg);
    dst.first.with_mask(all_mask<T>());
    dst.second.mask = dst.first.mask;
    return dst;
}
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_REORDER_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__
#endif
