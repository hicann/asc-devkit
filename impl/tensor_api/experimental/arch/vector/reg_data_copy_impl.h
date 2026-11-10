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
    "impl/tensor_api/experimental/arch/vector/reg_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/reg_data_copy.h\"\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_IMPL_H

namespace asc {
namespace te {
namespace experimental {
namespace detail {
template <typename T>
inline constexpr bool is_move_support_type =
    Std::is_one_of_v<T, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float>;
} // namespace detail

template <int16_t Offset, typename T>
__simd_callee__ inline reg_tensor<bool> move_mask(const reg_tensor<T>& src)
{
    static_assert(
        AscendC::Std::is_one_of_v<T, uint16_t, uint32_t>, "move_mask only supports uint16_t and uint32_t data types");
    static_assert(
        Offset >= 0 && ((AscendC::Std::is_same_v<T, uint16_t> && Offset < 16) ||
                        (AscendC::Std::is_same_v<T, uint32_t> && Offset < 32)),
        "move_mask offset is out of range");
    reg_tensor<bool> dst;
    dst.reg = asc_extract_mask(src.reg, Offset);
    dst.with_mask(all_mask<uint8_t>());
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> move(reg_tensor<T>& dst, const reg_tensor<T>& src)
{
    static_assert(
        detail::is_move_support_type<T>, "move with dst input only supports uint8_t, int8_t, uint16_t,"
                                         "int16_t, half, bfloat16_t, uint32_t, int32_t and float.");
    asc_copy(dst.reg, src.reg, src.mask);
    return dst;
}

__simd_callee__ inline reg_tensor<bool> move(const reg_tensor<bool>& src)
{
    reg_tensor<bool> dst;
    asc_copy(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}
} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_IMPL_H
#endif
