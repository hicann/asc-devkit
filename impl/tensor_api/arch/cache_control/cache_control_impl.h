/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License"). You may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cache_control/cache_control_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"tensor_api/arch/cache_control/cache_control.h\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H
#endif

/*!
 * \file cache_control_impl.h
 * \brief Data cache control implementation.
 */
#ifndef IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H
#define IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H

#include "c_api/asc_simd.h"
#include "tensor_api/utils/utils.h"

namespace asc {
namespace te {

template <typename T, typename = void>
struct cache_line_location {
    using type = get_attribute_location<T>;
};

template <typename T>
struct cache_line_location<T, void_t<typename T::ptr_pattern>> {
    using type = typename T::ptr_pattern;
};

template <typename T>
using cache_line_location_t = typename cache_line_location<T>::type;

__aicore__ inline void datacache_clean_and_invalid(clean_target target)
{
    switch (target) {
        case clean_target::all:
            asc_dcci_entire_all();
            break;
        case clean_target::ub:
#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
            asc_dcci_entire_ub();
#endif
            break;
        case clean_target::gm:
            asc_dcci_entire_out();
            break;
        case clean_target::atomic:
            asc_dcci_entire_atomic();
            break;
        default:
            break;
    }
}

__aicore__ inline void datacache_preload(__gm__ uint64_t* address, int64_t offset)
{
    asc_datacache_preload(address, offset);
}

template <typename T>
__aicore__ inline void datacache_clean_and_invalid_line(T address)
{
    using pointer_type = Std::remove_cvref_t<T>;
    using location_type = cache_line_location_t<pointer_type>;
    if constexpr (Std::is_same_v<location_type, location::gm>) {
        if constexpr (is_hardware_mem_ptr_v<pointer_type>) {
            __asc_aicore::asc_dcci_single(reinterpret_cast<__gm__ void*>(address.get()));
        } else {
            __asc_aicore::asc_dcci_single(reinterpret_cast<__gm__ void*>(address));
        }
    } else {
        if constexpr (is_hardware_mem_ptr_v<pointer_type>) {
            asc_ub_dcci_single(reinterpret_cast<__ubuf__ void*>(address.get()));
        } else {
            asc_ub_dcci_single(reinterpret_cast<__ubuf__ void*>(address));
        }
    }
}

__aicore__ inline void datacache_clean() { asc_dci(); }

__aicore__ inline int64_t get_icache_preload_status() { return asc_get_icache_preload_status(); }

__aicore__ inline load_l2_cache_mode get_scalar_load_cache_mode()
{
    return static_cast<load_l2_cache_mode>(static_cast<uint8_t>(asc_get_scalar_load_cache_mode()));
}

__aicore__ inline store_l2_cache_mode get_scalar_store_cache_mode()
{
    return static_cast<store_l2_cache_mode>(static_cast<uint8_t>(asc_get_scalar_store_cache_mode()));
}

__aicore__ inline void icache_preload(const void* addr, int64_t prefetch_len)
{
    asc_icache_preload(addr, prefetch_len);
}

__aicore__ inline void set_scalar_cache_mode(load_l2_cache_mode mode)
{
    asc_set_scalar_cache_mode(static_cast<asc_load_l2_cache_mode>(static_cast<uint8_t>(mode)));
}

__aicore__ inline void set_scalar_cache_mode(store_l2_cache_mode mode)
{
    asc_set_scalar_cache_mode(static_cast<asc_store_l2_cache_mode>(static_cast<uint8_t>(mode)));
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H

#if defined( \
    UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_IMPL_H
#endif
