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
#error "scatter/ub_scatter.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_SCATTER_UB_SCATTER_H
#define INCLUDE_C_API_REG_COMPUTE_SCATTER_UB_SCATTER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/ub_scatter_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked int8_t element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUi8_vi8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered int8_t elements; the type is __ubuf__ int8_t*
 * and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of int8_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ int8_t* dst, vector_int8_t src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked uint8_t element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUu8_vu8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered uint8_t elements; the type is __ubuf__ uint8_t*
 * and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of uint8_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ uint8_t* dst, vector_uint8_t src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked int16_t element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUi16_vi16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered int16_t elements; the type is __ubuf__ int16_t*
 * and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of int16_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ int16_t* dst, vector_int16_t src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked uint16_t element of a vector data register to the UB position given by the index
 * register, leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUu16_vu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered uint16_t elements; the type is __ubuf__
 * uint16_t* and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of uint16_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ uint16_t* dst, vector_uint16_t src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked int32_t element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUi32_vi32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered int32_t elements; the type is __ubuf__ int32_t*
 * and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of int32_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint32_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ int32_t* dst, vector_int32_t src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked uint32_t element of a vector data register to the UB position given by the index
 * register, leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUu32_vu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered uint32_t elements; the type is __ubuf__
 * uint32_t* and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of uint32_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint32_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked bfloat16_t element of a vector data register to the UB position given by the index
 * register, leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUbf16_vbf16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered bfloat16_t elements; the type is __ubuf__
 * bfloat16_t* and the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of bfloat16_t elements whose dtype must match the data type pointed to by
 * dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked half element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUf16_vf16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered half elements; the type is __ubuf__ half* and
 * the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of half elements whose dtype must match the data type pointed to by dst.
 * @param [in] index Element-index vector data register (uint16_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(__ubuf__ half* dst, vector_half src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.scatter
 * @brief Scatters each masked float element of a vector data register to the UB position given by the index register,
 * leaving destination data unchanged where the mask bit is 0.
 *
 * @override_id asc_scatter__PUf32_vf32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address receiving the scattered float elements; the type is __ubuf__ float* and
 * the start address must be 32-byte aligned.
 * @param [in] src Source vector data register of float elements whose dtype must match the data type pointed to by dst.
 * @param [in] index Element-index vector data register (uint32_t for this element width) giving the destination
 * position of each source element, measured in elements.
 * @param [in] mask Source-operand mask register: a 1 bit lets the element participate in the scatter while a 0 bit
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(__ubuf__ float* dst, vector_float src, vector_uint32_t index, vector_bool mask);
__simd_callee__ inline void asc_scatter(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, vector_uint16_t index, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
__simd_callee__ inline void asc_scatter(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, vector_uint16_t index, vector_bool mask);
#endif

ASC_DEPRECATED(
    9.1.0, "2027/09/07", asc_scatter(__ubuf__ int8_t* dst, vector_int8_t src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked int8_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvi8_PUi8_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int8_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered int8_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_int8_t& src, __ubuf__ int8_t* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ uint8_t* dst, vector_uint8_t src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked uint8_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvu8_PUu8_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the uint8_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered uint8_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_uint8_t& src, __ubuf__ uint8_t* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ int16_t* dst, vector_int16_t src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked int16_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvi16_PUi16_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int16_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered int16_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_int16_t& src, __ubuf__ int16_t* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked uint16_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvu16_PUu16_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the uint16_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered uint16_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_uint16_t& src, __ubuf__ uint16_t* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ int32_t* dst, vector_int32_t src, vector_uint32_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked int32_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvi32_PUi32_vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the int32_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered int32_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint32_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_int32_t& src, __ubuf__ int32_t* dst, vector_uint32_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_uint32_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked uint32_t element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvu32_PUu32_vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the uint32_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered uint32_t elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint32_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_uint32_t& src, __ubuf__ uint32_t* dst, vector_uint32_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07",
    asc_scatter(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked bfloat16_t element of a vector data register to the UB offset held
 * in the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvbf16_PUbf16_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the bfloat16_t elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered bfloat16_t elements; it must be 32-byte
 * aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_bfloat16_t& src, __ubuf__ bfloat16_t* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07", asc_scatter(__ubuf__ half* dst, vector_half src, vector_uint16_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked half element of a vector data register to the UB offset held in the
 * index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvf16_PUf16_vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the half elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered half elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint16_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(vector_half& src, __ubuf__ half* dst, vector_uint16_t index, vector_bool mask);

ASC_DEPRECATED(
    9.1.0, "2027/09/07", asc_scatter(__ubuf__ float* dst, vector_float src, vector_uint32_t index, vector_bool mask))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated scatter that stores each masked float element of a vector data register to the UB offset held in
 * the index register; use asc_scatter with the (dst, src, index, mask) parameter order instead.
 *
 * @override_id asc_scatter__Rvf32_PUf32_vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register holding the float elements to scatter.
 * @param [out] dst Destination UB start address receiving the scattered float elements; it must be 32-byte aligned.
 * @param [in] index Element-index vector register (uint32_t, matching this element width) giving the destination
 * element offset of each source element; it is measured in elements and its values must be unique.
 * @param [in] mask Mask register selecting the elements that participate: bit 1 scatters the source element and bit 0
 * leaves the destination data unchanged.
 */
__simd_callee__ inline void asc_scatter(
    vector_float& src, __ubuf__ float* dst, vector_uint32_t index, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_SCATTER_UB_SCATTER_H
#endif
