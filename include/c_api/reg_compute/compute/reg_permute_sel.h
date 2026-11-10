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
#error "compute/reg_permute_sel.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_PERMUTE_SEL_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_PERMUTE_SEL_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_PERMUTE_SEL_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
#include "impl/c_api/reg_base_impl/reg_permute_sel_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the mask-register bits selected for each element position from src0 when the corresponding
 * mask bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvb_vb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register that receives the selected values.
 * @param [in] src0 Operand 0 (mask register) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (mask register) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the uint8_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint8_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type uint8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the int8_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int8_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type int8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the fp8_e4m3fn_t elements selected for each element position from src0 when the corresponding
 * mask bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvf8e4m3_vf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e4m3fn_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type fp8_e4m3fn_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type fp8_e4m3fn_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the hifloat8_t elements selected for each element position from src0 when the corresponding
 * mask bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvhf8_vhf8_vhf8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type hifloat8_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type hifloat8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type hifloat8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the fp8_e5m2_t elements selected for each element position from src0 when the corresponding
 * mask bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvf8e5m2_vf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e5m2_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type fp8_e5m2_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type fp8_e5m2_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the uint16_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type uint16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the int16_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type int16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the half elements selected for each element position from src0 when the corresponding mask bit
 * is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type half) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type half) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the bfloat16_t elements selected for each element position from src0 when the corresponding
 * mask bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type bfloat16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type bfloat16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the uint32_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type uint32_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint32_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the int32_t elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type int32_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int32_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the float elements selected for each element position from src0 when the corresponding mask
 * bit is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the selected values.
 * @param [in] src0 Operand 0 (vector data register of type float) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type float) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline void asc_select(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the mask-register bits selected for each element position from src0 when the corresponding mask bit is
 * 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vb_vb_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (mask register) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (mask register) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_bool asc_select(vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the uint8_t elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type uint8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_uint8_t asc_select(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the int8_t elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type int8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_int8_t asc_select(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the fp8_e4m3fn_t elements selected for each element position from src0 when the corresponding mask bit
 * is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type fp8_e4m3fn_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type fp8_e4m3fn_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_select(
    vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the hifloat8_t elements selected for each element position from src0 when the corresponding mask bit
 * is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vhf8_vhf8_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type hifloat8_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type hifloat8_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_hifloat8_t asc_select(vector_hifloat8_t src0, vector_hifloat8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the fp8_e5m2_t elements selected for each element position from src0 when the corresponding mask bit
 * is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type fp8_e5m2_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type fp8_e5m2_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_select(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the uint16_t elements selected for each element position from src0 when the corresponding mask bit is
 * 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type uint16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_uint16_t asc_select(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the int16_t elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type int16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_int16_t asc_select(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the half elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type half) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type half) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_half asc_select(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the bfloat16_t elements selected for each element position from src0 when the corresponding mask bit
 * is 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type bfloat16_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type bfloat16_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_bfloat16_t asc_select(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the uint32_t elements selected for each element position from src0 when the corresponding mask bit is
 * 1 and from src1 when it is 0.
 *
 * @override_id asc_select__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type uint32_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type uint32_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_uint32_t asc_select(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the int32_t elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type int32_t) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type int32_t) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_int32_t asc_select(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns the float elements selected for each element position from src0 when the corresponding mask bit is 1
 * and from src1 when it is 0.
 *
 * @override_id asc_select__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Operand 0 (vector data register of type float) chosen for every element whose mask bit is 1.
 * @param [in] src1 Operand 1 (vector data register of type float) chosen for every element whose mask bit is 0.
 * @param [in] mask Selection mask register: bit i equal to 1 picks src0 at position i and bit i equal to 0 picks src1
 * at position i.
 */
__simd_callee__ inline vector_float asc_select(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the uint8_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint8_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type uint8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type uint8_t in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vu8_vb
 * @archs 3510
 * @param [in] src Source vector data register of type uint8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_uint8_t asc_squeeze(vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the int8_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int8_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type int8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type int8_t in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vi8_vb
 * @archs 3510
 * @param [in] src Source vector data register of type int8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_int8_t asc_squeeze(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the uint16_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type uint16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type uint16_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register of type uint16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_uint16_t asc_squeeze(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the int16_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type int16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type int16_t in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register of type int16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_int16_t asc_squeeze(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the uint32_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type uint32_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type uint32_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register of type uint32_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_uint32_t asc_squeeze(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the int32_t elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the compacted elements; present only
 * in the void form.
 * @param [in] src Source vector data register of type int32_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type int32_t in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register of type int32_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_int32_t asc_squeeze(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the half elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the compacted elements; present only in
 * the void form.
 * @param [in] src Source vector data register of type half whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type half in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of type half whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_half asc_squeeze(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the float elements of src whose mask bit is 1 into the low part of dst in their original order and
 * zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the compacted elements; present only in
 * the void form.
 * @param [in] src Source vector data register of type float whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type float in which the src elements whose mask bit is 1 are compacted into the
 * low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register of type float whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_float asc_squeeze(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the bfloat16_t elements of src whose mask bit is 1 into the low part of dst in their original order
 * and zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the compacted elements; present
 * only in the void form.
 * @param [in] src Source vector data register of type bfloat16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type bfloat16_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vbf16_vb
 * @archs 3510
 * @param [in] src Source vector data register of type bfloat16_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_bfloat16_t asc_squeeze(vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the hifloat8_t elements of src whose mask bit is 1 into the low part of dst in their original order
 * and zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvhf8_vhf8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type hifloat8_t that receives the compacted elements; present
 * only in the void form.
 * @param [in] src Source vector data register of type hifloat8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type hifloat8_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vhf8_vb
 * @archs 3510
 * @param [in] src Source vector data register of type hifloat8_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_hifloat8_t asc_squeeze(vector_hifloat8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the fp8_e4m3fn_t elements of src whose mask bit is 1 into the low part of dst in their original order
 * and zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e4m3fn_t that receives the compacted elements; present
 * only in the void form.
 * @param [in] src Source vector data register of type fp8_e4m3fn_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type fp8_e4m3fn_t in which the src elements whose mask bit is 1 are compacted
 * into the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vf8e4m3_vb
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e4m3fn_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_squeeze(vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the fp8_e5m2_t elements of src whose mask bit is 1 into the low part of dst in their original order
 * and zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e5m2_t that receives the compacted elements; present
 * only in the void form.
 * @param [in] src Source vector data register of type fp8_e5m2_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type fp8_e5m2_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vf8e5m2_vb
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e5m2_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_squeeze(vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Compacts the fp8_e8m0_t elements of src whose mask bit is 1 into the low part of dst in their original order
 * and zero-fills the remaining high positions.
 *
 * @override_id asc_squeeze__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e8m0_t that receives the compacted elements; present
 * only in the void form.
 * @param [in] src Source vector data register of type fp8_e8m0_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline void asc_squeeze(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a vector register of type fp8_e8m0_t in which the src elements whose mask bit is 1 are compacted into
 * the low part in their original order while the remaining high positions are zero.
 *
 * @override_id asc_squeeze__vf8e8m0_vb
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e8m0_t whose mask-selected elements are compacted.
 * @param [in] mask Selection mask register: a bit set to 1 keeps and compacts the corresponding element, a bit set to 0
 * drops it.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_squeeze(vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Clears the AR special register so that a group of continuous unaligned store-outs starts from a zero byte
 * offset.
 *
 * @override_id asc_squeeze_and_storeunalign_init__z
 * @archs 3510
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_init();

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the int8_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUi8_Rvi8_Rvstoreunalign_vi8_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination int8_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(int8_t).
 * @param [out] dst Destination vector_int8_t register passed by reference; receives the selected int8_t elements packed
 * from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_int8_t register whose int8_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int8_t* dst_addr, vector_int8_t& dst, vector_store_unalign& src0, vector_int8_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the uint8_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUu8_Rvu8_Rvstoreunalign_vu8_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination uint8_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(uint8_t).
 * @param [out] dst Destination vector_uint8_t register passed by reference; receives the selected uint8_t elements
 * packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_uint8_t register whose uint8_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint8_t* dst_addr, vector_uint8_t& dst, vector_store_unalign& src0, vector_uint8_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the int16_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUi16_Rvi16_Rvstoreunalign_vi16_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination int16_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(int16_t).
 * @param [out] dst Destination vector_int16_t register passed by reference; receives the selected int16_t elements
 * packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_int16_t register whose int16_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int16_t* dst_addr, vector_int16_t& dst, vector_store_unalign& src0, vector_int16_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the uint16_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUu16_Rvu16_Rvstoreunalign_vu16_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination uint16_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(uint16_t).
 * @param [out] dst Destination vector_uint16_t register passed by reference; receives the selected uint16_t elements
 * packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_uint16_t register whose uint16_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint16_t* dst_addr, vector_uint16_t& dst, vector_store_unalign& src0, vector_uint16_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the int32_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUi32_Rvi32_Rvstoreunalign_vi32_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination int32_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(int32_t).
 * @param [out] dst Destination vector_int32_t register passed by reference; receives the selected int32_t elements
 * packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_int32_t register whose int32_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int32_t* dst_addr, vector_int32_t& dst, vector_store_unalign& src0, vector_int32_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the uint32_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUu32_Rvu32_Rvstoreunalign_vu32_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination uint32_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(uint32_t).
 * @param [out] dst Destination vector_uint32_t register passed by reference; receives the selected uint32_t elements
 * packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_uint32_t register whose uint32_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ uint32_t* dst_addr, vector_uint32_t& dst, vector_store_unalign& src0, vector_uint32_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the half elements of src1 selected by squeeze_mask into dst and stores them consecutively to Unified
 * Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf16_Rvf16_Rvstoreunalign_vf16_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination half region in Unified Buffer; the packed elements are written
 * there at the byte offset held in the AR register, the pointer value itself is not modified, and the address must be
 * aligned to sizeof(half).
 * @param [out] dst Destination vector_half register passed by reference; receives the selected half elements packed
 * from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_half register whose half elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ half* dst_addr, vector_half& dst, vector_store_unalign& src0, vector_half src1, vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the float elements of src1 selected by squeeze_mask into dst and stores them consecutively to Unified
 * Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf32_Rvf32_Rvstoreunalign_vf32_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination float region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(float).
 * @param [out] dst Destination vector_float register passed by reference; receives the selected float elements packed
 * from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_float register whose float elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ float* dst_addr, vector_float& dst, vector_store_unalign& src0, vector_float src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the bfloat16_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUbf16_Rvbf16_Rvstoreunalign_vbf16_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination bfloat16_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(bfloat16_t).
 * @param [out] dst Destination vector_bfloat16_t register passed by reference; receives the selected bfloat16_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_bfloat16_t register whose bfloat16_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ bfloat16_t* dst_addr, vector_bfloat16_t& dst, vector_store_unalign& src0, vector_bfloat16_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the fp8_e4m3fn_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf8e4m3_Rvf8e4m3_Rvstoreunalign_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination fp8_e4m3fn_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(fp8_e4m3fn_t).
 * @param [out] dst Destination vector_fp8_e4m3fn_t register passed by reference; receives the selected fp8_e4m3fn_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_fp8_e4m3fn_t register whose fp8_e4m3fn_t elements selected by squeeze_mask are
 * compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e4m3fn_t* dst_addr, vector_fp8_e4m3fn_t& dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the fp8_e5m2_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf8e5m2_Rvf8e5m2_Rvstoreunalign_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination fp8_e5m2_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(fp8_e5m2_t).
 * @param [out] dst Destination vector_fp8_e5m2_t register passed by reference; receives the selected fp8_e5m2_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_fp8_e5m2_t register whose fp8_e5m2_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e5m2_t* dst_addr, vector_fp8_e5m2_t& dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the fp8_e8m0_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf8e8m0_Rvf8e8m0_Rvstoreunalign_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination fp8_e8m0_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(fp8_e8m0_t).
 * @param [out] dst Destination vector_fp8_e8m0_t register passed by reference; receives the selected fp8_e8m0_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_fp8_e8m0_t register whose fp8_e8m0_t elements selected by squeeze_mask are compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp8_e8m0_t* dst_addr, vector_fp8_e8m0_t& dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the fp4x2_e2m1_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf4e2m1x2_Rvf4e2m1x2_Rvstoreunalign_vf4e2m1x2_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination fp4x2_e2m1_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(fp4x2_e2m1_t).
 * @param [out] dst Destination vector_fp4x2_e2m1_t register passed by reference; receives the selected fp4x2_e2m1_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_fp4x2_e2m1_t register whose fp4x2_e2m1_t elements selected by squeeze_mask are
 * compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp4x2_e2m1_t* dst_addr, vector_fp4x2_e2m1_t& dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Squeezes the fp4x2_e1m2_t elements of src1 selected by squeeze_mask into dst and stores them consecutively to
 * Unified Buffer at dst_addr plus the AR register byte offset, staging the sub-32-byte tail in src0.
 *
 * @override_id asc_squeeze_and_storeunalign__PUf4e1m2x2_Rvf4e1m2x2_Rvstoreunalign_vf4e1m2x2_vb
 * @archs 3510
 * @param [out] dst_addr Start address of the destination fp4x2_e1m2_t region in Unified Buffer; the packed elements are
 * written there at the byte offset held in the AR register, the pointer value itself is not modified, and the address
 * must be aligned to sizeof(fp4x2_e1m2_t).
 * @param [out] dst Destination vector_fp4x2_e1m2_t register passed by reference; receives the selected fp4x2_e1m2_t
 * elements packed from the low end with the remaining elements set to 0.
 * @param [in,out] src0 32-byte unaligned register that supplies the pending sub-32-byte tail left by earlier calls and
 * receives the new tail produced by this call; the same register must be reused across consecutive calls.
 * @param [in] src1 Source vector_fp4x2_e1m2_t register whose fp4x2_e1m2_t elements selected by squeeze_mask are
 * compressed.
 * @param [in] squeeze_mask Mask register whose 1 bits select the src1 elements that are compressed and stored; 0 bits
 * drop the element.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ fp4x2_e1m2_t* dst_addr, vector_fp4x2_e1m2_t& dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1,
    vector_bool squeeze_mask);

__simd_callee__ inline void asc_squeeze_and_storeunalign(
    __ubuf__ int4b_t* dst_addr, vector_int4x2_t& dst, vector_store_unalign& src0, vector_int4x2_t src1,
    vector_bool squeeze_mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the int8_t unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUi8_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination int8_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(int8_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int8_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the uint8_t unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUu8_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination uint8_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(uint8_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint8_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the fp4x2_e2m1_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf4e2m1x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination fp4x2_e2m1_t pointer in UB that receives the tail block; it must repeat the dst_addr
 * used by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(fp4x2_e2m1_t), not necessarily to 32
 * bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the fp4x2_e1m2_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf4e1m2x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination fp4x2_e1m2_t pointer in UB that receives the tail block; it must repeat the dst_addr
 * used by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(fp4x2_e1m2_t), not necessarily to 32
 * bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the int4b_t unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUi4x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination int4b_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(int4b_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int4b_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the fp8_e8m0_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf8e8m0_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination fp8_e8m0_t pointer in UB that receives the tail block; it must repeat the dst_addr used
 * by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(fp8_e8m0_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the fp8_e5m2_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf8e5m2_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination fp8_e5m2_t pointer in UB that receives the tail block; it must repeat the dst_addr used
 * by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(fp8_e5m2_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the fp8_e4m3fn_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf8e4m3_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination fp8_e4m3fn_t pointer in UB that receives the tail block; it must repeat the dst_addr
 * used by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(fp8_e4m3fn_t), not necessarily to 32
 * bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the int16_t unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUi16_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination int16_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(int16_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int16_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the uint16_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUu16_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination uint16_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(uint16_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint16_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the half unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf16_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination half pointer in UB that receives the tail block; it must repeat the dst_addr used by the
 * group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(half), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ half* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the bfloat16_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUbf16_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination bfloat16_t pointer in UB that receives the tail block; it must repeat the dst_addr used
 * by the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(bfloat16_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ bfloat16_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the int32_t unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUi32_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination int32_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(int32_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ int32_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the uint32_t unaligned store register back to the destination UB
 * address, terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUu32_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination uint32_t pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(uint32_t), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ uint32_t* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes the pending tail block held in the float unaligned store register back to the destination UB address,
 * terminating one group of asc_squeeze_and_storeunalign calls.
 *
 * @override_id asc_squeeze_and_storeunalign_finalize__PUf32_vstoreunalign
 * @archs 3510
 * @param [out] dst Destination float pointer in UB that receives the tail block; it must repeat the dst_addr used by
 * the group's asc_squeeze_and_storeunalign calls and be aligned to sizeof(float), not necessarily to 32 bytes.
 * @param [in] src 32-byte vector_store_unalign register that buffers the tail block to flush; it must be the very same
 * register object that the group passed as src0 to asc_squeeze_and_storeunalign.
 */
__simd_callee__ inline void asc_squeeze_and_storeunalign_finalize(__ubuf__ float* dst, vector_store_unalign src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the bits of the mask registers src0 and src1 in 1-bit groups, writing the groups of their first
 * halves alternately into dst0 and of their second halves alternately into dst1.
 *
 * @override_id asc_intlv_b8__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register holding the interleaved lower half, taken alternately from the first
 * halves of src0 and src1 as 1-bit groups.
 * @param [out] dst1 Destination mask register holding the interleaved upper half, taken alternately from the second
 * halves of src0 and src1 as 1-bit groups.
 * @param [in] src0 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src1.
 * @param [in] src1 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src0.
 */
__simd_callee__ inline void asc_intlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the bits of the mask registers src0 and src1 in 2-bit groups, writing the groups of their first
 * halves alternately into dst0 and of their second halves alternately into dst1.
 *
 * @override_id asc_intlv_b16__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register holding the interleaved lower half, taken alternately from the first
 * halves of src0 and src1 as 2-bit groups.
 * @param [out] dst1 Destination mask register holding the interleaved upper half, taken alternately from the second
 * halves of src0 and src1 as 2-bit groups.
 * @param [in] src0 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src1.
 * @param [in] src1 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src0.
 */
__simd_callee__ inline void asc_intlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the bits of the mask registers src0 and src1 in 4-bit groups, writing the groups of their first
 * halves alternately into dst0 and of their second halves alternately into dst1.
 *
 * @override_id asc_intlv_b32__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register holding the interleaved lower half, taken alternately from the first
 * halves of src0 and src1 as 4-bit groups.
 * @param [out] dst1 Destination mask register holding the interleaved upper half, taken alternately from the second
 * halves of src0 and src1 as 4-bit groups.
 * @param [in] src0 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src1.
 * @param [in] src1 Source mask register whose first half is interleaved into dst0 and second half into dst1; may alias
 * src0.
 */
__simd_callee__ inline void asc_intlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the uint8_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvu8_Rvu8_vu8_vu8
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the int8_t elements of the first halves of src0 and src1 alternately into dst0 and of their second
 * halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvi8_Rvi8_vi8_vi8
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the uint16_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvu16_Rvu16_vu16_vu16
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the int16_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvi16_Rvi16_vi16_vi16
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the uint32_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvu32_Rvu32_vu32_vu32
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the int32_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvi32_Rvi32_vi32_vi32
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the half elements of the first halves of src0 and src1 alternately into dst0 and of their second
 * halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvf16_Rvf16_vf16_vf16
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the float elements of the first halves of src0 and src1 alternately into dst0 and of their second
 * halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvf32_Rvf32_vf32_vf32
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the fp8_e8m0_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvf8e8m0_Rvf8e8m0_vf8e8m0_vf8e8m0
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the fp8_e5m2_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvf8e5m2_Rvf8e5m2_vf8e5m2_vf8e5m2
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the fp8_e4m3fn_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvf8e4m3_Rvf8e4m3_vf8e4m3_vf8e4m3
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the hifloat8_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvhf8_Rvhf8_vhf8_vhf8
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Interleaves the bfloat16_t elements of the first halves of src0 and src1 alternately into dst0 and of their
 * second halves alternately into dst1.
 *
 * @override_id asc_intlv__Rvbf16_Rvbf16_vbf16_vbf16
 * @archs 3510
 * @param [out] dst0 Destination vector register holding the interleaved lower half, filled alternately from the first
 * halves of src0 and src1; dtype must equal that of src0, src1 and dst1.
 * @param [out] dst1 Destination vector register holding the interleaved upper half, filled alternately from the second
 * halves of src0 and src1; must be a different register from dst0.
 * @param [in] src0 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src1.
 * @param [in] src1 Source vector register whose first half is interleaved into dst0 and second half into dst1; may
 * alias src0.
 */
__simd_callee__ inline void asc_intlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvu8_vb
 * @archs 3510
 * @param [in,out] dst Vector data register receiving the decompressed uint8_t prefix counts; the doc lists it as an
 * input/output operand updated by the interface.
 * @param [in] mask Mask register whose set bits are tallied; the value at position i counts mask bits 0 to i-1 and the
 * highest mask bit is ignored.
 */
__simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvi8_vb
 * @archs 3510
 * @param [in,out] dst Vector data register that carries the decompressed prefix counts; the interface writes the count
 * results into it, and the doc classifies it as input/output.
 * @param [in] mask Mask register providing the predicate bits; the count at position i uses mask bits 0 to i-1 and the
 * highest mask bit is ignored.
 */
__simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvu16_vb
 * @archs 3510
 * @param [in,out] dst Vector data register receiving the uint16_t decompression counts; the doc marks it as
 * input/output because the interface writes the count results there.
 * @param [in] mask Mask register providing the predicate bits counted by the interface; the count at position i covers
 * mask bits 0 to i-1 and the top bit is ignored.
 */
__simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvi16_vb
 * @archs 3510
 * @param [in,out] dst Vector data register carrying the decompressed int16_t prefix counts; the interface updates it,
 * as the doc input/output classification states.
 * @param [in] mask Mask register supplying the counted bits; position i uses mask bits 0 to i-1 and the most
 * significant mask bit is not counted.
 */
__simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvu32_vb
 * @archs 3510
 * @param [in,out] dst Vector data register receiving the uint32_t decompression counts; the doc classifies it as
 * input/output because the interface updates it with the count results.
 * @param [in] mask Mask register whose lower bits are tallied; position i counts mask bits 0 to i-1 and the most
 * significant mask bit does not contribute.
 */
__simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Writes into dst the running count of mask ones preceding each position, starting with zero at element 0.
 *
 * @override_id asc_unsqueeze__Rvi32_vb
 * @archs 3510
 * @param [in,out] dst Vector data register holding the int32_t decompression counts; the interface writes the count
 * sequence into it, hence the input/output classification.
 * @param [in] mask Mask register providing the bits whose prefix population count forms the output; mask bits 0 to i-1
 * determine position i and the highest bit is ignored.
 */
__simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a uint8_t register holding the running count of mask ones before each position, with the first element
 * set to zero.
 *
 * @override_id asc_unsqueeze_u8__vb
 * @archs 3510
 * @param [in] mask Mask register whose lower bits are counted; element i of the result equals the number of ones in
 * mask bits 0 to i-1 and the top bit is ignored.
 */
__simd_callee__ inline vector_uint8_t asc_unsqueeze_u8(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns an int8_t register holding the running count of mask ones before each position, with the first element
 * set to zero.
 *
 * @override_id asc_unsqueeze_s8__vb
 * @archs 3510
 * @param [in] mask Mask register providing the bits to count; element i of the result counts mask bits 0 through i-1
 * and the highest bit is ignored.
 */
__simd_callee__ inline vector_int8_t asc_unsqueeze_s8(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a uint16_t register holding the running count of mask ones before each position, with the first
 * element set to zero.
 *
 * @override_id asc_unsqueeze_u16__vb
 * @archs 3510
 * @param [in] mask Mask register supplying the predicate bits; element i of the returned register counts ones in mask
 * bits 0 to i-1 and the top bit is skipped.
 */
__simd_callee__ inline vector_uint16_t asc_unsqueeze_u16(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns an int16_t register holding the running count of mask ones before each position, with the first
 * element set to zero.
 *
 * @override_id asc_unsqueeze_s16__vb
 * @archs 3510
 * @param [in] mask Mask register whose bits drive the count; element i of the result equals the number of ones in mask
 * bits 0 to i-1, ignoring the top bit.
 */
__simd_callee__ inline vector_int16_t asc_unsqueeze_s16(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns a uint32_t register holding the running count of mask ones before each position, with the first
 * element set to zero.
 *
 * @override_id asc_unsqueeze_u32__vb
 * @archs 3510
 * @param [in] mask Mask register providing the counted bits; element i of the returned register equals the number of
 * ones in mask bits 0 to i-1, top bit ignored.
 */
__simd_callee__ inline vector_uint32_t asc_unsqueeze_u32(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Returns an int32_t register holding the running count of mask ones before each position, with the first
 * element set to zero.
 *
 * @override_id asc_unsqueeze_s32__vb
 * @archs 3510
 * @param [in] mask Mask register whose set bits are counted; element i of the result counts mask bits 0 through i-1 and
 * the highest bit is not counted.
 */
__simd_callee__ inline vector_int32_t asc_unsqueeze_s32(vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each uint16_t element of src into the low half of dst.
 *
 * @override_id asc_pack_to_low__Rvu8_vu16
 * @archs 3510
 * @param [out] dst Destination vector register whose low half receives the packed uint8_t elements.
 * @param [in] src Source vector register holding the uint16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each int16_t element of src into the low half of dst.
 *
 * @override_id asc_pack_to_low__Rvu8_vi16
 * @archs 3510
 * @param [out] dst Destination vector register whose low half receives the packed uint8_t elements.
 * @param [in] src Source vector register holding the int16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each uint32_t element of src into the low half of dst.
 *
 * @override_id asc_pack_to_low__Rvu16_vu32
 * @archs 3510
 * @param [out] dst Destination vector register whose low half receives the packed uint16_t elements.
 * @param [in] src Source vector register holding the uint32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst, vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each int32_t element of src into the low half of dst.
 *
 * @override_id asc_pack_to_low__Rvu16_vi32
 * @archs 3510
 * @param [out] dst Destination vector register whose low half receives the packed uint16_t elements.
 * @param [in] src Source vector register holding the int32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst, vector_int32_t src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_pack_to_low(vector_uint32_t& dst, vector_uint64_t src);

__simd_callee__ inline void asc_pack_to_low(vector_uint32_t& dst, vector_int64_t src);
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the lower bit of every 2-bit pair of the src mask into the low half of dst.
 *
 * @override_id asc_pack_to_low__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination vector register whose low half receives the packed vector_bool elements.
 * @param [in] src Source mask register whose bits are compressed two-to-one into the destination half.
 */
__simd_callee__ inline void asc_pack_to_low(vector_bool& dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each uint16_t element of src into the high half of dst.
 *
 * @override_id asc_pack_to_high__Rvu8_vu16
 * @archs 3510
 * @param [out] dst Destination vector register whose high half receives the packed uint8_t elements.
 * @param [in] src Source vector register holding the uint16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_high(vector_uint8_t& dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each int16_t element of src into the high half of dst.
 *
 * @override_id asc_pack_to_high__Rvu8_vi16
 * @archs 3510
 * @param [out] dst Destination vector register whose high half receives the packed uint8_t elements.
 * @param [in] src Source vector register holding the int16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_high(vector_uint8_t& dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each uint32_t element of src into the high half of dst.
 *
 * @override_id asc_pack_to_high__Rvu16_vu32
 * @archs 3510
 * @param [out] dst Destination vector register whose high half receives the packed uint16_t elements.
 * @param [in] src Source vector register holding the uint32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_high(vector_uint16_t& dst, vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each int32_t element of src into the high half of dst.
 *
 * @override_id asc_pack_to_high__Rvu16_vi32
 * @archs 3510
 * @param [out] dst Destination vector register whose high half receives the packed uint16_t elements.
 * @param [in] src Source vector register holding the int32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline void asc_pack_to_high(vector_uint16_t& dst, vector_int32_t src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_pack_to_high(vector_uint32_t& dst, vector_uint64_t src);

__simd_callee__ inline void asc_pack_to_high(vector_uint32_t& dst, vector_int64_t src);
#endif

__simd_callee__ inline void asc_pack_to_high(vector_bool& dst, vector_bool src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_pack_datablock(vector_uint16_t& dst, vector_uint16_t src);

__simd_callee__ inline void asc_pack_datablock(vector_int16_t& dst, vector_int16_t src);

__simd_callee__ inline void asc_pack_datablock(vector_half& dst, vector_half src);

__simd_callee__ inline void asc_pack_datablock(vector_uint32_t& dst, vector_uint32_t src);

__simd_callee__ inline void asc_pack_datablock(vector_int32_t& dst, vector_int32_t src);

__simd_callee__ inline void asc_pack_datablock(vector_float& dst, vector_float src);

__simd_callee__ inline void asc_pack_datablock(vector_uint64_t& dst, vector_uint64_t src);

__simd_callee__ inline void asc_pack_datablock(vector_int64_t& dst, vector_int64_t src);
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each uint16_t element of src into the low half of the returned uint8_t vector.
 *
 * @override_id asc_pack_to_low__vu16
 * @archs 3510
 * @param [in] src Source vector register holding the uint16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline vector_uint8_t asc_pack_to_low(vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each int16_t element of src into the low half of the returned uint8_t vector.
 *
 * @override_id asc_pack_to_low__vi16
 * @archs 3510
 * @param [in] src Source vector register holding the int16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline vector_uint8_t asc_pack_to_low(vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each uint32_t element of src into the low half of the returned uint16_t vector.
 *
 * @override_id asc_pack_to_low__vu32
 * @archs 3510
 * @param [in] src Source vector register holding the uint32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline vector_uint16_t asc_pack_to_low(vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each int32_t element of src into the low half of the returned uint16_t vector.
 *
 * @override_id asc_pack_to_low__vi32
 * @archs 3510
 * @param [in] src Source vector register holding the int32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline vector_uint16_t asc_pack_to_low(vector_int32_t src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_uint32_t asc_pack_to_low(vector_uint64_t src);

__simd_callee__ inline vector_uint32_t asc_pack_to_low(vector_int64_t src);
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the lower bit of every 2-bit pair of the src mask into the low half of the returned mask register.
 *
 * @override_id asc_pack_to_low__vb
 * @archs 3510
 * @param [in] src Source mask register whose bits are compressed two-to-one into the destination half.
 */
__simd_callee__ inline vector_bool asc_pack_to_low(vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each uint16_t element of src into the high half of the returned uint8_t vector.
 *
 * @override_id asc_pack_to_high__vu16
 * @archs 3510
 * @param [in] src Source vector register holding the uint16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline vector_uint8_t asc_pack_to_high(vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 8 bits of each int16_t element of src into the high half of the returned uint8_t vector.
 *
 * @override_id asc_pack_to_high__vi16
 * @archs 3510
 * @param [in] src Source vector register holding the int16_t elements whose low 8 bits are packed.
 */
__simd_callee__ inline vector_uint8_t asc_pack_to_high(vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each uint32_t element of src into the high half of the returned uint16_t vector.
 *
 * @override_id asc_pack_to_high__vu32
 * @archs 3510
 * @param [in] src Source vector register holding the uint32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline vector_uint16_t asc_pack_to_high(vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Packs the low 16 bits of each int32_t element of src into the high half of the returned uint16_t vector.
 *
 * @override_id asc_pack_to_high__vi32
 * @archs 3510
 * @param [in] src Source vector register holding the int32_t elements whose low 16 bits are packed.
 */
__simd_callee__ inline vector_uint16_t asc_pack_to_high(vector_int32_t src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_uint32_t asc_pack_to_high(vector_uint64_t src);

__simd_callee__ inline vector_uint32_t asc_pack_to_high(vector_int64_t src);
#endif

__simd_callee__ inline vector_bool asc_pack_to_high(vector_bool src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_uint16_t asc_pack_datablock(vector_uint16_t src);

__simd_callee__ inline vector_int16_t asc_pack_datablock(vector_int16_t src);

__simd_callee__ inline vector_half asc_pack_datablock(vector_half src);

__simd_callee__ inline vector_uint32_t asc_pack_datablock(vector_uint32_t src);

__simd_callee__ inline vector_int32_t asc_pack_datablock(vector_int32_t src);

__simd_callee__ inline vector_float asc_pack_datablock(vector_float src);

__simd_callee__ inline vector_uint64_t asc_pack_datablock(vector_uint64_t src);

__simd_callee__ inline vector_int64_t asc_pack_datablock(vector_int64_t src);
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index int32_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvi32_Rvi32_vi32_vi32
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index int32_t elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index int32_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index int32_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index int32_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index uint32_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvu32_Rvu32_vu32_vu32
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index uint32_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index uint32_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index uint32_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index uint32_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index int16_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvi16_Rvi16_vi16_vi16
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index int16_t elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index int16_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index int16_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index int16_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index uint16_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvu16_Rvu16_vu16_vu16
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index uint16_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index uint16_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index uint16_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index uint16_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index int8_t elements of src0 and src1, writing the even elements to dst0
 * and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvi8_Rvi8_vi8_vi8
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index int8_t elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index int8_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index int8_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index int8_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index uint8_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvu8_Rvu8_vu8_vu8
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index uint8_t elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index uint8_t elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index uint8_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index uint8_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index fp8_e4m3fn_t elements of src0 and src1, writing the even elements
 * to dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvf8e4m3_Rvf8e4m3_vf8e4m3_vf8e4m3
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index fp8_e4m3fn_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index fp8_e4m3fn_t elements: the lower
 * half comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index fp8_e4m3fn_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index fp8_e4m3fn_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index hifloat8_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvhf8_Rvhf8_vhf8_vhf8
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index hifloat8_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index hifloat8_t elements: the lower
 * half comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index hifloat8_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index hifloat8_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index fp8_e5m2_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvf8e5m2_Rvf8e5m2_vf8e5m2_vf8e5m2
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index fp8_e5m2_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index fp8_e5m2_t elements: the lower
 * half comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index fp8_e5m2_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index fp8_e5m2_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index fp8_e8m0_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvf8e8m0_Rvf8e8m0_vf8e8m0_vf8e8m0
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index fp8_e8m0_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index fp8_e8m0_t elements: the lower
 * half comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index fp8_e8m0_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index fp8_e8m0_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index bfloat16_t elements of src0 and src1, writing the even elements to
 * dst0 and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvbf16_Rvbf16_vbf16_vbf16
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index bfloat16_t elements: the lower
 * half comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index bfloat16_t elements: the lower
 * half comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index bfloat16_t elements go to the lower half of dst0 and its
 * odd-index elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index bfloat16_t elements go to the upper half of dst0 and its
 * odd-index elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index float elements of src0 and src1, writing the even elements to dst0
 * and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvf32_Rvf32_vf32_vf32
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index float elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index float elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index float elements go to the lower half of dst0 and its odd-index
 * elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index float elements go to the upper half of dst0 and its odd-index
 * elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the even-index and odd-index half elements of src0 and src1, writing the even elements to dst0
 * and the odd elements to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv__Rvf16_Rvf16_vf16_vf16
 * @archs 3510
 * @param [out] dst0 Destination vector register receiving the deinterleaved even-index half elements: the lower half
 * comes from src0 and the upper half from src1; its dtype must match src0, src1 and dst1.
 * @param [out] dst1 Destination vector register receiving the deinterleaved odd-index half elements: the lower half
 * comes from src0 and the upper half from src1; must be a different register from dst0.
 * @param [in] src0 Source vector register; its even-index half elements go to the lower half of dst0 and its odd-index
 * elements to the lower half of dst1; may alias src1.
 * @param [in] src1 Source vector register; its even-index half elements go to the upper half of dst0 and its odd-index
 * elements to the upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the bits of the mask registers src0 and src1 in 1-bit groups, writing the even groups to dst0
 * and the odd groups to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv_b8__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register receiving the even 1-bit groups of src0 in its lower half and the even
 * groups of src1 in its upper half.
 * @param [out] dst1 Destination mask register receiving the odd 1-bit groups of src0 in its lower half and the odd
 * groups of src1 in its upper half.
 * @param [in] src0 Source mask register; its even 1-bit groups go to the lower half of dst0 and its odd groups to the
 * lower half of dst1 (all four operands must share the same bit grouping).
 * @param [in] src1 Source mask register; its even 1-bit groups go to the upper half of dst0 and its odd groups to the
 * upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the bits of the mask registers src0 and src1 in 2-bit groups, writing the even groups to dst0
 * and the odd groups to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv_b16__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register receiving the even 2-bit groups of src0 in its lower half and the even
 * groups of src1 in its upper half.
 * @param [out] dst1 Destination mask register receiving the odd 2-bit groups of src0 in its lower half and the odd
 * groups of src1 in its upper half.
 * @param [in] src0 Source mask register; its even 2-bit groups go to the lower half of dst0 and its odd groups to the
 * lower half of dst1 (all four operands must share the same bit grouping).
 * @param [in] src1 Source mask register; its even 2-bit groups go to the upper half of dst0 and its odd groups to the
 * upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Deinterleaves the bits of the mask registers src0 and src1 in 4-bit groups, writing the even groups to dst0
 * and the odd groups to dst1, with src0 filling the lower halves and src1 the upper halves.
 *
 * @override_id asc_deintlv_b32__Rvb_Rvb_vb_vb
 * @archs 3510
 * @param [out] dst0 Destination mask register receiving the even 4-bit groups of src0 in its lower half and the even
 * groups of src1 in its upper half.
 * @param [out] dst1 Destination mask register receiving the odd 4-bit groups of src0 in its lower half and the odd
 * groups of src1 in its upper half.
 * @param [in] src0 Source mask register; its even 4-bit groups go to the lower half of dst0 and its odd groups to the
 * lower half of dst1 (all four operands must share the same bit grouping).
 * @param [in] src1 Source mask register; its even 4-bit groups go to the upper half of dst0 and its odd groups to the
 * upper half of dst1; may alias src0.
 */
__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the vector_uint8_t source into dst, zero-extends its 128 elements to uint16_t and
 * overwriting all 128 destination elements.
 *
 * @override_id asc_unpack_upper__Rvu16_vu8
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register holding the 128 widened uint16_t elements; every valid element
 * is overwritten, so its previous contents are not used.
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; only its upper half (128 elements) is read
 * and widened to uint16_t.
 */
__simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the vector_uint8_t source into dst, zero-extends its 128 elements to uint16_t and
 * overwriting all 128 destination elements.
 *
 * @override_id asc_unpack_lower__Rvu16_vu8
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register holding the 128 widened uint16_t elements; every valid element
 * is overwritten, so its previous contents are not used.
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; only its lower half (128 elements) is read
 * and widened to uint16_t.
 */
__simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the vector_int8_t source into dst, sign-extends its 128 elements to int16_t and
 * overwriting all 128 destination elements.
 *
 * @override_id asc_unpack_upper__Rvi16_vi8
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register holding the 128 widened int16_t elements; every valid element is
 * overwritten, so its previous contents are not used.
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; only its upper half (128 elements) is read
 * and widened to int16_t.
 */
__simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the vector_int8_t source into dst, sign-extends its 128 elements to int16_t and
 * overwriting all 128 destination elements.
 *
 * @override_id asc_unpack_lower__Rvi16_vi8
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register holding the 128 widened int16_t elements; every valid element is
 * overwritten, so its previous contents are not used.
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; only its lower half (128 elements) is read
 * and widened to int16_t.
 */
__simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the vector_uint16_t source into dst, zero-extends its 64 elements to uint32_t and
 * overwriting all 64 destination elements.
 *
 * @override_id asc_unpack_upper__Rvu32_vu16
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register holding the 64 widened uint32_t elements; every valid element
 * is overwritten, so its previous contents are not used.
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; only its upper half (64 elements) is read
 * and widened to uint32_t.
 */
__simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the vector_uint16_t source into dst, zero-extends its 64 elements to uint32_t and
 * overwriting all 64 destination elements.
 *
 * @override_id asc_unpack_lower__Rvu32_vu16
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register holding the 64 widened uint32_t elements; every valid element
 * is overwritten, so its previous contents are not used.
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; only its lower half (64 elements) is read
 * and widened to uint32_t.
 */
__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the vector_int16_t source into dst, sign-extends its 64 elements to int32_t and
 * overwriting all 64 destination elements.
 *
 * @override_id asc_unpack_upper__Rvi32_vi16
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register holding the 64 widened int32_t elements; every valid element is
 * overwritten, so its previous contents are not used.
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; only its upper half (64 elements) is read
 * and widened to int32_t.
 */
__simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the vector_int16_t source into dst, sign-extends its 64 elements to int32_t and
 * overwriting all 64 destination elements.
 *
 * @override_id asc_unpack_lower__Rvi32_vi16
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register holding the 64 widened int32_t elements; every valid element is
 * overwritten, so its previous contents are not used.
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; only its lower half (64 elements) is read
 * and widened to int32_t.
 */
__simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Expands the upper half of the mask register src into dst by turning each bit into two bits with the high bit
 * 0, overwriting the whole destination mask.
 *
 * @override_id asc_unpack_upper__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool mask register; every valid bit is overwritten by the two-bit expansion of
 * the source, so its previous contents are not used.
 * @param [in] src Source vector_bool mask register (VL/8 = 32 bytes = 256 bits); only its upper half is read and each
 * of those bits is expanded into two bits with the high bit 0.
 */
__simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Expands the lower half of the mask register src into dst by turning each bit into two bits with the high bit
 * 0, overwriting the whole destination mask.
 *
 * @override_id asc_unpack_lower__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool mask register; every valid bit is overwritten by the two-bit expansion of
 * the source, so its previous contents are not used.
 * @param [in] src Source vector_bool mask register (VL/8 = 32 bytes = 256 bits); only its lower half is read and each
 * of those bits is expanded into two bits with the high bit 0.
 */
__simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the 256-element vector_uint8_t source, zero-extends its 128 elements to uint16_t and
 * returns them as a vector_uint16_t.
 *
 * @override_id asc_unpack_upper__vu8
 * @archs 3510
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; only its upper half (128 elements) is read
 * and widened to uint16_t.
 */
__simd_callee__ inline vector_uint16_t asc_unpack_upper(vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the 256-element vector_uint8_t source, zero-extends its 128 elements to uint16_t and
 * returns them as a vector_uint16_t.
 *
 * @override_id asc_unpack_lower__vu8
 * @archs 3510
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; only its lower half (128 elements) is read
 * and widened to uint16_t.
 */
__simd_callee__ inline vector_uint16_t asc_unpack_lower(vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the 256-element vector_int8_t source, sign-extends its 128 elements to int16_t and
 * returns them as a vector_int16_t.
 *
 * @override_id asc_unpack_upper__vi8
 * @archs 3510
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; only its upper half (128 elements) is read
 * and widened to int16_t.
 */
__simd_callee__ inline vector_int16_t asc_unpack_upper(vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the 256-element vector_int8_t source, sign-extends its 128 elements to int16_t and
 * returns them as a vector_int16_t.
 *
 * @override_id asc_unpack_lower__vi8
 * @archs 3510
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; only its lower half (128 elements) is read
 * and widened to int16_t.
 */
__simd_callee__ inline vector_int16_t asc_unpack_lower(vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the 128-element vector_uint16_t source, zero-extends its 64 elements to uint32_t and
 * returns them as a vector_uint32_t.
 *
 * @override_id asc_unpack_upper__vu16
 * @archs 3510
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; only its upper half (64 elements) is read
 * and widened to uint32_t.
 */
__simd_callee__ inline vector_uint32_t asc_unpack_upper(vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the 128-element vector_uint16_t source, zero-extends its 64 elements to uint32_t and
 * returns them as a vector_uint32_t.
 *
 * @override_id asc_unpack_lower__vu16
 * @archs 3510
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; only its lower half (64 elements) is read
 * and widened to uint32_t.
 */
__simd_callee__ inline vector_uint32_t asc_unpack_lower(vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the upper half of the 128-element vector_int16_t source, sign-extends its 64 elements to int32_t and
 * returns them as a vector_int32_t.
 *
 * @override_id asc_unpack_upper__vi16
 * @archs 3510
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; only its upper half (64 elements) is read
 * and widened to int32_t.
 */
__simd_callee__ inline vector_int32_t asc_unpack_upper(vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Unpacks the lower half of the 128-element vector_int16_t source, sign-extends its 64 elements to int32_t and
 * returns them as a vector_int32_t.
 *
 * @override_id asc_unpack_lower__vi16
 * @archs 3510
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; only its lower half (64 elements) is read
 * and widened to int32_t.
 */
__simd_callee__ inline vector_int32_t asc_unpack_lower(vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Expands the upper half of the mask register src by turning each bit into two bits with the high bit 0 and
 * returns the resulting vector_bool.
 *
 * @override_id asc_unpack_upper__vb
 * @archs 3510
 * @param [in] src Source vector_bool mask register (VL/8 = 32 bytes = 256 bits); only its upper half is read and each
 * of those bits is expanded into two bits with the high bit 0.
 */
__simd_callee__ inline vector_bool asc_unpack_upper(vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_permute_sel
 * @brief Expands the lower half of the mask register src by turning each bit into two bits with the high bit 0 and
 * returns the resulting vector_bool.
 *
 * @override_id asc_unpack_lower__vb
 * @archs 3510
 * @param [in] src Source vector_bool mask register (VL/8 = 32 bytes = 256 bits); only its lower half is read and each
 * of those bits is expanded into two bits with the high bit 0.
 */
__simd_callee__ inline vector_bool asc_unpack_lower(vector_bool src);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
//=================asc_intlv_half (for 9201)==================
__simd_callee__ inline void asc_intlv_half_low_b8(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_low_b16(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_low_b32(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_high_b8(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_high_b16(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_high_b32(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_low_b8(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_low_b16(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_low_b32(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_high_b8(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_high_b16(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_intlv_half_high_b32(vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_intlv_half_low(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_half& dst, vector_half src0, vector_half src1);

__simd_callee__ inline void asc_intlv_half_low(vector_float& dst, vector_float src0, vector_float src1);

__simd_callee__ inline void asc_intlv_half_low(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline void asc_intlv_half_low(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline vector_uint8_t asc_intlv_half_low(vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline vector_int8_t asc_intlv_half_low(vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline vector_uint16_t asc_intlv_half_low(vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline vector_int16_t asc_intlv_half_low(vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline vector_uint32_t asc_intlv_half_low(vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline vector_int32_t asc_intlv_half_low(vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline vector_half asc_intlv_half_low(vector_half src0, vector_half src1);

__simd_callee__ inline vector_float asc_intlv_half_low(vector_float src0, vector_float src1);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_intlv_half_low(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline vector_fp8_e6m2_t asc_intlv_half_low(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline vector_hifloat8_t asc_intlv_half_low(vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline vector_fp8_e5m2_t asc_intlv_half_low(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline vector_fp8_e8m0_t asc_intlv_half_low(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline vector_bfloat16_t asc_intlv_half_low(vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_half& dst, vector_half src0, vector_half src1);

__simd_callee__ inline void asc_intlv_half_high(vector_float& dst, vector_float src0, vector_float src1);

__simd_callee__ inline void asc_intlv_half_high(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline void asc_intlv_half_high(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline vector_uint8_t asc_intlv_half_high(vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline vector_int8_t asc_intlv_half_high(vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline vector_uint16_t asc_intlv_half_high(vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline vector_int16_t asc_intlv_half_high(vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline vector_uint32_t asc_intlv_half_high(vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline vector_int32_t asc_intlv_half_high(vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline vector_half asc_intlv_half_high(vector_half src0, vector_half src1);

__simd_callee__ inline vector_float asc_intlv_half_high(vector_float src0, vector_float src1);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_intlv_half_high(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline vector_fp8_e6m2_t asc_intlv_half_high(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline vector_hifloat8_t asc_intlv_half_high(vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline vector_fp8_e5m2_t asc_intlv_half_high(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline vector_fp8_e8m0_t asc_intlv_half_high(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline vector_bfloat16_t asc_intlv_half_high(vector_bfloat16_t src0, vector_bfloat16_t src1);

//=================asc_deintlv_half (for 9201)==================
__simd_callee__ inline void asc_deintlv_even_b8(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_even_b16(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_even_b32(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_odd_b8(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_odd_b16(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_odd_b32(vector_bool& dst, vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_even_b8(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_even_b16(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_even_b32(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_odd_b8(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_odd_b16(vector_bool src0, vector_bool src1);

__simd_callee__ inline vector_bool asc_deintlv_odd_b32(vector_bool src0, vector_bool src1);

__simd_callee__ inline void asc_deintlv_even(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_half& dst, vector_half src0, vector_half src1);

__simd_callee__ inline void asc_deintlv_even(vector_float& dst, vector_float src0, vector_float src1);

__simd_callee__ inline void asc_deintlv_even(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline void asc_deintlv_even(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline vector_uint8_t asc_deintlv_even(vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline vector_int8_t asc_deintlv_even(vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline vector_uint16_t asc_deintlv_even(vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline vector_int16_t asc_deintlv_even(vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline vector_uint32_t asc_deintlv_even(vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline vector_int32_t asc_deintlv_even(vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline vector_half asc_deintlv_even(vector_half src0, vector_half src1);

__simd_callee__ inline vector_float asc_deintlv_even(vector_float src0, vector_float src1);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_deintlv_even(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline vector_fp8_e6m2_t asc_deintlv_even(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline vector_hifloat8_t asc_deintlv_even(vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline vector_fp8_e5m2_t asc_deintlv_even(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline vector_fp8_e8m0_t asc_deintlv_even(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline vector_bfloat16_t asc_deintlv_even(vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_half& dst, vector_half src0, vector_half src1);

__simd_callee__ inline void asc_deintlv_odd(vector_float& dst, vector_float src0, vector_float src1);

__simd_callee__ inline void asc_deintlv_odd(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e6m2_t& dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline void asc_deintlv_odd(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

__simd_callee__ inline vector_uint8_t asc_deintlv_odd(vector_uint8_t src0, vector_uint8_t src1);

__simd_callee__ inline vector_int8_t asc_deintlv_odd(vector_int8_t src0, vector_int8_t src1);

__simd_callee__ inline vector_uint16_t asc_deintlv_odd(vector_uint16_t src0, vector_uint16_t src1);

__simd_callee__ inline vector_int16_t asc_deintlv_odd(vector_int16_t src0, vector_int16_t src1);

__simd_callee__ inline vector_uint32_t asc_deintlv_odd(vector_uint32_t src0, vector_uint32_t src1);

__simd_callee__ inline vector_int32_t asc_deintlv_odd(vector_int32_t src0, vector_int32_t src1);

__simd_callee__ inline vector_half asc_deintlv_odd(vector_half src0, vector_half src1);

__simd_callee__ inline vector_float asc_deintlv_odd(vector_float src0, vector_float src1);

__simd_callee__ inline vector_fp8_e4m3fn_t asc_deintlv_odd(vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

__simd_callee__ inline vector_fp8_e6m2_t asc_deintlv_odd(vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);

__simd_callee__ inline vector_hifloat8_t asc_deintlv_odd(vector_hifloat8_t src0, vector_hifloat8_t src1);

__simd_callee__ inline vector_fp8_e5m2_t asc_deintlv_odd(vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

__simd_callee__ inline vector_fp8_e8m0_t asc_deintlv_odd(vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

__simd_callee__ inline vector_bfloat16_t asc_deintlv_odd(vector_bfloat16_t src0, vector_bfloat16_t src1);
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint8_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint8_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint8_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded uint8_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int8_t elements of a vector register marked by mask into dst, packed consecutively
 * and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int8_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int8_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded int8_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint16_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint16_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint16_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded uint16_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int16_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int16_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int16_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded int16_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint32_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint32_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint32_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded uint32_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int32_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int32_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int32_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded int32_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the half elements of a vector register marked by mask into dst, packed consecutively
 * and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_half that receives the mask-selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_half whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded half positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_half& dst, vector_half src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the float elements of a vector register marked by mask into dst, packed consecutively
 * and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_float that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_float whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded float positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_float& dst, vector_float src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the bfloat16_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_bfloat16_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_bfloat16_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded bfloat16_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the hifloat8_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvhf8_vhf8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_hifloat8_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_hifloat8_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded hifloat8_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e4m3fn_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e4m3fn_t that receives the mask-selected
 * elements packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e4m3fn_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded fp8_e4m3fn_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e5m2_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e5m2_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e5m2_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded fp8_e5m2_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e8m0_t elements of a vector register marked by mask into dst, packed
 * consecutively and zero padded, while the number of valid elements is saved in the AR register; replace it with
 * asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_with_status__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e8m0_t that receives the mask-selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e8m0_t whose elements are tested against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the excluded fp8_e8m0_t positions are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_with_status(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint8_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint8_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint8_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the uint8_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int8_t elements of a vector register marked by mask, packed consecutively into dst
 * with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int8_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int8_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the int8_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint16_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint16_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint16_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the uint16_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int16_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int16_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int16_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the int16_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the uint32_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint32_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_uint32_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the uint32_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the int32_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int32_t that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_int32_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the int32_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the half elements of a vector register marked by mask, packed consecutively into dst
 * with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_half that receives the selected elements packed from
 * the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_half whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the half elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_half& dst, vector_half src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the float elements of a vector register marked by mask, packed consecutively into dst
 * with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_float that receives the selected elements packed
 * from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_float whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the float elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_float& dst, vector_float src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the bfloat16_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_bfloat16_t that receives the selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_bfloat16_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the bfloat16_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the hifloat8_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvhf8_vhf8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_hifloat8_t that receives the selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_hifloat8_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the hifloat8_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e4m3fn_t elements of a vector register marked by mask, packed consecutively
 * into dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e4m3fn_t that receives the selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e4m3fn_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the fp8_e4m3fn_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e5m2_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e5m2_t that receives the selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e5m2_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the fp8_e5m2_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated selection of the fp8_e8m0_t elements of a vector register marked by mask, packed consecutively into
 * dst with the remaining positions zeroed; replace it with asc_squeeze_and_storeunalign.
 *
 * @override_id asc_squeeze_v2__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_fp8_e8m0_t that receives the selected elements
 * packed from the low end, with all remaining positions set to zero.
 * @param [in] src Source vector data register of type vector_fp8_e8m0_t whose elements are examined against mask.
 * @param [in] mask Mask register selecting the source elements to process: a 1 bit includes the element, a 0 bit
 * excludes it, and the fp8_e8m0_t elements excluded by mask are written as zero into dst.
 */
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_low)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 8 bits of every uint16_t element of src and writes them into the
 * low half of the uint8_t destination register.
 *
 * @override_id asc_pack__Rvu8_vu16
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register written through a reference; the packed values occupy the low
 * half of the register.
 * @param [in] src Source vector_uint16_t vector register whose elements contribute their low 8 bits (uint16_t).
 */
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_uint16_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_low)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 8 bits of every int16_t element of src and writes them into the
 * low half of the uint8_t destination register.
 *
 * @override_id asc_pack__Rvu8_vi16
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register written through a reference; the packed values occupy the low
 * half of the register.
 * @param [in] src Source vector_int16_t vector register whose elements contribute their low 8 bits (int16_t).
 */
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_int16_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_low)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 16 bits of every uint32_t element of src and writes them into the
 * low half of the uint16_t destination register.
 *
 * @override_id asc_pack__Rvu16_vu32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register written through a reference; the packed values occupy the low
 * half of the register.
 * @param [in] src Source vector_uint32_t vector register whose elements contribute their low 16 bits (uint32_t).
 */
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_uint32_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_low)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 16 bits of every int32_t element of src and writes them into the
 * low half of the uint16_t destination register.
 *
 * @override_id asc_pack__Rvu16_vi32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register written through a reference; the packed values occupy the low
 * half of the register.
 * @param [in] src Source vector_int32_t vector register whose elements contribute their low 16 bits (int32_t).
 */
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_int32_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_low)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the mask bits of every bool element of src and writes them into the low
 * half of the bool destination register.
 *
 * @override_id asc_pack__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register written through a reference; the packed values occupy the low half
 * of the register.
 * @param [in] src Source vector_bool mask register whose bits are packed.
 */
__simd_callee__ inline void asc_pack(vector_bool& dst, vector_bool src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_high)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 8 bits of every uint16_t element of src and writes them into the
 * high half of the uint8_t destination register.
 *
 * @override_id asc_pack_v2__Rvu8_vu16
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register written through a reference; the packed values occupy the high
 * half of the register.
 * @param [in] src Source vector_uint16_t vector register whose elements contribute their low 8 bits (uint16_t).
 */
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_uint16_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_high)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 8 bits of every int16_t element of src and writes them into the
 * high half of the uint8_t destination register.
 *
 * @override_id asc_pack_v2__Rvu8_vi16
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register written through a reference; the packed values occupy the high
 * half of the register.
 * @param [in] src Source vector_int16_t vector register whose elements contribute their low 8 bits (int16_t).
 */
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_int16_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_high)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 16 bits of every uint32_t element of src and writes them into the
 * high half of the uint16_t destination register.
 *
 * @override_id asc_pack_v2__Rvu16_vu32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register written through a reference; the packed values occupy the high
 * half of the register.
 * @param [in] src Source vector_uint32_t vector register whose elements contribute their low 16 bits (uint32_t).
 */
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_uint32_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_high)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the low 16 bits of every int32_t element of src and writes them into the
 * high half of the uint16_t destination register.
 *
 * @override_id asc_pack_v2__Rvu16_vi32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register written through a reference; the packed values occupy the high
 * half of the register.
 * @param [in] src Source vector_int32_t vector register whose elements contribute their low 16 bits (int32_t).
 */
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_int32_t src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pack_to_high)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated pack operation that keeps the mask bits of every bool element of src and writes them into the high
 * half of the bool destination register.
 *
 * @override_id asc_pack_v2__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register written through a reference; the packed values occupy the high half
 * of the register.
 * @param [in] src Source vector_bool mask register whose bits are packed.
 */
__simd_callee__ inline void asc_pack_v2(vector_bool& dst, vector_bool src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_init)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: clears the AR special register that asc_squeeze_with_status accumulates into.
 *
 * @override_id asc_clear_ar_spr__z
 * @archs 3510
 */
__simd_callee__ inline void asc_clear_ar_spr();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_PERMUTE_SEL_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_PERMUTE_SEL_H
#endif
