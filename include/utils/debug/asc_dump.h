/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/*!
 * \file asc_dump.h
 * \brief
 */
#ifndef INCLUDE_UTILS_DEBUG_ASC_DUMP_H
#define INCLUDE_UTILS_DEBUG_ASC_DUMP_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_DUMP_H__
#endif

#include "impl/utils/debug/asc_simd_debug_enable.h"
#include "impl/utils/sys_macros.h"

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510)
#include "impl/utils/debug/asc_aicore_dump_impl.h"
#endif

#if __NPU_ARCH__ == 3510
#include "impl/utils/debug/asc_simd_dump_impl.h"
#endif

namespace __asc_aicore {
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size);

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size);

#if defined(__NPU_ARCH__)
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__biasbuf__ T* input, uint32_t desc, uint32_t dump_size);
#endif

template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__fbuf__ T* input, uint32_t desc, uint32_t dump_size);
}

namespace __asc_simd_vf {
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size);

template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size);

template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size);

template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size);
}

#if defined(__UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_DUMP_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_DUMP_H__
#endif

#endif
