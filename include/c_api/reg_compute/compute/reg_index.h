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
#error "compute/reg_index.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_INDEX_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_INDEX_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_INDEX_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_index_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int8_t holding the 256-element ascending index sequence value + i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange__i8
 * @archs 3510
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 255; dtype must match dst and the doc limits value to the int8_t representable range and
 * states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline vector_int8_t asc_arange(int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int16_t holding the 128-element ascending index sequence value + i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange__i16
 * @archs 3510
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 127; dtype must match dst and the doc limits value to the int16_t representable range
 * and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline vector_int16_t asc_arange(int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int32_t holding the 64-element ascending index sequence value + i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange__i32
 * @archs 3510
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 63; dtype must match dst and the doc limits value to the int32_t representable range and
 * states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline vector_int32_t asc_arange(int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_half holding the 128-element ascending index sequence value + i that is seeded by the scalar
 * value.
 *
 * @override_id asc_arange__f16
 * @archs 3510
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 127; dtype must match dst and the doc limits value to the half representable range; no
 * wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline vector_half asc_arange(half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_float holding the 64-element ascending index sequence value + i that is seeded by the scalar
 * value.
 *
 * @override_id asc_arange__f32
 * @archs 3510
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 63; dtype must match dst and the doc limits value to the float representable range; no
 * wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline vector_float asc_arange(float value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int8_t holding the 256-element descending index sequence value + 255 - i that is seeded by
 * the scalar value.
 *
 * @override_id asc_arange_descend__i8
 * @archs 3510
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 255) and each following element decreases by 1; dtype must match dst and the doc limits value to the int8_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline vector_int8_t asc_arange_descend(int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int16_t holding the 128-element descending index sequence value + 127 - i that is seeded by
 * the scalar value.
 *
 * @override_id asc_arange_descend__i16
 * @archs 3510
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 127) and each following element decreases by 1; dtype must match dst and the doc limits value to the int16_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline vector_int16_t asc_arange_descend(int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_int32_t holding the 64-element descending index sequence value + 63 - i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange_descend__i32
 * @archs 3510
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 63) and each following element decreases by 1; dtype must match dst and the doc limits value to the int32_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline vector_int32_t asc_arange_descend(int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_half holding the 128-element descending index sequence value + 127 - i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange_descend__f16
 * @archs 3510
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 127) and each following element decreases by 1; dtype must match dst and the doc limits value to the half
 * representable range; no wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline vector_half asc_arange_descend(half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Returns a vector_float holding the 64-element descending index sequence value + 63 - i that is seeded by the
 * scalar value.
 *
 * @override_id asc_arange_descend__f32
 * @archs 3510
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 63) and each following element decreases by 1; dtype must match dst and the doc limits value to the float
 * representable range; no wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline vector_float asc_arange_descend(float value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 256-element vector_int8_t destination with the ascending index sequence value + i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange__Rvi8_i8
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register; all 256 elements are overwritten with the ascending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 255; dtype must match dst and the doc limits value to the int8_t representable range and
 * states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 128-element vector_int16_t destination with the ascending index sequence value + i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange__Rvi16_i16
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register; all 128 elements are overwritten with the ascending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 127; dtype must match dst and the doc limits value to the int16_t representable range
 * and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 64-element vector_int32_t destination with the ascending index sequence value + i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange__Rvi32_i32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register; all 64 elements are overwritten with the ascending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 63; dtype must match dst and the doc limits value to the int32_t representable range and
 * states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128, -127, ...) without
 * raising.
 */
__simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 128-element vector_half destination with the ascending index sequence value + i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange__Rvf16_f16
 * @archs 3510
 * @param [out] dst Destination vector_half register; all 128 elements are overwritten with the ascending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 127; dtype must match dst and the doc limits value to the half representable range; no
 * wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline void asc_arange(vector_half& dst, half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 64-element vector_float destination with the ascending index sequence value + i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange__Rvf32_f32
 * @archs 3510
 * @param [out] dst Destination vector_float register; all 64 elements are overwritten with the ascending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the ascending sequence: element 0 equals value and each following element
 * increases by 1 up to value + 63; dtype must match dst and the doc limits value to the float representable range; no
 * wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline void asc_arange(vector_float& dst, float value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 256-element vector_int8_t destination with the descending index sequence value + 255 - i, seeded by
 * the scalar value, overwriting the register completely.
 *
 * @override_id asc_arange_descend__Rvi8_i8
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register; all 256 elements are overwritten with the descending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 255) and each following element decreases by 1; dtype must match dst and the doc limits value to the int8_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 128-element vector_int16_t destination with the descending index sequence value + 127 - i, seeded by
 * the scalar value, overwriting the register completely.
 *
 * @override_id asc_arange_descend__Rvi16_i16
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register; all 128 elements are overwritten with the descending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 127) and each following element decreases by 1; dtype must match dst and the doc limits value to the int16_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 64-element vector_int32_t destination with the descending index sequence value + 63 - i, seeded by
 * the scalar value, overwriting the register completely.
 *
 * @override_id asc_arange_descend__Rvi32_i32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register; all 64 elements are overwritten with the descending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 63) and each following element decreases by 1; dtype must match dst and the doc limits value to the int32_t
 * representable range and states that integer results wrap around on overflow (e.g. int8_t value=127 yields 127, -128,
 * -127, ...) without raising.
 */
__simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 128-element vector_half destination with the descending index sequence value + 127 - i, seeded by
 * the scalar value, overwriting the register completely.
 *
 * @override_id asc_arange_descend__Rvf16_f16
 * @archs 3510
 * @param [out] dst Destination vector_half register; all 128 elements are overwritten with the descending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 127) and each following element decreases by 1; dtype must match dst and the doc limits value to the half
 * representable range; no wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline void asc_arange_descend(vector_half& dst, half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_index
 * @brief Fills the 64-element vector_float destination with the descending index sequence value + 63 - i, seeded by the
 * scalar value, overwriting the register completely.
 *
 * @override_id asc_arange_descend__Rvf32_f32
 * @archs 3510
 * @param [out] dst Destination vector_float register; all 64 elements are overwritten with the descending index
 * sequence, so its previous contents do not matter.
 * @param [in] value Scalar start value of the descending sequence: the first element written is value + VL_T - 1 (=
 * value + 63) and each following element decreases by 1; dtype must match dst and the doc limits value to the float
 * representable range; no wrap-around clause is documented for this floating-point dtype.
 */
__simd_callee__ inline void asc_arange_descend(vector_float& dst, float value);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_INDEX_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_INDEX_H
#endif
