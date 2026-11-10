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
#error "compute/reg_compare.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_compare_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for greater-than-or-equal under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for greater-than-or-equal under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise
 * greater-than-or-equal.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise
 * greater-than-or-equal.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vector src elementwise with the int8_t scalar value for greater-than-or-equal under mask
 * and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_int8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vector src elementwise with the int16_t scalar value for greater-than-or-equal under mask
 * and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_int16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vector src elementwise with the int32_t scalar value for greater-than-or-equal under mask
 * and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_int32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vector src elementwise with the uint8_t scalar value for greater-than-or-equal under mask
 * and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_uint8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vector src elementwise with the uint16_t scalar value for greater-than-or-equal under
 * mask and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_uint16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vector src elementwise with the uint32_t scalar value for greater-than-or-equal under
 * mask and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_uint32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vector src elementwise with the half scalar value for greater-than-or-equal under mask and
 * writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_half register whose elements are compared with value under mask.
 * @param [in] value Scalar half value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vector src elementwise with the bfloat16_t scalar value for greater-than-or-equal
 * under mask and writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_bfloat16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar bfloat16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vector src elementwise with the float scalar value for greater-than-or-equal under mask and
 * writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_ge_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than-or-equal result bit per element;
 * written by the function.
 * @param [in] src Source vector_float register whose elements are compared with value under mask.
 * @param [in] value Scalar float value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vector src elementwise with the int8_t scalar value for greater-than-or-equal under mask
 * and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vector src elementwise with the int16_t scalar value for greater-than-or-equal under mask
 * and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vector src elementwise with the int32_t scalar value for greater-than-or-equal under mask
 * and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vector src elementwise with the uint8_t scalar value for greater-than-or-equal under mask
 * and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vector src elementwise with the uint16_t scalar value for greater-than-or-equal under
 * mask and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vector src elementwise with the uint32_t scalar value for greater-than-or-equal under
 * mask and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vector src elementwise with the half scalar value for greater-than-or-equal under mask and
 * returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector_half register whose elements are compared with value under mask.
 * @param [in] value Scalar half value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vector src elementwise with the bfloat16_t scalar value for greater-than-or-equal
 * under mask and returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar bfloat16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vector src elementwise with the float scalar value for greater-than-or-equal under mask and
 * returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_ge_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector_float register whose elements are compared with value under mask.
 * @param [in] value Scalar float value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_ge_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i <= src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and writes the src0_i <= src1_i
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i <= src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and returns a mask register
 * holding src0_i <= src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_le__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and writes the src_i
 * <= value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and writes the src_i <=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and writes the src_i
 * <= value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and writes the src_i <=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and writes the src_i
 * <= value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and writes the src_i <=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and writes the src_i <=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and writes the src_i
 * <= value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and writes the src_i <= value
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_le_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i <= value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and returns a mask register holding
 * src_i <= value per element, with masked-off bits cleared.
 *
 * @override_id asc_le_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_le_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for equality under mask and writes a vector_bool register
 * holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for equality under mask and returns a vector_bool register
 * holding one result bit per element.
 *
 * @override_id asc_eq__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for equality under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for equality under mask and writes a vector_bool register
 * holding one result bit per element to dst.
 *
 * @override_id asc_eq__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for equality under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_eq__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise equality.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise equality.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vector src elementwise with the int8_t scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_int8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vector src elementwise with the int8_t scalar value for equality under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vector src elementwise with the uint8_t scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_uint8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vector src elementwise with the uint8_t scalar value for equality under mask and returns
 * a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint8_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vector src elementwise with the int16_t scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_int16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vector src elementwise with the int16_t scalar value for equality under mask and returns
 * a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vector src elementwise with the uint16_t scalar value for equality under mask and writes
 * a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_uint16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vector src elementwise with the uint16_t scalar value for equality under mask and
 * returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vector src elementwise with the half scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_half register whose elements are compared with value under mask.
 * @param [in] value Scalar half value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vector src elementwise with the half scalar value for equality under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector_half register whose elements are compared with value under mask.
 * @param [in] value Scalar half value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vector src elementwise with the bfloat16_t scalar value for equality under mask and
 * writes a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_bfloat16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar bfloat16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vector src elementwise with the bfloat16_t scalar value for equality under mask and
 * returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register whose elements are compared with value under mask.
 * @param [in] value Scalar bfloat16_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vector src elementwise with the int32_t scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_int32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vector src elementwise with the int32_t scalar value for equality under mask and returns
 * a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar int32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vector src elementwise with the uint32_t scalar value for equality under mask and writes
 * a vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_uint32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vector src elementwise with the uint32_t scalar value for equality under mask and
 * returns a vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register whose elements are compared with value under mask.
 * @param [in] value Scalar uint32_t value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vector src elementwise with the float scalar value for equality under mask and writes a
 * vector_bool register holding one result bit per element to dst.
 *
 * @override_id asc_eq_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one equality result bit per element; written by the
 * function.
 * @param [in] src Source vector_float register whose elements are compared with value under mask.
 * @param [in] value Scalar float value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vector src elementwise with the float scalar value for equality under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_eq_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector_float register whose elements are compared with value under mask.
 * @param [in] value Scalar float value that every participating element of src is compared against.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_eq_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i < src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and writes the src0_i < src1_i
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i < src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and returns a mask register
 * holding src0_i < src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_lt__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and writes the src_i < value
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and returns a mask register holding
 * src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and writes the src_i
 * < value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and writes the src_i
 * < value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and writes the src_i <
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_lt_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i < value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i < value per element, with masked-off bits cleared.
 *
 * @override_id asc_lt_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_lt_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint8_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint8_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint8_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int8_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 First source vector_int8_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int8_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_half register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_half register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the float vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First source vector_float register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_float register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint16_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint16_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int16_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 First source vector_int16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int16_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the uint32_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 First source vector_uint32_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_uint32_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the int32_t vectors src0 and src1 elementwise for greater-than under mask and returns a vector_bool
 * register holding one result bit per element.
 *
 * @override_id asc_gt__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 First source vector_int32_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_int32_t register supplying the right operand of the elementwise greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for greater-than under mask and writes a vector_bool
 * register holding one result bit per element to dst.
 *
 * @override_id asc_gt__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bool register that receives one greater-than result bit per element; written by
 * the function.
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise
 * greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16_t vectors src0 and src1 elementwise for greater-than under mask and returns a
 * vector_bool register holding one result bit per element.
 *
 * @override_id asc_gt__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First source vector_bfloat16_t register supplying the left operand of the elementwise greater-than.
 * @param [in] src1 Second source vector_bfloat16_t register supplying the right operand of the elementwise
 * greater-than.
 * @param [in] mask Mask register selecting the elements that are compared: a set bit compares the element and a cleared
 * bit makes the corresponding result bit zero.
 */
