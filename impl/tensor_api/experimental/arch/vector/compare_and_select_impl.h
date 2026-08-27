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
    "impl/tensor_api/experimental/reg/compare_and_select_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_IMPL_H

#include <cstdint>

namespace asc {
namespace te {
namespace experimental {
namespace detail {
template <typename T>
inline constexpr bool is_select_support_type = Std::is_one_of_v<T, bool, uint8_t, int8_t, fp8_e4m3fn_t,
    hifloat8_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float>;

template <typename T>
inline constexpr bool is_cmp_support_type = Std::is_one_of_v<T, uint8_t, int8_t, uint16_t, int16_t,
    half, bfloat16_t, uint32_t, int32_t, float>;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> select(const reg_tensor<bool>& condition, const reg_tensor<T>& src0,
    const reg_tensor<T>& src1) {
    static_assert(detail::is_select_support_type<T>, "select only supports bool, uint8_t, int8_t, "
        "fp8_e4m3fn_t, hifloat8_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    reg_tensor<T> dst;
    asc_select(dst.reg, src0.reg, src1.reg, condition.reg);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator== only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_eq(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator!= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_ne(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator< only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_lt(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator<= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_le(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator> only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_gt(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(const reg_tensor<T>& src0, const reg_tensor<T>& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator>= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_ge(mask.reg, src0.reg, src1.reg, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator==(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator== only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_eq_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator!=(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator!= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_ne_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator< only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_lt_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator<=(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator<= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_le_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator> only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_gt_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}

template <typename T>
__simd_callee__ inline reg_tensor<bool> operator>=(const reg_tensor<T>& src0, const T& src1) {
    static_assert(detail::is_cmp_support_type<T>, "operator>= only supports uint8_t, int8_t, "
        "uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    reg_tensor<bool> mask;
    asc_ge_scalar(mask.reg, src0.reg, src1, src0.mask);
    return mask;
}
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPARE_AND_SELECT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPARE_AND_SELECT_IMPL_H
#endif
