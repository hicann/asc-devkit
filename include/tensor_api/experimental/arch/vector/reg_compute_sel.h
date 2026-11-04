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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_H
#endif

#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"

namespace asc {
namespace te {
namespace experimental {
/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_compute_sel
 * @brief Packs a register tensor into the selected low or high half-width representation.
 *
 * @override_id te_experimental_pack__reg_tensor_T__template_part_T
 * @archs 3510
 * @tparam part Destination half selected for the pack operation.
 * @tparam T Element type of the source register tensor.
 * @return A register tensor containing the packed data with an all-active result mask.
 * @param [in] src Source register tensor.
 */
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) pack(reg_tensor<T> src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_compute_sel
 * @brief Unpacks a register tensor from the selected low or high half-width representation.
 *
 * @override_id te_experimental_unpack__reg_tensor_T__template_part_T
 * @archs 3510
 * @tparam part Source half selected for the unpack operation.
 * @tparam T Element type of the source register tensor.
 * @return A register tensor containing the unpacked data with an all-active result mask.
 * @param [in] src Source register tensor.
 */
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) unpack(reg_tensor<T> src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_compute_sel
 * @brief Compresses the lanes selected by the source tensor mask into contiguous low lanes.
 *
 * @override_id te_experimental_compress__reg_tensor_T__template_T
 * @archs 3510
 * @tparam T Element type of the source register tensor.
 * @return A register tensor containing selected elements packed into low lanes.
 * @param [in] src Source register tensor whose mask selects the lanes to compress.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> compress(reg_tensor<T> src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_compute_sel
 * @brief Generates exclusive prefix-count indexes from a mask.
 *
 * @override_id te_experimental_expand__reg_tensor_bool__template_T
 * @archs 3510
 * @tparam T Element type of the generated index register tensor.
 * @return A register tensor containing prefix-count indexes with an all-active result mask.
 * @param [in] mask Lane-selection mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> expand(reg_tensor<bool> mask);
} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reg_compute_sel_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_COMPUTE_SEL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SEL_H
#endif
