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
    "impl/c_api/reg_base_impl/cache_ctrl_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_CACHE_CTRL_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_CACHE_CTRL_INTF_IMPL_H

union ctrl_scalar_cache_bits {
    uint64_t value;
    struct {
        uint64_t reserved0 : 16;
        uint64_t scalar_load_cache_mode : 4;
        uint64_t scalar_store_cache_mode : 4;
        uint64_t reserved1 : 40;
    };
};

__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len) { preload(addr, prefetch_len); }

__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset) { dc_preload(address, offset); }

namespace __asc_aicore {
__aicore__ inline void asc_dcci_single(__gm__ void* dst) { dcci(dst, cache_line_t::SINGLE_CACHE_LINE); }
} // namespace __asc_aicore

__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst) { dcci(dst, cache_line_t::SINGLE_CACHE_LINE); }

__aicore__ inline void asc_dcci_entire_all()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_ALL);
}

__aicore__ inline void asc_dcci_entire_out()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline void asc_dcci_entire_atomic()
{
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_ATOMIC);
}

__aicore__ inline int64_t asc_get_icache_preload_status() { return get_icache_prl_st(); }

__aicore__ inline void asc_dci() { dci(); }

__aicore__ inline void asc_set_scalar_cache_mode(asc_load_l2_cache_mode l2_cache_mode)
{
    ctrl_scalar_cache_bits ctrl;
    ctrl.value = static_cast<uint64_t>(get_ctrl());
    ctrl.scalar_load_cache_mode = static_cast<uint8_t>(l2_cache_mode);
    set_ctrl(static_cast<int64_t>(ctrl.value));
}

__aicore__ inline void asc_set_scalar_cache_mode(asc_store_l2_cache_mode l2_cache_mode)
{
    ctrl_scalar_cache_bits ctrl;
    ctrl.value = static_cast<uint64_t>(get_ctrl());
    ctrl.scalar_store_cache_mode = static_cast<uint8_t>(l2_cache_mode);
    set_ctrl(static_cast<int64_t>(ctrl.value));
}

__aicore__ inline asc_load_l2_cache_mode asc_get_scalar_load_cache_mode()
{
    constexpr uint8_t start_bit = 16; // READ mode: CTRL[19:16]
    uint8_t cache_bits = static_cast<uint8_t>((get_ctrl() >> start_bit) & 0xF);
    return static_cast<asc_load_l2_cache_mode>(cache_bits);
}

__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode()
{
    constexpr uint8_t start_bit = 20; // WRITE mode: CTRL[23:20]
    uint8_t cache_bits = static_cast<uint8_t>((get_ctrl() >> start_bit) & 0xF);
    return static_cast<asc_store_l2_cache_mode>(cache_bits);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
