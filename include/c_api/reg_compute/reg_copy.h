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
#error "reg_copy.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COPY_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_REG_COPY_H
#define INCLUDE_C_API_REG_COMPUTE_REG_COPY_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_copy_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the int8_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int8_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_int8_t whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding int8_t element
 * of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the uint8_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint8_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_uint8_t whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding uint8_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the int16_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int16_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_int16_t whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding int16_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the uint16_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint16_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_uint16_t whose mask-selected elements are copied; its
 * dtype must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding uint16_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the half elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_half; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_half whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding half element
 * of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the int32_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_int32_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_int32_t whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding int32_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the uint32_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_uint32_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_uint32_t whose mask-selected elements are copied; its
 * dtype must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding uint32_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the float elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_float; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_float whose mask-selected elements are copied; its dtype
 * must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding float element
 * of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the bfloat16_t elements of a vector data register that mask selects into another vector data register,
 * leaving the unselected destination elements at their previous value.
 *
 * @override_id asc_copy__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register of type vector_bfloat16_t; its dtype must match src exactly, and
 * positions where mask is 0 keep the previous content because the move uses merging mode, so dst must be initialised
 * first.
 * @param [in] src Source vector data register of type vector_bfloat16_t whose mask-selected elements are copied; its
 * dtype must match dst exactly.
 * @param [in] mask Source mask register selecting the elements to copy: a 1 bit copies the corresponding bfloat16_t
 * element of src and a 0 bit leaves the corresponding element of dst unchanged.
 */
__simd_callee__ inline void asc_copy(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the bits of a mask register that another mask register selects into a destination mask register,
 * clearing the bits the mask does not select.
 *
 * @override_id asc_copy__Rvb_vb_vb
 * @archs 3510
 * @param [out] dst Destination mask register that receives the selected bits of src; bits that mask does not select are
 * written as zero.
 * @param [in] src Source mask register whose bits are copied.
 * @param [in] mask Source mask register selecting the bits to transfer: a 1 bit copies the corresponding src bit, and
 * for b8 mode each mask bit controls one src bit while b16 and b32 modes use 2-bit and 4-bit groups whose LSB alone
 * controls one src bit.
 */
__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies every bit of a source mask register into a destination mask register without masking.
 *
 * @override_id asc_copy__Rvb_vb
 * @archs 3510
 * @param [out] dst Destination mask register that receives all bits of src.
 * @param [in] src Source mask register whose bits are copied in full.
 */
__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Broadcasts one 16-byte data block of a vector_uint16_t register, selected at compile time by part, into a
 * 32-byte mask register.
 *
 * @override_id asc_copy__Rvb_vu16_i16
 * @archs 3510
 * @param [out] dst Destination mask register written with the broadcast of the selected data block, filling the whole
 * 32-byte mask.
 * @param [in] src Source vector data register of type vector_uint16_t, which the interface views as 16 data blocks of
 * 16 bytes of 16 bytes each.
 * @param [in] part Compile-time constant selecting the data block of src to broadcast; it accepts an integer literal or
 * constexpr value only, never a runtime variable, and its range is [0, 15] for this uint16_t source.
 */
__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint16_t src, int16_t part);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Broadcasts one 8-byte data block of a vector_uint32_t register, selected at compile time by part, into a
 * 32-byte mask register.
 *
 * @override_id asc_copy__Rvb_vu32_i16
 * @archs 3510
 * @param [out] dst Destination mask register written with the broadcast of the selected data block, filling the whole
 * 32-byte mask.
 * @param [in] src Source vector data register of type vector_uint32_t, which the interface views as 32 data blocks of 8
 * bytes of 8 bytes each.
 * @param [in] part Compile-time constant selecting the data block of src to broadcast; it accepts an integer literal or
 * constexpr value only, never a runtime variable, and its range is [0, 31] for this uint32_t source.
 */
__simd_callee__ inline void asc_copy(vector_bool& dst, vector_uint32_t src, int16_t part);

