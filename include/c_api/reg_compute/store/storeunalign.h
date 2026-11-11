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
#error "store/storeunalign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREUNALIGN_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_STORE_STOREUNALIGN_H
#define INCLUDE_C_API_REG_COMPUTE_STORE_STOREUNALIGN_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/storeunalign_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int8_t elements of the vector_int8_t source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUi8_Rvstoreunalign_vi8_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the int8_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(int8_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_int8_t) holding the int8_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint8_t elements of the vector_uint8_t source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUu8_Rvstoreunalign_vu8_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the uint8_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(uint8_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_uint8_t) holding the uint8_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int16_t elements of the vector_int16_t source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUi16_Rvstoreunalign_vi16_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the int16_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(int16_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_int16_t) holding the int16_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint16_t elements of the vector_uint16_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUu16_Rvstoreunalign_vu16_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the uint16_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(uint16_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_uint16_t) holding the uint16_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int32_t elements of the vector_int32_t source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUi32_Rvstoreunalign_vi32_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the int32_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(int32_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_int32_t) holding the int32_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int64_t elements of the vector_int64_t source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUi64_Rvstoreunalign_vi64_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the int64_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(int64_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_int64_t) holding the int64_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count uint32_t elements of the vector_uint32_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUu32_Rvstoreunalign_vu32_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the uint32_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(uint32_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_uint32_t) holding the uint32_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count half elements of the vector_half source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf16_Rvstoreunalign_vf16_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the half data; it need not be 32-byte aligned but must
 * be aligned to sizeof(half).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_half) holding the half elements to store; its dtype must match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ half* dst, vector_store_unalign& src0, vector_half src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count float elements of the vector_float source register to a non-32-byte-aligned Unified
 * Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf32_Rvstoreunalign_vf32_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the float data; it need not be 32-byte aligned but
 * must be aligned to sizeof(float).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_float) holding the float elements to store; its dtype must match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ float* dst, vector_store_unalign& src0, vector_float src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count bfloat16_t elements of the vector_bfloat16_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUbf16_Rvstoreunalign_vbf16_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the bfloat16_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(bfloat16_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_bfloat16_t) holding the bfloat16_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e4m3fn_t elements of the vector_fp8_e4m3fn_t source register to a
 * non-32-byte-aligned Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf8e4m3_Rvstoreunalign_vf8e4m3_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the fp8_e4m3fn_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(fp8_e4m3fn_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_fp8_e4m3fn_t) holding the fp8_e4m3fn_t elements to store; its dtype
 * must match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count hifloat8_t elements of the vector_hifloat8_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUhf8_Rvstoreunalign_vhf8_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the hifloat8_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(hifloat8_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_hifloat8_t) holding the hifloat8_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ hifloat8_t* dst, vector_store_unalign& src0, vector_hifloat8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e5m2_t elements of the vector_fp8_e5m2_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf8e5m2_Rvstoreunalign_vf8e5m2_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the fp8_e5m2_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(fp8_e5m2_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_fp8_e5m2_t) holding the fp8_e5m2_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp8_e8m0_t elements of the vector_fp8_e8m0_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf8e8m0_Rvstoreunalign_vf8e8m0_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the fp8_e8m0_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(fp8_e8m0_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_fp8_e8m0_t) holding the fp8_e8m0_t elements to store; its dtype must
 * match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp4x2_e2m1_t elements of the vector_fp4x2_e2m1_t source register to a
 * non-32-byte-aligned Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf4e2m1x2_Rvstoreunalign_vf4e2m1x2_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the fp4x2_e2m1_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(fp4x2_e2m1_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_fp4x2_e2m1_t) holding the fp4x2_e2m1_t elements to store; its dtype
 * must match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count fp4x2_e1m2_t elements of the vector_fp4x2_e1m2_t source register to a
 * non-32-byte-aligned Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUf4e1m2x2_Rvstoreunalign_vf4e1m2x2_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the fp4x2_e1m2_t data; it need not be 32-byte aligned
 * but must be aligned to sizeof(fp4x2_e1m2_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_fp4x2_e1m2_t) holding the fp4x2_e1m2_t elements to store; its dtype
 * must match dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores the first count int4b_t elements of the vector_int4x2_t source register to a non-32-byte-aligned
 * Unified Buffer address and keeps this call's tail block in the unaligned register.
 *
 * @override_id asc_storeunalign__PUi4x2_Rvstoreunalign_vi4x2_u32
 * @archs 3510
 * @param [out] dst Start address in Unified Buffer that receives the int4b_t data; it need not be 32-byte aligned but
 * must be aligned to sizeof(int4b_t).
 * @param [in,out] src0 Unaligned register of 32 bytes that supplies the previous call's tail block and is updated in
 * place with this call's tail block; no initialization is required before the first call.
 * @param [in] src1 Source vector register (vector_int4x2_t) holding the int4b_t elements to store; its dtype must match
 * dst.
 * @param [in] count Number of elements to store, of type uint32_t; the maximum is the number of elements a single
 * vector register can hold.
 */
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count int8_t elements of the vector register src1 to the unaligned UB address dst, splicing the carried
 * 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUi8_Rvstoreunalign_vi8_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int8_t*& dst, vector_store_unalign& src0, vector_int8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count uint8_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUu8_Rvstoreunalign_vu8_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_uint8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count int16_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUi16_Rvstoreunalign_vi16_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int16_t*& dst, vector_store_unalign& src0, vector_int16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count uint16_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUu16_Rvstoreunalign_vu16_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_uint16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count int32_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUi32_Rvstoreunalign_vi32_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int32_t*& dst, vector_store_unalign& src0, vector_int32_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count uint32_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUu32_Rvstoreunalign_vu32_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_uint32_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count int64_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUi64_Rvstoreunalign_vi64_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int64_t) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int64_t*& dst, vector_store_unalign& src0, vector_int64_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count half elements of the vector register src1 to the unaligned UB address dst, splicing the carried
 * 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf16_Rvstoreunalign_vf16_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_half) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ half*& dst, vector_store_unalign& src0, vector_half src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count float elements of the vector register src1 to the unaligned UB address dst, splicing the carried
 * 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf32_Rvstoreunalign_vf32_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_float) holding the elements to store; its dtype must match the
 * data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ float*& dst, vector_store_unalign& src0, vector_float src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count bfloat16_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUbf16_Rvstoreunalign_vbf16_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_store_unalign& src0, vector_bfloat16_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count fp8_e4m3fn_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf8e4m3_Rvstoreunalign_vf8e4m3_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e4m3fn_t) holding the elements to store; its dtype must
 * match the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e4m3fn_t*& dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count hifloat8_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUhf8_Rvstoreunalign_vhf8_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_hifloat8_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ hifloat8_t*& dst, vector_store_unalign& src0, vector_hifloat8_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count fp8_e5m2_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf8e5m2_Rvstoreunalign_vf8e5m2_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e5m2_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e5m2_t*& dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count fp8_e8m0_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf8e8m0_Rvstoreunalign_vf8e8m0_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e8m0_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e8m0_t*& dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count fp4x2_e2m1_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf4e2m1x2_Rvstoreunalign_vf4e2m1x2_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp4x2_e2m1_t) holding the elements to store; its dtype must
 * match the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e2m1_t*& dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count fp4x2_e1m2_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUf4e1m2x2_Rvstoreunalign_vf4e1m2x2_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp4x2_e1m2_t) holding the elements to store; its dtype must
 * match the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e1m2_t*& dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores count int4x2_t elements of the vector register src1 to the unaligned UB address dst, splicing the
 * carried 32-byte tail from src0 into the main block, and advances dst past the stored elements.
 *
 * @override_id asc_storeunalign_postupdate__RPUi4x2_Rvstoreunalign_vi4x2_u32
 * @archs 3510
 * @param [in,out] dst UB address where the elements are stored; it is passed by reference and is advanced by count
 * elements after the call, so the same variable can be reused for the next unaligned store.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int4x2_t) holding the elements to store; its dtype must match
 * the data type dst points to.
 * @param [in] count Number of elements to store (uint32_t); it is also the amount by which dst is advanced and must not
 * exceed the element count a single vector data register can hold.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int4b_t*& dst, vector_store_unalign& src0, vector_int4x2_t src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src1 into contiguous bytes at the unaligned UB address dst through src0
 * and advances dst to the end of the written data.
 *
 * @override_id asc_storeunalign_postupdate__RPUu16_Rvstoreunalign_vb
 * @archs 3510
 * @param [in,out] dst UB address of the packed mask destination (uint16_t type); the address must be 16-byte aligned
 * for b16 and 8-byte aligned for b32, and after the call dst is advanced to the end of the bytes just written.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that carries the unaligned tail between
 * consecutive calls and must be reused for the whole sequence.
 * @param [in] src1 Source mask register (vector_bool) whose 32 bytes are compressed into contiguous bits at dst; this
 * mode takes no mask operand.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Compresses the 32-byte mask register src1 into contiguous bytes at the unaligned UB address dst through src0
 * and advances dst to the end of the written data.
 *
 * @override_id asc_storeunalign_postupdate__RPUu32_Rvstoreunalign_vb
 * @archs 3510
 * @param [in,out] dst UB address of the packed mask destination (uint32_t type); the address must be 16-byte aligned
 * for b16 and 8-byte aligned for b32, and after the call dst is advanced to the end of the bytes just written.
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that carries the unaligned tail between
 * consecutive calls and must be reused for the whole sequence.
 * @param [in] src1 Source mask register (vector_bool) whose 32 bytes are compressed into contiguous bits at dst; this
 * mode takes no mask operand.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_bool src1);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUi8_Rvstoreunalign_vi8_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ int8_t* pointer, so the
 * pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int8_t) holding the data to store; its dtype must match the data
 * type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUu8_Rvstoreunalign_vu8_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ uint8_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint8_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUi16_Rvstoreunalign_vi16_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ int16_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int16_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUu16_Rvstoreunalign_vu16_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ uint16_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint16_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUi32_Rvstoreunalign_vi32_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ int32_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int32_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUu32_Rvstoreunalign_vu32_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ uint32_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_uint32_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUi64_Rvstoreunalign_vi64_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ int64_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int64_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf16_Rvstoreunalign_vf16_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ half* pointer, so the
 * pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_half) holding the data to store; its dtype must match the data
 * type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ half* dst, vector_store_unalign& src0, vector_half src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf32_Rvstoreunalign_vf32_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ float* pointer, so the
 * pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_float) holding the data to store; its dtype must match the data
 * type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ float* dst, vector_store_unalign& src0, vector_float src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUbf16_Rvstoreunalign_vbf16_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ bfloat16_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_bfloat16_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e4m3_Rvstoreunalign_vf8e4m3_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ fp8_e4m3fn_t* pointer,
 * so the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e4m3fn_t) holding the data to store; its dtype must match
 * the data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUhf8_Rvstoreunalign_vhf8_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ hifloat8_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_hifloat8_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ hifloat8_t* dst, vector_store_unalign& src0, vector_hifloat8_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e5m2_Rvstoreunalign_vf8e5m2_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ fp8_e5m2_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e5m2_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e8m0_Rvstoreunalign_vf8e8m0_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ fp8_e8m0_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp8_e8m0_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf4e2m1x2_Rvstoreunalign_vf4e2m1x2_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ fp4x2_e2m1_t* pointer,
 * so the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp4x2_e2m1_t) holding the data to store; its dtype must match
 * the data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUf4e1m2x2_Rvstoreunalign_vf4e1m2x2_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ fp4x2_e1m2_t* pointer,
 * so the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_fp4x2_e1m2_t) holding the data to store; its dtype must match
 * the data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Stores a fixed VL of 256 bytes from the vector register src1 to the unaligned UB address dst, splicing the
 * carried tail from src0, and advances the addr_reg offset by one VL for the next call.
 *
 * @override_id asc_storeunalign_postupdate__PUi4x2_Rvstoreunalign_vi4x2_Rar
 * @archs 3510
 * @param [out] dst UB start address of the unaligned store target (declared as a plain __ubuf__ int4b_t* pointer, so
 * the pointer value itself is not updated; the iteration offset lives in count).
 * @param [in,out] src0 Non-aligned register (vector_store_unalign, 32 bytes) that supplies the tail carried over from
 * the previous call and receives the new tail; the same src0 must be reused by consecutive calls.
 * @param [in] src1 Source vector data register (vector_int4x2_t) holding the data to store; its dtype must match the
 * data type dst points to.
 * @param [in,out] count Address register holding the destination iteration offset; it does not control how much data
 * one call stores (a fixed VL of 256 bytes is stored) and after the instruction its offset is advanced by one VL; it
 * must be built by asc_update_addr_reg.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1, addr_reg& count);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset int8_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi8_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset int8_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset uint8_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu8_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset uint8_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset fp4x2_e2m1_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf4e2m1x2_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp4x2_e2m1_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset fp4x2_e2m1_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset fp4x2_e1m2_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf4e1m2x2_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp4x2_e1m2_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset fp4x2_e1m2_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset int4b_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi4x2_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int4b_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset int4b_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset fp8_e8m0_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e8m0_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e8m0_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset fp8_e8m0_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset fp8_e5m2_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e5m2_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e5m2_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset fp8_e5m2_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset fp8_e4m3fn_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e4m3_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e4m3fn_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset fp8_e4m3fn_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset hifloat8_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUhf8_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(hifloat8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset hifloat8_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ hifloat8_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset int16_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi16_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset int16_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset uint16_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu16_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset uint16_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset half elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf16_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(half) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset half elements is the end address of the data already
 * written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset bfloat16_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUbf16_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(bfloat16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset bfloat16_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset int32_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi32_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int32_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset int32_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset uint32_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu32_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint32_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset uint32_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset float elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf32_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(float) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset float elements is the end address of the data already
 * written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB so that it ends at dst plus
 * offset int64_t elements, completing the matching continuous unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi64_vstoreunalign_i32
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int64_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding asc_storeunalign or masked-mode asc_storeunalign_postupdate call.
 * @param [in] offset Signed element offset such that dst + offset int64_t elements is the end address of the data
 * already written by the preceding unaligned store, which is how the tail is positioned.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi8_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu8_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf4e2m1x2_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp4x2_e2m1_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(
    __ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf4e1m2x2_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp4x2_e1m2_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(
    __ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi4x2_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int4b_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e8m0_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e8m0_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e5m2_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e5m2_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf8e4m3_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(fp8_e4m3fn_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(
    __ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUhf8_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(hifloat8_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ hifloat8_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi16_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu16_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf16_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(half) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUbf16_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(bfloat16_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi32_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int32_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUu32_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(uint32_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUf32_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(float) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the pending 32-byte tail block held in the unaligned register src to UB at dst plus the element offset
 * carried by the addr_reg offset, completing the matching address-register unaligned store.
 *
 * @override_id asc_storeunalign_post__PUi64_vstoreunalign_ar
 * @archs 3510
 * @param [out] dst UB base address of the destination operand that receives the tail block; it must be aligned to
 * sizeof(int64_t) but need not be 32-byte aligned.
 * @param [in] src 32-byte vector_store_unalign register holding the pending tail block; it must be the same register
 * used by the preceding address-register-mode asc_storeunalign_postupdate call.
 * @param [in] offset addr_reg element offset relative to dst that positions the tail block end address; it must be the
 * same addr_reg used by the preceding address-register-mode store call and is not modified here.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for int8_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUi8_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ int8_t*&) that receives the tail block; normally left at the end
 * of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ int8_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for uint8_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUu8_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ uint8_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ uint8_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for fp4x2_e2m1_t data and
 * then advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf4e2m1x2_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ fp4x2_e2m1_t*&) that receives the tail block; normally left at
 * the end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp4x2_e2m1_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for fp4x2_e1m2_t data and
 * then advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf4e1m2x2_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ fp4x2_e1m2_t*&) that receives the tail block; normally left at
 * the end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp4x2_e1m2_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for int4b_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUi4x2_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ int4b_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ int4b_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for fp8_e8m0_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf8e8m0_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ fp8_e8m0_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp8_e8m0_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for fp8_e5m2_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf8e5m2_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ fp8_e5m2_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp8_e5m2_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for fp8_e4m3fn_t data and
 * then advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf8e4m3_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ fp8_e4m3fn_t*&) that receives the tail block; normally left at
 * the end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp8_e4m3fn_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for hifloat8_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUhf8_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ hifloat8_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ hifloat8_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for int16_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUi16_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ int16_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ int16_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for uint16_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUu16_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ uint16_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ uint16_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for half data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf16_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ half*&) that receives the tail block; normally left at the end
 * of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ half*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for bfloat16_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUbf16_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ bfloat16_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ bfloat16_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for int32_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUi32_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ int32_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ int32_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for uint32_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUu32_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ uint32_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ uint32_t*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for float data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUf32_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ float*&) that receives the tail block; normally left at the end
 * of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ float*& dst, vector_store_unalign src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.store
 * @brief Writes the tail block cached in the 32-byte unaligned register to Unified Buffer for int64_t data and then
 * advances the destination pointer by offset elements.
 *
 * @override_id asc_storeunalign_post_postupdate__RPUi64_vstoreunalign_i32
 * @archs 3510
 * @param [in,out] dst Destination UB address (__ubuf__ int64_t*&) that receives the tail block; normally left at the
 * end of the stored range by the preceding asc_storeunalign_postupdate, and advanced by offset elements afterwards.
 * @param [in] src Unaligned register of 32 bytes holding the tail block written out by this call; it must be the same
 * register used by the preceding asc_storeunalign_postupdate.
 * @param [in] offset Element offset of type int32_t by which dst is moved forward after the store; set it to 0 when dst
 * already points at the end address.
 */
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ int64_t*& dst, vector_store_unalign src, int32_t offset);

#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp8_e6m2_t* dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, uint32_t count);
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, uint32_t count);
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e6m2_t* dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, addr_reg& count);
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e6m2_t* dst, vector_store_unalign src, int32_t offset);
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e6m2_t* dst, vector_store_unalign src, addr_reg offset);
__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_store_unalign src, int32_t offset);
#endif

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_bool src1) or
        asc_storeunalign_postupdate(__ubuf__ uint32_t * &dst, vector_store_unalign& src0, vector_bool src1))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated b8 mask-register store that writes the selected bytes to Unified Buffer and advances the uint8_t
 * destination pointer.
 *
 * @override_id asc_storeunalign_postupdate__RPUu8_Rvstoreunalign_vb
 * @archs 3510
 * @param [in,out] dst Start address of the uint8_t destination range in Unified Buffer; after the store it is
 * automatically updated to the end address of this store.
 * @param [in,out] src0 Unaligned register of 32 bytes, of type vector_store_unalign, that holds the non-aligned data
 * and is updated by the store.
 * @param [in] src1 Source operand, the vector_bool mask register whose selected elements are stored to Unified Buffer.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_bool src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_int8_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUi8_Rvstoreunalign_vi8
 * @archs 3510
 * @param [out] dst Start address of the int8_t destination range in Unified Buffer; it need not be 32-byte aligned, and
 * the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_int8_t vector register (or mask register for the b8 prototype), whose
 * int8_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_uint8_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUu8_Rvstoreunalign_vu8
 * @archs 3510
 * @param [out] dst Start address of the uint8_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_uint8_t vector register (or mask register for the b8 prototype), whose
 * uint8_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_int16_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUi16_Rvstoreunalign_vi16
 * @archs 3510
 * @param [out] dst Start address of the int16_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_int16_t vector register (or mask register for the b8 prototype), whose
 * int16_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_uint16_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUu16_Rvstoreunalign_vu16
 * @archs 3510
 * @param [out] dst Start address of the uint16_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_uint16_t vector register (or mask register for the b8 prototype), whose
 * uint16_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_int32_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUi32_Rvstoreunalign_vi32
 * @archs 3510
 * @param [out] dst Start address of the int32_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_int32_t vector register (or mask register for the b8 prototype), whose
 * int32_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_uint32_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUu32_Rvstoreunalign_vu32
 * @archs 3510
 * @param [out] dst Start address of the uint32_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_uint32_t vector register (or mask register for the b8 prototype), whose
 * uint32_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_int64_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUi64_Rvstoreunalign_vi64
 * @archs 3510
 * @param [out] dst Start address of the int64_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_int64_t vector register (or mask register for the b8 prototype), whose
 * int64_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_half register to a non-32-byte-aligned Unified Buffer address that also updates
 * the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf16_Rvstoreunalign_vf16
 * @archs 3510
 * @param [out] dst Start address of the half destination range in Unified Buffer; it need not be 32-byte aligned, and
 * the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_half vector register (or mask register for the b8 prototype), whose half
 * elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ half* dst, vector_store_unalign& src0, vector_half src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_float register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf32_Rvstoreunalign_vf32
 * @archs 3510
 * @param [out] dst Start address of the float destination range in Unified Buffer; it need not be 32-byte aligned, and
 * the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_float vector register (or mask register for the b8 prototype), whose
 * float elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ float* dst, vector_store_unalign& src0, vector_float src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_bfloat16_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUbf16_Rvstoreunalign_vbf16
 * @archs 3510
 * @param [out] dst Start address of the bfloat16_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_bfloat16_t vector register (or mask register for the b8 prototype), whose
 * bfloat16_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_fp8_e4m3fn_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e4m3_Rvstoreunalign_vf8e4m3
 * @archs 3510
 * @param [out] dst Start address of the fp8_e4m3fn_t destination range in Unified Buffer; it need not be 32-byte
 * aligned, and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_fp8_e4m3fn_t vector register (or mask register for the b8 prototype),
 * whose fp8_e4m3fn_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_fp8_e5m2_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e5m2_Rvstoreunalign_vf8e5m2
 * @archs 3510
 * @param [out] dst Start address of the fp8_e5m2_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_fp8_e5m2_t vector register (or mask register for the b8 prototype), whose
 * fp8_e5m2_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_fp8_e8m0_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf8e8m0_Rvstoreunalign_vf8e8m0
 * @archs 3510
 * @param [out] dst Start address of the fp8_e8m0_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_fp8_e8m0_t vector register (or mask register for the b8 prototype), whose
 * fp8_e8m0_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_fp4x2_e2m1_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf4e2m1x2_Rvstoreunalign_vf4e2m1x2
 * @archs 3510
 * @param [out] dst Start address of the fp4x2_e2m1_t destination range in Unified Buffer; it need not be 32-byte
 * aligned, and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_fp4x2_e2m1_t vector register (or mask register for the b8 prototype),
 * whose fp4x2_e2m1_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_fp4x2_e1m2_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUf4e1m2x2_Rvstoreunalign_vf4e1m2x2
 * @archs 3510
 * @param [out] dst Start address of the fp4x2_e1m2_t destination range in Unified Buffer; it need not be 32-byte
 * aligned, and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_fp4x2_e1m2_t vector register (or mask register for the b8 prototype),
 * whose fp4x2_e1m2_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated store of the vector_int4x2_t register to a non-32-byte-aligned Unified Buffer address that also
 * updates the destination address and the unaligned tail register.
 *
 * @override_id asc_storeunalign_postupdate__PUi4x2_Rvstoreunalign_vi4x2
 * @archs 3510
 * @param [out] dst Start address of the int4b_t destination range in Unified Buffer; it need not be 32-byte aligned,
 * and the interface updates the destination address on every call.
 * @param [in,out] src0 Unaligned register of 32 bytes that holds and is updated with the non-aligned tail data of the
 * store.
 * @param [in] src1 Source operand, the vector_int4x2_t vector register (or mask register for the b8 prototype), whose
 * int4b_t elements are stored; its dtype must match dst.
 */
