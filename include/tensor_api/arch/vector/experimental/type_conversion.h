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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_PUBLIC_TYPE_CONVERSION_H__
#endif

#ifndef INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_TYPE_CONVERSION_H
#define INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_TYPE_CONVERSION_H

#include "tensor_api/utils/utils.h"
#include "tensor_api/arch/vector/experimental/reg_tensor.h"

namespace asc {
namespace te {

constexpr cast_options default_cast_ops = {
    cast_layout::zero, cast_round_mode::rint, cast_sat_mode::not_sat};

template <typename DstType, const cast_options& opt = default_cast_ops, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(reg_tensor<SrcType> src);

template <typename T>
__simd_callee__ inline reg_tensor<T> trunc(reg_tensor<T> src);

} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/arch/vector/experimental/type_conversion_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_TYPE_CONVERSION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_PUBLIC_TYPE_CONVERSION_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_PUBLIC_TYPE_CONVERSION_H__
#endif
