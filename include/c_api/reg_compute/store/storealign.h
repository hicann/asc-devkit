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
#error "reg_compute/store/storealign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_STORE_STOREALIGN_H
#define INCLUDE_C_API_REG_COMPUTE_STORE_STOREALIGN_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/storealign_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int8_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int8_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int8_t register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the int8_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint8_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint8_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int16_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int16_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int16_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint16_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint16_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int32_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int32_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint32_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint32_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_half register src to the 32-byte-aligned UB address dst, writing only
 * the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ half*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_half register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the half elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding half
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_float register src to the 32-byte-aligned UB address dst, writing only
 * the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ float*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_float register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the float elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding float
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_bfloat16_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ bfloat16_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_bfloat16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the bfloat16_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding bfloat16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e4m3fn_t register src to the 32-byte-aligned UB address dst,
 * writing only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e4m3fn_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e4m3fn_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp8_e4m3fn_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp8_e4m3fn_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_hifloat8_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUhf8_vhf8_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ hifloat8_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_hifloat8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the hifloat8_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding hifloat8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e5m2_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e5m2_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e5m2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e5m2_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e5m2_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e8m0_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e8m0_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e8m0_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e8m0_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e8m0_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e2m1_t register src to the 32-byte-aligned UB address dst,
 * writing only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf4e2m1x2_vf4e2m1x2_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e2m1_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e2m1_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e2m1_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e2m1_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e1m2_t register src to the 32-byte-aligned UB address dst,
 * writing only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUf4e1m2x2_vf4e1m2x2_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e1m2_t*; it must be 32-byte aligned and the
 * accessed range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e1m2_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e1m2_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e1m2_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int4x2_t register src to the 32-byte-aligned UB address dst, writing
 * only the elements marked by mask and leaving the remaining destination elements unchanged.
 *
 * @override_id asc_storealign__PUi4x2_vi4x2_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int4b_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int4x2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int4b_t elements of dst.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int4b_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int4b_t* dst, vector_int4x2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int8_t element of an int8_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUi8_vi8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int8_t register of int8_t elements; only the leading int8_t element participates in the
 * store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint8_t element of a uint8_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUu8_vu8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint8_t register of uint8_t elements; only the leading uint8_t element participates in
 * the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int16_t element of an int16_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUi16_vi16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int16_t elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int16_t register of int16_t elements; only the leading int16_t element participates in
 * the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint16_t element of a uint16_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUu16_vu16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint16_t register of uint16_t elements; only the leading uint16_t element participates
 * in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int32_t element of an int32_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUi32_vi32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int32_t register of int32_t elements; only the leading int32_t element participates in
 * the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint32_t element of a uint32_t vector data register to the dtype-aligned UB address dst; no
 * mask is applied.
 *
 * @override_id asc_storealign_1st__PUu32_vu32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint32_t register of uint32_t elements; only the leading uint32_t element participates
 * in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading half element of a half vector data register to the dtype-aligned UB address dst; no mask is
 * applied.
 *
 * @override_id asc_storealign_1st__PUf16_vf16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds half elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_half register of half elements; only the leading half element participates in the store
 * and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading float element of a float vector data register to the dtype-aligned UB address dst; no mask
 * is applied.
 *
 * @override_id asc_storealign_1st__PUf32_vf32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds float elements, four bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_float register of float elements; only the leading float element participates in the
 * store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading bfloat16_t element of a bfloat16_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUbf16_vbf16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds bfloat16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_bfloat16_t register of bfloat16_t elements; only the leading bfloat16_t element
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e4m3fn_t element of an fp8_e4m3fn_t vector data register to the dtype-aligned UB
 * address dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e4m3_vf8e4m3
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e4m3fn_t elements, one byte
 * each; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements; only the leading fp8_e4m3fn_t element
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading hifloat8_t element of a hifloat8_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUhf8_vhf8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds hifloat8_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_hifloat8_t register of hifloat8_t elements; only the leading hifloat8_t element
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e5m2_t element of an fp8_e5m2_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e5m2_vf8e5m2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e5m2_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e5m2_t register of fp8_e5m2_t elements; only the leading fp8_e5m2_t element
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e8m0_t element of an fp8_e8m0_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e8m0_vf8e8m0
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e8m0_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e8m0_t register of fp8_e8m0_t elements; only the leading fp8_e8m0_t element
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e2m1_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e2m1x2_vf4e2m1x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e1m2_t vector data register to the dtype-aligned UB address
 * dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e1m2x2_vf4e1m2x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed int4x2 unit (two 4-bit elements) of an int4b_t vector data register to the
 * dtype-aligned UB address dst; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi4x2_vi4x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed int4x2 units, two 4-bit
 * elements per byte; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int4x2_t register of int4b_t elements; only the leading packed int4x2 unit (two 4-bit
 * elements) participates in the store and the remaining elements are ignored.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi16_vi16_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int16_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int16_t* dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu16_vu16_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint16_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi32_vi32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int32_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint32_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi64_vi64_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int64_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int64_t* dst, vector_int64_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu64_vu64_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint64_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint64_t* dst, vector_uint64_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each half element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf16_vf16_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_half); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each float element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf32_vf32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_float); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ float* dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each bfloat16_t element of the vector register src into consecutive
 * elements at the 32-byte-aligned UB address dst, leaving the destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_bfloat16_t); its dtype must match the data type dst points to.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int8_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUi8_vi8_vi8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int8_t register of int8_t elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int8_t register of int8_t elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint8_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUu8_vu8_vu8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint8_t register of uint8_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint8_t register of uint8_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int16_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUi16_vi16_vi16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int16_t register of int16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int16_t register of int16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint16_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUu16_vu16_vu16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint16_t register of uint16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint16_t register of uint16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int32_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUi32_vi32_vi32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int32_t register of int32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int32_t register of int32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two float vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf32_vf32_vf32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds float elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_float register of float elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_float register of float elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ float* dst, vector_float src0, vector_float src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint32_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUu32_vu32_vu32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint32_t register of uint32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint32_t register of uint32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two half vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf16_vf16_vf16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds half elements, two bytes
 * each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_half register of half elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_half register of half elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ half* dst, vector_half src0, vector_half src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two bfloat16_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUbf16_vbf16_vbf16
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds bfloat16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_bfloat16_t register of bfloat16_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_bfloat16_t register of bfloat16_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e4m3fn_t vector data registers and stores the 512-byte result at the
 * 32-byte aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf8e4m3_vf8e4m3_vf8e4m3
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e4m3fn_t elements,
 * one byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two hifloat8_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUhf8_vhf8_vhf8
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds hifloat8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_hifloat8_t register of hifloat8_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_hifloat8_t register of hifloat8_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e5m2_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf8e5m2_vf8e5m2_vf8e5m2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e5m2_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e8m0_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf8e8m0_vf8e8m0_vf8e8m0
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e8m0_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e2m1_t vector data registers and stores the 512-byte result at the
 * 32-byte aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf4e2m1x2_vf4e2m1x2_vf4e2m1x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e1m2_t vector data registers and stores the 512-byte result at the
 * 32-byte aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUf4e1m2x2_vf4e1m2x2_vf4e1m2x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int4b_t vector data registers and stores the 512-byte result at the 32-byte
 * aligned UB address dst.
 *
 * @override_id asc_storealign_intlv__PUi4x2_vi4x2_vi4x2
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed int4x2 units, two
 * 4-bit elements per byte; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int4x2_t register of int4b_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int4x2_t register of int4b_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 */
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int8_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi8_vi8_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int8_t register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the int8_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of int8_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint8_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu8_vu8_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint8_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of uint8_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_uint8_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int16_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi16_vi16_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int16_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of int16_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst, vector_int16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint16_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu16_vu16_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint16_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of uint16_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int32_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int32_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of int32_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint32_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu32_vu32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint32_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of uint32_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_half register src to UB at dst plus offset elements, writing only the
 * mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf16_vf16_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ half*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_half register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the half elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of half; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding half
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_float register src to UB at dst plus offset elements, writing only the
 * mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf32_vf32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ float*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_float register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the float elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of float; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding float
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_bfloat16_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUbf16_vbf16_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ bfloat16_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_bfloat16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the bfloat16_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of bfloat16_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding bfloat16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e4m3fn_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e4m3_vf8e4m3_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e4m3fn_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e4m3fn_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp8_e4m3fn_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of fp8_e4m3fn_t;
 * the resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp8_e4m3fn_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_hifloat8_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUhf8_vhf8_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ hifloat8_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_hifloat8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the hifloat8_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of hifloat8_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding hifloat8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e5m2_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e5m2_vf8e5m2_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e5m2_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e5m2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e5m2_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of fp8_e5m2_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e5m2_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e8m0_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e8m0_vf8e8m0_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e8m0_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e8m0_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e8m0_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of fp8_e8m0_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e8m0_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e2m1_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf4e2m1x2_vf4e2m1x2_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e2m1_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e2m1_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e2m1_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of fp4x2_e2m1_t;
 * the resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e2m1_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e1m2_t register src to UB at dst plus offset elements, writing
 * only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf4e1m2x2_vf4e1m2x2_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e1m2_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e1m2_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e1m2_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of fp4x2_e1m2_t;
 * the resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e1m2_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int4x2_t register src to UB at dst plus offset elements, writing only
 * the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi4x2_vi4x2_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int4b_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int4x2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int4b_t elements of dst.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in elements of int4b_t; the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int4b_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int4b_t* dst, vector_int4x2_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int8_t element of an int8_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi8_vi8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int8_t register of int8_t elements; only the leading int8_t element participates in the
 * store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint8_t element of a uint8_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu8_vu8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint8_t register of uint8_t elements; only the leading uint8_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int16_t element of an int16_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi16_vi16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int16_t elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int16_t register of int16_t elements; only the leading int16_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint16_t element of a uint16_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu16_vu16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint16_t register of uint16_t elements; only the leading uint16_t element participates
 * in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int32_t element of an int32_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi32_vi32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int32_t register of int32_t elements; only the leading int32_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint32_t element of a uint32_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu32_vu32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint32_t register of uint32_t elements; only the leading uint32_t element participates
 * in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading half element of a half vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf16_vf16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds half elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_half register of half elements; only the leading half element participates in the store
 * and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading float element of a float vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf32_vf32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds float elements, four bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_float register of float elements; only the leading float element participates in the
 * store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading bfloat16_t element of a bfloat16_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUbf16_vbf16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds bfloat16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_bfloat16_t register of bfloat16_t elements; only the leading bfloat16_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e4m3fn_t element of an fp8_e4m3fn_t vector data register to the UB address dst shifted
 * by an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e4m3_vf8e4m3_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e4m3fn_t elements, one byte
 * each; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements; only the leading fp8_e4m3fn_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading hifloat8_t element of a hifloat8_t vector data register to the UB address dst shifted by an
 * element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUhf8_vhf8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds hifloat8_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_hifloat8_t register of hifloat8_t elements; only the leading hifloat8_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e5m2_t element of an fp8_e5m2_t vector data register to the UB address dst shifted by
 * an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e5m2_vf8e5m2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e5m2_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e5m2_t register of fp8_e5m2_t elements; only the leading fp8_e5m2_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e8m0_t element of an fp8_e8m0_t vector data register to the UB address dst shifted by
 * an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e8m0_vf8e8m0_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e8m0_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e8m0_t register of fp8_e8m0_t elements; only the leading fp8_e8m0_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, int32_t offset);
