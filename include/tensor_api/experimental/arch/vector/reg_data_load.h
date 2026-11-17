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
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads data from a UB tensor at the specified coordinate into one or two register tensors.
 *
 * @override_id te_experimental_load__const_Tensor_R_const_Coord_R__template_sideband_mode_Tensor_Coord
 * @archs 3510
 * @return A register pair for deintlv mode; otherwise, a register tensor.
 * @param [in] tensor Source tensor located in UB.
 * @param [in] coord Coordinate of the first source element.
 */
template <load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor, typename Coord>
__simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads data from the beginning of a UB tensor into one or two register tensors.
 *
 * @override_id te_experimental_load__const_Tensor_R__template_sideband_mode_Tensor
 * @archs 3510
 * @return A register pair for deintlv mode; otherwise, a register tensor.
 * @param [in] tensor Source tensor located in UB.
 */
template <load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor>
__simd_callee__ inline auto load(const Tensor& tensor);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads packed mask bits from a UB tensor at the specified coordinate.
 *
 * @override_id te_experimental_load__const_Tensor_R_const_Coord_R__template_RegDataType_sideband_mode_Tensor_Coord
 * @archs 3510
 * @return A Boolean register tensor containing the loaded mask bits in its reg member.
 * @param [in] tensor Source uint32_t tensor located in UB.
 * @param [in] coord Coordinate of the first source word.
 * @note RegDataType must be bool. Supported modes are direct, upsample, and downsample.
 * Direct mode loads packed bits unchanged; upsample repeats each bit twice; downsample retains even-indexed bits.
 * The effective source address must be 16-byte aligned for upsample and 32-byte aligned otherwise.
 * For a 256-byte vector data register, these modes read 32, 16, and 64 bytes, respectively.
 * The full load must remain within the source buffer.
 */
template <
    typename RegDataType, load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor,
    typename Coord>
__simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads packed mask bits from the beginning of a UB tensor.
 *
 * @override_id te_experimental_load__const_Tensor_R__template_RegDataType_sideband_mode_Tensor
 * @archs 3510
 * @return A Boolean register tensor containing the loaded mask bits in its reg member.
 * @param [in] tensor Source uint32_t tensor located in UB.
 * @note RegDataType must be bool. Supported modes are direct, upsample, and downsample.
 * Direct mode loads packed bits unchanged; upsample repeats each bit twice; downsample retains even-indexed bits.
 * The source address must be 16-byte aligned for upsample and 32-byte aligned otherwise.
 * For a 256-byte vector data register, these modes read 32, 16, and 64 bytes, respectively.
 * The full load must remain within the source buffer.
 */
template <typename RegDataType, load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Tensor>
__simd_callee__ inline auto load(const Tensor& tensor);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads and broadcasts data from a UB tensor at the specified coordinate.
 *
 * @override_id te_experimental_load_broadcast__const_Tensor_R_const_Coord_R__template_brc_mode_Tensor_Coord
 * @archs 3510
 * @return A register tensor populated according to the selected broadcast mode.
 * @param [in] tensor Source tensor located in UB.
 * @param [in] coord Coordinate of the first source element.
 * @note The source tensor element type must be supported by brc_mode. An int4b_t source
 * returns reg_tensor<int4x2_t>; other supported source types return a register tensor
 * with the corresponding element type. Only the returned reg member is initialized;
 * call with_mask before using the result in an operation that reads its mask.
 */
template <broadcast_mode brc_mode = broadcast_mode::elem, typename Tensor, typename Coord>
__simd_callee__ inline auto load_broadcast(const Tensor& tensor, const Coord& coord);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Loads and broadcasts data from the beginning of a UB tensor.
 *
 * @override_id te_experimental_load_broadcast__const_Tensor_R__template_brc_mode_Tensor
 * @archs 3510
 * @return A register tensor populated according to the selected broadcast mode.
 * @param [in] tensor Source tensor located in UB.
 * @note The source tensor element type must be supported by brc_mode. An int4b_t source
 * returns reg_tensor<int4x2_t>; other supported source types return a register tensor
 * with the corresponding element type. Only the returned reg member is initialized;
 * call with_mask before using the result in an operation that reads its mask.
 */
