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
    "impl/tensor_api/experimental/arch/vector/reg_gather_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/gather_compute.h\"\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_GATHER_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_GATHER_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_GATHER_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/tensor/tensor_interface.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <typename DataType, typename IndexType>
inline constexpr bool is_gather_reg_support_type_v =
    (Std::is_same_v<IndexType, uint8_t> &&
     AscendC::Std::is_one_of_v<DataType, int8_t, uint8_t, fp8_e4m3fn_t, hifloat8_t, fp8_e5m2_t, fp8_e8m0_t>) ||
    (Std::is_same_v<IndexType, uint16_t> && AscendC::Std::is_one_of_v<DataType, int16_t, uint16_t, half, bfloat16_t>) ||
    (Std::is_same_v<IndexType, uint32_t> && AscendC::Std::is_one_of_v<DataType, int32_t, uint32_t, float>);

} // namespace detail

template <typename DataType, typename IndexType>
__simd_callee__ inline reg_tensor<DataType> gather(const reg_tensor<DataType>& src, const reg_tensor<IndexType>& index)
{
    static_assert(
        detail::is_gather_reg_support_type_v<DataType, IndexType>,
        "gather from register does not support this source and index type combination");

    reg_tensor<DataType> dst;
    asc_gather(dst.reg, src.reg, index.reg);
    dst.with_mask(all_mask<DataType>());
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_GATHER_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_GATHER_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_GATHER_IMPL_H
#endif