// ==========asc_mask return-value overloads(u8/s8/half/u16/s16/float/u32/s32/bf16/bool)==========
/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected int8_t vector data elements of src into a returned vector_int8_t register.
 *
 * @override_id asc_mask__vi8_vb
 * @archs 3510
 * @param [in] src Source vector_int8_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_int8_t asc_mask(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected uint8_t vector data elements of src into a returned vector_uint8_t register.
 *
 * @override_id asc_mask__vu8_vb
 * @archs 3510
 * @param [in] src Source vector_uint8_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_uint8_t asc_mask(vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected int16_t vector data elements of src into a returned vector_int16_t register.
 *
 * @override_id asc_mask__vi16_vb
 * @archs 3510
 * @param [in] src Source vector_int16_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_int16_t asc_mask(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected uint16_t vector data elements of src into a returned vector_uint16_t register.
 *
 * @override_id asc_mask__vu16_vb
 * @archs 3510
 * @param [in] src Source vector_uint16_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_uint16_t asc_mask(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected half vector data elements of src into a returned vector_half register.
 *
 * @override_id asc_mask__vf16_vb
 * @archs 3510
 * @param [in] src Source vector_half register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_half asc_mask(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected int32_t vector data elements of src into a returned vector_int32_t register.
 *
 * @override_id asc_mask__vi32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_int32_t asc_mask(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected uint32_t vector data elements of src into a returned vector_uint32_t register.
 *
 * @override_id asc_mask__vu32_vb
 * @archs 3510
 * @param [in] src Source vector_uint32_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_uint32_t asc_mask(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected float vector data elements of src into a returned vector_float register.
 *
 * @override_id asc_mask__vf32_vb
 * @archs 3510
 * @param [in] src Source vector_float register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_float asc_mask(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the mask-selected bfloat16_t vector data elements of src into a returned vector_bfloat16_t register.
 *
 * @override_id asc_mask__vbf16_vb
 * @archs 3510
 * @param [in] src Source vector_bfloat16_t register whose elements selected by mask are copied.
 * @param [in] mask Source vector_bool mask register selecting which elements participate; a bit of 1 selects the
 * element and a bit of 0 leaves the corresponding position of the returned register undefined.
 */
__simd_callee__ inline vector_bfloat16_t asc_mask(vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies the src mask bits selected by mask into a returned vector_bool register and sets the unselected bits to
 * 0.
 *
 * @override_id asc_mask__vb_vb
 * @archs 3510
 * @param [in] src Source vector_bool mask register whose bits are copied where the controlling mask bit is 1.
 * @param [in] mask Source vector_bool mask register that selects the copied bits; bits that it does not select are set
 * to 0 in the returned register, and for the b8 mode each mask bit controls one src bit, for the b16 mode each group of
 * 2 mask bits is controlled by its LSB, and for the b32 mode each group of 4 mask bits is controlled by its LSB.
 */
__simd_callee__ inline vector_bool asc_mask(vector_bool src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Copies all bits of the src mask register into a returned vector_bool register without applying any mask.
 *
 * @override_id asc_mask__vb
 * @archs 3510
 * @param [in] src Source vector_bool mask register whose bits are copied unconditionally to the returned register.
 */
__simd_callee__ inline vector_bool asc_mask(vector_bool src);

// ==========asc_extract_mask return-value overloads(u16/u32)==========
/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Extracts one 16-byte data block of a uint16 vector register into a mask register, broadcasting each bit to two
 * bits, and returns the mask.
 *
 * @override_id asc_extract_mask__vu16_i16
 * @archs 3510
 * @param [in] src Source vector data register of uint16_t elements from which one data block is extracted.
 * @param [in] part Compile-time block index in the range [0, 15], selecting which 16-byte data block of the 256-byte
 * register is extracted.
 */
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint16_t src, int16_t part);

/**
 * @ingroup simd_c_api.reg_compute.reg_copy
 * @brief Extracts one 8-byte data block of a uint32 vector register into a mask register, broadcasting each bit to four
 * bits, and returns the mask.
 *
 * @override_id asc_extract_mask__vu32_i16
 * @archs 3510
 * @param [in] src Source vector data register of uint32_t elements from which one data block is extracted.
 * @param [in] part Compile-time block index in the range [0, 31], selecting which 8-byte data block of the 256-byte
 * register is extracted.
 */
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint32_t src, int16_t part);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COPY_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COPY_H
#endif
