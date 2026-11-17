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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_H
#endif

/*!
 * \file type_conversion.h
 * \brief Register data type conversion interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_H

#include "tensor_api/experimental/utils/reg_enum.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @ingroup tensor_api.experimental.reg_compute.reg_cast
 * @brief Converts the active elements of a register tensor to another element type.
 *
 * @override_id te_experimental_cast__const_reg_tensor_SrcType_R__template_DstType_opt_SrcType
 * @archs 3510
 * @return The converted register tensor with the source mask preserved.
 * @note Source and destination element types must differ. Supported option combinations are architecture-dependent.
 * @param [in] src Source register tensor.
 */
template <typename DstType, const cast_options& opt, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(const reg_tensor<SrcType>& src);

/**
 * @ingroup tensor_api.experimental.reg_compute.reg_cast
 * @brief Rounds each active register element using the selected mode.
 *
 * @override_id te_experimental_trunc__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @return The rounded register tensor with the source mask preserved.
 * @note Supported element types are half, bfloat16_t, and float. The default mode is trunc.
 * @tparam Mode Rounding mode: trunc, round, rint, ceil, or floor.
 * @param [in] src Source register tensor.
 */
template <cast_round_mode Mode = cast_round_mode::trunc, typename T>
__simd_callee__ inline reg_tensor<T> trunc(const reg_tensor<T>& src);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/type_conversion_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_H
#endif
