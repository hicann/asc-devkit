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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_REG_DATA_PADDING_H
#define IMPL_TENSOR_API_EXPERIMENTAL_REG_DATA_PADDING_H

#include <cstdint>

namespace asc {
namespace te {
template <size_t N = 1, typename T>
__simd_callee__ inline decltype(auto) fill(T value, reg_tensor<bool> mask = all_mask<T>());

template <size_t N = 1, typename T>
__simd_callee__ inline decltype(auto) fill(reg_tensor<T> val, reg_tensor<bool> mask = all_mask<T>());
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__)
#include "impl/tensor_api/arch/vector/experimental/data_padding_impl.h"
#endif

#endif // IMPL_TENSOR_API_EXPERIMENTAL_REG_DATA_PADDING_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_H
#endif
