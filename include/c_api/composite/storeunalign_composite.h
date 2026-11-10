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
#error "composite/storeunalign_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/storeunalign_composite_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized int8_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUi8_vi8
 * @archs 3510
 * @param [out] dst Destination UB start address of the int8_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_int8_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized uint8_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUu8_vu8
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint8_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_uint8_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized fp4x2_e2m1_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf4e2m1x2_vf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp4x2_e2m1_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_fp4x2_e2m1_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized fp4x2_e1m2_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf4e1m2x2_vf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp4x2_e1m2_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_fp4x2_e1m2_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized int4b_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUi4x2_vi4x2
 * @archs 3510
 * @param [out] dst Destination UB start address of the int4b_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_int4x2_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized fp8_e8m0_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf8e8m0_vf8e8m0
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e8m0_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_fp8_e8m0_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized fp8_e5m2_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf8e5m2_vf8e5m2
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e5m2_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_fp8_e5m2_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized fp8_e4m3fn_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf8e4m3_vf8e4m3
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e4m3fn_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_fp8_e4m3fn_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized hifloat8_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUhf8_vhf8
 * @archs 3510
 * @param [out] dst Destination UB start address of the hifloat8_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_hifloat8_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized int16_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUi16_vi16
 * @archs 3510
 * @param [out] dst Destination UB start address of the int16_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_int16_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized uint16_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUu16_vu16
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint16_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_uint16_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized half vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf16_vf16
 * @archs 3510
 * @param [out] dst Destination UB start address of the half data; it must be dtype-aligned and the write covers one VL
 * = 256 bytes.
 * @param [in] src Source vector_half register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ half* dst, vector_half src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized bfloat16_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUbf16_vbf16
 * @archs 3510
 * @param [out] dst Destination UB start address of the bfloat16_t data; it must be dtype-aligned and the write covers
 * one VL = 256 bytes.
 * @param [in] src Source vector_bfloat16_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized int32_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUi32_vi32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int32_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_int32_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized uint32_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUu32_vu32
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint32_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_uint32_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized float vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUf32_vf32
 * @archs 3510
 * @param [out] dst Destination UB start address of the float data; it must be dtype-aligned and the write covers one VL
 * = 256 bytes.
 * @param [in] src Source vector_float register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the whole VL-sized int64_t vector register src to UB at the dtype-aligned address dst through the
 * unaligned store path, keeping the data format and content unchanged.
 *
 * @override_id asc_store__PUi64_vi64
 * @archs 3510
 * @param [out] dst Destination UB start address of the int64_t data; it must be dtype-aligned and the write covers one
 * VL = 256 bytes.
 * @param [in] src Source vector_int64_t register whose complete VL content is stored to dst.
 */
__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int8_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUi8_vi8_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int8_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_int8_t register whose first count int8_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of int8_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256] int8_t
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst, vector_int8_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint8_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUu8_vu8_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint8_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_uint8_t register whose first count uint8_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of uint8_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256] uint8_t
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint8_t* dst, vector_uint8_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int16_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUi16_vi16_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int16_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_int16_t register whose first count int16_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of int16_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 128] int16_t
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ int16_t* dst, vector_int16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint16_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUu16_vu16_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint16_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_uint16_t register whose first count uint16_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of uint16_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 128]
 * uint16_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint16_t* dst, vector_uint16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int32_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUi32_vi32_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int32_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_int32_t register whose first count int32_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of int32_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 64] int32_t
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ int32_t* dst, vector_int32_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint32_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUu32_vu32_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the uint32_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_uint32_t register whose first count uint32_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of uint32_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 64]
 * uint32_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ uint32_t* dst, vector_uint32_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count half elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf16_vf16_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the half data; it must be dtype-aligned and the written range covers
 * count elements.
 * @param [in] src Source vector_half register whose first count half elements are stored; the remaining elements are
 * ignored.
 * @param [in] count Number of half elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 128] half
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ half* dst, vector_half src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count float elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf32_vf32_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the float data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_float register whose first count float elements are stored; the remaining elements are
 * ignored.
 * @param [in] count Number of float elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 64] float
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ float* dst, vector_float src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int64_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUi64_vi64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int64_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_int64_t register whose first count int64_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of int64_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 32] int64_t
 * elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ int64_t* dst, vector_int64_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count bfloat16_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUbf16_vbf16_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the bfloat16_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_bfloat16_t register whose first count bfloat16_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of bfloat16_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 128]
 * bfloat16_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e8m0_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf8e8m0_vf8e8m0_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e8m0_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_fp8_e8m0_t register whose first count fp8_e8m0_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of fp8_e8m0_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256]
 * fp8_e8m0_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e5m2_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf8e5m2_vf8e5m2_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e5m2_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_fp8_e5m2_t register whose first count fp8_e5m2_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of fp8_e5m2_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256]
 * fp8_e5m2_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e4m3fn_t elements of the vector register src to UB at the dtype-aligned address
 * dst, leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf8e4m3_vf8e4m3_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp8_e4m3fn_t data; it must be dtype-aligned and the written
 * range covers count elements.
 * @param [in] src Source vector_fp8_e4m3fn_t register whose first count fp8_e4m3fn_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of fp8_e4m3fn_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256]
 * fp8_e4m3fn_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count hifloat8_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUhf8_vhf8_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the hifloat8_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_hifloat8_t register whose first count hifloat8_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of hifloat8_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] = [0, 256]
 * hifloat8_t elements; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ hifloat8_t* dst, vector_hifloat8_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp4x2_e2m1_t elements of the vector register src to UB at the dtype-aligned address
 * dst, leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf4e2m1x2_vf4e2m1x2_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp4x2_e2m1_t data; it must be dtype-aligned and the written
 * range covers count elements.
 * @param [in] src Source vector_fp4x2_e2m1_t register whose first count fp4x2_e2m1_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of fp4x2_e2m1_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] with VL = 256
 * bytes and two 4-bit elements sharing each byte; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp4x2_e1m2_t elements of the vector register src to UB at the dtype-aligned address
 * dst, leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUf4e1m2x2_vf4e1m2x2_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the fp4x2_e1m2_t data; it must be dtype-aligned and the written
 * range covers count elements.
 * @param [in] src Source vector_fp4x2_e1m2_t register whose first count fp4x2_e1m2_t elements are stored; the remaining
 * elements are ignored.
 * @param [in] count Number of fp4x2_e1m2_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] with VL = 256
 * bytes and two 4-bit elements sharing each byte; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int4b_t elements of the vector register src to UB at the dtype-aligned address dst,
 * leaving the remaining elements of src unwritten.
 *
 * @override_id asc_store__PUi4x2_vi4x2_u32
 * @archs 3510
 * @param [out] dst Destination UB start address of the int4b_t data; it must be dtype-aligned and the written range
 * covers count elements.
 * @param [in] src Source vector_int4x2_t register whose first count int4b_t elements are stored; the remaining elements
 * are ignored.
 * @param [in] count Number of int4b_t elements to store, bounded by the doc to [0, VL/sizeof(dtype)] with VL = 256
 * bytes and two 4-bit elements sharing each byte; values outside that range are undefined behaviour.
 */
__simd_callee__ inline void asc_store(__ubuf__ int4b_t* dst, vector_int4x2_t src, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_STOREUNALIGN_COMPOSITE_H
#endif
