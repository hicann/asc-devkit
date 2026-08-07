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
    "impl/tensor_api/experimental/reg/type_get.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_GET
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_REG_TYPE_GET_H
#define IMPL_TENSOR_API_EXPERIMENTAL_REG_TYPE_GET_H

#include <cstdint>

#include "c_api/asc_simd.h"

namespace asc {
namespace te {

// Keep the primary template incomplete so unsupported element types fail at
// compile time when reg_tensor<T> requests the corresponding register type.
template <typename DataType>
struct type_get;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

template <>
struct type_get<bool> {
    using T = vector_bool;
};

template <>
struct type_get<uint8_t> {
    using T = vector_uint8_t;
};

template <>
struct type_get<uint16_t> {
    using T = vector_uint16_t;
};

template <>
struct type_get<uint32_t> {
    using T = vector_uint32_t;
};

template <>
struct type_get<uint64_t> {
    using T = vector_uint64_t;
};

template <>
struct type_get<int8_t> {
    using T = vector_int8_t;
};

template <>
struct type_get<int16_t> {
    using T = vector_int16_t;
};

template <>
struct type_get<int32_t> {
    using T = vector_int32_t;
};

template <>
struct type_get<int64_t> {
    using T = vector_int64_t;
};

template <>
struct type_get<half> {
    using T = vector_half;
};

template <>
struct type_get<bfloat16_t> {
    using T = vector_bfloat16_t;
};

template <>
struct type_get<float> {
    using T = vector_float;
};

template <>
struct type_get<hifloat8_t> {
    using T = vector_hifloat8_t;
};

template <>
struct type_get<fp8_e4m3fn_t> {
    using T = vector_fp8_e4m3fn_t;
};

template <>
struct type_get<fp8_e5m2_t> {
    using T = vector_fp8_e5m2_t;
};

template <>
struct type_get<fp8_e8m0_t> {
    using T = vector_fp8_e8m0_t;
};

template <>
struct type_get<int4x2_t> {
    using T = vector_int4x2_t;
};

template <>
struct type_get<fp4x2_e2m1_t> {
    using T = vector_fp4x2_e2m1_t;
};

template <>
struct type_get<fp4x2_e1m2_t> {
    using T = vector_fp4x2_e1m2_t;
};

#endif

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_REG_TYPE_GET_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_GET)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_GET
#endif
