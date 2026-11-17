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
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Stores register data to a UB tensor at the specified coordinate.
 *
 * @override_id
 * te_experimental_store__Tensor_R_const_Coord_R_const_reg_tensor_RegDataType_R__template_sideband_mode_Tensor_Coord_RegDataType
 * @archs 3510
 * @note For vector data registers, the tensor and register element types must match, except that an int4b_t
 * tensor requires an int4x2_t register. Mask registers (reg_tensor<bool>) support only store_sideband_mode::direct
 * and store_sideband_mode::pack, and only to uint8_t, uint16_t, or uint32_t tensors. Both modes ignore src.mask.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] coord Coordinate of the first destination element.
 * @param [in] src Source register tensor.
 */
template <
    store_sideband_mode sideband_mode = store_sideband_mode::direct, typename Tensor, typename Coord,
    typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Stores register data at the beginning of a UB tensor.
 *
 * @override_id
 * te_experimental_store__Tensor_R_const_reg_tensor_RegDataType_R__template_sideband_mode_Tensor_RegDataType
 * @archs 3510
 * @note For vector data registers, the tensor and register element types must match, except that an int4b_t
 * tensor requires an int4x2_t register. Mask registers (reg_tensor<bool>) support only store_sideband_mode::direct
 * and store_sideband_mode::pack, and only to uint8_t, uint16_t, or uint32_t tensors. Both modes ignore src.mask.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] src Source register tensor.
 */
template <store_sideband_mode sideband_mode = store_sideband_mode::direct, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const reg_tensor<RegDataType>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Interleaves two register tensors and stores the result at the specified coordinate of a UB tensor.
 *
 * @override_id
 * te_experimental_store__Tensor_R_const_Coord_R_const_reg_tensor_RegDataType_R_const_reg_tensor_RegDataType_R__template_sideband_mode_Tensor_Coord_RegDataType
 * @archs 3510
 * @note This overload supports only store_sideband_mode::intlv. The tensor and register element types must match.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] coord Coordinate of the first destination element.
 * @param [in] src0 First source register tensor.
 * @param [in] src1 Second source register tensor.
 */
template <
    store_sideband_mode sideband_mode = store_sideband_mode::intlv, typename Tensor, typename Coord,
    typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Interleaves two register tensors and stores the result at the beginning of a UB tensor.
 *
 * @override_id
 * te_experimental_store__Tensor_R_const_reg_tensor_RegDataType_R_const_reg_tensor_RegDataType_R__template_sideband_mode_Tensor_RegDataType
 * @archs 3510
 * @note This overload supports only store_sideband_mode::intlv. The tensor and register element types must match.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] src0 First source register tensor.
 * @param [in] src1 Second source register tensor.
 */
template <store_sideband_mode sideband_mode = store_sideband_mode::intlv, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Scatters register data from active source lanes to a UB tensor.
 *
 * @override_id
 * te_experimental_store_scatter__Tensor_R_const_reg_tensor_T_R_const_reg_tensor_Index_R__template_Tensor_T_Index
 * @archs 3510
 * @note The tensor and source register element types must match.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] src Source register tensor. Its active-lane mask selects the elements to store.
 * @param [in] index Register tensor containing the destination offsets.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding source element for storage;
 * a value of 0 excludes it. Unselected elements are not written; the corresponding destination memory is unchanged.
 */
template <typename Tensor, typename T, typename Index>
__simd_callee__ inline void store_scatter(Tensor& tensor, const reg_tensor<T>& src, const reg_tensor<Index>& index);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_store
 * @brief Scatters register data from active source lanes to a UB tensor at the specified coordinate.
 *
 * @override_id
 * te_experimental_store_scatter__Tensor_R_const_Coord_R_const_reg_tensor_T_R_const_reg_tensor_Index_R__template_Tensor_Coord_T_Index
 * @archs 3510
 * @note The tensor and source register element types must match.
 * @param [out] tensor Destination tensor located in UB.
 * @param [in] coord Coordinate used as the base destination position.
 * @param [in] src Source register tensor. Its active-lane mask selects the elements to store.
 * @param [in] index Register tensor containing the destination offsets relative to the base position.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding source element for storage;
 * a value of 0 excludes it. Unselected elements are not written; the corresponding destination memory is unchanged.
 */
template <typename Tensor, typename Coord, typename T, typename Index>
__simd_callee__ inline void store_scatter(
    Tensor& tensor, const Coord& coord, const reg_tensor<T>& src, const reg_tensor<Index>& index);

/**
 * @brief Maintains the hardware state for a sequence of unaligned register-to-UB stores.
 * @note Each store writes one full vector register and does not use reg_tensor::mask.
 * Destination addresses must form one contiguous stream. All tensors passed to one
 * session must have the same element type. Call flush() exactly once with the first
 * destination address after the stores are complete.
 */
struct unalign_store {
    __simd_callee__ unalign_store() = default;

    /**
     * @brief Stores one full vector register at the beginning of a UB tensor.
     * @param dst_tile Tensor whose beginning is the destination address for this store.
     * @param src Source register tensor. Its mask is not used. An int4b_t destination
     * requires reg_tensor<int4x2_t>.
     * @param count Number of elements to store.
     */
    template <typename Tensor, typename RegDataType>
    __simd_callee__ inline void store(Tensor dst_tile, const reg_tensor<RegDataType>& src, uint32_t count);

    /**
     * @brief Stores register data to a UB tensor at the specified coordinate.
     * @param dst_tile Destination tensor in the unaligned store stream.
     * @param coord Coordinate of the first destination element.
     * @param src Source register tensor. Its mask is not used. An int4b_t destination
     * requires reg_tensor<int4x2_t>.
     * @param count Number of elements to store.
     */
    template <typename Tensor, typename Coord, typename RegDataType>
    __simd_callee__ inline void store(
        Tensor dst_tile, const Coord& coord, const reg_tensor<RegDataType>& src, uint32_t count);

    /**
     * @brief Commits the pending unaligned stores.
     * @param dst_base Tensor whose beginning is the destination address of the first store.
     * @param coord Coordinate passed to the unaligned-store post operation.
     */
    template <typename Tensor>
    __simd_callee__ inline void flush(Tensor dst_base, int32_t coord);

private:
    vector_store_unalign ureg;
};

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
