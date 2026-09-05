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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_H
#endif

/*!
 * \file reg_data_store.h
 * \brief Register data store interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_H

#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @brief Stores register data to a UB tensor at the specified coordinate.
 * @param tensor Destination tensor located in UB.
 * @param coord Coordinate of the first destination element.
 * @param src Source register tensor.
 * @note The tensor and register element types must match.
 */
template <
    store_sideband_mode sideband_mode = store_sideband_mode::direct, typename Tensor, typename Coord,
    typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src);

/**
 * @brief Stores register data at the beginning of a UB tensor.
 * @param tensor Destination tensor located in UB.
 * @param src Source register tensor.
 * @note The tensor and register element types must match.
 */
template <store_sideband_mode sideband_mode = store_sideband_mode::direct, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const reg_tensor<RegDataType>& src);

/**
 * @brief Interleaves two register tensors and stores the result at the specified coordinate of a UB tensor.
 * @param tensor Destination tensor located in UB.
 * @param coord Coordinate of the first destination element.
 * @param src0 First source register tensor.
 * @param src1 Second source register tensor.
 * @note This overload supports only store_sideband_mode::intlv. The tensor and register element types must match.
 */
template <
    store_sideband_mode sideband_mode = store_sideband_mode::intlv, typename Tensor, typename Coord,
    typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1);

/**
 * @brief Interleaves two register tensors and stores the result at the beginning of a UB tensor.
 * @param tensor Destination tensor located in UB.
 * @param src0 First source register tensor.
 * @param src1 Second source register tensor.
 * @note This overload supports only store_sideband_mode::intlv. The tensor and register element types must match.
 */
template <store_sideband_mode sideband_mode = store_sideband_mode::intlv, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reg_data_store_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_H
#endif
