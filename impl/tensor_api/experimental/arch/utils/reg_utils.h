/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the license.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_UTILS
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_UTILS_REG_UTILS_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_UTILS_REG_UTILS_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/utils/utils.h"
#include "utils/std/type_traits.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <typename T>
inline constexpr bool is_b4_type_v = AscendC::Std::is_one_of_v<T, fp4x2_e1m2_t, fp4x2_e2m1_t, int4x2_t>;

template <typename T>
inline constexpr bool is_b8_type_v =
    AscendC::Std::is_one_of_v<T, int8_t, uint8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t>;

template <typename T>
inline constexpr bool is_b16_type_v = AscendC::Std::is_one_of_v<T, int16_t, uint16_t, half, bfloat16_t>;

template <typename T>
inline constexpr bool is_b32_type_v = AscendC::Std::is_one_of_v<T, int32_t, uint32_t, float>;

template <typename T>
inline constexpr bool is_b64_type_v = AscendC::Std::is_one_of_v<T, int64_t>;

template <typename T, typename U>
__simd_callee__ inline reg_tensor<T> make_reg_operand(const U& value, const vector_bool& mask)
{
    if constexpr (is_reg_tensor_v<U>) {
        return value;
    } else {
        reg_tensor<T> dst;
        asc_duplicate_scalar(dst.reg, static_cast<T>(value), mask);
        dst.mask = mask;
        return dst;
    }
}

template <typename T>
inline constexpr bool supports_float_math_v = ::AscendC::Std::is_one_of_v<T, half, float>;

template <typename T>
inline constexpr bool supports_add_sub_v =
    ::AscendC::Std::is_one_of_v<T, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float>;

template <typename T>
inline constexpr bool supports_mul_v =
    ::AscendC::Std::is_one_of_v<T, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float>;

template <typename T>
inline constexpr bool supports_mul_scalar_v =
    ::AscendC::Std::is_one_of_v<T, int16_t, uint16_t, half, int32_t, uint32_t, float>;

template <typename T>
inline constexpr bool supports_min_max_v =
    ::AscendC::Std::is_one_of_v<T, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float>;

template <typename T>
inline constexpr bool supports_or_v =
    ::AscendC::Std::is_one_of_v<T, bool, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float>;

} // namespace detail
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_UTILS_REG_UTILS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_UTILS)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_UTILS
#endif
