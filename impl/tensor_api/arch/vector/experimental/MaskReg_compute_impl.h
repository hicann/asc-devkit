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
    "impl/tensor_api/arch/vector/experimental/MaskReg_compute_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/arch/vector/experimental/MaskReg_compute.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MASK_REG_COMPUTE_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_MASK_REG_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_MASK_REG_COMPUTE_IMPL_H

#include "tensor_api/arch/vector/experimental/MaskReg_compute.h"

namespace asc {
namespace te {
namespace detail {

template <typename DataType, typename PatternType>
__simd_callee__ inline reg_tensor<bool> make_reg_mask(PatternType pattern)
{
    static_assert(sizeof(DataType) == sizeof(uint8_t) || sizeof(DataType) == sizeof(uint16_t)
                      || sizeof(DataType) == sizeof(uint32_t),
                  "mask helpers only support 8-bit, 16-bit, and 32-bit element types");

    reg_tensor<bool> result;
    if constexpr (sizeof(DataType) == sizeof(uint32_t)) {
        result.reg = asc_create_mask_b32(pattern);
    } else if constexpr (sizeof(DataType) == sizeof(uint16_t)) {
        result.reg = asc_create_mask_b16(pattern);
    } else if constexpr (sizeof(DataType) == sizeof(uint8_t)) {
        result.reg = asc_create_mask_b8(pattern);
    }
    return result;
}

template <typename DataType>
__simd_callee__ inline reg_tensor<bool> make_updated_reg_mask(uint32_t& remain)
{
    static_assert(sizeof(DataType) == sizeof(uint8_t) || sizeof(DataType) == sizeof(uint16_t)
                      || sizeof(DataType) == sizeof(uint32_t),
                  "update_mask only supports 8-bit, 16-bit, and 32-bit element types");

    reg_tensor<bool> result;
    if constexpr (sizeof(DataType) == sizeof(uint32_t)) {
        result.reg = asc_update_mask_b32(remain);
    } else if constexpr (sizeof(DataType) == sizeof(uint16_t)) {
        result.reg = asc_update_mask_b16(remain);
    } else if constexpr (sizeof(DataType) == sizeof(uint8_t)) {
        result.reg = asc_update_mask_b8(remain);
    }
    return result;
}

} // namespace detail

template <mask_pattern Pattern, typename DataType>
__simd_callee__ inline reg_tensor<bool> make_mask()
{
    if constexpr (Pattern == mask_pattern::all) {
        return detail::make_reg_mask<DataType>(PAT_ALL);
    } else if constexpr (Pattern == mask_pattern::vl1) {
        return detail::make_reg_mask<DataType>(PAT_VL1);
    } else if constexpr (Pattern == mask_pattern::vl2) {
        return detail::make_reg_mask<DataType>(PAT_VL2);
    } else if constexpr (Pattern == mask_pattern::vl3) {
        return detail::make_reg_mask<DataType>(PAT_VL3);
    } else if constexpr (Pattern == mask_pattern::vl4) {
        return detail::make_reg_mask<DataType>(PAT_VL4);
    } else if constexpr (Pattern == mask_pattern::vl8) {
        return detail::make_reg_mask<DataType>(PAT_VL8);
    } else if constexpr (Pattern == mask_pattern::vl16) {
        return detail::make_reg_mask<DataType>(PAT_VL16);
    } else if constexpr (Pattern == mask_pattern::vl32) {
        return detail::make_reg_mask<DataType>(PAT_VL32);
    } else if constexpr (Pattern == mask_pattern::vl64) {
        return detail::make_reg_mask<DataType>(PAT_VL64);
    } else if constexpr (Pattern == mask_pattern::vl128) {
        return detail::make_reg_mask<DataType>(PAT_VL128);
    } else if constexpr (Pattern == mask_pattern::every3) {
        return detail::make_reg_mask<DataType>(PAT_M3);
    } else if constexpr (Pattern == mask_pattern::every4) {
        return detail::make_reg_mask<DataType>(PAT_M4);
    } else if constexpr (Pattern == mask_pattern::half) {
        return detail::make_reg_mask<DataType>(PAT_H);
    } else if constexpr (Pattern == mask_pattern::quarter) {
        return detail::make_reg_mask<DataType>(PAT_Q);
    } else if constexpr (Pattern == mask_pattern::none) {
        return detail::make_reg_mask<DataType>(PAT_ALLF);
    } else {
        static_assert(Pattern == mask_pattern::all, "unsupported mask pattern");
    }
}

template <typename DataType>
__simd_callee__ inline reg_tensor<bool> all_mask()
{
    return make_mask<mask_pattern::all, DataType>();
}

template <typename DataType>
__simd_callee__ inline reg_tensor<bool> none_mask()
{
    return make_mask<mask_pattern::none, DataType>();
}

template <typename DataType>
__simd_callee__ inline reg_tensor<bool> update_mask(uint32_t& remain)
{
    return detail::make_updated_reg_mask<DataType>(remain);
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_MASK_REG_COMPUTE_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MASK_REG_COMPUTE_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_MASK_REG_COMPUTE_IMPL_H__
#endif
