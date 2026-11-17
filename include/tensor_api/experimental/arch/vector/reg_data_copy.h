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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_H
#endif

/*!
 * \file reg_data_copy.h
 * \brief Register data copy interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_H

#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"

namespace asc {
namespace te {
namespace experimental {
/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_copy
 * @brief Extracts bits from a selected data block of a register tensor and expands them into a register mask.
 *
 * @override_id te_experimental_move_mask__const_reg_tensor_T_R__template_Offset_T
 * @archs 3510
 * @return A register mask containing the extracted and expanded bits.
 * @note Offset selects the source data block. Its valid range is [0, 15] for uint16_t and [0, 31] for uint32_t.
 * @note T must be uint16_t or uint32_t.
 * @param [in] src Source register tensor.
 */
template <int16_t Offset, typename T>
__simd_callee__ inline reg_tensor<bool> move_mask(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_copy
 * @brief Copies active register elements selected by the source register tensor's mask in merging mode.
 *
 * @override_id te_experimental_move__reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @return The destination register tensor after the copy. Its mask remains unchanged.
 * @note T must be one of uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, or float.
 * @param [in,out] dst Destination register tensor. Inactive elements and its mask retain their original values.
 * @param [in] src Source register tensor. Its mask selects the active elements to copy.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> move(reg_tensor<T>& dst, const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_copy
 * @brief Copies active bits selected by the source register tensor's mask in zeroing mode.
 *
 * @override_id te_experimental_move__const_reg_tensor_bool_R
 * @archs 3510
 * @return A register tensor containing the copied bits. Inactive bits are set to zero, and its mask is copied from
 * src.mask.
 * @param [in] src Source register tensor. Its mask selects the active bits to copy.
 */
__simd_callee__ inline reg_tensor<bool> move(const reg_tensor<bool>& src);
} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reg_data_copy_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_COPY_H
#endif
