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
#error "load/loadunalign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADUNALIGN_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_LOAD_LOADUNALIGN_H
#define INCLUDE_C_API_REG_COMPUTE_LOAD_LOADUNALIGN_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/loadunalign_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int8_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi8
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the int8_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint8_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu8
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the uint8_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp4x2_e2m1_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * fp4x2_e2m1_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp4x2_e1m2_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * fp4x2_e1m2_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int4b_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi4x2
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the int4b_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e8m0_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * fp8_e8m0_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e5m2_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * fp8_e5m2_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e4m3fn_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * fp8_e4m3fn_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of hifloat8_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUhf8
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * hifloat8_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int16_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi16
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the int16_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint16_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu16
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the uint16_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of half data at the 32-byte boundary at or below the dtype-aligned UB address src into the
 * unaligned register dst, priming the front data for a following unaligned load without writing any vector register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf16
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the half data
 * cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of bfloat16_t data at the 32-byte boundary at or below the dtype-aligned UB address src
 * into the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUbf16
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the
 * bfloat16_t data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int32_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi32
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the int32_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint32_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu32
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the uint32_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of float data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf32
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the float
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int64_t data at the 32-byte boundary at or below the dtype-aligned UB address src into
 * the unaligned register dst, priming the front data for a following unaligned load without writing any vector
 * register.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi64
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache
 * consumed by a following asc_loadunalign or asc_loadunalign_postupdate load.
 * @param [in] src Dtype-aligned source UB address; the 32-byte block whose range contains that address is the int64_t
 * data cached into dst.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int8_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of int8_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint8_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of uint8_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp4x2_e2m1_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of fp4x2_e2m1_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp4x2_e1m2_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of fp4x2_e1m2_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int4b_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of int4b_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e8m0_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of fp8_e8m0_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e5m2_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of fp8_e5m2_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of fp8_e4m3fn_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of fp8_e4m3fn_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of hifloat8_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of hifloat8_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int16_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of int16_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint16_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of uint16_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of half data at the 32-byte boundary at or below the address formed by src and the element
 * offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of half data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of bfloat16_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of bfloat16_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int32_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of int32_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of uint32_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of uint32_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of float data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of float data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Caches the 32 bytes of int64_t data at the 32-byte boundary at or below the address formed by src and the
 * element offset in offset into the unaligned register dst, priming an addr_reg-mode unaligned load.
 *
 * @override_id asc_loadunalign_pre__Rvloadunalign_PUi64_ar
 * @archs 3510
 * @param [out] dst Destination unaligned register (vector_load_unalign) that receives the 32-byte front-data cache for
 * the following address-register unaligned load.
 * @param [in] src Source UB base address; together with offset it determines the dtype-aligned effective address whose
 * containing 32-byte block of int64_t data is cached.
 * @param [in] offset addr_reg element offset relative to src (b8/b16/b32 modes count 1/2/4 bytes per unit); it is
 * passed by value and is not modified by this preprocessing call.
 */
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of int8_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_int8_t destination register.
 *
 * @override_id asc_loadunalign__Rvi8_Rvloadunalign_PUi8
 * @archs 3510
 * @param [out] dst Destination vector register (vector_int8_t) that receives VL bytes of int8_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the int8_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of uint8_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_uint8_t destination register.
 *
 * @override_id asc_loadunalign__Rvu8_Rvloadunalign_PUu8
 * @archs 3510
 * @param [out] dst Destination vector register (vector_uint8_t) that receives VL bytes of uint8_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the uint8_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of int16_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_int16_t destination register.
 *
 * @override_id asc_loadunalign__Rvi16_Rvloadunalign_PUi16
 * @archs 3510
 * @param [out] dst Destination vector register (vector_int16_t) that receives VL bytes of int16_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the int16_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of uint16_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_uint16_t destination register.
 *
 * @override_id asc_loadunalign__Rvu16_Rvloadunalign_PUu16
 * @archs 3510
 * @param [out] dst Destination vector register (vector_uint16_t) that receives VL bytes of uint16_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the uint16_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of int32_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_int32_t destination register.
 *
 * @override_id asc_loadunalign__Rvi32_Rvloadunalign_PUi32
 * @archs 3510
 * @param [out] dst Destination vector register (vector_int32_t) that receives VL bytes of int32_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the int32_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of uint32_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_uint32_t destination register.
 *
 * @override_id asc_loadunalign__Rvu32_Rvloadunalign_PUu32
 * @archs 3510
 * @param [out] dst Destination vector register (vector_uint32_t) that receives VL bytes of uint32_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the uint32_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of half data from a dtype-aligned Unified Buffer address and joins them with the 32-byte prefix
 * cached in the unaligned register, filling the vector_half destination register.
 *
 * @override_id asc_loadunalign__Rvf16_Rvloadunalign_PUf16
 * @archs 3510
 * @param [out] dst Destination vector register (vector_half) that receives VL bytes of half data; its dtype must match
 * src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the half data in Unified Buffer, which must be dtype-aligned; the pointer
 * is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_half& dst, vector_load_unalign& src0, __ubuf__ half* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of float data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_float destination register.
 *
 * @override_id asc_loadunalign__Rvf32_Rvloadunalign_PUf32
 * @archs 3510
 * @param [out] dst Destination vector register (vector_float) that receives VL bytes of float data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the float data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_float& dst, vector_load_unalign& src0, __ubuf__ float* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of int64_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_int64_t destination register.
 *
 * @override_id asc_loadunalign__Rvi64_Rvloadunalign_PUi64
 * @archs 3510
 * @param [out] dst Destination vector register (vector_int64_t) that receives VL bytes of int64_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the int64_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of bfloat16_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_bfloat16_t destination register.
 *
 * @override_id asc_loadunalign__Rvbf16_Rvloadunalign_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector register (vector_bfloat16_t) that receives VL bytes of bfloat16_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the bfloat16_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of fp8_e4m3fn_t data from a dtype-aligned Unified Buffer address and joins them with the
 * 32-byte prefix cached in the unaligned register, filling the vector_fp8_e4m3fn_t destination register.
 *
 * @override_id asc_loadunalign__Rvf8e4m3_Rvloadunalign_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector register (vector_fp8_e4m3fn_t) that receives VL bytes of fp8_e4m3fn_t data; its
 * dtype must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the fp8_e4m3fn_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of hifloat8_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_hifloat8_t destination register.
 *
 * @override_id asc_loadunalign__Rvhf8_Rvloadunalign_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector register (vector_hifloat8_t) that receives VL bytes of hifloat8_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the hifloat8_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of fp8_e5m2_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_fp8_e5m2_t destination register.
 *
 * @override_id asc_loadunalign__Rvf8e5m2_Rvloadunalign_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector register (vector_fp8_e5m2_t) that receives VL bytes of fp8_e5m2_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the fp8_e5m2_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of fp8_e8m0_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_fp8_e8m0_t destination register.
 *
 * @override_id asc_loadunalign__Rvf8e8m0_Rvloadunalign_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector register (vector_fp8_e8m0_t) that receives VL bytes of fp8_e8m0_t data; its dtype
 * must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the fp8_e8m0_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of fp4x2_e2m1_t data from a dtype-aligned Unified Buffer address and joins them with the
 * 32-byte prefix cached in the unaligned register, filling the vector_fp4x2_e2m1_t destination register.
 *
 * @override_id asc_loadunalign__Rvf4e2m1x2_Rvloadunalign_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector register (vector_fp4x2_e2m1_t) that receives VL bytes of fp4x2_e2m1_t data; its
 * dtype must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the fp4x2_e2m1_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of fp4x2_e1m2_t data from a dtype-aligned Unified Buffer address and joins them with the
 * 32-byte prefix cached in the unaligned register, filling the vector_fp4x2_e1m2_t destination register.
 *
 * @override_id asc_loadunalign__Rvf4e1m2x2_Rvloadunalign_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector register (vector_fp4x2_e1m2_t) that receives VL bytes of fp4x2_e1m2_t data; its
 * dtype must match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the fp4x2_e1m2_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL bytes of int4b_t data from a dtype-aligned Unified Buffer address and joins them with the 32-byte
 * prefix cached in the unaligned register, filling the vector_int4x2_t destination register.
 *
 * @override_id asc_loadunalign__Rvi4x2_Rvloadunalign_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector register (vector_int4x2_t) that receives VL bytes of int4b_t data; its dtype must
 * match src1.
 * @param [in] src0 Read-only unaligned register holding the 32-byte prefix cache of the source produced by
 * asc_loadunalign_pre for this transfer.
 * @param [in] src1 Effective start address of the int4b_t data in Unified Buffer, which must be dtype-aligned; the
 * pointer is not advanced by this call.
 */
