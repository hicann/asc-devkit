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
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H__
#endif

#ifndef INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_H
#define INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_H

#include "impl/tensor_api/arch/vector/experimental/type_get.h"

namespace asc {
namespace te {

template <typename DataType>
struct reg_tensor {
    using elem_type = DataType;
    using reg_type = typename detail::type_get<elem_type>::type;

    __simd_callee__ reg_tensor() = default;
    __simd_callee__ reg_tensor(vector_bool maskValue) : mask(maskValue) {}

    template <typename ScalarType>
    __simd_callee__ inline reg_tensor& operator=(ScalarType src);

    __simd_callee__ inline reg_tensor& with_mask(const reg_tensor<bool>& maskReg);

    reg_type reg;
    vector_bool mask;
};

template <typename T>
struct is_reg_tensor : AscendC::Std::false_type {};

template <typename DataType>
struct is_reg_tensor<reg_tensor<DataType>> : AscendC::Std::true_type {};

template <typename T>
inline constexpr bool is_reg_tensor_v = is_reg_tensor<AscendC::Std::remove_cvref_t<T>>::value;

template <typename DataType>
struct reg_pair {
    reg_tensor<DataType> first;
    reg_tensor<DataType> second;
};

} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__)
#include "impl/tensor_api/arch/vector/experimental/reg_tensor_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_REG_TENSOR_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H__
#endif