template <broadcast_mode brc_mode = broadcast_mode::elem, typename Tensor>
__simd_callee__ inline auto load_broadcast(const Tensor& tensor);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Gathers elements or data blocks from a UB tensor at the specified coordinate.
 *
 * @override_id
 * te_experimental_load_gather__const_Tensor_R_const_Coord_R_const_reg_tensor_Index_const_reg_tensor_bool_R__template_mode_Tensor_Coord_Index
 * @archs 3510
 * @return A register tensor containing the gathered data.
 * @param [in] tensor Source tensor located in UB.
 * @param [in] coord Coordinate used as the base source position.
 * @param [in] index Register tensor containing the source offsets relative to the base position.
 * @param [in] gather_mask Register mask that selects the values to gather.
 * @note The active-lane mask is taken from gather_mask.reg. In element mode, a value of 1 selects the corresponding
 * element; a value of 0 excludes it. Unselected elements are set to zero in element mode. In datablock mode, mask bits
 * 0, 4, 8, 12, 16, 20, 24, and 28 select the eight data blocks. The result preserves gather_mask.reg as its active-lane
 * mask.
 */
template <gather_mode mode = gather_mode::element, typename Tensor, typename Coord, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor, const Coord& coord, const reg_tensor<Index> index, const reg_tensor<bool>& gather_mask);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.reg_data_load
 * @brief Gathers elements or data blocks from the beginning of a UB tensor.
 *
 * @override_id
 * te_experimental_load_gather__const_Tensor_R_const_reg_tensor_Index_const_reg_tensor_bool_R__template_mode_Tensor_Index
 * @archs 3510
 * @return A register tensor containing the gathered data.
 * @param [in] tensor Source tensor located in UB.
 * @param [in] index Register tensor containing the source offsets.
 * @param [in] gather_mask Register mask that selects the values to gather.
 * @note The active-lane mask is taken from gather_mask.reg. In element mode, a value of 1 selects the corresponding
 * element; a value of 0 excludes it. Unselected elements are set to zero in element mode. In datablock mode, mask bits
 * 0, 4, 8, 12, 16, 20, 24, and 28 select the eight data blocks. The result preserves gather_mask.reg as its active-lane
 * mask.
 */
template <gather_mode mode = gather_mode::element, typename Tensor, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor, const reg_tensor<Index> index, const reg_tensor<bool>& gather_mask);

/**
 * @brief Maintains the hardware state for an unaligned UB-to-register load.
 * @note load() does not advance the prepared state. Call prepare() for each new
 * source address, using a tensor whose beginning matches the load address.
 */
struct unalign_load {
    __simd_callee__ unalign_load() = default;

    /**
     * @brief Prepares an unaligned load stream at the beginning of a UB tensor.
     * @param src_base Tensor whose beginning is the first address of the stream.
     */
    template <typename Tensor>
    __simd_callee__ inline void prepare(const Tensor& src_base);

    /**
     * @brief Loads one full vector register from the prepared source address.
     * @param src_tile Tensor whose beginning is the source address for this load.
     * @return One register tensor containing the loaded data. An int4b_t source
     * returns reg_tensor<int4x2_t>.
     */
    template <typename Tensor>
    __simd_callee__ inline auto load(const Tensor& src_tile);

    /**
     * @brief Loads one full vector register from a UB tensor at the specified coordinate.
     * @param src_tile Source tensor in the prepared unaligned stream.
     * @param coord Coordinate of the first source element.
     * @return One register tensor containing the loaded data. An int4b_t source
     * returns reg_tensor<int4x2_t>.
     */
    template <typename Tensor, typename Coord>
    __simd_callee__ inline auto load(const Tensor& src_tile, const Coord& coord);

private:
    vector_load_unalign ureg;
};

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
