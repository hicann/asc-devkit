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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_H
#endif

/*!
 * \file reg_data_load.h
 * \brief Register data load and broadcast interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @brief Loads data from a UB tensor at the specified coordinate into one or two register tensors.
 * @param tensor Source tensor located in UB.
 * @param coord Coordinate of the first source element.
 * @return A register pair for deintlv mode; otherwise, a register tensor.
 */
template <load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor, typename Coord>
__simd_callee__ inline decltype(auto) load(const Tensor& tensor, const Coord& coord);

/**
 * @brief Loads data from the beginning of a UB tensor into one or two register tensors.
 * @param tensor Source tensor located in UB.
 * @return A register pair for deintlv mode; otherwise, a register tensor.
 */
template <load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor>
__simd_callee__ inline decltype(auto) load(const Tensor& tensor);

/**
 * @brief Loads and broadcasts data from a UB tensor at the specified coordinate.
 * @param tensor Source tensor located in UB.
 * @param coord Coordinate of the first source element.
 * @return A register tensor populated according to the selected broadcast mode.
 */
template <broadcast_mode brc_mode = broadcast_mode::elem, typename Tensor, typename Coord>
__simd_callee__ inline decltype(auto) load_broadcast(const Tensor& tensor, const Coord& coord);

/**
 * @brief Loads and broadcasts data from the beginning of a UB tensor.
 * @param tensor Source tensor located in UB.
 * @return A register tensor populated according to the selected broadcast mode.
 */
template <broadcast_mode brc_mode = broadcast_mode::elem, typename Tensor>
__simd_callee__ inline decltype(auto) load_broadcast(const Tensor& tensor);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reg_data_load_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_H
#endif