__simd_callee__ inline void asc_storealign_1st_postupdate(
    __ubuf__ fp8_e8m0_t*& dst, vector_fp8_e8m0_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e2m1_t vector data register to the UB address dst shifted by
 * an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e2m1x2_vf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e1m2_t vector data register to the UB address dst shifted by
 * an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e1m2x2_vf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed int4x2 unit (two 4-bit elements) of an int4b_t vector data register to the UB
 * address dst shifted by an element-unit int32_t offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi4x2_vi4x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed int4x2 units, two 4-bit
 * elements per byte; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int4x2_t register of int4b_t elements; only the leading packed int4x2 unit (two 4-bit
 * elements) participates in the store and the remaining elements are ignored.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi16_vi16_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int16_t* dst, vector_int16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu16_vu16_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint16_t* dst, vector_uint16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int32_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu32_vu32_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint32_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi64_vi64_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int64_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int64_t* dst, vector_int64_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu64_vu64_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint64_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint64_t* dst, vector_uint64_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each half element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf16_vf16_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_half); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each float element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions of
 * mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf32_vf32_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_float); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each bfloat16_t element of the vector register src into consecutive
 * elements at the 32-byte-aligned UB address dst starting offset elements after dst, leaving the destination positions
 * of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUbf16_vbf16_i32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_bfloat16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (int32_t); the offset-adjusted address must still
 * be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int8_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUi8_vi8_vi8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int8_t register of int8_t elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int8_t register of int8_t elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint8_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUu8_vu8_vu8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint8_t register of uint8_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint8_t register of uint8_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int16_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUi16_vi16_vi16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int16_t register of int16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int16_t register of int16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint16_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUu16_vu16_vu16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint16_t register of uint16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint16_t register of uint16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int32_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUi32_vi32_vi32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int32_t register of int32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int32_t register of int32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two float vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf32_vf32_vf32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds float elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_float register of float elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_float register of float elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ float* dst, vector_float src0, vector_float src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint32_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUu32_vu32_vu32_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint32_t register of uint32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint32_t register of uint32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two half vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf16_vf16_vf16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds half elements, two bytes
 * each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_half register of half elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_half register of half elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ half* dst, vector_half src0, vector_half src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two bfloat16_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUbf16_vbf16_vbf16_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds bfloat16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_bfloat16_t register of bfloat16_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_bfloat16_t register of bfloat16_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e4m3fn_t vector data registers and stores the 512-byte result at dst plus
 * an element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf8e4m3_vf8e4m3_vf8e4m3_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e4m3fn_t elements,
 * one byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two hifloat8_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUhf8_vhf8_vhf8_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds hifloat8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_hifloat8_t register of hifloat8_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_hifloat8_t register of hifloat8_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e5m2_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf8e5m2_vf8e5m2_vf8e5m2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e5m2_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e8m0_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf8e8m0_vf8e8m0_vf8e8m0_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e8m0_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e2m1_t vector data registers and stores the 512-byte result at dst plus
 * an element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf4e2m1x2_vf4e2m1x2_vf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e1m2_t vector data registers and stores the 512-byte result at dst plus
 * an element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUf4e1m2x2_vf4e1m2x2_vf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int4b_t vector data registers and stores the 512-byte result at dst plus an
 * element-unit int32_t offset.
 *
 * @override_id asc_storealign_intlv__PUi4x2_vi4x2_vi4x2_i32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed int4x2 units, two
 * 4-bit elements per byte; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int4x2_t register of int4b_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int4x2_t register of int4b_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Element offset added to dst before this store; the caller may advance either this offset or dst
 * between calls.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int8_t register to Unified Buffer at dst, overwriting only the mask-selected int8_t
 * elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi8_vi8_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int8_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int8_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(int8_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int8_t*& dst, vector_int8_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint8_t register to Unified Buffer at dst, overwriting only the mask-selected
 * uint8_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu8_vu8_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint8_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint8_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(uint8_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint8_t*& dst, vector_uint8_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int16_t register to Unified Buffer at dst, overwriting only the mask-selected
 * int16_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi16_vi16_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(int16_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint16_t register to Unified Buffer at dst, overwriting only the mask-selected
 * uint16_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu16_vu16_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(uint16_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int32_t register to Unified Buffer at dst, overwriting only the mask-selected
 * int32_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi32_vi32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int32_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int32_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(int32_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint32_t register to Unified Buffer at dst, overwriting only the mask-selected
 * uint32_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu32_vu32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint32_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint32_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(uint32_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_half register to Unified Buffer at dst, overwriting only the mask-selected half
 * elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf16_vf16_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ half*& pointer
 * so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type must match
 * src.
 * @param [in] src Source vector data register (vector_half) holding the 256 bytes to store; its element type must match
 * dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(half) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ half*& dst, vector_half src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_float register to Unified Buffer at dst, overwriting only the mask-selected float
 * elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf32_vf32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ float*& pointer
 * so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type must match
 * src.
 * @param [in] src Source vector data register (vector_float) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(float) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_bfloat16_t register to Unified Buffer at dst, overwriting only the mask-selected
 * bfloat16_t elements, then advances the dst pointer by offset elements (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUbf16_vbf16_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ bfloat16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_bfloat16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] offset Post Update increment of the destination pointer expressed in elements, so the pointer moves by
 * offset multiplied by sizeof(bfloat16_t) bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first int8_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUi8_vi8_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to int8_t, passed by reference: it must be aligned to sizeof(int8_t) for
 * the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of int8_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(int8_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int8_t*& dst, vector_int8_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first uint8_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUu8_vu8_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to uint8_t, passed by reference: it must be aligned to sizeof(uint8_t) for
 * the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of uint8_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(uint8_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint8_t*& dst, vector_uint8_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first int16_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUi16_vi16_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to int16_t, passed by reference: it must be aligned to sizeof(int16_t) for
 * the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of int16_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(int16_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first uint16_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUu16_vu16_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to uint16_t, passed by reference: it must be aligned to sizeof(uint16_t)
 * for the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of uint16_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(uint16_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first int32_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUi32_vi32_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to int32_t, passed by reference: it must be aligned to sizeof(int32_t) for
 * the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of int32_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(int32_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first uint32_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUu32_vu32_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to uint32_t, passed by reference: it must be aligned to sizeof(uint32_t)
 * for the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of uint32_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(uint32_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first half element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUf16_vf16_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to half, passed by reference: it must be aligned to sizeof(half) for the
 * store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of half whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(half) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ half*& dst, vector_half src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first float element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUf32_vf32_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to float, passed by reference: it must be aligned to sizeof(float) for the
 * store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of float whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(float) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__ float*& dst, vector_float src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first bfloat16_t element of a source vector data register to the UB address held in dst and then
 * post-updates dst by offset elements, without any mask configuration.
 *
 * @override_id asc_storealign_1st_postupdate__RPUbf16_vbf16_i32
 * @archs 3510
 * @param [in,out] dst Destination UB pointer to bfloat16_t, passed by reference: it must be aligned to
 * sizeof(bfloat16_t) for the store and it is automatically post-updated afterwards.
 * @param [in] src Source vector data register of bfloat16_t whose first element is written out.
 * @param [in] offset Address offset in elements: after the call the underlying UB address of dst moves forward by
 * offset * sizeof(bfloat16_t) bytes.
 */
__simd_callee__ inline void asc_storealign_1st_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected int16_t elements of a vector register into consecutive UB locations keeping only their
 * low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUi16_vi16_i32_vb
 * @archs 3510
 * @param [in,out] dst Pointer to the destination UB address, passed by reference; must start on a 32-byte boundary and
 * is automatically advanced by offset times the element size after the store.
 * @param [in] src Source vector data register of int16_t elements whose selected values are compressed and stored; read
 * only.
 * @param [in] offset Post-update step in elements; the destination address advances by offset multiplied by
 * sizeof(int16_t) bytes.
 * @param [in] mask Mask register marking elements to store; a 0 bit leaves the corresponding packed destination
 * position at its previous value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected uint16_t elements of a vector register into consecutive UB locations keeping only
 * their low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu16_vu16_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint16_t type; requires 32-byte alignment and is
 * advanced by offset times two bytes after the store.
 * @param [in] src Source vector data register of uint16_t elements whose selected values are packed out; read only.
 * @param [in] offset Post-update step counted in elements, converted by hardware into offset multiplied by
 * sizeof(uint16_t) bytes.
 * @param [in] mask Mask register selecting stored elements; positions with mask bit 0 keep their previous destination
 * value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected int32_t elements of a vector register into consecutive UB locations keeping only their
 * low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUi32_vi32_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of int32_t type; requires 32-byte alignment and is
 * advanced by offset times four bytes after the store.
 * @param [in] src Source vector data register of int32_t elements whose selected values are compressed to memory; read
 * only.
 * @param [in] offset Post-update step counted in elements; the pointer advances by offset times sizeof(int32_t) bytes.
 * @param [in] mask Mask register selecting the stored elements; a 0 bit leaves the corresponding packed destination
 * position at its prior value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected uint32_t elements of a vector register into consecutive UB locations keeping only
 * their low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu32_vu32_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint32_t type; must be 32-byte aligned and is advanced
 * by offset times four bytes after the store.
 * @param [in] src Source vector data register of uint32_t elements whose selected values are packed out; read only.
 * @param [in] offset Post-update step in elements, converted by hardware to offset multiplied by sizeof(uint32_t)
 * bytes.
 * @param [in] mask Mask register selecting participating elements; positions with mask bit 0 keep their previous
 * destination value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected int64_t elements of a vector register into consecutive UB locations keeping only their
 * low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUi64_vi64_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of int64_t type; must be 32-byte aligned and is advanced
 * by offset times eight bytes after the store.
 * @param [in] src Source vector data register of int64_t elements whose selected values are packed out; read only.
 * @param [in] offset Post-update step counted in elements; the pointer advances by offset times sizeof(int64_t) bytes.
 * @param [in] mask Mask register selecting stored elements; positions with mask bit 0 keep their previous destination
 * value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ int64_t*& dst, vector_int64_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected uint64_t elements of a vector register into consecutive UB locations keeping only
 * their low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu64_vu64_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint64_t type; requires 32-byte alignment and is
 * advanced by offset times eight bytes after the store.
 * @param [in] src Source vector data register of uint64_t elements whose selected values are compressed to memory; read
 * only.
 * @param [in] offset Post-update step expressed in elements; the pointer advances by offset times sizeof(uint64_t)
 * bytes.
 * @param [in] mask Mask register marking participating elements; a 0 bit leaves the corresponding packed destination
 * position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ uint64_t*& dst, vector_uint64_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected half elements of a vector register into consecutive UB locations keeping only their
 * low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUf16_vf16_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of half type; must be 32-byte aligned and is advanced by
 * offset times sizeof(half) bytes after the store.
 * @param [in] src Source vector data register of half elements whose mask-selected values are compressed to memory;
 * read only.
 * @param [in] offset Post-update step expressed in elements; the pointer advances by offset times the two-byte element
 * size.
 * @param [in] mask Mask register selecting the elements to store; a cleared bit leaves the corresponding packed
 * destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ half*& dst, vector_half src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected float elements of a vector register into consecutive UB locations keeping only their
 * low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUf32_vf32_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of float type; requires 32-byte alignment and is advanced
 * by offset times four bytes after the store.
 * @param [in] src Source vector data register of float elements whose selected values are compressed to memory; read
 * only.
 * @param [in] offset Post-update step expressed in elements; the pointer advances by offset times sizeof(float) bytes.
 * @param [in] mask Mask register selecting the elements to store; a cleared bit leaves the corresponding packed
 * destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected bfloat16_t elements of a vector register into consecutive UB locations keeping only
 * their low half bits, then advances the destination pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate__RPUbf16_vbf16_i32_vb
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of bfloat16_t type; requires 32-byte alignment and is
 * advanced by offset times the element size after the store.
 * @param [in] src Source vector data register of bfloat16_t elements whose selected values are packed out; read only.
 * @param [in] offset Post-update step in elements, translated by the hardware into offset multiplied by
 * sizeof(bfloat16_t) bytes.
 * @param [in] mask Mask register indicating which elements are stored; positions whose mask bit is 0 keep their
 * previous value.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int8_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi8_vi8_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int8_t register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the int8_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst, vector_int8_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint8_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu8_vu8_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint8_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint8_t* dst, vector_uint8_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int16_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi16_vi16_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int16_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint16_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu16_vu16_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint16_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int32_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi32_vi32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int32_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_uint32_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUu32_vu32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint32_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding uint32_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_half register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf16_vf16_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ half*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_half register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the half elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding half
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ half* dst, vector_half src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_float register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf32_vf32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ float*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_float register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the float elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding float
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_bfloat16_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUbf16_vbf16_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ bfloat16_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_bfloat16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the bfloat16_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding bfloat16_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e4m3fn_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e4m3_vf8e4m3_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e4m3fn_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e4m3fn_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp8_e4m3fn_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp8_e4m3fn_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_hifloat8_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUhf8_vhf8_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ hifloat8_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_hifloat8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the hifloat8_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding hifloat8_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e5m2_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e5m2_vf8e5m2_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e5m2_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e5m2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e5m2_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e5m2_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp8_e8m0_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf8e8m0_vf8e8m0_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e8m0_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp8_e8m0_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e8m0_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding fp8_e8m0_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e2m1_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf4e2m1x2_vf4e2m1x2_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e2m1_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e2m1_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e2m1_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e2m1_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_fp4x2_e1m2_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUf4e1m2x2_vf4e1m2x2_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp4x2_e1m2_t*; it must be 32-byte aligned and the
 * accessed range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_fp4x2_e1m2_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp4x2_e1m2_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding
 * fp4x2_e1m2_t element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one VL (256 bytes) of the vector_int4x2_t register src to UB at dst plus the offset carried by the
 * addr_reg, writing only the mask-marked elements and keeping the remaining destination data.
 *
 * @override_id asc_storealign__PUi4x2_vi4x2_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int4b_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int4x2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int4b_t elements of dst.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 * @param [in] mask Source mask register that marks the elements to store; a set bit writes the corresponding int4b_t
 * element and a cleared bit keeps the destination data at that position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int4b_t* dst, vector_int4x2_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int8_t element of an int8_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi8_vi8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int8_t register of int8_t elements; only the leading int8_t element participates in the
 * store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst, vector_int8_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint8_t element of a uint8_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu8_vu8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint8_t elements, one byte each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint8_t register of uint8_t elements; only the leading uint8_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint8_t* dst, vector_uint8_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int16_t element of an int16_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi16_vi16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int16_t elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int16_t register of int16_t elements; only the leading int16_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint16_t element of a uint16_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu16_vu16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint16_t register of uint16_t elements; only the leading uint16_t element participates
 * in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading int32_t element of an int32_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi32_vi32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds int32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int32_t register of int32_t elements; only the leading int32_t element participates in
 * the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading uint32_t element of a uint32_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUu32_vu32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds uint32_t elements, four bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_uint32_t register of uint32_t elements; only the leading uint32_t element participates
 * in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading half element of a half vector data register to the UB address dst shifted by an addr_reg
 * offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf16_vf16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds half elements, two bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_half register of half elements; only the leading half element participates in the store
 * and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ half* dst, vector_half src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading float element of a float vector data register to the UB address dst shifted by an addr_reg
 * offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf32_vf32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds float elements, four bytes each; it
 * must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_float register of float elements; only the leading float element participates in the
 * store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ float* dst, vector_float src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading bfloat16_t element of a bfloat16_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUbf16_vbf16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds bfloat16_t elements, two bytes each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_bfloat16_t register of bfloat16_t elements; only the leading bfloat16_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e4m3fn_t element of an fp8_e4m3fn_t vector data register to the UB address dst shifted
 * by an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e4m3_vf8e4m3_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e4m3fn_t elements, one byte
 * each; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements; only the leading fp8_e4m3fn_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading hifloat8_t element of a hifloat8_t vector data register to the UB address dst shifted by an
 * addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUhf8_vhf8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds hifloat8_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_hifloat8_t register of hifloat8_t elements; only the leading hifloat8_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e5m2_t element of an fp8_e5m2_t vector data register to the UB address dst shifted by
 * an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e5m2_vf8e5m2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e5m2_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e5m2_t register of fp8_e5m2_t elements; only the leading fp8_e5m2_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading fp8_e8m0_t element of an fp8_e8m0_t vector data register to the UB address dst shifted by
 * an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf8e8m0_vf8e8m0_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds fp8_e8m0_t elements, one byte each;
 * it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp8_e8m0_t register of fp8_e8m0_t elements; only the leading fp8_e8m0_t element
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e2m1_t vector data register to the UB address dst shifted by
 * an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e2m1x2_vf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed 4-bit pair of an fp4x2_e1m2_t vector data register to the UB address dst shifted by
 * an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUf4e1m2x2_vf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed 4-bit pairs in the b8 width
 * class; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements; only the leading packed 4-bit pair
 * participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the leading packed int4x2 unit (two 4-bit elements) of an int4b_t vector data register to the UB
 * address dst shifted by an addr_reg offset; no mask is applied.
 *
 * @override_id asc_storealign_1st__PUi4x2_vi4x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the destination, which holds packed int4x2 units, two 4-bit
 * elements per byte; it must be dtype-aligned and stay inside the UB address space.
 * @param [in] src Source vector_int4x2_t register of int4b_t elements; only the leading packed int4x2 unit (two 4-bit
 * elements) participates in the store and the remaining elements are ignored.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int4b_t* dst, vector_int4x2_t src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi16_vi16_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int16_t* dst, vector_int16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint16_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu16_vu16_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint16_t* dst, vector_uint16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi32_vi32_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int32_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint32_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu32_vu32_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint32_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each int64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUi64_vi64_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_int64_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ int64_t* dst, vector_int64_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each uint64_t element of the vector register src into consecutive elements
 * at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination
 * positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUu64_vu64_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_uint64_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ uint64_t* dst, vector_uint64_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each half element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination positions
 * of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf16_vf16_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_half); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ half* dst, vector_half src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each float element of the vector register src into consecutive elements at
 * the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the destination positions
 * of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUf32_vf32_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_float); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the mask-selected low half of each bfloat16_t element of the vector register src into consecutive
 * elements at the 32-byte-aligned UB address dst starting at the addr_reg offset relative to dst, leaving the
 * destination positions of mask-cleared elements unchanged.
 *
 * @override_id asc_storealign_pack__PUbf16_vbf16_ar_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the packed destination; the start address must be 32-byte aligned and the
 * offset-adjusted address must stay 32-byte aligned and inside UB.
 * @param [in] src Source vector data register (vector_bfloat16_t); its dtype must match the data type dst points to.
 * @param [in] offset Offset relative to the start of dst, in elements (addr_reg); the offset-adjusted address must
 * still be 32-byte aligned inside UB.
 * @param [in] mask Mask register marking the elements that participate; a set bit writes the low half of that element
 * to the packed destination position and a cleared bit leaves that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int8_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUi8_vi8_vi8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int8_t register of int8_t elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int8_t register of int8_t elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int8_t* dst, vector_int8_t src0, vector_int8_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint8_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUu8_vu8_vu8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint8_t register of uint8_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint8_t register of uint8_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint8_t* dst, vector_uint8_t src0, vector_uint8_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int16_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUi16_vi16_vi16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int16_t register of int16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int16_t register of int16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int16_t* dst, vector_int16_t src0, vector_int16_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint16_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUu16_vu16_vu16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint16_t register of uint16_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint16_t register of uint16_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint16_t* dst, vector_uint16_t src0, vector_uint16_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int32_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUi32_vi32_vi32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds int32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int32_t register of int32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int32_t register of int32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int32_t* dst, vector_int32_t src0, vector_int32_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two float vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf32_vf32_vf32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds float elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_float register of float elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_float register of float elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ float* dst, vector_float src0, vector_float src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two uint32_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUu32_vu32_vu32_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds uint32_t elements, four
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_uint32_t register of uint32_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_uint32_t register of uint32_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ uint32_t* dst, vector_uint32_t src0, vector_uint32_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two half vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf16_vf16_vf16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds half elements, two bytes
 * each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_half register of half elements, supplying the even-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_half register of half elements, supplying the odd-position elements of the
 * interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ half* dst, vector_half src0, vector_half src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two bfloat16_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUbf16_vbf16_vbf16_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds bfloat16_t elements, two
 * bytes each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_bfloat16_t register of bfloat16_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_bfloat16_t register of bfloat16_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src0, vector_bfloat16_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e4m3fn_t vector data registers and stores the 512-byte result at dst plus
 * an addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf8e4m3_vf8e4m3_vf8e4m3_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e4m3fn_t elements,
 * one byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e4m3fn_t register of fp8_e4m3fn_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two hifloat8_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUhf8_vhf8_vhf8_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds hifloat8_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_hifloat8_t register of hifloat8_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_hifloat8_t register of hifloat8_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src0, vector_hifloat8_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e5m2_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf8e5m2_vf8e5m2_vf8e5m2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e5m2_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e5m2_t register of fp8_e5m2_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp8_e8m0_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf8e8m0_vf8e8m0_vf8e8m0_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds fp8_e8m0_t elements, one
 * byte each; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the even-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp8_e8m0_t register of fp8_e8m0_t elements, supplying the odd-position elements
 * of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e2m1_t vector data registers and stores the 512-byte result at dst plus
 * an addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf4e2m1x2_vf4e2m1x2_vf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e2m1_t register of fp4x2_e2m1_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two fp4x2_e1m2_t vector data registers and stores the 512-byte result at dst plus
 * an addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUf4e1m2x2_vf4e1m2x2_vf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed 4-bit pairs in
 * the b8 width class; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the even-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_fp4x2_e1m2_t register of fp4x2_e1m2_t elements, supplying the odd-position
 * elements of the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Interleaves the elements of two int4b_t vector data registers and stores the 512-byte result at dst plus an
 * addr_reg offset.
 *
 * @override_id asc_storealign_intlv__PUi4x2_vi4x2_vi4x2_ar
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer of the interleaved destination, which holds packed int4x2 units, two
 * 4-bit elements per byte; it must be 32-byte aligned and stay inside the UB address space.
 * @param [in] src0 First source vector_int4x2_t register of int4b_t elements, supplying the even-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] src1 Second source vector_int4x2_t register of int4b_t elements, supplying the odd-position elements of
 * the interleaved result; its dtype must match dst.
 * @param [in] offset Offset carried by an addr_reg previously built with the address-register construction interface
 * (asc_update_addr_reg), applied relative to dst.
 */
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ int4b_t* dst, vector_int4x2_t src0, vector_int4x2_t src1, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_int8_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUi8_vi8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int8_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int8_t register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the int8_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int8_t* dst, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_uint8_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUu8_vu8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint8_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint8_t* dst, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_int16_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUi16_vi16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int16_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int16_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int16_t* dst, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_uint16_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUu16_vu16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint16_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint16_t* dst, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_int32_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUi32_vi32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ int32_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_int32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the int32_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ int32_t* dst, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_uint32_t register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUu32_vu32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_uint32_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the uint32_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ uint32_t* dst, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_bfloat16_t register src to UB starting at the
 * 32-byte-aligned dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with
 * only the mask-marked elements written.
 *
 * @override_id asc_storealign__PUbf16_vbf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ bfloat16_t*; it must be 32-byte aligned and the
 * accessed range derived from the stride parameters must still lie inside the usable UB capacity or the call errors
 * out.
 * @param [in] src Source vector_bfloat16_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the bfloat16_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ bfloat16_t* dst, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_fp8_e4m3fn_t register src to UB starting at the
 * 32-byte-aligned dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with
 * only the mask-marked elements written.
 *
 * @override_id asc_storealign__PUf8e4m3_vf8e4m3_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e4m3fn_t*; it must be 32-byte aligned and the
 * accessed range derived from the stride parameters must still lie inside the usable UB capacity or the call errors
 * out.
 * @param [in] src Source vector_fp8_e4m3fn_t register; one VL of 256 bytes is stored for the vector case and the
 * element type must match the fp8_e4m3fn_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_hifloat8_t register src to UB starting at the
 * 32-byte-aligned dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with
 * only the mask-marked elements written.
 *
 * @override_id asc_storealign__PUhf8_vhf8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ hifloat8_t*; it must be 32-byte aligned and the
 * accessed range derived from the stride parameters must still lie inside the usable UB capacity or the call errors
 * out.
 * @param [in] src Source vector_hifloat8_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the hifloat8_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ hifloat8_t* dst, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_fp8_e5m2_t register src to UB starting at the
 * 32-byte-aligned dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with
 * only the mask-marked elements written.
 *
 * @override_id asc_storealign__PUf8e5m2_vf8e5m2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e5m2_t*; it must be 32-byte aligned and the
 * accessed range derived from the stride parameters must still lie inside the usable UB capacity or the call errors
 * out.
 * @param [in] src Source vector_fp8_e5m2_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e5m2_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_fp8_e8m0_t register src to UB starting at the
 * 32-byte-aligned dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with
 * only the mask-marked elements written.
 *
 * @override_id asc_storealign__PUf8e8m0_vf8e8m0_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ fp8_e8m0_t*; it must be 32-byte aligned and the
 * accessed range derived from the stride parameters must still lie inside the usable UB capacity or the call errors
 * out.
 * @param [in] src Source vector_fp8_e8m0_t register; one VL of 256 bytes is stored for the vector case and the element
 * type must match the fp8_e8m0_t elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_half register src to UB starting at the 32-byte-aligned dst,
 * stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the mask-marked
 * elements written.
 *
 * @override_id asc_storealign__PUf16_vf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ half*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_half register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the half elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ half* dst, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the eight 32-byte DataBlocks of the vector_float register src to UB starting at the 32-byte-aligned
 * dst, stepping block_stride units between adjacent DataBlocks and repeat_stride units from dst, with only the
 * mask-marked elements written.
 *
 * @override_id asc_storealign__PUf32_vf32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ float*; it must be 32-byte aligned and the accessed
 * range derived from the stride parameters must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source vector_float register; one VL of 256 bytes is stored for the vector case and the element type
 * must match the float elements of dst.
 * @param [in] block_stride Distance between the start addresses of adjacent destination DataBlocks, counted in 32-byte
 * DataBlock units.
 * @param [in] repeat_stride Offset of the destination start address relative to dst, counted in 32-byte DataBlock
 * units.
 * @param [in] mask Source mask register that marks the elements to store; the operation moves whole 32-byte DataBlocks
 * and a cleared bit keeps the destination data at that element position.
 */
__simd_callee__ inline void asc_storealign(
    __ubuf__ float* dst, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int8_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi8_vi8_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int8_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int8_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int8_t*& dst, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint8_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu8_vu8_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint8_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint8_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint8_t*& dst, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int16_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi16_vi16_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int16_t*& dst, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint16_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu16_vu16_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint16_t*& dst, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_int32_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUi32_vi32_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ int32_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_int32_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_uint32_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu32_vu32_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint32_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_uint32_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_bfloat16_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUbf16_vbf16_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ bfloat16_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_bfloat16_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_fp8_e4m3fn_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf8e4m3_vf8e4m3_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ fp8_e4m3fn_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_fp8_e4m3fn_t) holding the 256 bytes to store; its element type
 * must match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e4m3fn_t*& dst, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_hifloat8_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUhf8_vhf8_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ hifloat8_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_hifloat8_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ hifloat8_t*& dst, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_fp8_e5m2_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf8e5m2_vf8e5m2_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ fp8_e5m2_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_fp8_e5m2_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e5m2_t*& dst, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_fp8_e8m0_t register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf8e8m0_vf8e8m0_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ fp8_e8m0_t*&
 * pointer so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type
 * must match src.
 * @param [in] src Source vector data register (vector_fp8_e8m0_t) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e8m0_t*& dst, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_half register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by block_stride,
 * then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf16_vf16_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ half*& pointer
 * so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type must match
 * src.
 * @param [in] src Source vector data register (vector_half) holding the 256 bytes to store; its element type must match
 * dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ half*& dst, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores one 256-byte vector_float register to Unified Buffer as 8 DataBlocks of 32 bytes spaced by
 * block_stride, then advances the dst pointer by repeat_stride 32-byte units (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUf32_vf32_u16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ float*& pointer
 * so the interface can rewrite the pointer itself; the address must be 32-byte aligned and the element type must match
 * src.
 * @param [in] src Source vector data register (vector_float) holding the 256 bytes to store; its element type must
 * match dst.
 * @param [in] block_stride Step between the start addresses of adjacent destination DataBlocks, in units of 32 bytes;
 * one instruction stores 8 such DataBlocks.
 * @param [in] repeat_stride Post Update increment applied to the destination pointer, in units of 32 bytes.
 * @param [in] mask Mask register selecting which elements are stored; elements whose bit is 0 keep the previous content
 * of the destination.
 */
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ float*& dst, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus the offset carried by the addr_reg,
 * with data format and content unchanged.
 *
 * @override_id asc_storealign__PUu32_vb_ar
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting at the addr_reg offset relative to dst.
 *
 * @override_id asc_storealign_pack__PUu32_vb_ar
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (addr_reg) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus the offset carried by the addr_reg,
 * with data format and content unchanged.
 *
 * @override_id asc_storealign__PUu16_vb_ar
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting at the addr_reg offset relative to dst.
 *
 * @override_id asc_storealign_pack__PUu16_vb_ar
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (addr_reg) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus the offset carried by the addr_reg,
 * with data format and content unchanged.
 *
 * @override_id asc_storealign__PUu8_vb_ar
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Offset carried by an addr_reg that must first be built with the address-register construction API
 * (asc_update_addr_reg_b8/b16/b32); the paired address-register page defines its step in bytes (1, 2 or 4 bytes
 * according to the mode), so the register has to be built in a mode matching the stored element width, and the
 * resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting at the addr_reg offset relative to dst.
 *
 * @override_id asc_storealign_pack__PUu8_vb_ar
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (addr_reg) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus offset bytes, with data format and
 * content unchanged.
 *
 * @override_id asc_storealign__PUu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst.
 *
 * @override_id asc_storealign_pack__PUu32_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to the 32-byte-aligned UB address dst with data
 * format and content unchanged.
 *
 * @override_id asc_storealign__PUu16_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst.
 *
 * @override_id asc_storealign_pack__PUu16_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to the 32-byte-aligned UB address dst with data
 * format and content unchanged.
 *
 * @override_id asc_storealign__PUu8_vb
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst.
 *
 * @override_id asc_storealign_pack__PUu8_vb
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus offset bytes, with data format and
 * content unchanged.
 *
 * @override_id asc_storealign__PUu32_vb_i32
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint32_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in bytes (Constraints,
 * mask-store scene); the resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting offset bytes after dst.
 *
 * @override_id asc_storealign_pack__PUu32_vb_i32
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (int32_t) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus offset bytes, with data format and
 * content unchanged.
 *
 * @override_id asc_storealign__PUu16_vb_i32
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint16_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in bytes (Constraints,
 * mask-store scene); the resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting offset bytes after dst.
 *
 * @override_id asc_storealign_pack__PUu16_vb_i32
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (int32_t) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the full 32-byte content of the mask register src to UB at dst plus offset bytes, with data format and
 * content unchanged.
 *
 * @override_id asc_storealign__PUu8_vb_i32
 * @archs 3510
 * @param [out] dst Destination UB start address of type __ubuf__ uint8_t*; it must be 32-byte aligned and the accessed
 * range after the offset must still lie inside the usable UB capacity or the call errors out.
 * @param [in] src Source mask register (vector_bool); its full 32-byte content is stored as-is and this mode takes no
 * separate mask argument.
 * @param [in] offset Immediate signed offset relative to the dst start address, counted in bytes (Constraints,
 * mask-store scene); the resulting address must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src by discarding every other bit and stores the retained bits
 * contiguously at the 16-byte-aligned UB address dst starting offset bytes after dst.
 *
 * @override_id asc_storealign_pack__PUu8_vb_i32
 * @archs 3510
 * @param [out] dst Start address in UB of the compressed mask output; the start address must be 16-byte aligned and the
 * offset-adjusted address must stay 16-byte aligned inside UB.
 * @param [in] src Source mask register (vector_bool) whose 32 bytes are compressed by keeping one bit and dropping the
 * next; this mode does not accept a mask operand.
 * @param [in] offset Offset relative to the start of dst; it is counted in bytes (int32_t) and the offset-adjusted
 * address must remain 16-byte aligned inside UB.
 */
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the 32-byte mask register src to Unified Buffer at dst through a uint32_t pointer view, then advances
 * the dst pointer by offset bytes (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu32_vb_i32
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint32_t*&
 * pointer so the interface can rewrite it; the address must be 32-byte aligned.
 * @param [in] src Source mask register (vector_bool) whose 32-byte payload is stored; this mask-register mode takes no
 * mask parameter.
 * @param [in] offset Post Update increment of the destination pointer in bytes, so the underlying Unified Buffer
 * address moves by offset bytes.
 */
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses a mask register into a UB uint32_t-addressed buffer by dropping every second bit, then advances the
 * destination pointer by offset bytes.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu32_vb_i32
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint32_t type; must be 16-byte aligned and is advanced
 * by offset bytes after the store.
 * @param [in] src Source mask register whose bits are compressed one out of every two; read only, and this mode
 * supports no mask parameter.
 * @param [in] offset Post-update step in bytes; the pointer address is increased by offset bytes once the store
 * completes.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the 32-byte mask register src to Unified Buffer at dst through a uint16_t pointer view, then advances
 * the dst pointer by offset bytes (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu16_vb_i32
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint16_t*&
 * pointer so the interface can rewrite it; the address must be 32-byte aligned.
 * @param [in] src Source mask register (vector_bool) whose 32-byte payload is stored; this mask-register mode takes no
 * mask parameter.
 * @param [in] offset Post Update increment of the destination pointer in bytes, so the underlying Unified Buffer
 * address moves by offset bytes.
 */
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses a mask register into a UB uint16_t-addressed buffer by dropping every second bit, then advances the
 * destination pointer by offset bytes.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu16_vb_i32
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint16_t type; requires 16-byte alignment and is
 * advanced by offset bytes after the store.
 * @param [in] src Source mask register compressed by discarding every other bit; read only, with no mask configuration
 * available in this mode.
 * @param [in] offset Post-update step in bytes; the underlying UB address of dst is increased by offset bytes.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the 32-byte mask register src to Unified Buffer at dst through a uint8_t pointer view, then advances
 * the dst pointer by offset bytes (Post Update).
 *
 * @override_id asc_storealign_postupdate__RPUu8_vb_i32
 * @archs 3510
 * @param [in,out] dst Destination start address in Unified Buffer, taken as a reference to the __ubuf__ uint8_t*&
 * pointer so the interface can rewrite it; the address must be 32-byte aligned.
 * @param [in] src Source mask register (vector_bool) whose 32-byte payload is stored; this mask-register mode takes no
 * mask parameter.
 * @param [in] offset Post Update increment of the destination pointer in bytes, so the underlying Unified Buffer
 * address moves by offset bytes.
 */
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses a mask register into a UB byte buffer by dropping every second bit, then advances the destination
 * pointer by offset bytes.
 *
 * @override_id asc_storealign_pack_postupdate__RPUu8_vb_i32
 * @archs 3510
 * @param [in,out] dst Reference to the destination UB pointer of uint8_t type; must be 16-byte aligned and is advanced
 * by offset bytes after the store.
 * @param [in] src Source mask register whose bits are compressed by discarding every other bit; read only, and no mask
 * can be configured in this mode.
 * @param [in] offset Post-update step in bytes; the underlying UB address of dst is increased by exactly offset bytes.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked int32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ int32_t*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_int32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked uint32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ uint32_t*) of the compressed data; it must be 32-byte
 * aligned.
 * @param [in] src Source vector data register (vector_uint32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked float elements of a vector data register into a quarter-width UB region, writing the low
 * 8 bits of each masked element at the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ float*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_float) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ float* dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked int32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ int32_t*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_int32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an int32_t immediate whose post-offset
 * address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ int32_t* dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked uint32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUu32_vu32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ uint32_t*) of the compressed data; it must be 32-byte
 * aligned.
 * @param [in] src Source vector data register (vector_uint32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an int32_t immediate whose post-offset
 * address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked float elements of a vector data register into a quarter-width UB region, writing the low
 * 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUf32_vf32_i32_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ float*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_float) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an int32_t immediate whose post-offset
 * address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ float* dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked int32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUi32_vi32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ int32_t*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_int32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an addr_reg value that must be pre-built by
 * the matching address-register constructor and used with asc_update_addr_reg.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ int32_t* dst, vector_int32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked uint32_t elements of a vector data register into a quarter-width UB region, writing the
 * low 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUu32_vu32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ uint32_t*) of the compressed data; it must be 32-byte
 * aligned.
 * @param [in] src Source vector data register (vector_uint32_t) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an addr_reg value that must be pre-built by
 * the matching address-register constructor and used with asc_update_addr_reg.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ uint32_t* dst, vector_uint32_t src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the masked float elements of a vector data register into a quarter-width UB region, writing the low
 * 8 bits of each masked element at an element offset from the dst start address.
 *
 * @override_id asc_storealign_pack_quarter__PUf32_vf32_ar_vb
 * @archs 3510
 * @param [out] dst Destination UB start address (__ubuf__ float*) of the compressed data; it must be 32-byte aligned.
 * @param [in] src Source vector data register (vector_float) whose dtype must match dst; each masked 32-bit element
 * contributes its low 8 bits.
 * @param [in] offset Offset relative to the dst start address, in elements: an addr_reg value that must be pre-built by
 * the matching address-register constructor and used with asc_update_addr_reg.
 * @param [in] mask Mask register indicating the elements that take part: bit 1 writes the low 8 bits of the element at
 * the corresponding packed destination position and bit 0 keeps that destination position unchanged.
 */
__simd_callee__ inline void asc_storealign_pack_quarter(
    __ubuf__ float* dst, vector_float src, addr_reg offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the low 8 bits of the masked 32-bit elements of src into consecutive bytes at dst and then advances the
 * dst pointer by offset elements.
 *
 * @override_id asc_storealign_pack_quarter_postupdate__RPUi32_vi32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination UB pointer passed by reference; it must be 32-byte aligned, receives the packed bytes
 * and is advanced by offset elements after the store.
 * @param [in] src int32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements: after the store the destination address advances by offset *
 * sizeof(dtype) bytes.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored and keep
 * the previous destination content.
 */
__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ int32_t*& dst, vector_int32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the low 8 bits of the masked 32-bit elements of src into consecutive bytes at dst and then advances the
 * dst pointer by offset elements.
 *
 * @override_id asc_storealign_pack_quarter_postupdate__RPUu32_vu32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination UB pointer passed by reference; it must be 32-byte aligned, receives the packed bytes
 * and is advanced by offset elements after the store.
 * @param [in] src uint32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements: after the store the destination address advances by offset *
 * sizeof(dtype) bytes.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored and keep
 * the previous destination content.
 */
__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ uint32_t*& dst, vector_uint32_t src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Packs the low 8 bits of the masked 32-bit elements of src into consecutive bytes at dst and then advances the
 * dst pointer by offset elements.
 *
 * @override_id asc_storealign_pack_quarter_postupdate__RPUf32_vf32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination UB pointer passed by reference; it must be 32-byte aligned, receives the packed bytes
 * and is advanced by offset elements after the store.
 * @param [in] src float source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements: after the store the destination address advances by offset *
 * sizeof(dtype) bytes.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored and keep
 * the previous destination content.
 */
__simd_callee__ inline void asc_storealign_pack_quarter_postupdate(
    __ubuf__ float*& dst, vector_float src, int32_t offset, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the valid data length currently stored in the AR special register after asc_squeeze_and_storeunalign to
 * the Unified Buffer address dst.
 *
 * @override_id asc_storealign_squeeze_status__PUu32
 * @archs 3510
 * @param [out] dst Destination base address in Unified Buffer where the accumulated valid byte count is written; it
 * must be 4-byte aligned.
 */
__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the valid data length currently stored in the AR special register after asc_squeeze_and_storeunalign to
 * the Unified Buffer at dst plus the byte offset.
 *
 * @override_id asc_storealign_squeeze_status__PUu32_i32
 * @archs 3510
 * @param [out] dst Destination base address in Unified Buffer; the value is written at dst plus offset and the address
 * must be 4-byte aligned.
 * @param [in] offset Signed byte offset relative to dst; the actual write address is dst + offset and 0 reproduces the
 * no-offset overload.
 */
__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the accumulated valid-data length from the AR register into UB and post-updates the destination pointer
 * by a byte offset.
 *
 * @override_id asc_storealign_squeeze_status_postupdate__RPUu32_i32
 * @archs 3510
 * @param [in,out] dst Base address in UB of type __ubuf__ uint32_t*& : the current AR value is written at this address
 * and the pointer is then advanced by offset bytes; both the initial and every post-updated address must be 4-byte
 * aligned and stay within the usable UB range.
 * @param [in] offset Destination address update amount of type int32_t, in bytes.
 */
__simd_callee__ inline void asc_storealign_squeeze_status_postupdate(__ubuf__ uint32_t*& dst, int32_t offset);

#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, vector_bool mask);
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, int32_t offset, vector_bool mask);
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, addr_reg offset, vector_bool mask);
__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);
__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_fp8_e6m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src);
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, int32_t offset);
__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, addr_reg offset);
__simd_callee__ inline void asc_storealign_1st_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_fp8_e6m2_t src, int32_t offset);

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1);
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1, int32_t offset);
__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1, addr_reg offset);
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit int32_t elements of src into
 * consecutive bytes at dst_align32b with the caller maintaining the address.
 *
 * @override_id asc_storealign_pack_v2__PUi32_vi32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src int32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit uint32_t elements of src into
 * consecutive bytes at dst_align32b with the caller maintaining the address.
 *
 * @override_id asc_storealign_pack_v2__PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src uint32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit float elements of src into
 * consecutive bytes at dst_align32b with the caller maintaining the address.
 *
 * @override_id asc_storealign_pack_v2__PUf32_vf32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src float source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit int32_t elements of src into
 * consecutive bytes at dst_align32b positioned by an int32_t element offset.
 *
 * @override_id asc_storealign_pack_v2__PUi32_vi32_i32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src int32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Offset relative to the dst_align32b start address, in elements; the shifted access address must
 * stay inside UB and remain 32-byte aligned.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit uint32_t elements of src into
 * consecutive bytes at dst_align32b positioned by an int32_t element offset.
 *
 * @override_id asc_storealign_pack_v2__PUu32_vu32_i32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src uint32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Offset relative to the dst_align32b start address, in elements; the shifted access address must
 * stay inside UB and remain 32-byte aligned.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit float elements of src into
 * consecutive bytes at dst_align32b positioned by an int32_t element offset.
 *
 * @override_id asc_storealign_pack_v2__PUf32_vf32_i32_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src float source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Offset relative to the dst_align32b start address, in elements; the shifted access address must
 * stay inside UB and remain 32-byte aligned.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store with post update: writes the low 8 bits of the masked 32-bit elements of src to
 * the UB pointer and lets the hardware advance that pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate_v2__RPUi32_vi32_i32_vb
 * @archs 3510
 * @param [in,out] dst_align32b Destination UB pointer passed by reference; it must be 32-byte aligned, receives the
 * packed bytes and is advanced by offset * sizeof(dtype) bytes after the store.
 * @param [in] src int32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements; the destination address is advanced by offset * sizeof(dtype)
 * bytes, and the new address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store with post update: writes the low 8 bits of the masked 32-bit elements of src to
 * the UB pointer and lets the hardware advance that pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate_v2__RPUu32_vu32_i32_vb
 * @archs 3510
 * @param [in,out] dst_align32b Destination UB pointer passed by reference; it must be 32-byte aligned, receives the
 * packed bytes and is advanced by offset * sizeof(dtype) bytes after the store.
 * @param [in] src uint32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements; the destination address is advanced by offset * sizeof(dtype)
 * bytes, and the new address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store with post update: writes the low 8 bits of the masked 32-bit elements of src to
 * the UB pointer and lets the hardware advance that pointer by offset elements.
 *
 * @override_id asc_storealign_pack_postupdate_v2__RPUf32_vf32_i32_vb
 * @archs 3510
 * @param [in,out] dst_align32b Destination UB pointer passed by reference; it must be 32-byte aligned, receives the
 * packed bytes and is advanced by offset * sizeof(dtype) bytes after the store.
 * @param [in] src float source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset Post-update amount in elements; the destination address is advanced by offset * sizeof(dtype)
 * bytes, and the new address must stay 32-byte aligned and inside UB.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit int32_t elements of src into
 * consecutive bytes at dst_align32b positioned by an addr_reg offset.
 *
 * @override_id asc_storealign_pack_v2__PUi32_vi32_ar_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src int32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset addr_reg address-register offset prepared beforehand with asc_update_addr_reg; it positions the
 * destination of the packed store.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, addr_reg offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit uint32_t elements of src into
 * consecutive bytes at dst_align32b positioned by an addr_reg offset.
 *
 * @override_id asc_storealign_pack_v2__PUu32_vu32_ar_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src uint32_t source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset addr_reg address-register offset prepared beforehand with asc_update_addr_reg; it positions the
 * destination of the packed store.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, addr_reg offset, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated quarter-pack store: compresses the low 8 bits of the masked 32-bit float elements of src into
 * consecutive bytes at dst_align32b positioned by an addr_reg offset.
 *
 * @override_id asc_storealign_pack_v2__PUf32_vf32_ar_vb
 * @archs 3510
 * @param [out] dst_align32b Start address of the destination UB buffer that receives the packed bytes; it must be
 * 32-byte aligned.
 * @param [in] src float source vector data register whose masked elements contribute their low 8 bits.
 * @param [in] offset addr_reg address-register offset prepared beforehand with asc_update_addr_reg; it positions the
 * destination of the packed store.
 * @param [in] mask Mask register marking the elements that participate; elements with a 0 bit are not stored.
 */
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, addr_reg offset, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREALIGN_H
#endif
