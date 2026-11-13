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
#error "compute/reg_logic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
#include "impl/c_api/reg_base_impl/reg_logic_intf_impl.h"
#endif
#include "c_api/composite/reg_logic_composite.h"

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two mask registers src0 and src1 under mask and writes the result into dst.
 *
 * @override_id asc_and__Rvb_vb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register passed by reference; receives the bitwise AND result.
 * @param [in] src0 First source mask register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline void asc_and(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two mask registers src0 and src1 under mask and returns the result as a
 * vector_bool.
 *
 * @override_id asc_and__vb_vb_vb
 * @archs 3510
 * @param [in] src0 First source mask register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline vector_bool asc_and(vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_and__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination uint8 vector data register passed by reference; receives the bitwise AND result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint8 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint8_t.
 *
 * @override_id asc_and__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint8_t asc_and(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_and__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination int8 vector data register passed by reference; receives the bitwise AND result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int8 vector data registers src0 and src1 under mask and returns the result
 * as a vector_int8_t.
 *
 * @override_id asc_and__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int8_t asc_and(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint16 vector data registers src0 and src1 under mask and writes the
 * result into dst.
 *
 * @override_id asc_and__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination uint16 vector data register passed by reference; receives the bitwise AND result and
 * must use the same dtype as src0 and src1.
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint16 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint16_t.
 *
 * @override_id asc_and__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint16_t asc_and(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int16 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_and__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination int16 vector data register passed by reference; receives the bitwise AND result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int16 vector data registers src0 and src1 under mask and returns the
 * result as a vector_int16_t.
 *
 * @override_id asc_and__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int16_t asc_and(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint32 vector data registers src0 and src1 under mask and writes the
 * result into dst.
 *
 * @override_id asc_and__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination uint32 vector data register passed by reference; receives the bitwise AND result and
 * must use the same dtype as src0 and src1.
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two uint32 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint32_t.
 *
 * @override_id asc_and__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint32_t asc_and(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int32 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_and__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination int32 vector data register passed by reference; receives the bitwise AND result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_and(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise AND of the two int32 vector data registers src0 and src1 under mask and returns the
 * result as a vector_int32_t.
 *
 * @override_id asc_and__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise AND.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise AND.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int32_t asc_and(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination int8 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int8 vector data registers src0 and src1 under mask and returns the result
 * as a vector_int8_t.
 *
 * @override_id asc_or__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int8_t asc_or(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination uint8 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint8 vector data registers src0 and src1 under mask and returns the result
 * as a vector_uint8_t.
 *
 * @override_id asc_or__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint8_t asc_or(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int16 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination int16 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int16 vector data registers src0 and src1 under mask and returns the result
 * as a vector_int16_t.
 *
 * @override_id asc_or__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int16_t asc_or(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint16 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination uint16 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint16 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint16_t.
 *
 * @override_id asc_or__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint16_t asc_or(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int32 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination int32 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two int32 vector data registers src0 and src1 under mask and returns the result
 * as a vector_int32_t.
 *
 * @override_id asc_or__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int32_t asc_or(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint32 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_or__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination uint32 vector data register passed by reference; receives the bitwise OR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_or(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two uint32 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint32_t.
 *
 * @override_id asc_or__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint32_t asc_or(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two mask registers src0 and src1 under mask and writes the result into dst.
 *
 * @override_id asc_or__Rvb_vb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register passed by reference; receives the bitwise OR result.
 * @param [in] src0 First source mask register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline void asc_or(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise OR of the two mask registers src0 and src1 under mask and returns the result as a
 * vector_bool.
 *
 * @override_id asc_or__vb_vb_vb
 * @archs 3510
 * @param [in] src0 First source mask register, read as the left operand of the bitwise OR.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise OR.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline vector_bool asc_or(vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 8-bit integer element of src under mask and writes the complemented
 * elements into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t written with the bitwise complement of each unsigned 8-bit integer
 * element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_uint8_t input holding 256 unsigned 8-bit integer elements to be inverted bitwise at
 * 8-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 8-bit integer element of src under mask and returns the complemented
 * vector_uint8_t.
 *
 * @override_id asc_not__vu8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t input holding 256 unsigned 8-bit integer elements to be inverted bitwise at
 * 8-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_uint8_t asc_not(vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 8-bit integer element of src under mask and writes the complemented elements
 * into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t written with the bitwise complement of each signed 8-bit integer element;
 * masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_int8_t input holding 256 signed 8-bit integer elements to be inverted bitwise at 8-bit
 * width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 8-bit integer element of src under mask and returns the complemented
 * vector_int8_t.
 *
 * @override_id asc_not__vi8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t input holding 256 signed 8-bit integer elements to be inverted bitwise at 8-bit
 * width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_int8_t asc_not(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 16-bit integer element of src under mask and writes the complemented
 * elements into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t written with the bitwise complement of each unsigned 16-bit integer
 * element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_uint16_t input holding 128 unsigned 16-bit integer elements to be inverted bitwise at
 * 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 16-bit integer element of src under mask and returns the complemented
 * vector_uint16_t.
 *
 * @override_id asc_not__vu16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t input holding 128 unsigned 16-bit integer elements to be inverted bitwise at
 * 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_uint16_t asc_not(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 16-bit integer element of src under mask and writes the complemented elements
 * into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t written with the bitwise complement of each signed 16-bit integer
 * element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_int16_t input holding 128 signed 16-bit integer elements to be inverted bitwise at
 * 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 16-bit integer element of src under mask and returns the complemented
 * vector_int16_t.
 *
 * @override_id asc_not__vi16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t input holding 128 signed 16-bit integer elements to be inverted bitwise at
 * 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_int16_t asc_not(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each half-precision float element of src under mask and writes the complemented elements
 * into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half written with the bitwise complement of each IEEE 754 binary16 half-precision
 * floating-point element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_half input holding 128 IEEE 754 binary16 half-precision floating-point elements to be
 * inverted bitwise at 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each half-precision float element of src under mask and returns the complemented
 * vector_half.
 *
 * @override_id asc_not__vf16_vb
 * @archs 3510
 * @param [in] src Source vector_half input holding 128 IEEE 754 binary16 half-precision floating-point elements to be
 * inverted bitwise at 16-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_half asc_not(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 32-bit integer element of src under mask and writes the complemented
 * elements into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t written with the bitwise complement of each unsigned 32-bit integer
 * element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_uint32_t input holding 64 unsigned 32-bit integer elements to be inverted bitwise at
 * 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each unsigned 32-bit integer element of src under mask and returns the complemented
 * vector_uint32_t.
 *
 * @override_id asc_not__vu32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t input holding 64 unsigned 32-bit integer elements to be inverted bitwise at
 * 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_uint32_t asc_not(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 32-bit integer element of src under mask and writes the complemented elements
 * into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t written with the bitwise complement of each signed 32-bit integer
 * element; masked-off elements are written as 0 through the zeroing mode used in the implementation.
 * @param [in] src Source vector_int32_t input holding 64 signed 32-bit integer elements to be inverted bitwise at
 * 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each signed 32-bit integer element of src under mask and returns the complemented
 * vector_int32_t.
 *
 * @override_id asc_not__vi32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t input holding 64 signed 32-bit integer elements to be inverted bitwise at
 * 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_int32_t asc_not(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each single-precision float element of src under mask and writes the complemented
 * elements into dst, zeroing masked-off elements.
 *
 * @override_id asc_not__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float written with the bitwise complement of each IEEE 754 binary32
 * single-precision floating-point element; masked-off elements are written as 0 through the zeroing mode used in the
 * implementation.
 * @param [in] src Source vector_float input holding 64 IEEE 754 binary32 single-precision floating-point elements to be
 * inverted bitwise at 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_not(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of each single-precision float element of src under mask and returns the complemented
 * vector_float.
 *
 * @override_id asc_not__vf32_vb
 * @archs 3510
 * @param [in] src Source vector_float input holding 64 IEEE 754 binary32 single-precision floating-point elements to be
 * inverted bitwise at 32-bit width; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_float asc_not(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of the mask-register source under mask and writes the complemented bits into dst, zeroing
 * bits whose mask bit is 0.
 *
 * @override_id asc_not__Rvb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the complemented bits of src; result bits whose mask bit is 0
 * are written as 0.
 * @param [in] src Source mask register (vector_bool, 32 bytes / 256 bits) whose bits are inverted; read only.
 * @param [in] mask Mask register indicating which bits of the mask-register source are valid: a mask bit of 1 lets the
 * source bit be inverted and a mask bit of 0 forces the result bit to 0.
 */
__simd_callee__ inline void asc_not(vector_bool& dst, vector_bool src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Inverts every bit of the mask-register source under mask and returns the complemented mask register.
 *
 * @override_id asc_not__vb_vb
 * @archs 3510
 * @param [in] src Source mask register (vector_bool, 32 bytes / 256 bits) whose bits are inverted; read only.
 * @param [in] mask Mask register indicating which bits of the mask-register source are valid: a mask bit of 1 lets the
 * source bit be inverted and a mask bit of 0 forces the result bit to 0.
 */
__simd_callee__ inline vector_bool asc_not(vector_bool src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int32 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_xor__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination int32 vector data register passed by reference; receives the bitwise XOR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint32 vector data registers src0 and src1 under mask and writes the
 * result into dst.
 *
 * @override_id asc_xor__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination uint32 vector data register passed by reference; receives the bitwise XOR result and
 * must use the same dtype as src0 and src1.
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint16 vector data registers src0 and src1 under mask and writes the
 * result into dst.
 *
 * @override_id asc_xor__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination uint16 vector data register passed by reference; receives the bitwise XOR result and
 * must use the same dtype as src0 and src1.
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int16 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_xor__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination int16 vector data register passed by reference; receives the bitwise XOR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_xor__Rvu8_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination uint8 vector data register passed by reference; receives the bitwise XOR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int8 vector data registers src0 and src1 under mask and writes the result
 * into dst.
 *
 * @override_id asc_xor__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination int8 vector data register passed by reference; receives the bitwise XOR result and must
 * use the same dtype as src0 and src1.
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two mask registers src0 and src1 under mask and writes the result into dst.
 *
 * @override_id asc_xor__Rvb_vb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register passed by reference; receives the bitwise XOR result.
 * @param [in] src0 First source mask register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int32 vector data registers src0 and src1 under mask and returns the
 * result as a vector_int32_t.
 *
 * @override_id asc_xor__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source int32 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int32 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int32_t asc_xor(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint32 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint32_t.
 *
 * @override_id asc_xor__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source uint32 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint32 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint32_t asc_xor(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint16 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint16_t.
 *
 * @override_id asc_xor__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source uint16 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint16 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint16_t asc_xor(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int16 vector data registers src0 and src1 under mask and returns the
 * result as a vector_int16_t.
 *
 * @override_id asc_xor__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source int16 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int16 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int16_t asc_xor(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two uint8 vector data registers src0 and src1 under mask and returns the
 * result as a vector_uint8_t.
 *
 * @override_id asc_xor__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source uint8 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source uint8 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_uint8_t asc_xor(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two int8 vector data registers src0 and src1 under mask and returns the result
 * as a vector_int8_t.
 *
 * @override_id asc_xor__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source int8 vector data register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source int8 vector data register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register whose 1 bits select the elements that take part in the operation; elements not
 * selected are written as zero.
 */
__simd_callee__ inline vector_int8_t asc_xor(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Computes the bitwise XOR of the two mask registers src0 and src1 under mask and returns the result as a
 * vector_bool.
 *
 * @override_id asc_xor__vb_vb_vb
 * @archs 3510
 * @param [in] src0 First source mask register, read as the left operand of the bitwise XOR.
 * @param [in] src1 Second source mask register, read as the right operand of the bitwise XOR.
 * @param [in] mask Mask register that indicates which bits of src0 and src1 are valid for the operation.
 */
__simd_callee__ inline vector_bool asc_xor(vector_bool src0, vector_bool src1, vector_bool mask);

#if !defined(__NPU_ARCH__) || (__NPU_ARCH__ != 9201)
//=================asc_shiftleft (signed shift overloads, 3510)==================
/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src left by the corresponding signed 8-bit integer lane
 * of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvu8_vu8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src left by the corresponding signed 8-bit integer lane
 * of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src left by the corresponding signed 16-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvu16_vu16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src left by the corresponding signed 16-bit integer lane
 * of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src left by the corresponding signed 32-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvu32_vu32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src left by the corresponding signed 32-bit integer lane
 * of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src left by the corresponding signed 8-bit integer lane
 * of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vu8_vi8_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint8_t asc_shiftleft(vector_uint8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src left by the corresponding signed 8-bit integer lane
 * of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vi8_vi8_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int8_t asc_shiftleft(vector_int8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src left by the corresponding signed 16-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vu16_vi16_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint16_t asc_shiftleft(vector_uint16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src left by the corresponding signed 16-bit integer lane
 * of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vi16_vi16_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int16_t asc_shiftleft(vector_int16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src left by the corresponding signed 32-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vu32_vi32_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint32_t asc_shiftleft(vector_uint32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src left by the corresponding signed 32-bit integer lane
 * of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft__vi32_vi32_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted left.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int32_t asc_shiftleft(vector_int32_t src, vector_int32_t shift, vector_bool mask);

//=================asc_shiftright (signed shift overloads, 3510)==================
/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src right by the corresponding signed 8-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvu8_vu8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_uint8_t& dst, vector_uint8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src right by the corresponding signed 8-bit integer lane
 * of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvi8_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_int8_t& dst, vector_int8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src right by the corresponding signed 16-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvu16_vu16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_uint16_t& dst, vector_uint16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src right by the corresponding signed 16-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_int16_t& dst, vector_int16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src right by the corresponding signed 32-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvu32_vu32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_uint32_t& dst, vector_uint32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src right by the corresponding signed 32-bit integer
 * lane of shift under mask and writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftright(
    vector_int32_t& dst, vector_int32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src right by the corresponding signed 8-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vu8_vi8_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint8_t asc_shiftright(vector_uint8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src right by the corresponding signed 8-bit integer lane
 * of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vi8_vi8_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int8_t, whose lane width must match src (8 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int8_t asc_shiftright(vector_int8_t src, vector_int8_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src right by the corresponding signed 16-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vu16_vi16_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint16_t asc_shiftright(vector_uint16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src right by the corresponding signed 16-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vi16_vi16_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int16_t, whose lane width must match src (16 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int16_t asc_shiftright(vector_int16_t src, vector_int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src right by the corresponding signed 32-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vu32_vi32_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint32_t asc_shiftright(vector_uint32_t src, vector_int32_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src right by the corresponding signed 32-bit integer
 * lane of shift under mask and returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftright__vi32_vi32_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted right.
 * @param [in] shift Per-lane shift-amount vector register of type int32_t, whose lane width must match src (32 bits);
 * negative amounts are not supported and give undefined results.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int32_t asc_shiftright(vector_int32_t src, vector_int32_t shift, vector_bool mask);

//=================asc_shiftleft_scalar (signed shift overloads, 3510)==================
/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvu8_vu8_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvi8_vi8_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 8-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvu16_vu16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 16-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvu32_vu32_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted unsigned 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src left by the int16_t scalar shift under mask and
 * writes the result into dst, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__Rvi32_vi32_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the shifted signed 32-bit integer values; its data type must
 * match src.
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 writes 0 to that output lane.
 */
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 8-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vu8_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 8-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint8_t asc_shiftleft_scalar(vector_uint8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 8-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vi8_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 8-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int8_t asc_shiftleft_scalar(vector_int8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 16-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vu16_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 16-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint16_t asc_shiftleft_scalar(vector_uint16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 16-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 16-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int16_t asc_shiftleft_scalar(vector_int16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected unsigned 32-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vu32_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of unsigned 32-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_uint32_t asc_shiftleft_scalar(vector_uint32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Shifts each selected signed 32-bit integer element of src left by the int16_t scalar shift under mask and
 * returns the shifted vector, with mask-cleared lanes set to zero.
 *
 * @override_id asc_shiftleft_scalar__vi32_i16_vb
 * @archs 3510
 * @param [in] src Source vector register of signed 32-bit integer elements to be shifted left.
 * @param [in] shift Scalar left-shift amount of type int16_t applied to every selected element; negative values are not
 * supported.
 * @param [in] mask Mask register selecting the lanes to shift: bit 1 shifts src, bit 0 leaves that returned lane at 0.
 */
__simd_callee__ inline vector_int32_t asc_shiftleft_scalar(vector_int32_t src, int16_t shift, vector_bool mask);

//=================asc_shiftright_scalar (signed shift overloads, 3510)==================
/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the logical right shift by a scalar bit count of the masked uint8_t elements of src into dst, zeroing
 * unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvu8_vu8_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the logically shifted uint8_t elements; type must
 * match src.
 * @param [in] src Source vector data register of unsigned 8-bit elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; zero fill from the high end, non-negative.
 * @param [in] mask Mask register indicating the elements to shift; lanes whose mask bit is 0 are written as zero in
 * dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint8_t& dst, vector_uint8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the arithmetic right shift by a scalar bit count of the masked int8_t elements of src into dst, zeroing
 * unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvi8_vi8_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the shifted int8_t elements; must have the same data type
 * as src.
 * @param [in] src Source vector data register of signed 8-bit elements that are read for the shift.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, with sign replication for the signed
 * 8-bit data.
 * @param [in] mask Mask register selecting which elements participate; elements with mask bit 0 are written as zero in
 * dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_int8_t& dst, vector_int8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the logical right shift by a scalar bit count of the masked uint16_t elements of src into dst, zeroing
 * unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvu16_vu16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the logically shifted uint16_t elements; type must
 * match src.
 * @param [in] src Source vector data register of unsigned 16-bit elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; high bits fill with 0 and the value must not be
 * negative.
 * @param [in] mask Mask register selecting elements to shift; elements with mask bit 0 are written as zero in dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint16_t& dst, vector_uint16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the arithmetic right shift by a scalar bit count of the masked int16_t elements of src into dst,
 * zeroing unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvi16_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the arithmetically shifted int16_t elements; same type as
 * src.
 * @param [in] src Source vector data register of signed 16-bit elements read for the shift.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, sign-extending for signed data.
 * @param [in] mask Mask register selecting participating elements; a 0 bit writes zero for that element in dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_int16_t& dst, vector_int16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the logical right shift by a scalar bit count of the masked uint32_t elements of src into dst, zeroing
 * unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvu32_vu32_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the logically shifted uint32_t elements; type must match
 * src.
 * @param [in] src Source vector data register of unsigned 32-bit elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; zero-filled from the high end and required to be
 * non-negative.
 * @param [in] mask Mask register marking participating elements; a cleared bit excludes the element and writes zero in
 * dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint32_t& dst, vector_uint32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Writes the arithmetic right shift by a scalar bit count of the masked int32_t elements of src into dst,
 * zeroing unselected elements.
 *
 * @override_id asc_shiftright_scalar__Rvi32_vi32_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the arithmetically shifted int32_t elements; same type as
 * src.
 * @param [in] src Source vector data register of signed 32-bit elements that are read for the shift.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, with sign replication for signed 32-bit
 * data.
 * @param [in] mask Mask register selecting elements to shift; elements whose bit is 0 are written as zero in dst.
 */
__simd_callee__ inline void asc_shiftright_scalar(
    vector_int32_t& dst, vector_int32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked uint8_t elements are shifted right logically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vu8_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 8-bit (uint8_t) elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, and shifts wider than 8 bits produce
 * zero for unsigned elements.
 * @param [in] mask Mask register marking elements that take part; elements with mask bit 0 are not shifted and are
 * zeroed in the result.
 */
__simd_callee__ inline vector_uint8_t asc_shiftright_scalar(vector_uint8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked int8_t elements are shifted right arithmetically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vi8_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 8-bit (int8_t) elements; only read as the shift input.
 * @param [in] shift Scalar int16_t shift amount in bit positions; negative values are undefined and shifts wider than 8
 * bits copy the sign bit.
 * @param [in] mask Mask register selecting elements to shift; elements whose mask bit is 0 are not shifted and are
 * written as zero.
 */
__simd_callee__ inline vector_int8_t asc_shiftright_scalar(vector_int8_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked uint16_t elements are shifted right logically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vu16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 16-bit (uint16_t) elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; zero fill on the high end, and shifts wider than 16
 * bits yield zero.
 * @param [in] mask Mask register indicating participating elements; a 0 bit keeps that element out of the shift and
 * zeroes it in the result.
 */
__simd_callee__ inline vector_uint16_t asc_shiftright_scalar(vector_uint16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked int16_t elements are shifted right arithmetically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 16-bit (int16_t) elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, with sign replication for shifts wider
 * than 16 bits.
 * @param [in] mask Mask register selecting the elements to shift; unselected elements are zeroed in the returned
 * register.
 */
__simd_callee__ inline vector_int16_t asc_shiftright_scalar(vector_int16_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked uint32_t elements are shifted right logically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vu32_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of unsigned 32-bit (uint32_t) elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; high bits are filled with zero and shifts wider than
 * 32 bits yield zero.
 * @param [in] mask Mask register marking participating elements; a cleared bit excludes the element and zeroes its
 * result.
 */
__simd_callee__ inline vector_uint32_t asc_shiftright_scalar(vector_uint32_t src, int16_t shift, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_logic
 * @brief Returns a register whose masked int32_t elements are shifted right arithmetically by a scalar bit count, with
 * unselected elements zeroed.
 *
 * @override_id asc_shiftright_scalar__vi32_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register of signed 32-bit (int32_t) elements; read only.
 * @param [in] shift Scalar int16_t shift amount in bit positions; non-negative, sign-extending for signed 32-bit data.
 * @param [in] mask Mask register selecting the elements to shift; elements whose bit is 0 are zeroed in the returned
 * register.
 */
__simd_callee__ inline vector_int32_t asc_shiftright_scalar(vector_int32_t src, int16_t shift, vector_bool mask);

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
//=================asc_shiftleft (unsigned shift overloads, 9201)==================
__simd_callee__ inline void asc_shiftleft(
    vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_int8_t& dst, vector_int8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_int16_t& dst, vector_int16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftleft(
    vector_int32_t& dst, vector_int32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftleft(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftleft(vector_int8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftleft(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftleft(vector_int16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftleft(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftleft(vector_int32_t src0, vector_uint32_t src1, vector_bool mask);

//=================asc_shiftleft_scalar (uint value overloads, 9201)==================
__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int8_t& dst, vector_int8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int16_t& dst, vector_int16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftleft_scalar(
    vector_int32_t& dst, vector_int32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftleft_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftleft_scalar(vector_int8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftleft_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftleft_scalar(vector_int16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftleft_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftleft_scalar(vector_int32_t src, uint32_t value, vector_bool mask);

//=================asc_shiftright (unsigned shift overloads, 9201)==================
__simd_callee__ inline void asc_shiftright(
    vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int8_t& dst, vector_int8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int16_t& dst, vector_int16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline void asc_shiftright(
    vector_int32_t& dst, vector_int32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftright(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftright(vector_int8_t src0, vector_uint8_t src1, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftright(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftright(vector_int16_t src0, vector_uint16_t src1, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftright(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftright(vector_int32_t src0, vector_uint32_t src1, vector_bool mask);

//=================asc_shiftright_scalar (uint value overloads, 9201)==================
__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int8_t& dst, vector_int8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int16_t& dst, vector_int16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline void asc_shiftright_scalar(
    vector_int32_t& dst, vector_int32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_uint8_t asc_shiftright_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_int8_t asc_shiftright_scalar(vector_int8_t src, uint8_t value, vector_bool mask);

__simd_callee__ inline vector_uint16_t asc_shiftright_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_int16_t asc_shiftright_scalar(vector_int16_t src, uint16_t value, vector_bool mask);

__simd_callee__ inline vector_uint32_t asc_shiftright_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

__simd_callee__ inline vector_int32_t asc_shiftright_scalar(vector_int32_t src, uint32_t value, vector_bool mask);
#endif
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the bitwise AND of the vector registers src0 and src1 under mask into dst; this vector_fp8_e4m3fn_t
 * register overload is deprecated in favour of the equal-bit-width integer overload of asc_and.
 *
 * @override_id asc_and__Rvf8e4m3_vf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives the masked bitwise AND.
 * @param [in] src0 First vector_fp8_e4m3fn_t register whose bits are combined.
 * @param [in] src1 Second vector_fp8_e4m3fn_t register whose bits are combined.
 * @param [in] mask Mask register selecting the elements to combine; a 0 bit forces the corresponding dst bit to 0.
 */
__simd_callee__ inline void asc_and(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the bitwise AND of the vector registers src0 and src1 under mask into dst; this vector_fp8_e5m2_t
 * register overload is deprecated in favour of the equal-bit-width integer overload of asc_and.
 *
 * @override_id asc_and__Rvf8e5m2_vf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives the masked bitwise AND.
 * @param [in] src0 First vector_fp8_e5m2_t register whose bits are combined.
 * @param [in] src1 Second vector_fp8_e5m2_t register whose bits are combined.
 * @param [in] mask Mask register selecting the elements to combine; a 0 bit forces the corresponding dst bit to 0.
 */
__simd_callee__ inline void asc_and(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the bitwise AND of the vector registers src0 and src1 under mask into dst; this vector_half register
 * overload is deprecated in favour of the equal-bit-width integer overload of asc_and.
 *
 * @override_id asc_and__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the masked bitwise AND.
 * @param [in] src0 First vector_half register whose bits are combined.
 * @param [in] src1 Second vector_half register whose bits are combined.
 * @param [in] mask Mask register selecting the elements to combine; a 0 bit forces the corresponding dst bit to 0.
 */
__simd_callee__ inline void asc_and(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the bitwise AND of the vector registers src0 and src1 under mask into dst; this vector_bfloat16_t
 * register overload is deprecated in favour of the equal-bit-width integer overload of asc_and.
 *
 * @override_id asc_and__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the masked bitwise AND.
 * @param [in] src0 First vector_bfloat16_t register whose bits are combined.
 * @param [in] src1 Second vector_bfloat16_t register whose bits are combined.
 * @param [in] mask Mask register selecting the elements to combine; a 0 bit forces the corresponding dst bit to 0.
 */
__simd_callee__ inline void asc_and(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the bitwise AND of the vector registers src0 and src1 under mask into dst; this vector_float register
 * overload is deprecated in favour of the equal-bit-width integer overload of asc_and.
 *
 * @override_id asc_and__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives the masked bitwise AND.
 * @param [in] src0 First vector_float register whose bits are combined.
 * @param [in] src1 Second vector_float register whose bits are combined.
 * @param [in] mask Mask register selecting the elements to combine; a 0 bit forces the corresponding dst bit to 0.
 */
__simd_callee__ inline void asc_and(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes dst = src0 | src1 under mask on AIV for the deprecated half overload, zeroing output bits whose mask
 * bit is 0.
 *
 * @override_id asc_or__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the bitwise OR result; bits with a 0 mask bit are written
 * as 0.
 * @param [in] src0 First source vector data register of the bitwise OR.
 * @param [in] src1 Second source vector data register of the bitwise OR.
 * @param [in] mask Mask register controlling which elements participate; it must be assigned before the call.
 */
__simd_callee__ inline void asc_or(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or integer overload)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes dst = src0 | src1 under mask on AIV for the deprecated float overload, zeroing output bits whose mask
 * bit is 0.
 *
 * @override_id asc_or__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the bitwise OR result; bits with a 0 mask bit are written
 * as 0.
 * @param [in] src0 First source vector data register of the bitwise OR.
 * @param [in] src1 Second source vector data register of the bitwise OR.
 * @param [in] mask Mask register controlling which elements participate; it must be assigned before the call.
 */
__simd_callee__ inline void asc_or(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_LOGIC_H
#endif