__simd_callee__ inline vector_bool asc_gt(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and writes the src_i > value
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and writes the src_i
 * > value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and writes the src_i
 * > value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and writes the src_i >
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_gt_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i > value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and returns a mask register holding
 * src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i > value per element, with masked-off bits cleared.
 *
 * @override_id asc_gt_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_gt_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vu8_vu8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint8_t) holding the left-hand unsigned 8-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the right-hand unsigned 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_uint8_t src0, vector_uint8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 8-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vi8_vi8_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int8_t) holding the left-hand signed 8-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the right-hand signed 8-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_int8_t src0, vector_int8_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 16-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vu16_vu16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint16_t) holding the left-hand unsigned 16-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the right-hand unsigned 16-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 16-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vi16_vi16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int16_t) holding the left-hand signed 16-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the right-hand signed 16-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the unsigned 32-bit integer elements of src0 and src1 element by element under mask and returns a
 * mask register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vu32_vu32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_uint32_t) holding the left-hand unsigned 32-bit integer operands
 * of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the right-hand unsigned 32-bit integer
 * operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the signed 32-bit integer elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vi32_vi32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_int32_t) holding the left-hand signed 32-bit integer operands of
 * the comparison; read only.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the right-hand signed 32-bit integer operands
 * compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the half-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_half) holding the left-hand IEEE 754 binary16 half-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_half) holding the right-hand IEEE 754 binary16 half-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and writes the src0_i != src1_i
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the bfloat16 elements of src0 and src1 element by element under mask and returns a mask register
 * holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_bfloat16_t) holding the left-hand bfloat16 floating-point (8-bit
 * exponent, 7-bit mantissa) operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the right-hand bfloat16 floating-point
 * (8-bit exponent, 7-bit mantissa) operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and writes the
 * src0_i != src1_i predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne__Rvb_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src0_i != src1_i predicate; masked-off
 * elements are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares the single-precision float elements of src0 and src1 element by element under mask and returns a mask
 * register holding src0_i != src1_i per element, with masked-off bits cleared.
 *
 * @override_id asc_ne__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Source vector data register (vector_float) holding the left-hand IEEE 754 binary32 single-precision
 * floating-point operands of the comparison; read only.
 * @param [in] src1 Source vector data register (vector_float) holding the right-hand IEEE 754 binary32 single-precision
 * floating-point operands compared against src0; read only.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and writes the src_i
 * != value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and writes the src_i !=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and writes the src_i
 * != value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and writes the src_i !=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and writes the src_i
 * != value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and writes the src_i !=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and writes the src_i !=
 * value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and writes the src_i
 * != value predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and writes the src_i != value
 * predicate into dst, clearing the bits of masked-off elements.
 *
 * @override_id asc_ne_scalar__Rvb_vbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination mask register written with the per-element src_i != value predicate; masked-off elements
 * are written as 0. Used only by the reference-output prototype, which returns void.
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vu8_u8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint8_t) whose 256 unsigned 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint8_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint8_t src, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 8-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vi8_i8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int8_t) whose 256 signed 8-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int8_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b8 operand maps
 * a single mask bit per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_int8_t src, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vu16_u16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint16_t) whose 128 unsigned 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint16_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint16_t src, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 16-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vi16_i16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int16_t) whose 128 signed 16-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int16_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_int16_t src, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each unsigned 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vu32_u32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint32_t) whose 64 unsigned 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar uint32_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_uint32_t src, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each signed 32-bit integer element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vi32_i32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int32_t) whose 64 signed 32-bit integer elements are compared
 * against value; read only.
 * @param [in] value Scalar int32_t compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_int32_t src, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each half-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_half) whose 128 IEEE 754 binary16 half-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar half compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each single-precision float element of src against the scalar value under mask and returns a mask
 * register holding src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_float) whose 64 IEEE 754 binary32 single-precision floating-point
 * elements are compared against value; read only.
 * @param [in] value Scalar float compared against every element of src; the declaration fixes it to the same element
 * type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b32 operand maps
 * 4 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_compare
 * @brief Compares each bfloat16 element of src against the scalar value under mask and returns a mask register holding
 * src_i != value per element, with masked-off bits cleared.
 *
 * @override_id asc_ne_scalar__vbf16_bf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_bfloat16_t) whose 128 bfloat16 floating-point (8-bit exponent,
 * 7-bit mantissa) elements are compared against value; read only.
 * @param [in] value Scalar bfloat16_t compared against every element of src; the declaration fixes it to the same
 * element type as src. The page additionally requires src and value to keep the same data type.
 * @param [in] mask Mask register that selects the elements taking part in the operation: a mask bit of 1 includes the
 * element and 0 excludes it, forcing that element's result bit to 0. In the 256-bit mask register this b16 operand maps
 * 2 mask bits per element, of which only the LSB is valid.
 */
__simd_callee__ inline vector_bool asc_ne_scalar(vector_bfloat16_t src, bfloat16_t value, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_COMPARE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_COMPARE_H
#endif
