/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_DEVICE_ATOMIC_FUNCTIONS_H
#define INCLUDE_SIMT_API_DEVICE_ATOMIC_FUNCTIONS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_ATOMIC_FUNCTIONS_H__
#endif

#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510)

namespace __asc_simt_vf {
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_add(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_add(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_add(__ubuf__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_add(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_add(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_add(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_add(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_add(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_sub(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_sub(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_sub(__ubuf__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_sub(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_sub(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_sub(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_sub(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_sub(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_exch(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_exch(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_exch(__ubuf__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_exch(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_exch(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_exch(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_exch(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_exch(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_max(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_max(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_max(__ubuf__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_max(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_max(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_max(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_max(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_max(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_min(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_min(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_min(__ubuf__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_min(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_min(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_min(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_min(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_min(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_inc(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_inc(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_inc(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_dec(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_dec(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_dec(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_cas(__ubuf__ int32_t* address, int32_t compare, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_cas(
    __ubuf__ uint32_t* address, uint32_t compare, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_cas(__ubuf__ float* address, float compare, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_cas(__gm__ int32_t* address, int32_t compare, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_cas(__gm__ uint32_t* address, uint32_t compare, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_cas(__gm__ int64_t* address, int64_t compare, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_cas(__gm__ uint64_t* address, uint64_t compare, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_cas(__gm__ float* address, float compare, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_and(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_and(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_and(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_and(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_and(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_and(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_or(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_or(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_or(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_or(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_or(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_or(__gm__ uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_xor(__ubuf__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_xor(__ubuf__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_xor(__gm__ int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_xor(__gm__ uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_xor(__gm__ int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_xor(__gm__ uint64_t* address, uint64_t val);

#else
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_add(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_add(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_add(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_add(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_add(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_sub(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_sub(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_sub(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_sub(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_sub(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_exch(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_exch(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_exch(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_exch(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_exch(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_max(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_max(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_max(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_max(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_max(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_min(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_min(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_min(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_min(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_min(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_inc(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_inc(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_dec(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_dec(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_cas(int32_t* address, int32_t compare, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_cas(uint32_t* address, uint32_t compare, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_cas(int64_t* address, int64_t compare, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_cas(uint64_t* address, uint64_t compare, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_atomic_cas(float* address, float compare, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_and(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_and(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_and(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_and(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_or(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_or(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_or(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_or(uint64_t* address, uint64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t asc_atomic_xor(int32_t* address, int32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_atomic_xor(uint32_t* address, uint32_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int64_t asc_atomic_xor(int64_t* address, int64_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_atomic_xor(uint64_t* address, uint64_t val);
#endif
#endif
} // namespace __asc_simt_vf
#include "impl/simt_api/device_atomic_functions_impl.h"

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_ATOMIC_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_ATOMIC_FUNCTIONS_H__
#endif

#endif // INCLUDE_SIMT_API_DEVICE_ATOMIC_FUNCTIONS_H
