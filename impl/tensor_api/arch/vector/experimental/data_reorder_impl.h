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
    "impl/tensor_api/arch/vector/experimental/data_reorder_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/arch/vector/experimental/data_reorder.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_DATA_REORDER_IMPL_H
#define IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_DATA_REORDER_IMPL_H

#include "tensor_api/arch/vector/experimental/data_reorder.h"

namespace asc {
namespace te {

template <typename DataType>
__simd_callee__ inline reg_pair<bool> interleave(reg_tensor<bool> src0, reg_tensor<bool> src1)
{
    static_assert(sizeof(DataType) == sizeof(uint8_t) || sizeof(DataType) == sizeof(uint16_t)
                      || sizeof(DataType) == sizeof(uint32_t),
                  "interleave only supports 8-bit, 16-bit, and 32-bit element types");

    reg_pair<bool> result;
    if constexpr (sizeof(DataType) == sizeof(uint32_t)) {
        asc_intlv_b32(result.first.reg, result.second.reg, src0.reg, src1.reg);
    } else if constexpr (sizeof(DataType) == sizeof(uint16_t)) {
        asc_intlv_b16(result.first.reg, result.second.reg, src0.reg, src1.reg);
    } else if constexpr (sizeof(DataType) == sizeof(uint8_t)) {
        asc_intlv_b8(result.first.reg, result.second.reg, src0.reg, src1.reg);
    }
    return result;
}

template <typename DataType>
__simd_callee__ inline reg_pair<bool> deinterleave(reg_tensor<bool> src0, reg_tensor<bool> src1)
{
    static_assert(sizeof(DataType) == sizeof(uint8_t) || sizeof(DataType) == sizeof(uint16_t)
                      || sizeof(DataType) == sizeof(uint32_t),
                  "deinterleave only supports 8-bit, 16-bit, and 32-bit element types");

    reg_pair<bool> result;
    if constexpr (sizeof(DataType) == sizeof(uint32_t)) {
        asc_deintlv_b32(result.first.reg, result.second.reg, src0.reg, src1.reg);
    } else if constexpr (sizeof(DataType) == sizeof(uint16_t)) {
        asc_deintlv_b16(result.first.reg, result.second.reg, src0.reg, src1.reg);
    } else if constexpr (sizeof(DataType) == sizeof(uint8_t)) {
        asc_deintlv_b8(result.first.reg, result.second.reg, src0.reg, src1.reg);
    }
    return result;
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_DATA_REORDER_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_REORDER_IMPL_H__
#endif
