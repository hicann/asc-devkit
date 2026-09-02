/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cache_ctrl.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H
#endif

#ifndef INCLUDE_C_API_CACHE_CTRL_CACHE_CTRL_H
#define INCLUDE_C_API_CACHE_CTRL_CACHE_CTRL_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

#include "impl/c_api/instr_impl/npu_arch_2201/cache_ctrl_impl.h"

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

#include "impl/c_api/instr_impl/npu_arch_3510/cache_ctrl_impl.h"

#endif

__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len);

namespace __asc_aicore {
__aicore__ inline void asc_dcci_single(__gm__ void* dst);
}

__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst);

__aicore__ inline void asc_dcci_entire_ub();

__aicore__ inline void asc_dcci_entire_out();

__aicore__ inline void asc_dcci_entire_all();

__aicore__ inline void asc_dcci_entire_atomic();

__aicore__ inline int64_t asc_get_icache_preload_status();

__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset);

__aicore__ inline void asc_dci();

__aicore__ inline void asc_set_scalar_cache_mode(asc_load_l2_cache_mode l2_cache_mode);

__aicore__ inline void asc_set_scalar_cache_mode(asc_store_l2_cache_mode l2_cache_mode);

__aicore__ inline asc_load_l2_cache_mode asc_get_scalar_load_cache_mode();

__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H
#endif
