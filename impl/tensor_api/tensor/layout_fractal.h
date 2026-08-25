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
    "impl/tensor_api/tensor/layout_fractal.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_fractal.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_FRACTAL_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_FRACTAL_H

#include "impl/tensor_api/utils/utils_impl.h"

namespace AscendC {
namespace Te {

template <typename T, typename U, size_t... Is>
__aicore__ inline decltype(auto) MakeFractalShape(T originShape, U innerShape, Std::index_sequence<Is...>)
{
    auto outerShape = Std::make_tuple(Std::ceil_division(Std::get<Is>(originShape), Std::get<Is>(innerShape))...);
    return MakeShape(MakeShape(Std::get<Is>(innerShape), Std::get<Is>(outerShape))...);
}

template <typename T, typename U>
__aicore__ inline decltype(auto) MakeFractalShape(T originShape, U innerShape)
{
    static_assert(Std::tuple_size_v<T> == Std::tuple_size_v<U>, "OriginShape and InnerShape must match");
    return MakeFractalShape(originShape, innerShape, Std::make_index_sequence<Std::tuple_size_v<U>>{});
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_FRACTAL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
