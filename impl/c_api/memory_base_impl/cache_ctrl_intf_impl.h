/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/memory_base_impl/cache_ctrl_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_CACHE_CTRL_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_CACHE_CTRL_INTF_IMPL_H

namespace __asc_aicore {
__aicore__ inline void asc_dcci_single(__gm__ void* dst) { dcci(dst, cache_line_t::SINGLE_CACHE_LINE); }
} // namespace __asc_aicore

__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst) { dcci(dst, cache_line_t::SINGLE_CACHE_LINE); }

__aicore__ inline void asc_dcci_entire_all()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_ALL);
}

__aicore__ inline void asc_dcci_entire_ub()
{
    dcci((__ubuf__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_UB);
}

__aicore__ inline void asc_dcci_entire_out()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline void asc_dcci_entire_atomic()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_ATOMIC);
}

__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset) { dc_preload(address, offset); }

__aicore__ inline int64_t asc_get_icache_preload_status() { return get_icache_prl_st(); }

__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len) { preload(addr, prefetch_len); }

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