__simd_callee__ inline void asc_loadunalign(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t* src1);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of int8_t data read from the
 * dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and refreshes
 * src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi8_Rvloadunalign_RPUi8_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives one VL of spliced int8_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the int8_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(int8_t) bytes).
 * @param [in] count Post-update step in int8_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(int8_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of uint8_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu8_Rvloadunalign_RPUu8_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives one VL of spliced uint8_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the uint8_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(uint8_t) bytes).
 * @param [in] count Post-update step in uint8_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(uint8_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of int16_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi16_Rvloadunalign_RPUi16_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives one VL of spliced int16_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the int16_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(int16_t) bytes).
 * @param [in] count Post-update step in int16_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(int16_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of uint16_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu16_Rvloadunalign_RPUu16_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives one VL of spliced uint16_t data;
 * its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the uint16_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(uint16_t) bytes).
 * @param [in] count Post-update step in uint16_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(uint16_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of int32_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi32_Rvloadunalign_RPUi32_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives one VL of spliced int32_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the int32_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(int32_t) bytes).
 * @param [in] count Post-update step in int32_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(int32_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of uint32_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu32_Rvloadunalign_RPUu32_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives one VL of spliced uint32_t data;
 * its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the uint32_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(uint32_t) bytes).
 * @param [in] count Post-update step in uint32_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(uint32_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of half data read from the
 * dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and refreshes
 * src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf16_Rvloadunalign_RPUf16_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives one VL of spliced half data; its dtype
 * must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the half data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(half) bytes).
 * @param [in] count Post-update step in half elements by which src1 is advanced after the load, i.e. the pointer moves
 * count x sizeof(half) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_half& dst, vector_load_unalign& src0, __ubuf__ half*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of float data read from the
 * dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and refreshes
 * src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf32_Rvloadunalign_RPUf32_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives one VL of spliced float data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the float data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(float) bytes).
 * @param [in] count Post-update step in float elements by which src1 is advanced after the load, i.e. the pointer moves
 * count x sizeof(float) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_float& dst, vector_load_unalign& src0, __ubuf__ float*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of int64_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi64_Rvloadunalign_RPUi64_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives one VL of spliced int64_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the int64_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(int64_t) bytes).
 * @param [in] count Post-update step in int64_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(int64_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of bfloat16_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvbf16_Rvloadunalign_RPUbf16_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives one VL of spliced bfloat16_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the bfloat16_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(bfloat16_t) bytes).
 * @param [in] count Post-update step in bfloat16_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(bfloat16_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of fp8_e4m3fn_t data read
 * from the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e4m3_Rvloadunalign_RPUf8e4m3_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives one VL of spliced fp8_e4m3fn_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the fp8_e4m3fn_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(fp8_e4m3fn_t) bytes).
 * @param [in] count Post-update step in fp8_e4m3fn_t elements by which src1 is advanced after the load, i.e. the
 * pointer moves count x sizeof(fp8_e4m3fn_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of hifloat8_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvhf8_Rvloadunalign_RPUhf8_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives one VL of spliced hifloat8_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the hifloat8_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(hifloat8_t) bytes).
 * @param [in] count Post-update step in hifloat8_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(hifloat8_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of fp8_e5m2_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e5m2_Rvloadunalign_RPUf8e5m2_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives one VL of spliced fp8_e5m2_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the fp8_e5m2_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(fp8_e5m2_t) bytes).
 * @param [in] count Post-update step in fp8_e5m2_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(fp8_e5m2_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of fp4x2_e2m1_t data read
 * from the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf4e2m1x2_Rvloadunalign_RPUf4e2m1x2_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives one VL of spliced fp4x2_e2m1_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the fp4x2_e2m1_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(fp4x2_e2m1_t) bytes).
 * @param [in] count Post-update step in fp4x2_e2m1_t elements by which src1 is advanced after the load, i.e. the
 * pointer moves count x sizeof(fp4x2_e2m1_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of fp4x2_e1m2_t data read
 * from the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf4e1m2x2_Rvloadunalign_RPUf4e1m2x2_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives one VL of spliced fp4x2_e1m2_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the fp4x2_e1m2_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(fp4x2_e1m2_t) bytes).
 * @param [in] count Post-update step in fp4x2_e1m2_t elements by which src1 is advanced after the load, i.e. the
 * pointer moves count x sizeof(fp4x2_e1m2_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of int4b_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi4x2_Rvloadunalign_RPUi4x2_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives one VL of spliced int4b_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the int4b_t data to load; the effective read address must be dtype-aligned,
 * and after the load the pointer is advanced by count elements (count x sizeof(int4b_t) bytes).
 * @param [in] count Post-update step in int4b_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(int4b_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache held in the unaligned register src0 with VL bytes of fp8_e8m0_t data read from
 * the dtype-aligned UB address src1 into the destination register dst, then advances src1 by count elements and
 * refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e8m0_Rvloadunalign_RPUf8e8m0_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives one VL of spliced fp8_e8m0_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed
 * with the front data required by the address src1 holds after the update, so it can feed the next load.
 * @param [in,out] src1 Source UB address of the fp8_e8m0_t data to load; the effective read address must be
 * dtype-aligned, and after the load the pointer is advanced by count elements (count x sizeof(fp8_e8m0_t) bytes).
 * @param [in] count Post-update step in fp8_e8m0_t elements by which src1 is advanced after the load, i.e. the pointer
 * moves count x sizeof(fp8_e8m0_t) bytes.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t*& src1, uint32_t count);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of int8_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi8_Rvloadunalign_PUi8_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives one VL of spliced int8_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the int8_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in int8_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of uint8_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu8_Rvloadunalign_PUu8_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives one VL of spliced uint8_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the uint8_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in uint8_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of int16_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi16_Rvloadunalign_PUi16_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives one VL of spliced int16_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the int16_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in int16_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of uint16_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu16_Rvloadunalign_PUu16_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives one VL of spliced uint16_t data;
 * its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the uint16_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in uint16_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of int32_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi32_Rvloadunalign_PUi32_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives one VL of spliced int32_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the int32_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in int32_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of uint32_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvu32_Rvloadunalign_PUu32_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives one VL of spliced uint32_t data;
 * its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the uint32_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in uint32_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of half data at the dtype-aligned address formed by src1
 * and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf16_Rvloadunalign_PUf16_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives one VL of spliced half data; its dtype
 * must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the half data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in half elements added to the offset held in offset after the load (offset = offset
 * + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_half& dst, vector_load_unalign& src0, __ubuf__ half* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of float data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf32_Rvloadunalign_PUf32_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives one VL of spliced float data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the float data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in float elements added to the offset held in offset after the load (offset = offset
 * + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_float& dst, vector_load_unalign& src0, __ubuf__ float* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of int64_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi64_Rvloadunalign_PUi64_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives one VL of spliced int64_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the int64_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in int64_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of bfloat16_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvbf16_Rvloadunalign_PUbf16_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives one VL of spliced bfloat16_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the bfloat16_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in bfloat16_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of fp8_e4m3fn_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e4m3_Rvloadunalign_PUf8e4m3_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives one VL of spliced fp8_e4m3fn_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the fp8_e4m3fn_t data; together with offset it determines the
 * dtype-aligned effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in fp8_e4m3fn_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of hifloat8_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvhf8_Rvloadunalign_PUhf8_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives one VL of spliced hifloat8_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the hifloat8_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in hifloat8_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of fp8_e5m2_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e5m2_Rvloadunalign_PUf8e5m2_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives one VL of spliced fp8_e5m2_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the fp8_e5m2_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in fp8_e5m2_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of fp4x2_e2m1_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf4e2m1x2_Rvloadunalign_PUf4e2m1x2_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives one VL of spliced fp4x2_e2m1_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the fp4x2_e2m1_t data; together with offset it determines the
 * dtype-aligned effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in fp4x2_e2m1_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of fp4x2_e1m2_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf4e1m2x2_Rvloadunalign_PUf4e1m2x2_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives one VL of spliced fp4x2_e1m2_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the fp4x2_e1m2_t data; together with offset it determines the
 * dtype-aligned effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in fp4x2_e1m2_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of int4b_t data at the dtype-aligned address formed by
 * src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvi4x2_Rvloadunalign_PUi4x2_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives one VL of spliced int4b_t data; its
 * dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the int4b_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in int4b_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t* src1, addr_reg& offset, uint32_t inc);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Splices the 32-byte front cache in src0 with VL bytes of fp8_e8m0_t data at the dtype-aligned address formed
 * by src1 and the element offset in offset into dst, then increases offset by inc and refreshes src0.
 *
 * @override_id asc_loadunalign_postupdate__Rvf8e8m0_Rvloadunalign_PUf8e8m0_Rar_u32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives one VL of spliced fp8_e8m0_t
 * data; its dtype must match src1.
 * @param [in,out] src0 32-byte unaligned register whose cached front data is spliced into dst and is then refreshed for
 * the address determined by src1 and the updated offset, ready for the next load in the same iteration.
 * @param [in] src1 Source UB base address of the fp8_e8m0_t data; together with offset it determines the dtype-aligned
 * effective read address, and the base pointer itself is not modified.
 * @param [in,out] offset addr_reg holding the element offset relative to src1 (b8/b16/b32 modes count 1/2/4 bytes per
 * unit); after the load the interface increases the stored offset by inc.
 * @param [in] inc Post-update step in fp8_e8m0_t elements added to the offset held in offset after the load (offset =
 * offset + inc); src1 is unchanged.
 */
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t* src1, addr_reg& offset, uint32_t inc);

#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e6m2_t* src);
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset);
__simd_callee__ inline void asc_loadunalign(
    vector_fp8_e6m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e6m2_t* src1);
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e6m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e6m2_t*& src1, uint32_t count);
__simd_callee__ inline void asc_loadunalign_postupdate(
    vector_fp8_e6m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e6m2_t* src1, addr_reg& offset, uint32_t inc);
#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADUNALIGN_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADUNALIGN_H
#endif
