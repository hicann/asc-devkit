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
#error "scalar_atomic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_SCALAR_ATOMIC_H
#endif

#ifndef INCLUDE_C_API_ATOMIC_SCALAR_ATOMIC_H
#define INCLUDE_C_API_ATOMIC_SCALAR_ATOMIC_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/scalar_atomic_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/scalar_atomic_intf_impl.h"
#endif

namespace __asc_aicore {
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically adds val to the 32-bit signed integer (int32_t) element at the Global Memory address and returns
 * the element's previous value.
 *
 * @override_id asc_atomic_add__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value added to the addressed element. Typed 32-bit signed integer (int32_t), matching the addressed
 * element.
 */
__aicore__ inline int32_t asc_atomic_add(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically adds val to the 32-bit unsigned integer (uint32_t) element at the Global Memory address and returns
 * the element's previous value.
 *
 * @override_id asc_atomic_add__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value added to the addressed element. Typed 32-bit unsigned integer (uint32_t), matching the
 * addressed element.
 */
__aicore__ inline uint32_t asc_atomic_add(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically adds val to the float (fp32) element at the Global Memory address and returns the element's
 * previous value.
 *
 * @override_id asc_atomic_add__PGf32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] val Value added to the addressed element. Typed float (fp32), matching the addressed element.
 */
__aicore__ inline float asc_atomic_add(__gm__ float* address, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically adds val to the 64-bit signed integer (int64_t) element at the Global Memory address and returns
 * the element's previous value.
 *
 * @override_id asc_atomic_add__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value added to the addressed element. Typed 64-bit signed integer (int64_t), matching the addressed
 * element.
 */
__aicore__ inline int64_t asc_atomic_add(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically adds val to the 64-bit unsigned integer (uint64_t) element at the Global Memory address and returns
 * the element's previous value.
 *
 * @override_id asc_atomic_add__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value added to the addressed element. Typed 64-bit unsigned integer (uint64_t), matching the
 * addressed element.
 */
__aicore__ inline uint64_t asc_atomic_add(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically subtracts val from the 32-bit signed integer (int32_t) element at the Global Memory address and
 * returns its previous value.
 *
 * @override_id asc_atomic_sub__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value subtracted from the addressed element. Typed 32-bit signed integer (int32_t), matching the
 * addressed element.
 */
__aicore__ inline int32_t asc_atomic_sub(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically subtracts val from the 32-bit unsigned integer (uint32_t) element at the Global Memory address and
 * returns its previous value.
 *
 * @override_id asc_atomic_sub__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value subtracted from the addressed element. Typed 32-bit unsigned integer (uint32_t), matching the
 * addressed element.
 */
__aicore__ inline uint32_t asc_atomic_sub(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically subtracts val from the float (fp32) element at the Global Memory address and returns its previous
 * value.
 *
 * @override_id asc_atomic_sub__PGf32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] val Value subtracted from the addressed element. Typed float (fp32), matching the addressed element.
 */
__aicore__ inline float asc_atomic_sub(__gm__ float* address, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically subtracts val from the 64-bit signed integer (int64_t) element at the Global Memory address and
 * returns its previous value.
 *
 * @override_id asc_atomic_sub__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value subtracted from the addressed element. Typed 64-bit signed integer (int64_t), matching the
 * addressed element.
 */
__aicore__ inline int64_t asc_atomic_sub(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically subtracts val from the 64-bit unsigned integer (uint64_t) element at the Global Memory address and
 * returns its previous value.
 *
 * @override_id asc_atomic_sub__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value subtracted from the addressed element. Typed 64-bit unsigned integer (uint64_t), matching the
 * addressed element.
 */
__aicore__ inline uint64_t asc_atomic_sub(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically stores val into the 32-bit signed integer (int32_t) element at the Global Memory address and
 * returns the previous value.
 *
 * @override_id asc_atomic_exch__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value stored into the addressed element, replacing its old value. Typed 32-bit signed integer
 * (int32_t), matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_exch(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically stores val into the 32-bit unsigned integer (uint32_t) element at the Global Memory address and
 * returns the previous value.
 *
 * @override_id asc_atomic_exch__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value stored into the addressed element, replacing its old value. Typed 32-bit unsigned integer
 * (uint32_t), matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_exch(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically stores val into the float (fp32) element at the Global Memory address and returns the previous
 * value.
 *
 * @override_id asc_atomic_exch__PGf32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] val Value stored into the addressed element, replacing its old value. Typed float (fp32), matching the
 * addressed element.
 */
__aicore__ inline float asc_atomic_exch(__gm__ float* address, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically stores val into the 64-bit signed integer (int64_t) element at the Global Memory address and
 * returns the previous value.
 *
 * @override_id asc_atomic_exch__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value stored into the addressed element, replacing its old value. Typed 64-bit signed integer
 * (int64_t), matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_exch(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically stores val into the 64-bit unsigned integer (uint64_t) element at the Global Memory address and
 * returns the previous value.
 *
 * @override_id asc_atomic_exch__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value stored into the addressed element, replacing its old value. Typed 64-bit unsigned integer
 * (uint64_t), matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_exch(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the larger of val and the 32-bit signed integer (int32_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_max__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value compared with the addressed element; the larger of the two is written back. Typed 32-bit signed
 * integer (int32_t), matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_max(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the larger of val and the 32-bit unsigned integer (uint32_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_max__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value compared with the addressed element; the larger of the two is written back. Typed 32-bit
 * unsigned integer (uint32_t), matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_max(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the larger of val and the float (fp32) element at the Global Memory address back into that
 * element and returns the old value.
 *
 * @override_id asc_atomic_max__PGf32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] val Value compared with the addressed element; the larger of the two is written back. Typed float (fp32),
 * matching the addressed element.
 */
__aicore__ inline float asc_atomic_max(__gm__ float* address, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the larger of val and the 64-bit signed integer (int64_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_max__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value compared with the addressed element; the larger of the two is written back. Typed 64-bit signed
 * integer (int64_t), matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_max(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the larger of val and the 64-bit unsigned integer (uint64_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_max__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value compared with the addressed element; the larger of the two is written back. Typed 64-bit
 * unsigned integer (uint64_t), matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_max(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the smaller of val and the 32-bit signed integer (int32_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_min__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value compared with the addressed element; the smaller of the two is written back. Typed 32-bit
 * signed integer (int32_t), matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_min(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the smaller of val and the 32-bit unsigned integer (uint32_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_min__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value compared with the addressed element; the smaller of the two is written back. Typed 32-bit
 * unsigned integer (uint32_t), matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_min(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the smaller of val and the float (fp32) element at the Global Memory address back into that
 * element and returns the old value.
 *
 * @override_id asc_atomic_min__PGf32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] val Value compared with the addressed element; the smaller of the two is written back. Typed float
 * (fp32), matching the addressed element.
 */
__aicore__ inline float asc_atomic_min(__gm__ float* address, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the smaller of val and the 64-bit signed integer (int64_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_min__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Value compared with the addressed element; the smaller of the two is written back. Typed 64-bit
 * signed integer (int64_t), matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_min(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes the smaller of val and the 64-bit unsigned integer (uint64_t) element at the Global Memory
 * address back into that element and returns the old value.
 *
 * @override_id asc_atomic_min__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Value compared with the addressed element; the smaller of the two is written back. Typed 64-bit
 * unsigned integer (uint64_t), matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_min(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes val into the 32-bit signed integer (int32_t) element at the Global Memory address only when
 * its old value equals compare, and returns the old value.
 *
 * @override_id asc_atomic_cas__PGi32_i32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] compare Value the addressed element must equal for the new value to be stored; typed 32-bit signed
 * integer (int32_t) like the addressed element.
 * @param [in] val Value written into the addressed element when the comparison succeeds. Typed 32-bit signed integer
 * (int32_t), matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_cas(__gm__ int32_t* address, int32_t compare, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes val into the 32-bit unsigned integer (uint32_t) element at the Global Memory address only
 * when its old value equals compare, and returns the old value.
 *
 * @override_id asc_atomic_cas__PGu32_u32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] compare Value the addressed element must equal for the new value to be stored; typed 32-bit unsigned
 * integer (uint32_t) like the addressed element.
 * @param [in] val Value written into the addressed element when the comparison succeeds. Typed 32-bit unsigned integer
 * (uint32_t), matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_cas(__gm__ uint32_t* address, uint32_t compare, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes val into the float (fp32) element at the Global Memory address only when its old value
 * equals compare, and returns the old value.
 *
 * @override_id asc_atomic_cas__PGf32_f32_f32
 * @archs 3510
 * @param [in,out] address Address of the single float (fp32) element in Global Memory that is read and updated
 * atomically.
 * @param [in] compare Value the addressed element must equal for the new value to be stored; typed float (fp32) like
 * the addressed element.
 * @param [in] val Value written into the addressed element when the comparison succeeds. Typed float (fp32), matching
 * the addressed element.
 */
__aicore__ inline float asc_atomic_cas(__gm__ float* address, float compare, float val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes val into the 64-bit signed integer (int64_t) element at the Global Memory address only when
 * its old value equals compare, and returns the old value.
 *
 * @override_id asc_atomic_cas__PGi64_i64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] compare Value the addressed element must equal for the new value to be stored; typed 64-bit signed
 * integer (int64_t) like the addressed element.
 * @param [in] val Value written into the addressed element when the comparison succeeds. Typed 64-bit signed integer
 * (int64_t), matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_cas(__gm__ int64_t* address, int64_t compare, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically writes val into the 64-bit unsigned integer (uint64_t) element at the Global Memory address only
 * when its old value equals compare, and returns the old value.
 *
 * @override_id asc_atomic_cas__PGu64_u64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] compare Value the addressed element must equal for the new value to be stored; typed 64-bit unsigned
 * integer (uint64_t) like the addressed element.
 * @param [in] val Value written into the addressed element when the comparison succeeds. Typed 64-bit unsigned integer
 * (uint64_t), matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_cas(__gm__ uint64_t* address, uint64_t compare, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit signed integer (int32_t) element at the Global Memory address with the bitwise
 * AND of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_and__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise AND operand combined with the addressed element. Typed 32-bit signed integer (int32_t),
 * matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_and(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit unsigned integer (uint32_t) element at the Global Memory address with the
 * bitwise AND of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_and__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise AND operand combined with the addressed element. Typed 32-bit unsigned integer (uint32_t),
 * matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_and(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit signed integer (int64_t) element at the Global Memory address with the bitwise
 * AND of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_and__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise AND operand combined with the addressed element. Typed 64-bit signed integer (int64_t),
 * matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_and(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit unsigned integer (uint64_t) element at the Global Memory address with the
 * bitwise AND of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_and__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise AND operand combined with the addressed element. Typed 64-bit unsigned integer (uint64_t),
 * matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_and(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit signed integer (int32_t) element at the Global Memory address with the bitwise
 * OR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_or__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise OR operand combined with the addressed element. Typed 32-bit signed integer (int32_t),
 * matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_or(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit unsigned integer (uint32_t) element at the Global Memory address with the
 * bitwise OR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_or__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise OR operand combined with the addressed element. Typed 32-bit unsigned integer (uint32_t),
 * matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_or(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit signed integer (int64_t) element at the Global Memory address with the bitwise
 * OR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_or__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise OR operand combined with the addressed element. Typed 64-bit signed integer (int64_t),
 * matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_or(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit unsigned integer (uint64_t) element at the Global Memory address with the
 * bitwise OR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_or__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise OR operand combined with the addressed element. Typed 64-bit unsigned integer (uint64_t),
 * matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_or(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit signed integer (int32_t) element at the Global Memory address with the bitwise
 * XOR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_xor__PGi32_i32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit signed integer (int32_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise XOR operand combined with the addressed element. Typed 32-bit signed integer (int32_t),
 * matching the addressed element.
 */
__aicore__ inline int32_t asc_atomic_xor(__gm__ int32_t* address, int32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 32-bit unsigned integer (uint32_t) element at the Global Memory address with the
 * bitwise XOR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_xor__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the single 32-bit unsigned integer (uint32_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise XOR operand combined with the addressed element. Typed 32-bit unsigned integer (uint32_t),
 * matching the addressed element.
 */
__aicore__ inline uint32_t asc_atomic_xor(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit signed integer (int64_t) element at the Global Memory address with the bitwise
 * XOR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_xor__PGi64_i64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit signed integer (int64_t) element in Global Memory that is read
 * and updated atomically.
 * @param [in] val Bitwise XOR operand combined with the addressed element. Typed 64-bit signed integer (int64_t),
 * matching the addressed element.
 */
__aicore__ inline int64_t asc_atomic_xor(__gm__ int64_t* address, int64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically replaces the 64-bit unsigned integer (uint64_t) element at the Global Memory address with the
 * bitwise XOR of its old value and val and returns the old value.
 *
 * @override_id asc_atomic_xor__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the single 64-bit unsigned integer (uint64_t) element in Global Memory that is
 * read and updated atomically.
 * @param [in] val Bitwise XOR operand combined with the addressed element. Typed 64-bit unsigned integer (uint64_t),
 * matching the addressed element.
 */
__aicore__ inline uint64_t asc_atomic_xor(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically increments the uint32_t element at address in Global Memory and returns its previous value
 * (new_value = (old_value >= val) ? 0 : (old_value + 1)).
 *
 * @override_id asc_atomic_inc__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the uint32_t element in Global Memory that is read, updated and written back
 * atomically.
 * @param [in] val uint32_t scalar used by the update rule; the value written back depends on the comparison with the
 * old element.
 */
__aicore__ inline uint32_t asc_atomic_inc(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically increments the uint64_t element at address in Global Memory and returns its previous value
 * (new_value = (old_value >= val) ? 0 : (old_value + 1)).
 *
 * @override_id asc_atomic_inc__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the uint64_t element in Global Memory that is read, updated and written back
 * atomically.
 * @param [in] val uint64_t scalar used by the update rule; the value written back depends on the comparison with the
 * old element.
 */
__aicore__ inline uint64_t asc_atomic_inc(__gm__ uint64_t* address, uint64_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically decrements the uint32_t element at address in Global Memory and returns its previous value
 * (new_value = (old_value == 0 or old_value > val) ? val : (old_value - 1)).
 *
 * @override_id asc_atomic_dec__PGu32_u32
 * @archs 3510
 * @param [in,out] address Address of the uint32_t element in Global Memory that is read, updated and written back
 * atomically.
 * @param [in] val uint32_t scalar used by the update rule; the value written back depends on the comparison with the
 * old element.
 */
__aicore__ inline uint32_t asc_atomic_dec(__gm__ uint32_t* address, uint32_t val);
/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Atomically decrements the uint64_t element at address in Global Memory and returns its previous value
 * (new_value = (old_value == 0 or old_value > val) ? val : (old_value - 1)).
 *
 * @override_id asc_atomic_dec__PGu64_u64
 * @archs 3510
 * @param [in,out] address Address of the uint64_t element in Global Memory that is read, updated and written back
 * atomically.
 * @param [in] val uint64_t scalar used by the update rule; the value written back depends on the comparison with the
 * old element.
 */
__aicore__ inline uint64_t asc_atomic_dec(__gm__ uint64_t* address, uint64_t val);
} // namespace __asc_aicore

/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Sets the atomic-operation enable bit and the atomic-operation type for subsequent store atomics on A2/A3
 * generation products.
 *
 * @override_id asc_set_store_atomic_config_v1__u16_u16
 * @archs 2201
 * @param [in] type Atomic-operation enable and data type: 0 none, 1 float, 2 half, 3 int16_t, 4 int32_t, 5 int8_t, 6
 * bfloat16_t.
 * @param [in] op Atomic-operation kind; 0 selects the sum operation.
 */
__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type, uint16_t op);

/**
 * @ingroup simd_c_api.atomic.scalar_atomic
 * @brief Reads back the currently configured atomic-operation enable bits and atomic-operation type into an
 * asc_store_atomic_config value.
 *
 * @override_id asc_get_store_atomic_config__Ratomiccfg
 * @archs 2201 3510
 * @param [out] config Reference receiving the current atomic type/op configuration; its config field packs a 3-bit
 * atomic_type and a 2-bit atomic_op.
 */
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config);

ASC_DEPRECATED(9.1.0, "2027/09/07", asc_atomic_add)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated setter of the atomic-operation enable bits and atomic operation type.
 *
 * @override_id asc_set_store_atomic_config_v2__u16_u16
 * @archs 3510
 * @param [in] type Atomic operation enable selector, where 1 enables int32_t, 4 half, 5 float, 7 bfloat16_t, 9 int16_t
 * and 10 int8_t atomic operations; all other values have no defined meaning.
 * @param [in] op Atomic operation type, where 2 selects the summation operation; all other values have no defined
 * meaning.
 */
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_SCALAR_ATOMIC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_SCALAR_ATOMIC_H
#endif
