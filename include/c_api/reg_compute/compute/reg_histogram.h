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
#error "compute/reg_histogram.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_HISTOGRAM_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_HISTOGRAM_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_HISTOGRAM_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_histogram_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_histogram
 * @brief Accumulates a cumulative histogram into uint16 dst by adding, for each dst index n in [0, 127], the count of
 * uint8 src elements not greater than n.
 *
 * @override_id asc_cumulative_histogram_bin0__Rvu16_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register holding 128 uint16_t counters that are read and accumulated in
 * place.
 * @param [in] src Source vector data register of uint8_t elements whose values are counted.
 * @param [in] mask Mask register selecting which src elements are counted; it does not affect the dst write behavior.
 */
__simd_callee__ inline void asc_cumulative_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_histogram
 * @brief Accumulates a cumulative histogram into uint16 dst by adding, for each dst index n in [0, 127], the count of
 * uint8 src elements not greater than 128 + n.
 *
 * @override_id asc_cumulative_histogram_bin1__Rvu16_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register holding 128 uint16_t counters that are read and accumulated in
 * place.
 * @param [in] src Source vector data register of uint8_t elements whose values are counted.
 * @param [in] mask Mask register selecting which src elements are counted; it does not affect the dst write behavior.
 */
__simd_callee__ inline void asc_cumulative_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_histogram
 * @brief Accumulates a frequency histogram into uint16 dst by adding the occurrence count of each uint8 src value in
 * the range [0, 127] to the matching dst element.
 *
 * @override_id asc_frequency_histogram_bin0__Rvu16_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register holding 128 uint16_t counters that are read and accumulated in
 * place.
 * @param [in] src Source vector data register of uint8_t elements whose value frequencies are counted.
 * @param [in] mask Mask register selecting which src elements are counted; it does not affect the dst write behavior.
 */
__simd_callee__ inline void asc_frequency_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_histogram
 * @brief Accumulates a frequency histogram into uint16 dst by adding the occurrence count of each uint8 src value in
 * the range [128, 255] to the matching dst element.
 *
 * @override_id asc_frequency_histogram_bin1__Rvu16_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register holding 128 uint16_t counters that are read and accumulated in
 * place.
 * @param [in] src Source vector data register of uint8_t elements whose value frequencies are counted.
 * @param [in] mask Mask register selecting which src elements are counted; it does not affect the dst write behavior.
 */
__simd_callee__ inline void asc_frequency_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_HISTOGRAM_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_HISTOGRAM_H
#endif
