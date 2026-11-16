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
    "impl/tensor_api/experimental/arch/vector/data_padding_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/data_padding.h\"\" and use public functions or variables defined in interface headers files."
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

template <high_low_part Part, merge_mode Mode>
struct fill_impl_dispatch;

template <>
struct fill_impl_dispatch<high_low_part::low, merge_mode::zeroing> {
    template <typename T>
    __simd_callee__ static inline void apply(reg_tensor<T>& dst, const reg_tensor<T>& src, const reg_tensor<bool>& mask)
    {
        asc_duplicate(dst.reg, src.reg, mask.reg);
    }
};

template <>
struct fill_impl_dispatch<high_low_part::low, merge_mode::merging> {
    template <typename T>
    __simd_callee__ static inline void apply(reg_tensor<T>& dst, const reg_tensor<T>& src, const reg_tensor<bool>& mask)
    {
        asc_duplicate_merge(dst.reg, src.reg, mask.reg);
    }
};

template <>
struct fill_impl_dispatch<high_low_part::high, merge_mode::zeroing> {
    template <typename T>
    __simd_callee__ static inline void apply(reg_tensor<T>& dst, const reg_tensor<T>& src, const reg_tensor<bool>& mask)
    {
        asc_duplicate_highest(dst.reg, src.reg, mask.reg);
    }
};

template <>
struct fill_impl_dispatch<high_low_part::high, merge_mode::merging> {
    template <typename T>
    __simd_callee__ static inline void apply(reg_tensor<T>& dst, const reg_tensor<T>& src, const reg_tensor<bool>& mask)
    {
        asc_duplicate_highest_merge(dst.reg, src.reg, mask.reg);
    }
};
} // namespace detail

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

template <high_low_part part, size_t N, typename T>
__simd_callee__ inline reg_tensor<T> fill(const reg_tensor<T>& src, const reg_tensor<bool>& mask)
{
    static_assert(
        detail::is_fill_support_type<T>,
        "fill only supports uint8_t, int8_t, "
        "fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    static_assert(N == 1, "fill only supports N==1.");
    reg_tensor<T> dst;
    detail::fill_impl_dispatch<part, merge_mode::zeroing>::apply(dst, src, mask);
    dst.mask = mask.reg;
    return dst;
}

template <merge_mode Mode, size_t N, typename T>
__simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const T& value, const reg_tensor<bool>& mask)
{
    static_assert(
        detail::is_fill_support_type<T>,
        "fill only supports uint8_t, int8_t, "
        "fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    static_assert(N == 1, "fill only supports N==1.");
    if constexpr (Mode == merge_mode::zeroing) {
        asc_duplicate_scalar(dst.reg, value, mask.reg);
    } else {
        asc_duplicate_scalar_merge(dst.reg, value, mask.reg);
    }
    return dst;
}

template <high_low_part part, merge_mode Mode, size_t N, typename T>
__simd_callee__ inline reg_tensor<T> fill(reg_tensor<T>& dst, const reg_tensor<T>& src, const reg_tensor<bool>& mask)
{
    static_assert(
        detail::is_fill_support_type<T>,
        "fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t "
        "and float.");
    static_assert(N == 1, "fill only supports N==1.");
    detail::fill_impl_dispatch<part, Mode>::apply(dst, src, mask);
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
