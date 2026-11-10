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
#error "composite/loadunalign_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_LOADUNALIGN_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_LOADUNALIGN_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_LOADUNALIGN_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/loadunalign_composite_intf_impl.h"
#endif

// ========== return-value load APIs ==========
// ========== asc_load ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int4b_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged, with the result typed vector_int4x2_t because int4b_t maps to that
 * packed 4-bit register type; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUi4x2
 * @archs 3510
 * @param [in] src Start address in UB of the int4b_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_int4x2_t asc_load(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int8_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUi8
 * @archs 3510
 * @param [in] src Start address in UB of the int8_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_int8_t asc_load(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint8_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUu8
 * @archs 3510
 * @param [in] src Start address in UB of the uint8_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_uint8_t asc_load(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp4x2_e2m1_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_load(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp4x2_e1m2_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_load(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of hifloat8_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUhf8
 * @archs 3510
 * @param [in] src Start address in UB of the hifloat8_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_hifloat8_t asc_load(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e8m0_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf8e8m0
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e8m0_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_load(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e5m2_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf8e5m2
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e5m2_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_load(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e4m3fn_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf8e4m3
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_load(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int16_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUi16
 * @archs 3510
 * @param [in] src Start address in UB of the int16_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_int16_t asc_load(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint16_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUu16
 * @archs 3510
 * @param [in] src Start address in UB of the uint16_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_uint16_t asc_load(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of half data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf16
 * @archs 3510
 * @param [in] src Start address in UB of the half source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_half asc_load(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of bfloat16_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUbf16
 * @archs 3510
 * @param [in] src Start address in UB of the bfloat16_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_bfloat16_t asc_load(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int32_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUi32
 * @archs 3510
 * @param [in] src Start address in UB of the int32_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_int32_t asc_load(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint32_t data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUu32
 * @archs 3510
 * @param [in] src Start address in UB of the uint32_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline vector_uint32_t asc_load(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of float data from the dtype-aligned UB address src and returns it as the matching vector
 * register, keeping format and content unchanged; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__PUf32
 * @archs 3510
 * @param [in] src Start address in UB of the float source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline vector_float asc_load(__ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int8_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the int8_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint8_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the uint8_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int16_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the int16_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint16_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the uint16_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int32_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the int32_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of uint32_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the uint32_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of half data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length block; the page requires
 * its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the half source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of float data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length block; the page requires
 * its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the float source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int64_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvi64_PUi64
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the int64_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_int64_t& dst, __ubuf__ int64_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of bfloat16_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the bfloat16_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e4m3fn_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of hifloat8_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the hifloat8_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e5m2_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the fp8_e5m2_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp8_e8m0_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the fp8_e8m0_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp4x2_e2m1_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of fp4x2_e1m2_t data from the dtype-aligned UB address src into the destination vector register
 * dst, keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive
 * call.
 *
 * @override_id asc_load__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source block; the actual read address must be aligned to the
 * dtype size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads one VL of int4b_t data from the dtype-aligned UB address src into the destination vector register dst,
 * keeping format and content unchanged and returning nothing; the caller must advance src before each consecutive call.
 *
 * @override_id asc_load__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length block; the page
 * requires its dtype to match src, and int4b_t maps to vector_int4x2_t.
 * @param [in] src Start address in UB of the int4b_t source block; the actual read address must be aligned to the dtype
 * size and the interface only reads from it.
 */
__simd_callee__ inline void asc_load(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_LOADUNALIGN_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_LOADUNALIGN_COMPOSITE_H
#endif
