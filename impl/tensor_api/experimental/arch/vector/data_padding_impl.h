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
    "impl/tensor_api/experimental/arch/vector/data_padding_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_PADDING_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_PADDING_IMPL_H

#include <cstdint>

namespace asc {
namespace te {
namespace experimental {
namespace detail {
template <typename T>
inline constexpr bool is_fill_support_type = Std::is_one_of_v<
    T, uint8_t, int8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t,
    float>;
}

template <size_t N, typename T>
__simd_callee__ inline reg_tensor<T> fill(const T& value, const reg_tensor<bool>& mask)
{
    static_assert(
        detail::is_fill_support_type<T>,
        "fill only supports uint8_t, int8_t, "
        "fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    static_assert(N == 1, "fill only supports N==1.");
    reg_tensor<T> dst;
    asc_duplicate_scalar(dst.reg, value, mask.reg);
    dst.mask = mask.reg;
    return dst;
}

template <size_t N, typename T>
__simd_callee__ inline reg_tensor<T> fill(const reg_tensor<T>& src, const reg_tensor<bool>& mask)
{
    static_assert(
        detail::is_fill_support_type<T>,
        "fill only supports uint8_t, int8_t, "
        "fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    static_assert(N == 1, "fill only supports N==1.");
    reg_tensor<T> dst;
    asc_duplicate(dst.reg, src.reg, mask.reg);
    dst.mask = mask.reg;
    return dst;
}
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_DATA_PADDING_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_PADDING_IMPL_H
#endif
