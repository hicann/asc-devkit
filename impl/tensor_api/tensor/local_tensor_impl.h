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
#warning \
    "impl/tensor_api/tensor/local_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/tensor/local_tensor.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H
#define IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H

#include "tensor_api/tensor/local_tensor.h"

namespace asc {
namespace te {

template <typename EngineT, typename LayoutT>
__aicore__ inline local_tensor<EngineT, LayoutT>::local_tensor() = default;

template <typename EngineT, typename LayoutT>
__aicore__ inline local_tensor<EngineT, LayoutT>::local_tensor(const EngineT& engine, const LayoutT& layout)
    : tensor_api_base(engine, layout)
{}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__
#endif
