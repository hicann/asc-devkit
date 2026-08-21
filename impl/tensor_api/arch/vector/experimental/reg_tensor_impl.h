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
    "impl/tensor_api/arch/vector/experimental/reg_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/arch/vector/experimental/reg_tensor.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_IMPL_H
#define IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_IMPL_H

#include "tensor_api/arch/vector/experimental/reg_tensor.h"
#include "impl/tensor_api/utils/utils_impl.h"

namespace asc {
namespace te {

template <typename DataType>
template <typename ScalarType>
__simd_callee__ inline reg_tensor<DataType>& reg_tensor<DataType>::operator=(ScalarType src)
{
    static_assert(Std::is_one_of_v<ScalarType, bool, uint8_t, int8_t, fp8_e4m3fn_t,
    fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float>, "operator= only supports bool、uint8_t, int8_t, "
    "fp8_e4m3fn_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
    "and float.");
    if constexpr (AscendC::Std::is_same_v<elem_type, bool>) {
        asc_duplicate_scalar(reinterpret_cast<vector_int8_t&>(reg), static_cast<int8_t>(src), mask);
    } else {
        asc_duplicate_scalar(reg, static_cast<elem_type>(src), mask);
    }
    return *this;
}

template <typename DataType>
__simd_callee__ inline reg_tensor<DataType>& reg_tensor<DataType>::with_mask(const reg_tensor<bool>& maskReg)
{
    mask = maskReg.reg;
    return *this;
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_IMPL_H__
#endif
