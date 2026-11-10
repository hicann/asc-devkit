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
#error "compute/reg_arith.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ARITH_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ARITH_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_arith_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint8_t elements src1 from src0 and returns the element-wise differences as a
 * vector_uint8_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint8_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint8_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint8_t asc_sub(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int8_t elements src1 from src0 and returns the element-wise differences as a
 * vector_int8_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int8_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int8_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int8_t asc_sub(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint16_t elements src1 from src0 and returns the element-wise differences as a
 * vector_uint16_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint16_t asc_sub(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int16_t elements src1 from src0 and returns the element-wise differences as a
 * vector_int16_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int16_t asc_sub(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected half elements src1 from src0 and returns the element-wise differences as a
 * vector_half value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of half; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of half, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_half asc_sub(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected bfloat16_t elements src1 from src0 and returns the element-wise differences as a
 * vector_bfloat16_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of bfloat16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of bfloat16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_sub(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint32_t elements src1 from src0 and returns the element-wise differences as a
 * vector_uint32_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint32_t asc_sub(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int32_t elements src1 from src0 and returns the element-wise differences as a
 * vector_int32_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int32_t asc_sub(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected float elements src1 from src0 and returns the element-wise differences as a
 * vector_float value, with mask-cleared elements zeroed.
 *
 * @override_id asc_sub__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of float; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of float, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_float asc_sub(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint8_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint8_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint8_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint8_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int8_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int8_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int8_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int8_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint16_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint16_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int16_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int16_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected half elements src1 from src0 and stores each difference in dst, writing zero where
 * the mask bit is 0.
 *
 * @override_id asc_sub__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of half; receives the element-wise difference of src0 and src1 for
 * each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of half; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of half, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected bfloat16_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of bfloat16_t; receives the element-wise difference of src0 and
 * src1 for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of bfloat16_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of bfloat16_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected uint32_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected int32_t elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t; receives the element-wise difference of src0 and src1
 * for each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts the mask-selected float elements src1 from src0 and stores each difference in dst, writing zero
 * where the mask bit is 0.
 *
 * @override_id asc_sub__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of float; receives the element-wise difference of src0 and src1 for
 * each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of float; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of float, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes src0 - src1 as 32-bit two's-complement addition, writes the low 32 bits of each difference to dst and
 * stores the per-element no-borrow bit in the carry mask.
 *
 * @override_id asc_sub__Rvb_Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving one no-borrow bit per element; it is the carry-out of the src0
 * + ~src1 + 1 two's-complement addition.
 * @param [out] dst Destination vector data register of uint32_t that receives the low 32 bits ((src0_i + ~src1_i + 1) &
 * 0xFFFFFFFF); the upper part is only visible through carry.
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of uint32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes src0 - src1 as 32-bit two's-complement addition, writes the low 32 bits of each difference to dst and
 * stores the per-element no-borrow bit in the carry mask.
 *
 * @override_id asc_sub__Rvb_Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving one no-borrow bit per element; it is the carry-out of the src0
 * + ~src1 + 1 two's-complement addition.
 * @param [out] dst Destination vector data register of int32_t that receives the low 32 bits ((src0_i + ~src1_i + 1) &
 * 0xFFFFFFFF); the upper part is only visible through carry.
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the minuends.
 * @param [in] src1 Second source vector data register of int32_t, subtracted element-wise from src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_sub(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts uint32 vector src1 from src0 with borrow-in carry_src element-wise, writing the low 32 bits to dst
 * and the borrow flags to carry.
 *
 * @override_id asc_subc__Rvb_Rvu32_vu32_vu32_vb_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving the borrow output of each element (no-borrow is encoded as a
 * two's-complement carry).
 * @param [out] dst Destination vector data register receiving the low 32 bits of the difference.
 * @param [in] src0 Minuend source vector data register.
 * @param [in] src1 Subtrahend source vector data register.
 * @param [in] carry_src Source mask register supplying the incoming borrow flag of each element.
 * @param [in] mask Mask register selecting which elements participate; unselected output elements are zeroed.
 */
__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Subtracts int32 vector src1 from src0 with borrow-in carry_src element-wise, writing the low 32 bits to dst
 * and the borrow flags to carry.
 *
 * @override_id asc_subc__Rvb_Rvi32_vi32_vi32_vb_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving the borrow output of each element (no-borrow is encoded as a
 * two's-complement carry).
 * @param [out] dst Destination vector data register receiving the low 32 bits of the difference.
 * @param [in] src0 Minuend source vector data register.
 * @param [in] src1 Subtrahend source vector data register.
 * @param [in] carry_src Source mask register supplying the incoming borrow flag of each element.
 * @param [in] mask Mask register selecting which elements participate; unselected output elements are zeroed.
 */
__simd_callee__ inline void asc_subc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int8_t vectors src0 and src1 under mask and returns the resulting
 * vector_int8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline vector_int8_t asc_min(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int16_t vectors src0 and src1 under mask and returns the resulting
 * vector_int16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline vector_int16_t asc_min(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int32_t vectors src0 and src1 under mask and returns the resulting
 * vector_int32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline vector_int32_t asc_min(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint8_t vectors src0 and src1 under mask and returns the resulting
 * vector_uint8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline vector_uint8_t asc_min(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint16_t vectors src0 and src1 under mask and returns the resulting
 * vector_uint16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline vector_uint16_t asc_min(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint32_t vectors src0 and src1 under mask and returns the resulting
 * vector_uint32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline vector_uint32_t asc_min(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the half vectors src0 and src1 under mask and returns the resulting
 * vector_half register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline vector_half asc_min(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the bfloat16_t vectors src0 and src1 under mask and returns the resulting
 * vector_bfloat16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_min(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the float vectors src0 and src1 under mask and returns the resulting
 * vector_float register, with masked-off elements set to zero.
 *
 * @override_id asc_min__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline vector_float asc_min(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int8_t vectors src0 and src1 under mask and writes the resulting
 * vector_int8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int16_t vectors src0 and src1 under mask and writes the resulting
 * vector_int16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int32_t vectors src0 and src1 under mask and writes the resulting
 * vector_int32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint8_t vectors src0 and src1 under mask and writes the resulting
 * vector_uint8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint16_t vectors src0 and src1 under mask and writes the resulting
 * vector_uint16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint32_t vectors src0 and src1 under mask and writes the resulting
 * vector_uint32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline void asc_min(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the half vectors src0 and src1 under mask and writes the resulting
 * vector_half register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline void asc_min(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the bfloat16_t vectors src0 and src1 under mask and writes the resulting
 * vector_bfloat16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline void asc_min(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the float vectors src0 and src1 under mask and writes the resulting
 * vector_float register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise minimum.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline void asc_min(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise negation of the mask-selected int8_t elements of src into dst.
 *
 * @override_id asc_neg__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int8_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int8_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_neg(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise negation of the mask-selected int8_t elements of src and returns the result as a
 * vector_int8_t.
 *
 * @override_id asc_neg__vi8_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int8_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int8_t asc_neg(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise negation of the mask-selected int16_t elements of src into dst.
 *
 * @override_id asc_neg__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int16_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int16_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_neg(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise negation of the mask-selected int16_t elements of src and returns the result as a
 * vector_int16_t.
 *
 * @override_id asc_neg__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int16_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int16_t asc_neg(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise negation of the mask-selected int32_t elements of src into dst.
 *
 * @override_id asc_neg__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int32_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int32_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_neg(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise negation of the mask-selected int32_t elements of src and returns the result as a
 * vector_int32_t.
 *
 * @override_id asc_neg__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int32_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int32_t asc_neg(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise negation of the mask-selected half elements of src into dst.
 *
 * @override_id asc_neg__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the half result elements; mask-cleared lanes are set
 * to 0.
 * @param [in] src Source vector data register holding the half elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_neg(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise negation of the mask-selected half elements of src and returns the result as a
 * vector_half.
 *
 * @override_id asc_neg__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the half elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_half asc_neg(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise negation of the mask-selected float elements of src into dst.
 *
 * @override_id asc_neg__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the float result elements; mask-cleared lanes are set
 * to 0.
 * @param [in] src Source vector data register holding the float elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_neg(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise negation of the mask-selected float elements of src and returns the result as a
 * vector_float.
 *
 * @override_id asc_neg__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the float elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_float asc_neg(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise absolute value of the mask-selected int8_t elements of src into dst.
 *
 * @override_id asc_abs__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int8_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int8_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_abs(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise absolute value of the mask-selected int16_t elements of src into dst.
 *
 * @override_id asc_abs__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int16_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int16_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_abs(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise absolute value of the mask-selected int32_t elements of src into dst.
 *
 * @override_id asc_abs__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the int32_t result elements; mask-cleared lanes are
 * set to 0.
 * @param [in] src Source vector data register holding the int32_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_abs(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise absolute value of the mask-selected half elements of src into dst.
 *
 * @override_id asc_abs__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the half result elements; mask-cleared lanes are set
 * to 0.
 * @param [in] src Source vector data register holding the half elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_abs(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Writes the element-wise absolute value of the mask-selected float elements of src into dst.
 *
 * @override_id asc_abs__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the float result elements; mask-cleared lanes are set
 * to 0.
 * @param [in] src Source vector data register holding the float elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline void asc_abs(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise absolute value of the mask-selected int8_t elements of src and returns the result as
 * a vector_int8_t.
 *
 * @override_id asc_abs__vi8_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int8_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int8_t asc_abs(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise absolute value of the mask-selected int16_t elements of src and returns the result
 * as a vector_int16_t.
 *
 * @override_id asc_abs__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int16_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int16_t asc_abs(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise absolute value of the mask-selected int32_t elements of src and returns the result
 * as a vector_int32_t.
 *
 * @override_id asc_abs__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int32_t elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_int32_t asc_abs(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise absolute value of the mask-selected half elements of src and returns the result as a
 * vector_half.
 *
 * @override_id asc_abs__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the half elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_half asc_abs(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise absolute value of the mask-selected float elements of src and returns the result as
 * a vector_float.
 *
 * @override_id asc_abs__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the float elements whose mask-selected lanes are processed.
 * @param [in] mask Mask register whose set bits select the lanes that participate; unselected lanes yield 0 in the
 * result.
 */
__simd_callee__ inline vector_float asc_abs(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies uint32 vector operands src0 and src1 under mask, writing the low 32 bits of each 64-bit product to
 * dst0 and the high 32 bits to dst1.
 *
 * @override_id asc_mull__Rvu32_Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst0 Destination vector data register receiving the low 32 bits of each product; must use the same data
 * type as src0.
 * @param [out] dst1 Destination vector data register receiving the high 32 bits of each product; must differ from dst0.
 * @param [in] src0 First source vector data register of the multiplication.
 * @param [in] src1 Second source vector data register; must use the same data type as src0.
 * @param [in] mask Mask register selecting participating elements; a 0 bit zeroes both dst0 and dst1 at that position.
 */
__simd_callee__ inline void asc_mull(
    vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies int32 vector operands src0 and src1 under mask, writing the low 32 bits of each 64-bit product to
 * dst0 and the high 32 bits to dst1.
 *
 * @override_id asc_mull__Rvi32_Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst0 Destination vector data register receiving the low 32 bits of each product; must use the same data
 * type as src0.
 * @param [out] dst1 Destination vector data register receiving the high 32 bits of each product; must differ from dst0.
 * @param [in] src0 First source vector data register of the multiplication.
 * @param [in] src1 Second source vector data register; must use the same data type as src0.
 * @param [in] mask Mask register selecting participating elements; a 0 bit zeroes both dst0 and dst1 at that position.
 */
__simd_callee__ inline void asc_mull(
    vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise square root of src under mask control and returns the half result vector.
 *
 * @override_id asc_sqrt__vf16_vb
 * @archs 3510
 * @param [in] src Source vector register of half elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline vector_half asc_sqrt(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise square root of src under mask control and returns the float result vector.
 *
 * @override_id asc_sqrt__vf32_vb
 * @archs 3510
 * @param [in] src Source vector register of float elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline vector_float asc_sqrt(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise square root of src under mask control and writes the half results to dst.
 *
 * @override_id asc_sqrt__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register that receives the half results.
 * @param [in] src Source vector register of half elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline void asc_sqrt(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise square root of src under mask control and writes the float results to dst.
 *
 * @override_id asc_sqrt__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register that receives the float results.
 * @param [in] src Source vector register of float elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline void asc_sqrt(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_int16_t vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_int16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_int16_t asc_mul(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_uint16_t vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_uint16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_uint16_t asc_mul(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_half vector holding the lane-wise product of src0 and src1, with mask-selected lanes computed
 * and the rest zeroed.
 *
 * @override_id asc_mul__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_half register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_half asc_mul(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_bfloat16_t vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_bfloat16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_mul(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_int32_t vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_int32_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_int32_t asc_mul(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_uint32_t vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_uint32_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_uint32_t asc_mul(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns a vector_float vector holding the lane-wise product of src0 and src1, with mask-selected lanes
 * computed and the rest zeroed.
 *
 * @override_id asc_mul__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_float register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; lanes with a 0 bit are not multiplied and come
 * out as zero.
 */
__simd_callee__ inline vector_float asc_mul(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 16-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register receiving the element-wise products; lanes whose mask bit is 0
 * are set to zero.
 * @param [in] src0 First source vector_int16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_int16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 16-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register receiving the element-wise products; lanes whose mask bit is 0
 * are set to zero.
 * @param [in] src0 First source vector_uint16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_uint16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 16-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register receiving the element-wise products; lanes whose mask bit is 0 are
 * set to zero.
 * @param [in] src0 First source vector_half register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_half register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 16-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register receiving the element-wise products; lanes whose mask bit is
 * 0 are set to zero.
 * @param [in] src0 First source vector_bfloat16_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_bfloat16_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 32-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register receiving the element-wise products; lanes whose mask bit is 0
 * are set to zero.
 * @param [in] src0 First source vector_int32_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_int32_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 32-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register receiving the element-wise products; lanes whose mask bit is 0
 * are set to zero.
 * @param [in] src0 First source vector_uint32_t register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_uint32_t register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies 32-bit elements of the two source vector registers lane by lane under mask control and writes the
 * products into dst.
 *
 * @override_id asc_mul__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register receiving the element-wise products; lanes whose mask bit is 0 are
 * set to zero.
 * @param [in] src0 First source vector_float register holding the multiplicand of each lane.
 * @param [in] src1 Second source vector_float register holding the multiplier of each lane.
 * @param [in] mask Mask register selecting the lanes that participate; a 0 bit suppresses the lane and zeroes the
 * corresponding dst element.
 */
__simd_callee__ inline void asc_mul(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the signed 16-bit integer vector src by the int16_t scalar value under mask and
 * writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for signed 16-bit integer vector data; its type
 * must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type int16_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the unsigned 16-bit integer vector src by the uint16_t scalar value under mask and
 * writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvu16_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for unsigned 16-bit integer vector data; its type
 * must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type uint16_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the 16-bit floating point vector (half) src by the half scalar value under mask
 * and writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for 16-bit floating point vector (half) data; its
 * type must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type half applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the signed 32-bit integer vector src by the int32_t scalar value under mask and
 * writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for signed 32-bit integer vector data; its type
 * must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type int32_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the unsigned 32-bit integer vector src by the uint32_t scalar value under mask and
 * writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvu32_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for unsigned 32-bit integer vector data; its type
 * must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type uint32_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the 32-bit floating point vector (float) src by the float scalar value under mask
 * and writes the product into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the products for 32-bit floating point vector (float) data;
 * its type must match src.
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type float applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves that output lane zeroed.
 */
__simd_callee__ inline void asc_mul_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint8_t elements of src0 and src1 and returns the element-wise sums as a vector_uint8_t
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint8_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint8_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint8_t asc_add(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int8_t elements of src0 and src1 and returns the element-wise sums as a vector_int8_t
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int8_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int8_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int8_t asc_add(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint16_t elements of src0 and src1 and returns the element-wise sums as a
 * vector_uint16_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint16_t asc_add(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int16_t elements of src0 and src1 and returns the element-wise sums as a vector_int16_t
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int16_t asc_add(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected half elements of src0 and src1 and returns the element-wise sums as a vector_half
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of half; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of half, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_half asc_add(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected float elements of src0 and src1 and returns the element-wise sums as a vector_float
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of float; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of float, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_float asc_add(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int32_t elements of src0 and src1 and returns the element-wise sums as a vector_int32_t
 * value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_int32_t asc_add(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint32_t elements of src0 and src1 and returns the element-wise sums as a
 * vector_uint32_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_uint32_t asc_add(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected bfloat16_t elements of src0 and src1 and returns the element-wise sums as a
 * vector_bfloat16_t value, with mask-cleared elements zeroed.
 *
 * @override_id asc_add__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector data register of bfloat16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of bfloat16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_add(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the signed 16-bit integer vector src by the int16_t scalar value under mask and
 * returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type int16_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_int16_t asc_mul_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the unsigned 16-bit integer vector src by the uint16_t scalar value under mask and
 * returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type uint16_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_uint16_t asc_mul_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the 16-bit floating point vector (half) src by the half scalar value under mask
 * and returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type half applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_half asc_mul_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the signed 32-bit integer vector src by the int32_t scalar value under mask and
 * returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type int32_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_int32_t asc_mul_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the unsigned 32-bit integer vector src by the uint32_t scalar value under mask and
 * returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type uint32_t applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_uint32_t asc_mul_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Multiplies every element of the 32-bit floating point vector (float) src by the float scalar value under mask
 * and returns the product vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_mul_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector register whose selected lanes are multiplied by value.
 * @param [in] value Scalar multiplier of type float applied to every selected lane of src.
 * @param [in] mask Mask register over the computed lanes: bit 1 multiplies, bit 0 leaves the returned lane at zero.
 */
__simd_callee__ inline vector_float asc_mul_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint8_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint8_t; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint8_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint8_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int8_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int8_t; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int8_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int8_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint16_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint16_t; receives the element-wise sum of src0 and src1 for
 * each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int16_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int16_t; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected half elements of src0 and src1 and stores each sum in dst, writing zero where the mask
 * bit is 0.
 *
 * @override_id asc_add__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of half; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of half; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of half, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected float elements of src0 and src1 and stores each sum in dst, writing zero where the mask
 * bit is 0.
 *
 * @override_id asc_add__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of float; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of float; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of float, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected int32_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of int32_t; receives the element-wise sum of src0 and src1 for each
 * mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected uint32_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of uint32_t; receives the element-wise sum of src0 and src1 for
 * each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the mask-selected bfloat16_t elements of src0 and src1 and stores each sum in dst, writing zero where the
 * mask bit is 0.
 *
 * @override_id asc_add__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of bfloat16_t; receives the element-wise sum of src0 and src1 for
 * each mask-selected element, with mask-cleared elements written as zero.
 * @param [in] src0 First source vector data register of bfloat16_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of bfloat16_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds src0 and src1 as 32-bit unsigned values, writes the low 32 bits of each sum to dst and stores the
 * per-element carry-out in the carry mask.
 *
 * @override_id asc_add__Rvb_Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving one carry bit per element of the 32-bit unsigned addition; bit
 * 32 of the 33-bit result is reported here.
 * @param [out] dst Destination vector data register of uint32_t that receives the low 32 bits ((src0_i + src1_i) &
 * 0xFFFFFFFF); the upper part is only visible through carry.
 * @param [in] src0 First source vector data register of uint32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of uint32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds src0 and src1 as 32-bit unsigned values, writes the low 32 bits of each sum to dst and stores the
 * per-element carry-out in the carry mask.
 *
 * @override_id asc_add__Rvb_Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving one carry bit per element of the 32-bit unsigned addition; bit
 * 32 of the 33-bit result is reported here.
 * @param [out] dst Destination vector data register of int32_t that receives the low 32 bits ((src0_i + src1_i) &
 * 0xFFFFFFFF); the upper part is only visible through carry.
 * @param [in] src0 First source vector data register of int32_t; its mask-selected elements are the addends.
 * @param [in] src1 Second source vector data register of int32_t, added element-wise to src0.
 * @param [in] mask Mask register selecting the elements that participate; a bit of 0 makes the corresponding result
 * element zero.
 */
__simd_callee__ inline void asc_add(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds uint32 vector operands src0, src1 and the incoming carry mask carry_src element-wise, writing the low 32
 * bits to dst and the carry-out flags to carry.
 *
 * @override_id asc_addc__Rvb_Rvu32_vu32_vu32_vb_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving the carry-out flag of each element (written in the lowest bit
 * of every 4-bit group).
 * @param [out] dst Destination vector data register receiving the low 32 bits of the sum.
 * @param [in] src0 First source vector data register of the addition.
 * @param [in] src1 Second source vector data register of the addition.
 * @param [in] carry_src Source mask register supplying the incoming carry flag of each element.
 * @param [in] mask Mask register selecting which elements participate; unselected output elements are zeroed.
 */
__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds int32 vector operands src0, src1 and the incoming carry mask carry_src element-wise, writing the low 32
 * bits to dst and the carry-out flags to carry.
 *
 * @override_id asc_addc__Rvb_Rvi32_vi32_vi32_vb_vb
 * @archs 3510
 * @param [out] carry Destination mask register receiving the carry-out flag of each element (written in the lowest bit
 * of every 4-bit group).
 * @param [out] dst Destination vector data register receiving the low 32 bits of the sum.
 * @param [in] src0 First source vector data register of the addition.
 * @param [in] src1 Second source vector data register of the addition.
 * @param [in] carry_src Source mask register supplying the incoming carry flag of each element.
 * @param [in] mask Mask register selecting which elements participate; unselected output elements are zeroed.
 */
__simd_callee__ inline void asc_addc(
    vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 256 uint8_t elements of src0 and src1 that mask selects as a
 * vector_uint8_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint8_t register with 256 uint8_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint8_t register with 256 uint8_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_uint8_t asc_max(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 256 int8_t elements of src0 and src1 that mask selects as a
 * vector_int8_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector_int8_t register with 256 int8_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int8_t register with 256 int8_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_int8_t asc_max(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 128 uint16_t elements of src0 and src1 that mask selects as a
 * vector_uint16_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register with 128 uint16_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint16_t register with 128 uint16_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_uint16_t asc_max(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 128 int16_t elements of src0 and src1 that mask selects as a
 * vector_int16_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register with 128 int16_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int16_t register with 128 int16_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_int16_t asc_max(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 64 int32_t elements of src0 and src1 that mask selects as a
 * vector_int32_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register with 64 int32_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int32_t register with 64 int32_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_int32_t asc_max(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 64 uint32_t elements of src0 and src1 that mask selects as a
 * vector_uint32_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register with 64 uint32_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint32_t register with 64 uint32_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_uint32_t asc_max(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 64 float elements of src0 and src1 that mask selects as a
 * vector_float, with masked-off elements set to 0.
 *
 * @override_id asc_max__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register with 64 float elements, read as the left operand of the pairwise
 * maximum.
 * @param [in] src1 Second source vector_float register with 64 float elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_float asc_max(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 128 half elements of src0 and src1 that mask selects as a vector_half,
 * with masked-off elements set to 0.
 *
 * @override_id asc_max__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register with 128 half elements, read as the left operand of the pairwise
 * maximum.
 * @param [in] src1 Second source vector_half register with 128 half elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_half asc_max(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Returns the element-wise maximum of the 128 bfloat16_t elements of src0 and src1 that mask selects as a
 * vector_bfloat16_t, with masked-off elements set to 0.
 *
 * @override_id asc_max__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register with 128 bfloat16_t elements, read as the left operand of
 * the pairwise maximum.
 * @param [in] src1 Second source vector_bfloat16_t register with 128 bfloat16_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline vector_bfloat16_t asc_max(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 256 uint8_t elements of src0 and src1 that mask selects and writes
 * the vector_uint8_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register receiving the element-wise maxima; all 256 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_uint8_t register with 256 uint8_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint8_t register with 256 uint8_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 256 int8_t elements of src0 and src1 that mask selects and writes the
 * vector_int8_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register receiving the element-wise maxima; all 256 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_int8_t register with 256 int8_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int8_t register with 256 int8_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 128 uint16_t elements of src0 and src1 that mask selects and writes
 * the vector_uint16_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register receiving the element-wise maxima; all 128 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_uint16_t register with 128 uint16_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint16_t register with 128 uint16_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 128 int16_t elements of src0 and src1 that mask selects and writes
 * the vector_int16_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register receiving the element-wise maxima; all 128 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_int16_t register with 128 int16_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int16_t register with 128 int16_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 64 int32_t elements of src0 and src1 that mask selects and writes the
 * vector_int32_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register receiving the element-wise maxima; all 64 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_int32_t register with 64 int32_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_int32_t register with 64 int32_t elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 64 uint32_t elements of src0 and src1 that mask selects and writes
 * the vector_uint32_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register receiving the element-wise maxima; all 64 elements are written
 * and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_uint32_t register with 64 uint32_t elements, read as the left operand of the
 * pairwise maximum.
 * @param [in] src1 Second source vector_uint32_t register with 64 uint32_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 64 float elements of src0 and src1 that mask selects and writes the
 * vector_float result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register receiving the element-wise maxima; all 64 elements are written and
 * elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_float register with 64 float elements, read as the left operand of the pairwise
 * maximum.
 * @param [in] src1 Second source vector_float register with 64 float elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 128 half elements of src0 and src1 that mask selects and writes the
 * vector_half result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register receiving the element-wise maxima; all 128 elements are written and
 * elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_half register with 128 half elements, read as the left operand of the pairwise
 * maximum.
 * @param [in] src1 Second source vector_half register with 128 half elements of the same dtype, read as the right
 * operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise maximum of the 128 bfloat16_t elements of src0 and src1 that mask selects and writes
 * the vector_bfloat16_t result into dst, zeroing masked-off elements.
 *
 * @override_id asc_max__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register receiving the element-wise maxima; all 128 elements are
 * written and elements whose mask bit is 0 come out as 0.
 * @param [in] src0 First source vector_bfloat16_t register with 128 bfloat16_t elements, read as the left operand of
 * the pairwise maximum.
 * @param [in] src1 Second source vector_bfloat16_t register with 128 bfloat16_t elements of the same dtype, read as the
 * right operand of the pairwise maximum.
 * @param [in] mask vector_bool mask register selecting the compared elements: a mask bit of 1 lets the element
 * participate while a bit of 0 leaves the corresponding output element at 0.
 */
__simd_callee__ inline void asc_max(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int32_t vector src and the int32_t scalar value under mask and writes
 * the resulting vector_int32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_int32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int32_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint32_t vector src and the uint32_t scalar value under mask and
 * writes the resulting vector_uint32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvu32_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_uint32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint32_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int16_t vector src and the int16_t scalar value under mask and writes
 * the resulting vector_int16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_int16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint16_t vector src and the uint16_t scalar value under mask and
 * writes the resulting vector_uint16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvu16_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_uint16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int8_t vector src and the int8_t scalar value under mask and writes
 * the resulting vector_int8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvi8_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_int8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int8_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint8_t vector src and the uint8_t scalar value under mask and writes
 * the resulting vector_uint8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvu8_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_uint8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint8_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the float vector src and the float scalar value under mask and writes the
 * resulting vector_float register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_float register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar float value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the half vector src and the half scalar value under mask and writes the
 * resulting vector_half register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_half register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar half value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline void asc_min_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the bfloat16_t vector src and the bfloat16_t scalar value under mask and
 * writes the resulting vector_bfloat16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__Rvbf16_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the elementwise minimum result; written by the
 * function.
 * @param [in] src Source vector_bfloat16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar bfloat16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline void asc_min_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise natural exponential of src under mask control and returns the half result vector.
 *
 * @override_id asc_exp__vf16_vb
 * @archs 3510
 * @param [in] src Source vector register of half elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline vector_half asc_exp(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise natural exponential of src under mask control and returns the float result vector.
 *
 * @override_id asc_exp__vf32_vb
 * @archs 3510
 * @param [in] src Source vector register of float elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline vector_float asc_exp(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int32_t vector src and the int32_t scalar value under mask and returns
 * the resulting vector_int32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int32_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline vector_int32_t asc_min_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint32_t vector src and the uint32_t scalar value under mask and
 * returns the resulting vector_uint32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint32_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline vector_uint32_t asc_min_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int16_t vector src and the int16_t scalar value under mask and returns
 * the resulting vector_int16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline vector_int16_t asc_min_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint16_t vector src and the uint16_t scalar value under mask and
 * returns the resulting vector_uint16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline vector_uint16_t asc_min_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the int8_t vector src and the int8_t scalar value under mask and returns
 * the resulting vector_int8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int8_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline vector_int8_t asc_min_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the uint8_t vector src and the uint8_t scalar value under mask and returns
 * the resulting vector_uint8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint8_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline vector_uint8_t asc_min_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the float vector src and the float scalar value under mask and returns the
 * resulting vector_float register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector_float register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar float value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline vector_float asc_min_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the half vector src and the half scalar value under mask and returns the
 * resulting vector_half register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector_half register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar half value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline vector_half asc_min_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise minimum of the bfloat16_t vector src and the bfloat16_t scalar value under mask and
 * returns the resulting vector_bfloat16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_min_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar bfloat16_t value applied to every participating element of src for the elementwise minimum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_min_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise natural exponential of src under mask control and writes the half results to dst.
 *
 * @override_id asc_exp__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register that receives the half results.
 * @param [in] src Source vector register of half elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline void asc_exp(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise natural exponential of src under mask control and writes the float results to dst.
 *
 * @override_id asc_exp__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register that receives the float results.
 * @param [in] src Source vector register of float elements.
 * @param [in] mask Element mask; a bit of 1 lets that element participate in the computation and a bit of 0 zeroes the
 * corresponding output element.
 */
__simd_callee__ inline void asc_exp(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 256 int8_t elements of src that mask selects and writes the vector_int8_t
 * sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvi8_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register receiving the element-wise sums; all 256 elements are written and
 * mask-filtered elements come out as 0.
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int8_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^8 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 256 uint8_t elements of src that mask selects and writes the
 * vector_uint8_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvu8_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register receiving the element-wise sums; all 256 elements are written
 * and mask-filtered elements come out as 0.
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar uint8_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^8 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 int16_t elements of src that mask selects and writes the
 * vector_int16_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register receiving the element-wise sums; all 128 elements are written
 * and mask-filtered elements come out as 0.
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int16_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^16 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 uint16_t elements of src that mask selects and writes the
 * vector_uint16_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvu16_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register receiving the element-wise sums; all 128 elements are written
 * and mask-filtered elements come out as 0.
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; each element is read once and added to
 * the scalar.
 * @param [in] value Scalar uint16_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^16 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 half elements of src that mask selects and writes the vector_half
 * sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register receiving the element-wise sums; all 128 elements are written and
 * mask-filtered elements come out as 0.
 * @param [in] src Source vector_half register with 128 half elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar half addend broadcast to every selected element; its dtype must match src (result follows
 * half-precision arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 bfloat16_t elements of src that mask selects and writes the
 * vector_bfloat16_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvbf16_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register receiving the element-wise sums; all 128 elements are written
 * and mask-filtered elements come out as 0.
 * @param [in] src Source vector_bfloat16_t register with 128 bfloat16_t elements; each element is read once and added
 * to the scalar.
 * @param [in] value Scalar bfloat16_t addend broadcast to every selected element; its dtype must match src (result
 * follows bfloat16 arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 int32_t elements of src that mask selects and writes the
 * vector_int32_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register receiving the element-wise sums; all 64 elements are written and
 * mask-filtered elements come out as 0.
 * @param [in] src Source vector_int32_t register with 64 int32_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int32_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^32 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 uint32_t elements of src that mask selects and writes the
 * vector_uint32_t sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvu32_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register receiving the element-wise sums; all 64 elements are written
 * and mask-filtered elements come out as 0.
 * @param [in] src Source vector_uint32_t register with 64 uint32_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar uint32_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^32 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 float elements of src that mask selects and writes the vector_float
 * sums into dst, zeroing destination elements whose mask bit is 0.
 *
 * @override_id asc_add_scalar__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register receiving the element-wise sums; all 64 elements are written and
 * mask-filtered elements come out as 0.
 * @param [in] src Source vector_float register with 64 float elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar float addend broadcast to every selected element; its dtype must match src (result follows
 * single-precision arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline void asc_add_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 256 int8_t elements of src that mask selects and returns the
 * vector_int8_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register with 256 int8_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int8_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^8 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_int8_t asc_add_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 256 uint8_t elements of src that mask selects and returns the
 * vector_uint8_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register with 256 uint8_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar uint8_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^8 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_uint8_t asc_add_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 int16_t elements of src that mask selects and returns the
 * vector_int16_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register with 128 int16_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int16_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^16 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_int16_t asc_add_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 uint16_t elements of src that mask selects and returns the
 * vector_uint16_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register with 128 uint16_t elements; each element is read once and added to
 * the scalar.
 * @param [in] value Scalar uint16_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^16 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_uint16_t asc_add_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 half elements of src that mask selects and returns the vector_half
 * result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector_half register with 128 half elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar half addend broadcast to every selected element; its dtype must match src (result follows
 * half-precision arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_half asc_add_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 128 bfloat16_t elements of src that mask selects and returns the
 * vector_bfloat16_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register with 128 bfloat16_t elements; each element is read once and added
 * to the scalar.
 * @param [in] value Scalar bfloat16_t addend broadcast to every selected element; its dtype must match src (result
 * follows bfloat16 arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_bfloat16_t asc_add_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 int32_t elements of src that mask selects and returns the
 * vector_int32_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register with 64 int32_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar int32_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^32 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_int32_t asc_add_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 uint32_t elements of src that mask selects and returns the
 * vector_uint32_t result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register with 64 uint32_t elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar uint32_t addend broadcast to every selected element; its dtype must match src (result wraps
 * modulo 2^32 on overflow).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_uint32_t asc_add_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Adds the scalar value to each of the 64 float elements of src that mask selects and returns the vector_float
 * result, with elements whose mask bit is 0 set to 0.
 *
 * @override_id asc_add_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector_float register with 64 float elements; each element is read once and added to the
 * scalar.
 * @param [in] value Scalar float addend broadcast to every selected element; its dtype must match src (result follows
 * single-precision arithmetic).
 * @param [in] mask vector_bool mask register gating the lanes: an element is computed only when its mask bit is 1,
 * otherwise the corresponding result element is set to 0.
 */
__simd_callee__ inline vector_float asc_add_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise natural logarithm of half vector src under mask and writes ln(src) into dst.
 *
 * @override_id asc_ln__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving ln(src); positions with a 0 mask bit are set to 0.
 * @param [in] src Source vector data register of half elements whose natural logarithm is computed.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit excludes the element from the
 * calculation.
 */
__simd_callee__ inline void asc_ln(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise natural logarithm of float vector src under mask and writes ln(src) into dst.
 *
 * @override_id asc_ln__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving ln(src); positions with a 0 mask bit are set to 0.
 * @param [in] src Source vector data register of float elements whose natural logarithm is computed.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit excludes the element from the
 * calculation.
 */
__simd_callee__ inline void asc_ln(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for 16-bit floating point vector (half) operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_half asc_div(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for 32-bit floating point vector (float) operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_float asc_div(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for signed 16-bit integer vector operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_int16_t asc_div(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for unsigned 16-bit integer vector operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_uint16_t asc_div(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for signed 32-bit integer vector operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_int32_t asc_div(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for unsigned 32-bit integer vector operands under mask and returns the
 * quotient vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline vector_uint32_t asc_div(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise natural logarithm of half vector src under mask and writes the result into dst.
 *
 * @override_id asc_log__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the logarithm; positions with a 0 mask bit are set to 0.
 * @param [in] src Source vector data register of half elements whose natural logarithm is computed.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit excludes the element from the
 * calculation.
 */
__simd_callee__ inline void asc_log(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the element-wise natural logarithm of float vector src under mask and writes the result into dst.
 *
 * @override_id asc_log__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the logarithm; positions with a 0 mask bit are set to 0.
 * @param [in] src Source vector data register of float elements whose natural logarithm is computed.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit excludes the element from the
 * calculation.
 */
__simd_callee__ inline void asc_log(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for 16-bit floating point vector (half) operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for 16-bit floating point vector
 * (half) data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for 32-bit floating point vector (float) operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for 32-bit floating point vector
 * (float) data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for signed 16-bit integer vector operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for signed 16-bit integer vector
 * data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for unsigned 16-bit integer vector operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for unsigned 16-bit integer vector
 * data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for signed 32-bit integer vector operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for signed 32-bit integer vector
 * data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Divides src0 element-wise by src1 for unsigned 32-bit integer vector operands under mask and writes each
 * quotient into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_div__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the per-element quotient for unsigned 32-bit integer vector
 * data; its type must match src0.
 * @param [in] src0 Dividend vector register; each selected element is divided by the corresponding element of src1.
 * @param [in] src1 Divisor vector register; a zero element produces the boundary result documented for Div.
 * @param [in] mask Mask register selecting which lanes are computed: bit 1 computes the quotient, bit 0 leaves that
 * output lane zeroed.
 */
__simd_callee__ inline void asc_div(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int8_t vector src and the int8_t scalar value under mask and writes
 * the resulting vector_int8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvi8_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_int8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int8_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int16_t vector src and the int16_t scalar value under mask and writes
 * the resulting vector_int16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_int16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int32_t vector src and the int32_t scalar value under mask and writes
 * the resulting vector_int32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_int32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int32_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint8_t vector src and the uint8_t scalar value under mask and writes
 * the resulting vector_uint8_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvu8_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_uint8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint8_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint16_t vector src and the uint16_t scalar value under mask and
 * writes the resulting vector_uint16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvu16_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_uint16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint32_t vector src and the uint32_t scalar value under mask and
 * writes the resulting vector_uint32_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvu32_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_uint32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint32_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the half vector src and the half scalar value under mask and writes the
 * resulting vector_half register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_half register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar half value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_half& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the float vector src and the float scalar value under mask and writes the
 * resulting vector_float register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_float register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar float value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline void asc_max_scalar(vector_float& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the bfloat16_t vector src and the bfloat16_t scalar value under mask and
 * writes the resulting vector_bfloat16_t register to dst, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__Rvbf16_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the elementwise maximum result; written by the
 * function.
 * @param [in] src Source vector_bfloat16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar bfloat16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline void asc_max_scalar(
    vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int8_t vector src and the int8_t scalar value under mask and returns
 * the resulting vector_int8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int8_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int8_t result element zero.
 */
__simd_callee__ inline vector_int8_t asc_max_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int16_t vector src and the int16_t scalar value under mask and returns
 * the resulting vector_int16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int16_t result element zero.
 */
__simd_callee__ inline vector_int16_t asc_max_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the int32_t vector src and the int32_t scalar value under mask and returns
 * the resulting vector_int32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar int32_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_int32_t result element zero.
 */
__simd_callee__ inline vector_int32_t asc_max_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint8_t vector src and the uint8_t scalar value under mask and returns
 * the resulting vector_uint8_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint8_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint8_t result element zero.
 */
__simd_callee__ inline vector_uint8_t asc_max_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint16_t vector src and the uint16_t scalar value under mask and
 * returns the resulting vector_uint16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint16_t result element zero.
 */
__simd_callee__ inline vector_uint16_t asc_max_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the uint32_t vector src and the uint32_t scalar value under mask and
 * returns the resulting vector_uint32_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar uint32_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_uint32_t result element zero.
 */
__simd_callee__ inline vector_uint32_t asc_max_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the half vector src and the half scalar value under mask and returns the
 * resulting vector_half register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector_half register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar half value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_half result element zero.
 */
__simd_callee__ inline vector_half asc_max_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the float vector src and the float scalar value under mask and returns the
 * resulting vector_float register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector_float register holding the elements that are combined with the scalar value under mask.
 * @param [in] value Scalar float value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_float result element zero.
 */
__simd_callee__ inline vector_float asc_max_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_arith
 * @brief Computes the elementwise maximum of the bfloat16_t vector src and the bfloat16_t scalar value under mask and
 * returns the resulting vector_bfloat16_t register, with masked-off elements set to zero.
 *
 * @override_id asc_max_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register holding the elements that are combined with the scalar value under
 * mask.
 * @param [in] value Scalar bfloat16_t value applied to every participating element of src for the elementwise maximum.
 * @param [in] mask Mask register selecting the participating elements: a set bit computes the element and a cleared bit
 * makes the corresponding vector_bfloat16_t result element zero.
 */
__simd_callee__ inline vector_bfloat16_t asc_max_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_max_merge(
    vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(
    vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(
    vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_max_merge(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_add_merge(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

__simd_callee__ inline void asc_min_merge(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);
#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ARITH_H
#endif
