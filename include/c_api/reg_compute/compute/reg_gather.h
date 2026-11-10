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
#error "compute/reg_gather.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_GATHER_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_GATHER_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_GATHER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_gather_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type int8_t whose elements are collected from src at the positions held in the
 * uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vi8_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type int8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_int8_t asc_gather(vector_int8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type uint8_t whose elements are collected from src at the positions held in the
 * uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vu8_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type uint8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_uint8_t asc_gather(vector_uint8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type int16_t whose elements are collected from src at the positions held in the
 * uint16_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vi16_vu16
 * @archs 3510
 * @param [in] src Source vector data register of type int16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_int16_t asc_gather(vector_int16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type uint16_t whose elements are collected from src at the positions held in the
 * uint16_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vu16_vu16
 * @archs 3510
 * @param [in] src Source vector data register of type uint16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_uint16_t asc_gather(vector_uint16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type int32_t whose elements are collected from src at the positions held in the
 * uint32_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vi32_vu32
 * @archs 3510
 * @param [in] src Source vector data register of type int32_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_int32_t asc_gather(vector_int32_t src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type uint32_t whose elements are collected from src at the positions held in the
 * uint32_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vu32_vu32
 * @archs 3510
 * @param [in] src Source vector data register of type uint32_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_uint32_t asc_gather(vector_uint32_t src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type float whose elements are collected from src at the positions held in the
 * uint32_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vf32_vu32
 * @archs 3510
 * @param [in] src Source vector data register of type float from which the elements are collected; its dtype must match
 * dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_float asc_gather(vector_float src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type half whose elements are collected from src at the positions held in the
 * uint16_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vf16_vu16
 * @archs 3510
 * @param [in] src Source vector data register of type half from which the elements are collected; its dtype must match
 * dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_half asc_gather(vector_half src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type bfloat16_t whose elements are collected from src at the positions held in
 * the uint16_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vbf16_vu16
 * @archs 3510
 * @param [in] src Source vector data register of type bfloat16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_bfloat16_t asc_gather(vector_bfloat16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type fp8_e4m3fn_t whose elements are collected from src at the positions held in
 * the uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vf8e4m3_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e4m3fn_t from which the elements are collected; its dtype
 * must match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_gather(vector_fp8_e4m3fn_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type hifloat8_t whose elements are collected from src at the positions held in
 * the uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vhf8_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type hifloat8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_hifloat8_t asc_gather(vector_hifloat8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type fp8_e5m2_t whose elements are collected from src at the positions held in
 * the uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vf8e5m2_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e5m2_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_gather(vector_fp8_e5m2_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Returns a vector register of type fp8_e8m0_t whose elements are collected from src at the positions held in
 * the uint8_t index register, so that dst[i] equals src[index[i]].
 *
 * @override_id asc_gather__vf8e8m0_vu8
 * @archs 3510
 * @param [in] src Source vector data register of type fp8_e8m0_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_gather(vector_fp8_e8m0_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects int8_t elements into dst by using each element of the uint8_t index register as a position in src, so
 * that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvi8_vi8_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type int8_t that receives the collected elements.
 * @param [in] src Source vector data register of type int8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_int8_t& dst, vector_int8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects uint8_t elements into dst by using each element of the uint8_t index register as a position in src,
 * so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvu8_vu8_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint8_t that receives the collected elements.
 * @param [in] src Source vector data register of type uint8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_uint8_t& dst, vector_uint8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects int16_t elements into dst by using each element of the uint16_t index register as a position in src,
 * so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvi16_vi16_vu16
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the collected elements.
 * @param [in] src Source vector data register of type int16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_int16_t& dst, vector_int16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects uint16_t elements into dst by using each element of the uint16_t index register as a position in src,
 * so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvu16_vu16_vu16
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the collected elements.
 * @param [in] src Source vector data register of type uint16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_uint16_t& dst, vector_uint16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects int32_t elements into dst by using each element of the uint32_t index register as a position in src,
 * so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvi32_vi32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the collected elements.
 * @param [in] src Source vector data register of type int32_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_int32_t& dst, vector_int32_t src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects uint32_t elements into dst by using each element of the uint32_t index register as a position in src,
 * so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvu32_vu32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the collected elements.
 * @param [in] src Source vector data register of type uint32_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_uint32_t& dst, vector_uint32_t src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects float elements into dst by using each element of the uint32_t index register as a position in src, so
 * that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvf32_vf32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the collected elements.
 * @param [in] src Source vector data register of type float from which the elements are collected; its dtype must match
 * dst.
 * @param [in] index Index vector data register of type uint32_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_float& dst, vector_float src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects half elements into dst by using each element of the uint16_t index register as a position in src, so
 * that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvf16_vf16_vu16
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the collected elements.
 * @param [in] src Source vector data register of type half from which the elements are collected; its dtype must match
 * dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_half& dst, vector_half src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects bfloat16_t elements into dst by using each element of the uint16_t index register as a position in
 * src, so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvbf16_vbf16_vu16
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the collected elements.
 * @param [in] src Source vector data register of type bfloat16_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint16_t; each value is an element position in src and an index
 * i equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_uint16_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects fp8_e4m3fn_t elements into dst by using each element of the uint8_t index register as a position in
 * src, so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvf8e4m3_vf8e4m3_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e4m3fn_t that receives the collected elements.
 * @param [in] src Source vector data register of type fp8_e4m3fn_t from which the elements are collected; its dtype
 * must match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects hifloat8_t elements into dst by using each element of the uint8_t index register as a position in
 * src, so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvhf8_vhf8_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type hifloat8_t that receives the collected elements.
 * @param [in] src Source vector data register of type hifloat8_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects fp8_e5m2_t elements into dst by using each element of the uint8_t index register as a position in
 * src, so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvf8e5m2_vf8e5m2_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e5m2_t that receives the collected elements.
 * @param [in] src Source vector data register of type fp8_e5m2_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_uint8_t index);

/**
 * @ingroup simd_c_api.reg_compute.reg_gather
 * @brief Collects fp8_e8m0_t elements into dst by using each element of the uint8_t index register as a position in
 * src, so that dst[i] equals src[index[i]], without a mask.
 *
 * @override_id asc_gather__Rvf8e8m0_vf8e8m0_vu8
 * @archs 3510
 * @param [out] dst Destination vector data register of type fp8_e8m0_t that receives the collected elements.
 * @param [in] src Source vector data register of type fp8_e8m0_t from which the elements are collected; its dtype must
 * match dst.
 * @param [in] index Index vector data register of type uint8_t; each value is an element position in src and an index i
 * equal to or above VL_T, the element capacity of src, is reduced to i modulo VL_T.
 */
__simd_callee__ inline void asc_gather(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_uint8_t index);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_GATHER_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_GATHER_H
#endif
