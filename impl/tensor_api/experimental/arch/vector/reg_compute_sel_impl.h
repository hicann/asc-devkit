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
    "impl/tensor_api/experimental/arch/vector/reg_compute_sel_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/reg_compute_sel.h\"\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_IMPL_H

#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {
template <typename T>
struct pack_dst_type {
    using type = void;
};

template <>
struct pack_dst_type<bool> {
    using type = bool;
};

template <>
struct pack_dst_type<uint16_t> {
    using type = uint8_t;
};

template <>
struct pack_dst_type<int16_t> {
    using type = uint8_t;
};

template <>
struct pack_dst_type<uint32_t> {
    using type = uint16_t;
};

template <>
struct pack_dst_type<int32_t> {
    using type = uint16_t;
};

template <typename T>
using pack_dst_t = typename pack_dst_type<T>::type;

template <typename T>
inline constexpr bool is_pack_support_type_v = !AscendC::Std::is_same_v<pack_dst_t<T>, void>;

template <typename T>
struct unpack_dst_type {
    using type = void;
};

template <>
struct unpack_dst_type<bool> {
    using type = bool;
};

template <>
struct unpack_dst_type<uint8_t> {
    using type = uint16_t;
};

template <>
struct unpack_dst_type<int8_t> {
    using type = int16_t;
};

template <>
struct unpack_dst_type<uint16_t> {
    using type = uint32_t;
};

template <>
struct unpack_dst_type<int16_t> {
    using type = int32_t;
};

template <typename T>
using unpack_dst_t = typename unpack_dst_type<T>::type;

template <typename T>
inline constexpr bool is_unpack_support_type_v = !AscendC::Std::is_same_v<unpack_dst_t<T>, void>;

template <typename T>
inline constexpr bool is_compress_support_type_v = AscendC::Std::is_one_of_v<
    T, uint8_t, int8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, hifloat8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t,
    int32_t, float>;

template <typename T>
struct expand_index_type {
    using type = void;
};

template <>
struct expand_index_type<int8_t> {
    using type = uint8_t;
};

template <>
struct expand_index_type<uint8_t> {
    using type = uint8_t;
};

template <>
struct expand_index_type<int16_t> {
    using type = uint16_t;
};

template <>
struct expand_index_type<uint16_t> {
    using type = uint16_t;
};

template <>
struct expand_index_type<int32_t> {
    using type = uint32_t;
};

template <>
struct expand_index_type<uint32_t> {
    using type = uint32_t;
};

template <typename T>
using expand_index_t = typename expand_index_type<T>::type;

template <typename T>
inline constexpr bool is_expand_support_type_v =
    AscendC::Std::is_one_of_v<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>;

} // namespace detail

template <high_low_part part, typename T>
__simd_callee__ inline decltype(auto) pack(reg_tensor<T> src)
{
    static_assert(
        part == high_low_part::low || part == high_low_part::high,
        "pack only supports high_low_part::low and high_low_part::high");
    static_assert(
        detail::is_pack_support_type_v<T>, "pack with reg_tensor<T> only supports "
                                           "bool, uint16_t, int16_t, uint32_t and int32_t.");

    using dst_type = detail::pack_dst_t<T>;
    reg_tensor<dst_type> dst;
    if constexpr (part == high_low_part::low) {
        asc_pack_to_low(dst.reg, src.reg);
    } else if constexpr (part == high_low_part::high) {
        asc_pack_to_high(dst.reg, src.reg);
    }
    dst.with_mask(all_mask<dst_type>());
    return dst;
}

template <high_low_part part, typename T>
__simd_callee__ inline decltype(auto) unpack(reg_tensor<T> src)
{
    static_assert(
        part == high_low_part::low || part == high_low_part::high,
        "unpack only supports high_low_part::low and high_low_part::high");
    static_assert(
        detail::is_unpack_support_type_v<T>, "unpack with reg_tensor<T> only supports "
                                             "bool, uint8_t, int8_t, uint16_t and int16_t.");

    using dst_type = detail::unpack_dst_t<T>;
    reg_tensor<dst_type> dst;
    if constexpr (part == high_low_part::low) {
        asc_unpack_lower(dst.reg, src.reg);
    } else if constexpr (part == high_low_part::high) {
        asc_unpack_upper(dst.reg, src.reg);
    }
    dst.with_mask(all_mask<dst_type>());
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> compress(reg_tensor<T> src)
{
    static_assert(
        detail::is_compress_support_type_v<T>,
        "compress with reg_tensor<T> only supports "
        "uint8_t, int8_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e5m2_t, hifloat8_t, uint16_t, int16_t, half, "
        "bfloat16_t, uint32_t, int32_t and float.");

    reg_tensor<T> dst;
    asc_squeeze(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> expand(reg_tensor<bool> mask)
{
    static_assert(
        detail::is_expand_support_type_v<T>, "expand with reg_tensor<T> only supports "
                                             "int8_t, uint8_t, int16_t, uint16_t, int32_t and uint32_t.");

    reg_tensor<T> dst;
    asc_unsqueeze(dst.reg, mask.reg);
    dst.with_mask(all_mask<T>());
    return dst;
}
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_IMPL_H
#endif
