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
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_H__
#endif

#ifndef INCLUDE_TENSOR_API_TENSOR_LOCAL_TENSOR_H
#define INCLUDE_TENSOR_API_TENSOR_LOCAL_TENSOR_H

#include "tensor_api/tensor/tensor.h"
#include "tensor_api/utils/utils.h"

namespace asc {
namespace te {

template <typename EngineT, typename LayoutT>
struct local_tensor : public base_tensor<EngineT, LayoutT> {
    using tensor_api_base = base_tensor<EngineT, LayoutT>;
    using element_type = typename tensor_api_base::element_type;
    using data_type = get_attribute_element_type<element_type*>;
    using layoutType = typename tensor_api_base::layout_type;

    __aicore__ inline local_tensor();
    __aicore__ inline local_tensor(const EngineT& engine, const LayoutT& layout);
};

} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/tensor/local_tensor_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_TENSOR_LOCAL_TENSOR_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_H__
#endif
