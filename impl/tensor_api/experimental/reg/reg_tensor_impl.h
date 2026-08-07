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
    "impl/tensor_api/experimental/reg/reg_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_REG_REG_TENSOR_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_REG_REG_TENSOR_IMPL_H

#include "impl/tensor_api/experimental/reg/type_get.h"

namespace asc {
namespace te {

template <typename DataType>
struct reg_tensor;

template <>
struct reg_tensor<bool>;

template <typename DataType>
struct reg_tensor {
    using type = DataType;
    using reg_type = typename type_get<DataType>::T;

    __simd_callee__ inline reg_tensor with_mask(const reg_tensor<bool>& maskReg){
        mask = maskReg.reg;
        return *this;
    }

    reg_type reg;
    vector_bool mask;
};

template <>
struct reg_tensor<bool> {
    using type = bool;
    using reg_type = typename type_get<bool>::T;

    reg_type reg;
};
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_REG_REG_TENSOR_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR
#endif
