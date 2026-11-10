/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "compute/reg_reduce.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
#include "impl/c_api/reg_base_impl/reg_reduce_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked int16_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the maximum in element 0, the index raw bit pattern
 * in element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of signed 16-bit elements; read only.
 * @param [in] mask Mask register selecting elements that participate; lanes with bit 0 are treated as the int16_t
 * minimum.
 */
__simd_callee__ inline void asc_reduce_max(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked int32_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the maximum in element 0, the index raw bit pattern in
 * element 1 and zeros in the remaining elements.
 * @param [in] src Source vector data register of signed 32-bit elements; read only.
 * @param [in] mask Mask register selecting the reduced elements; unselected lanes are treated as the int32_t minimum.
 */
__simd_callee__ inline void asc_reduce_max(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked uint16_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the maximum in element 0, the index raw bit pattern in
 * element 1 and zeros in the other elements.
 * @param [in] src Source vector data register of unsigned 16-bit elements; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element act as the uint16_t
 * minimum.
 */
__simd_callee__ inline void asc_reduce_max(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked uint32_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the maximum in element 0, the index raw bit pattern
 * in element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of unsigned 32-bit elements; read only.
 * @param [in] mask Mask register indicating which elements participate; lanes with bit 0 act as the uint32_t minimum.
 */
__simd_callee__ inline void asc_reduce_max(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked half elements to dst element 0 and the index raw bit pattern to element 1,
 * propagating NaN and zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register holding the maximum in element 0, the index raw bit pattern in
 * element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of half elements; read only.
 * @param [in] mask Mask register selecting elements for the reduction; unselected lanes are treated as -inf and NaN
 * inputs are forwarded.
 */
__simd_callee__ inline void asc_reduce_max(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the maximum of the masked float elements to dst element 0 and the index raw bit pattern to element 1,
 * propagating NaN and zeroing the rest.
 *
 * @override_id asc_reduce_max__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the maximum in element 0, the index raw bit pattern in
 * element 1 and zeros for the other elements.
 * @param [in] src Source vector data register of float elements; read only.
 * @param [in] mask Mask register selecting the elements that join the reduction; unselected lanes are treated as -inf
 * and NaN is forwarded.
 */
__simd_callee__ inline void asc_reduce_max(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked int16_t elements and whose element 1 holds the
 * raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_max__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements scanned for the maximum; read only.
 * @param [in] mask Mask register controlling which elements join the reduction; unselected elements count as the dtype
 * minimum and are not reported.
 */
__simd_callee__ inline vector_int16_t asc_reduce_max(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked int32_t elements and whose element 1 holds the
 * raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_max__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements being reduced; read only.
 * @param [in] mask Mask register defining participating elements; lanes with a cleared bit are treated as the dtype
 * minimum.
 */
__simd_callee__ inline vector_int32_t asc_reduce_max(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked uint16_t elements and whose element 1 holds
 * the raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_max__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements being reduced; read only.
 * @param [in] mask Mask register selecting the elements that take part; elements with a 0 bit are treated as the dtype
 * minimum.
 */
__simd_callee__ inline vector_uint16_t asc_reduce_max(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked uint32_t elements and whose element 1 holds
 * the raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_max__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements; read only during the reduction.
 * @param [in] mask Mask register that selects participating elements; a 0 bit makes the element act as the dtype
 * minimum instead of a candidate.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_max(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked half elements and whose element 1 holds the
 * index raw bit pattern, with NaN propagated.
 *
 * @override_id asc_reduce_max__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements scanned for the maximum; read only.
 * @param [in] mask Mask register selecting the reduced elements; unselected lanes are treated as -inf and a NaN in src
 * is propagated to the result.
 */
__simd_callee__ inline vector_half asc_reduce_max(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the maximum of the masked float elements and whose element 1 holds the
 * index raw bit pattern, with NaN propagated.
 *
 * @override_id asc_reduce_max__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements scanned for the maximum; read only.
 * @param [in] mask Mask register selecting the elements that participate; unselected lanes act as -inf and NaN inputs
 * are forwarded to the result.
 */
__simd_callee__ inline vector_float asc_reduce_max(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked int16_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the minimum in element 0, the index raw bit pattern
 * in element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of signed 16-bit elements; read only.
 * @param [in] mask Mask register selecting elements that participate; lanes with bit 0 are treated as the int16_t
 * maximum.
 */
__simd_callee__ inline void asc_reduce_min(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked int32_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the minimum in element 0, the index raw bit pattern in
 * element 1 and zeros in the remaining elements.
 * @param [in] src Source vector data register of signed 32-bit elements; read only.
 * @param [in] mask Mask register selecting the reduced elements; unselected lanes are treated as the int32_t maximum.
 */
__simd_callee__ inline void asc_reduce_min(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked uint16_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the minimum in element 0, the index raw bit pattern in
 * element 1 and zeros in the other elements.
 * @param [in] src Source vector data register of unsigned 16-bit elements; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element act as the uint16_t
 * maximum.
 */
__simd_callee__ inline void asc_reduce_min(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked uint32_t elements to dst element 0 and the raw bit pattern of its smallest
 * index to element 1, zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the minimum in element 0, the index raw bit pattern
 * in element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of unsigned 32-bit elements; read only.
 * @param [in] mask Mask register indicating which elements participate; lanes with bit 0 act as the uint32_t maximum.
 */
__simd_callee__ inline void asc_reduce_min(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked half elements to dst element 0 and the index raw bit pattern to element 1,
 * propagating NaN and zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register holding the minimum in element 0, the index raw bit pattern in
 * element 1 and zeros elsewhere.
 * @param [in] src Source vector data register of half elements; read only.
 * @param [in] mask Mask register selecting elements for the reduction; unselected lanes are treated as +inf and NaN
 * inputs are forwarded.
 */
__simd_callee__ inline void asc_reduce_min(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the minimum of the masked float elements to dst element 0 and the index raw bit pattern to element 1,
 * propagating NaN and zeroing the rest.
 *
 * @override_id asc_reduce_min__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the minimum in element 0, the index raw bit pattern in
 * element 1 and zeros for the other elements.
 * @param [in] src Source vector data register of float elements; read only.
 * @param [in] mask Mask register selecting the elements that join the reduction; unselected lanes are treated as +inf
 * and NaN is forwarded.
 */
__simd_callee__ inline void asc_reduce_min(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked int16_t elements and whose element 1 holds the
 * raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_min__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements scanned for the minimum; read only.
 * @param [in] mask Mask register controlling which elements participate; unselected elements count as the int16_t
 * maximum and are not reported.
 */
__simd_callee__ inline vector_int16_t asc_reduce_min(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked int32_t elements and whose element 1 holds the
 * raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_min__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements being reduced; read only.
 * @param [in] mask Mask register defining participating elements; unselected lanes count as the int32_t maximum.
 */
__simd_callee__ inline vector_int32_t asc_reduce_min(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked uint16_t elements and whose element 1 holds
 * the raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_min__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements being reduced; read only.
 * @param [in] mask Mask register selecting participating elements; lanes with a 0 bit are treated as the uint16_t
 * maximum.
 */
__simd_callee__ inline vector_uint16_t asc_reduce_min(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked uint32_t elements and whose element 1 holds
 * the raw bit pattern of its smallest index.
 *
 * @override_id asc_reduce_min__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements; read only during the reduction.
 * @param [in] mask Mask register that selects participating elements; a 0 bit makes the element act as the uint32_t
 * maximum instead of a candidate.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_min(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked half elements and whose element 1 holds the
 * index raw bit pattern, with NaN propagated.
 *
 * @override_id asc_reduce_min__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements scanned for the minimum; read only.
 * @param [in] mask Mask register selecting the reduced elements; unselected lanes are treated as +inf and NaN inputs
 * are forwarded to the result.
 */
__simd_callee__ inline vector_half asc_reduce_min(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register whose element 0 is the minimum of the masked float elements and whose element 1 holds the
 * index raw bit pattern, with NaN propagated.
 *
 * @override_id asc_reduce_min__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements scanned for the minimum; read only.
 * @param [in] mask Mask register selecting the elements that participate; unselected lanes act as +inf and NaN inputs
 * are forwarded to the result.
 */
__simd_callee__ inline vector_float asc_reduce_min(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked int16_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_min_datablock__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight 32-byte DataBlock minima consecutively in its
 * low elements, with the rest zeroed.
 * @param [in] src Source vector data register of signed 16-bit elements reduced block by block; read only.
 * @param [in] mask Mask register selecting participating elements; unselected elements count as the int16_t maximum in
 * their block.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked int32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_min_datablock__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock minima consecutively in its low
 * elements and zeros elsewhere.
 * @param [in] src Source vector data register of signed 32-bit elements reduced block by block; read only.
 * @param [in] mask Mask register marking participating elements; unselected elements count as the int32_t maximum in
 * their block.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked uint16_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_min_datablock__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the eight DataBlock minima in its low elements and
 * zeros in the remaining elements.
 * @param [in] src Source vector data register of unsigned 16-bit elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements for the reduction; a 0 bit makes the element act as the uint16_t
 * maximum.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked uint32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_min_datablock__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the eight DataBlock minima in its low elements and
 * zeros in the other elements.
 * @param [in] src Source vector data register of unsigned 32-bit elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element act as the uint32_t
 * maximum.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked half elements into the low elements of dst, with +inf for
 * unselected lanes and zeros elsewhere.
 *
 * @override_id asc_reduce_min_datablock__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock minima in its low elements and zeros
 * in the remaining elements.
 * @param [in] src Source vector data register of half elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as +inf and -0 outranks +0.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock minima of the masked float elements into the low elements of dst, with +inf for
 * unselected lanes and zeros elsewhere.
 *
 * @override_id asc_reduce_min_datablock__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock minima in its low elements and zeros
 * in the remaining elements.
 * @param [in] src Source vector data register of float elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are +inf and min(-0, +0) resolves to
 * -0.
 */
__simd_callee__ inline void asc_reduce_min_datablock(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked int16_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements reduced block by block over 32-byte DataBlocks;
 * read only.
 * @param [in] mask Mask register controlling which elements participate; unselected elements count as the int16_t
 * maximum in their block.
 */
__simd_callee__ inline vector_int16_t asc_reduce_min_datablock(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked int32_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register defining participating elements; unselected elements act as the int32_t maximum in
 * their block.
 */
__simd_callee__ inline vector_int32_t asc_reduce_min_datablock(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked uint16_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting the participating elements; a 0 bit makes the element act as the uint16_t
 * maximum in its block.
 */
__simd_callee__ inline vector_uint16_t asc_reduce_min_datablock(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked uint32_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element count as the
 * uint32_t maximum.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_min_datablock(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked half elements, with the eight block results
 * in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as +inf and min(-0, +0)
 * resolves to -0.
 */
__simd_callee__ inline vector_half asc_reduce_min_datablock(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock minima of the masked float elements, with the eight block results
 * in the low elements.
 *
 * @override_id asc_reduce_min_datablock__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as +inf and min(-0, +0)
 * yields -0.
 */
__simd_callee__ inline vector_float asc_reduce_min_datablock(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Adds each pair of adjacent half elements of src and stores the half-width sum result in the low half of dst
 * with the high half cleared, treating mask-cleared elements as 0.
 *
 * @override_id asc_pair_reduce_sum__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register: the 64 pairwise sums occupy its low half and its high half is set to 0.
 * @param [in] src Source half vector data register whose adjacent element pairs (src[2i], src[2i+1]) are summed; read
 * only.
 * @param [in] mask vector_bool mask register gating the source elements: an element whose bit is 0 contributes 0 to its
 * pair sum.
 */
__simd_callee__ inline void asc_pair_reduce_sum(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Adds each pair of adjacent float elements of src and stores the half-width sum result in the low half of dst
 * with the high half cleared, treating mask-cleared elements as 0.
 *
 * @override_id asc_pair_reduce_sum__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register: the 32 pairwise sums occupy its low half and its high half is set to 0.
 * @param [in] src Source float vector data register whose adjacent element pairs (src[2i], src[2i+1]) are summed; read
 * only.
 * @param [in] mask vector_bool mask register gating the source elements: an element whose bit is 0 contributes 0 to its
 * pair sum.
 */
__simd_callee__ inline void asc_pair_reduce_sum(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Adds each pair of adjacent half elements of src and returns a vector_half whose low half holds the sums and
 * whose high half is zero, treating mask-cleared elements as 0.
 *
 * @override_id asc_pair_reduce_sum__vf16_vb
 * @archs 3510
 * @param [in] src Source half vector data register whose adjacent element pairs (src[2i], src[2i+1]) are summed; read
 * only.
 * @param [in] mask vector_bool mask register gating the source elements: an element whose bit is 0 contributes 0 to its
 * pair sum.
 */
__simd_callee__ inline vector_half asc_pair_reduce_sum(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Adds each pair of adjacent float elements of src and returns a vector_float whose low half holds the sums and
 * whose high half is zero, treating mask-cleared elements as 0.
 *
 * @override_id asc_pair_reduce_sum__vf32_vb
 * @archs 3510
 * @param [in] src Source float vector data register whose adjacent element pairs (src[2i], src[2i+1]) are summed; read
 * only.
 * @param [in] mask vector_bool mask register gating the source elements: an element whose bit is 0 contributes 0 to its
 * pair sum.
 */
__simd_callee__ inline vector_float asc_pair_reduce_sum(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked half elements into the low elements of dst, with -inf for
 * unselected lanes and zeros elsewhere.
 *
 * @override_id asc_reduce_max_datablock__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock maxima in its low elements and zeros
 * in the remaining elements.
 * @param [in] src Source vector data register of half elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as -inf and +0 outranks -0.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked float elements into the low elements of dst, with -inf for
 * unselected lanes and zeros elsewhere.
 *
 * @override_id asc_reduce_max_datablock__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock maxima in its low elements and zeros
 * in the remaining elements.
 * @param [in] src Source vector data register of float elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are -inf and max(-0, +0) resolves to
 * +0.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked uint16_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_max_datablock__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the eight DataBlock maxima in its low elements and
 * zeros in the remaining elements.
 * @param [in] src Source vector data register of unsigned 16-bit elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting elements for the reduction; a 0 bit makes the element act as the uint16_t
 * minimum.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked int16_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_max_datablock__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight 32-byte DataBlock maxima consecutively in its
 * low elements, with the rest zeroed.
 * @param [in] src Source vector data register of signed 16-bit elements reduced block by block; read only.
 * @param [in] mask Mask register selecting participating elements; unselected elements count as the int16_t minimum in
 * their block.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked uint32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_max_datablock__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the eight DataBlock maxima in its low elements and
 * zeros in the other elements.
 * @param [in] src Source vector data register of unsigned 32-bit elements reduced over 32-byte blocks; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element act as the uint32_t
 * minimum.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock maxima of the masked int32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_max_datablock__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the eight DataBlock maxima consecutively in its low
 * elements and zeros elsewhere.
 * @param [in] src Source vector data register of signed 32-bit elements reduced block by block; read only.
 * @param [in] mask Mask register marking participating elements; unselected elements count as the int32_t minimum in
 * their block.
 */
__simd_callee__ inline void asc_reduce_max_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked int16_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements reduced block by block over 32-byte DataBlocks;
 * read only.
 * @param [in] mask Mask register controlling which elements participate; unselected elements count as the int16_t
 * minimum in their block.
 */
__simd_callee__ inline vector_int16_t asc_reduce_max_datablock(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked int32_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register defining participating elements; unselected elements act as the int32_t minimum in
 * their block.
 */
__simd_callee__ inline vector_int32_t asc_reduce_max_datablock(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked uint16_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting the participating elements; a 0 bit makes the element act as the uint16_t
 * minimum in its block.
 */
__simd_callee__ inline vector_uint16_t asc_reduce_max_datablock(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked uint32_t elements, with the eight block
 * results in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting participating elements; a cleared bit makes the element count as the
 * uint32_t minimum.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_max_datablock(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked half elements, with the eight block results
 * in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as -inf and max(-0, +0)
 * resolves to +0.
 */
__simd_callee__ inline vector_half asc_reduce_max_datablock(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a register holding the per-DataBlock maxima of the masked float elements, with the eight block results
 * in the low elements.
 *
 * @override_id asc_reduce_max_datablock__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements reduced over 32-byte DataBlocks; read only.
 * @param [in] mask Mask register selecting elements per block; unselected lanes are treated as -inf and max(-0, +0)
 * yields +0.
 */
__simd_callee__ inline vector_float asc_reduce_max_datablock(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked int16_t elements into dst element 0, zeroing the other elements of
 * the int32_t destination.
 *
 * @override_id asc_reduce_sum__Rvi32_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t elements that receives the promoted sum in element 0 and
 * zeros in the remaining elements.
 * @param [in] src Source vector data register of signed 16-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register selecting the summed elements; unselected lanes contribute 0 and an all-zero mask
 * produces 0.
 */
__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked int32_t elements into dst element 0 and zeroes the remaining
 * elements.
 *
 * @override_id asc_reduce_sum__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t elements receiving the sum in element 0 and zeros in the
 * other elements.
 * @param [in] src Source vector data register of signed 32-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register controlling which elements are added; a cleared bit makes the lane contribute 0.
 */
__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked uint16_t elements into dst element 0, zeroing the other elements of
 * the uint32_t destination.
 *
 * @override_id asc_reduce_sum__Rvu32_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t elements receiving the promoted sum in element 0 and
 * zeros elsewhere.
 * @param [in] src Source vector data register of unsigned 16-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register selecting participating elements; unselected lanes act as 0 in the accumulation.
 */
__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked uint32_t elements into dst element 0 and zeroes the remaining
 * elements.
 *
 * @override_id asc_reduce_sum__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t elements that receives the sum in element 0 and zeros
 * elsewhere.
 * @param [in] src Source vector data register of unsigned 32-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register selecting participating elements; unselected lanes contribute 0.
 */
__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked half elements into dst element 0 and zeroes the remaining elements.
 *
 * @override_id asc_reduce_sum__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of half elements that receives the sum in element 0 and zeros in
 * the remaining elements.
 * @param [in] src Source vector data register of half elements whose selected values are summed; read only.
 * @param [in] mask Mask register selecting elements to add; unselected lanes contribute 0 and an empty selection gives
 * +0.
 */
__simd_callee__ inline void asc_reduce_sum(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the tree-summed total of the masked float elements into dst element 0 and zeroes the remaining
 * elements.
 *
 * @override_id asc_reduce_sum__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of float elements receiving the sum in element 0 and zeros in the
 * other elements.
 * @param [in] src Source vector data register of float elements whose selected values are summed; read only.
 * @param [in] mask Mask register selecting elements to add; unselected lanes act as 0 and an empty selection yields +0.
 */
__simd_callee__ inline void asc_reduce_sum(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns an int32_t register holding the tree-summed total of the masked int16_t elements in element 0 with the
 * remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements whose masked values are summed; read only.
 * @param [in] mask Mask register controlling which elements are added; unselected elements contribute 0 to the sum.
 */
__simd_callee__ inline vector_int32_t asc_reduce_sum(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns an int32_t register holding the tree-summed total of the masked int32_t elements in element 0 with the
 * remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register selecting the summed elements; a cleared bit makes that element contribute 0.
 */
__simd_callee__ inline vector_int32_t asc_reduce_sum(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a uint32_t register holding the tree-summed total of the masked uint16_t elements in element 0 with
 * the remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements whose selected values are summed; read only.
 * @param [in] mask Mask register selecting the elements to add; lanes with bit 0 contribute 0 to the total.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_sum(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a uint32_t register holding the tree-summed total of the masked uint32_t elements in element 0 with
 * the remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements whose selected values are added; read only.
 * @param [in] mask Mask register marking participating elements; unselected lanes contribute 0 to the sum.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_sum(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a half register holding the tree-summed total of the masked half elements in element 0 with the
 * remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements whose selected values are summed; read only.
 * @param [in] mask Mask register selecting elements for the sum; unselected elements are treated as 0 in the
 * accumulation.
 */
__simd_callee__ inline vector_half asc_reduce_sum(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a float register holding the tree-summed total of the masked float elements in element 0 with the
 * remaining elements zeroed.
 *
 * @override_id asc_reduce_sum__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements whose selected values are summed; read only.
 * @param [in] mask Mask register selecting elements to add; unselected lanes act as 0 and an empty selection yields +0.
 */
__simd_callee__ inline vector_float asc_reduce_sum(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked half elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of half elements receiving one sum per 32-byte DataBlock at the low
 * end, with the rest zeroed.
 * @param [in] src Source vector data register of half elements summed inside each 32-byte DataBlock; read only.
 * @param [in] mask Mask register selecting the added elements; unselected lanes contribute 0 and all-zero blocks write
 * +0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked float elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of float elements receiving one sum per 32-byte DataBlock at the
 * low end, with zeros elsewhere.
 * @param [in] src Source vector data register of float elements summed within each 32-byte DataBlock; read only.
 * @param [in] mask Mask register selecting elements to add; unselected lanes act as 0 and fully cleared blocks write
 * +0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked uint16_t elements into the low uint32_t elements of dst, zeroing
 * the remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvu32_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t elements receiving the promoted per-DataBlock sums
 * consecutively at the low end, with zeros elsewhere.
 * @param [in] src Source vector data register of unsigned 16-bit elements summed within each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register selecting participating elements; unselected lanes act as 0 and cleared blocks yield
 * 0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked int16_t elements into the low int32_t elements of dst, zeroing the
 * remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvi32_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t elements receiving the promoted per-DataBlock sums
 * consecutively at the low end, with the rest zeroed.
 * @param [in] src Source vector data register of signed 16-bit elements summed within each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register selecting the summed elements; unselected lanes contribute 0 and all-zero blocks
 * produce 0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked uint32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t elements that receives the per-DataBlock sums
 * consecutively at the low end, with the rest zeroed.
 * @param [in] src Source vector data register of unsigned 32-bit elements summed inside each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register selecting participating elements; lanes with bit 0 contribute 0 and all-zero blocks
 * produce 0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Writes the per-DataBlock sums of the masked int32_t elements into the low elements of dst and zeroes the
 * remaining elements.
 *
 * @override_id asc_reduce_sum_datablock__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t elements receiving one sum per 32-byte DataBlock at the
 * low end, with zeros elsewhere.
 * @param [in] src Source vector data register of signed 32-bit elements summed within each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register controlling which elements are added; unselected lanes contribute 0 and cleared blocks
 * give 0.
 */
__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a half register holding the per-DataBlock sums of the masked half elements in its low elements, with
 * the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of half elements summed within each 32-byte DataBlock; read only.
 * @param [in] mask Mask register selecting the summed elements; unselected lanes act as 0 and all-zero blocks write +0.
 */
__simd_callee__ inline vector_half asc_reduce_sum_datablock(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a float register holding the per-DataBlock sums of the masked float elements in its low elements, with
 * the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of float elements summed within each 32-byte DataBlock; read only.
 * @param [in] mask Mask register selecting elements to add; unselected lanes act as 0 and fully cleared blocks write
 * +0.
 */
__simd_callee__ inline vector_float asc_reduce_sum_datablock(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a uint32_t register holding the per-DataBlock sums of the masked uint16_t elements in its low
 * elements, with the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit elements summed within each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register selecting the added elements; unselected lanes contribute 0 and fully cleared blocks
 * produce 0.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_sum_datablock(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns an int32_t register holding the per-DataBlock sums of the masked int16_t elements in its low elements,
 * with the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit elements summed inside each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register controlling which elements are added; unselected elements contribute 0 and an all-zero
 * block yields 0.
 */
__simd_callee__ inline vector_int32_t asc_reduce_sum_datablock(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns a uint32_t register holding the per-DataBlock sums of the masked uint32_t elements in its low
 * elements, with the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit elements summed within each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register selecting the added elements; lanes with bit 0 contribute 0 and cleared blocks give 0.
 */
__simd_callee__ inline vector_uint32_t asc_reduce_sum_datablock(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_reduce
 * @brief Returns an int32_t register holding the per-DataBlock sums of the masked int32_t elements in its low elements,
 * with the rest zeroed.
 *
 * @override_id asc_reduce_sum_datablock__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit elements summed inside each 32-byte DataBlock; read
 * only.
 * @param [in] mask Mask register marking participating elements; unselected lanes contribute 0 and empty blocks yield
 * 0.
 */
__simd_callee__ inline vector_int32_t asc_reduce_sum_datablock(vector_int32_t src, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
//=================asc_datablock_reduce_max_stride==================
__simd_callee__ inline void asc_datablock_reduce_max_stride(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_max_stride(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_max_stride(
    vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_max_stride(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_max_stride(
    vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_max_stride(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_datablock_reduce_max_stride(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_datablock_reduce_max_stride(vector_float src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_datablock_reduce_max_stride(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_datablock_reduce_max_stride(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_datablock_reduce_max_stride(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_datablock_reduce_max_stride(vector_int32_t src, vector_bool mask);

//=================asc_datablock_reduce_min_stride==================
__simd_callee__ inline void asc_datablock_reduce_min_stride(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_min_stride(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_min_stride(
    vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_min_stride(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_min_stride(
    vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_min_stride(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_datablock_reduce_min_stride(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_datablock_reduce_min_stride(vector_float src, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_datablock_reduce_min_stride(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_datablock_reduce_min_stride(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_datablock_reduce_min_stride(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_datablock_reduce_min_stride(vector_int32_t src, vector_bool mask);

//=================asc_datablock_reduce_sum_stride==================
__simd_callee__ inline void asc_datablock_reduce_sum_stride(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_sum_stride(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_sum_stride(
    vector_uint32_t& dst, vector_uint16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_sum_stride(vector_int32_t& dst, vector_int16_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_sum_stride(
    vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

__simd_callee__ inline void asc_datablock_reduce_sum_stride(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

__simd_callee__ inline vector_half asc_datablock_reduce_sum_stride(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_datablock_reduce_sum_stride(vector_float src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_datablock_reduce_sum_stride(vector_uint16_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_datablock_reduce_sum_stride(vector_int16_t src, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_datablock_reduce_sum_stride(vector_uint32_t src, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_datablock_reduce_sum_stride(vector_int32_t src, vector_bool mask);

//=================asc_pair_reduce_sum_stride==================
__simd_callee__ inline void asc_pair_reduce_sum_stride(vector_half& dst, vector_half src, vector_bool mask);

__simd_callee__ inline void asc_pair_reduce_sum_stride(vector_float& dst, vector_float src, vector_bool mask);

__simd_callee__ inline vector_half asc_pair_reduce_sum_stride(vector_half src, vector_bool mask);

__simd_callee__ inline vector_float asc_pair_reduce_sum_stride(vector_float src, vector_bool mask);

//=================asc_reduce_max_last_elem==================
__simd_callee__ inline void asc_reduce_max_last_elem(vector_half& dst, vector_half src);

__simd_callee__ inline void asc_reduce_max_last_elem(vector_float& dst, vector_float src);

__simd_callee__ inline void asc_reduce_max_last_elem(vector_uint16_t& dst, vector_uint16_t src);

__simd_callee__ inline void asc_reduce_max_last_elem(vector_int16_t& dst, vector_int16_t src);

__simd_callee__ inline void asc_reduce_max_last_elem(vector_uint32_t& dst, vector_uint32_t src);

__simd_callee__ inline void asc_reduce_max_last_elem(vector_int32_t& dst, vector_int32_t src);

__simd_callee__ inline vector_half asc_reduce_max_last_elem(vector_half src);

__simd_callee__ inline vector_float asc_reduce_max_last_elem(vector_float src);

__simd_callee__ inline vector_uint16_t asc_reduce_max_last_elem(vector_uint16_t src);

__simd_callee__ inline vector_int16_t asc_reduce_max_last_elem(vector_int16_t src);

__simd_callee__ inline vector_uint32_t asc_reduce_max_last_elem(vector_uint32_t src);

__simd_callee__ inline vector_int32_t asc_reduce_max_last_elem(vector_int32_t src);

//=================asc_reduce_min_last_elem==================
__simd_callee__ inline void asc_reduce_min_last_elem(vector_half& dst, vector_half src);

__simd_callee__ inline void asc_reduce_min_last_elem(vector_float& dst, vector_float src);

__simd_callee__ inline void asc_reduce_min_last_elem(vector_uint16_t& dst, vector_uint16_t src);

__simd_callee__ inline void asc_reduce_min_last_elem(vector_int16_t& dst, vector_int16_t src);

__simd_callee__ inline void asc_reduce_min_last_elem(vector_uint32_t& dst, vector_uint32_t src);

__simd_callee__ inline void asc_reduce_min_last_elem(vector_int32_t& dst, vector_int32_t src);

__simd_callee__ inline vector_half asc_reduce_min_last_elem(vector_half src);

__simd_callee__ inline vector_float asc_reduce_min_last_elem(vector_float src);

__simd_callee__ inline vector_uint16_t asc_reduce_min_last_elem(vector_uint16_t src);

__simd_callee__ inline vector_int16_t asc_reduce_min_last_elem(vector_int16_t src);

__simd_callee__ inline vector_uint32_t asc_reduce_min_last_elem(vector_uint32_t src);

__simd_callee__ inline vector_int32_t asc_reduce_min_last_elem(vector_int32_t src);

//=================asc_reduce_sum_last_elem==================
__simd_callee__ inline void asc_reduce_sum_last_elem(vector_int32_t& dst, vector_int32_t src);

__simd_callee__ inline void asc_reduce_sum_last_elem(vector_uint32_t& dst, vector_uint32_t src);

__simd_callee__ inline void asc_reduce_sum_last_elem(vector_half& dst, vector_half src);

__simd_callee__ inline void asc_reduce_sum_last_elem(vector_float& dst, vector_float src);

__simd_callee__ inline vector_int32_t asc_reduce_sum_last_elem(vector_int32_t src);

__simd_callee__ inline vector_uint32_t asc_reduce_sum_last_elem(vector_uint32_t src);

__simd_callee__ inline vector_half asc_reduce_sum_last_elem(vector_half src);

__simd_callee__ inline vector_float asc_reduce_sum_last_elem(vector_float src);
#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_REDUCE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_REDUCE_H
#endif