__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * int8_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUi8_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the int8_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(int8_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * uint8_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUu8_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the uint8_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(uint8_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * fp4x2_e2m1_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf4e2m1x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the fp4x2_e2m1_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(fp4x2_e2m1_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * fp4x2_e1m2_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf4e1m2x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the fp4x2_e1m2_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(fp4x2_e1m2_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * int4b_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUi4x2_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the int4b_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(int4b_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * fp8_e8m0_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf8e8m0_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the fp8_e8m0_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(fp8_e8m0_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * fp8_e5m2_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf8e5m2_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the fp8_e5m2_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(fp8_e5m2_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * fp8_e4m3fn_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf8e4m3_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the fp8_e4m3fn_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(fp8_e4m3fn_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * int16_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUi16_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the int16_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(int16_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * uint16_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUu16_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the uint16_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(uint16_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the half
 * destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf16_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the half destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(half) byte
 * aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * bfloat16_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUbf16_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the bfloat16_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(bfloat16_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * int32_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUi32_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the int32_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(int32_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * uint32_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUu32_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the uint32_t destination elements in Unified Buffer that receives the unaligned
 * tail block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be
 * sizeof(uint32_t) byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the float
 * destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUf32_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the float destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(float) byte
 * aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_squeeze_and_storeunalign_finalize)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Writes the cached tail block of the unaligned store sequence from the unaligned register src back to the
 * int64_t destination at dst in UB, complementing the 32-byte aligned main block already written by
 * asc_storeunalign_postupdate; the interface is deprecated and replaced by asc_squeeze_and_storeunalign_finalize.
 *
 * @override_id asc_storeunalign_post__PUi64_vstoreunalign
 * @archs 3510
 * @param [out] dst Start address of the int64_t destination elements in Unified Buffer that receives the unaligned tail
 * block of fewer than 32 bytes staged by the preceding asc_storeunalign_postupdate call; dst must be sizeof(int64_t)
 * byte aligned but need not be 32-byte aligned.
 * @param [in] src Unaligned register (vector_store_unalign, a 32-byte vector_align register) holding the tail elements
 * cached by the preceding asc_storeunalign_postupdate call; it is only read and must be the same register instance used
 * by that call.
 */
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREUNALIGN_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_STORE_STOREUNALIGN_H
#endif
