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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CONTROL_H
#endif

/*!
 * \file cache_control.h
 * \brief Data cache control interfaces.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_H
#define INCLUDE_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_H

#include "tensor_api/utils/utils.h"
#include "tensor_api/tensor/pointer.h"

namespace asc {
namespace te {

enum class load_l2_cache_mode : uint8_t {
    normal_first_victim = 0,
    normal_last_victim = 1,
    normal_persistent = 2,
    notalloc_keep = 4,
    notalloc_clean = 5,
    notalloc_drop = 6
};

enum class store_l2_cache_mode : uint8_t {
    normal_first_victim = 0,
    normal_last_victim = 1,
    normal_persistent = 2,
    notalloc_clean = 4
};

/** @brief Target domain of an entire-DCache clean-and-invalidate operation.
 *
 * @param all Refreshes all valid cache lines.
 * @param ub Refreshes valid cache lines containing Unified Buffer addresses.
 * @param gm Refreshes valid cache lines containing Global Memory addresses.
 * @param atomic Refreshes valid cache lines related to scalar atomic operations.
 */
enum class clean_target {
    all,    //!< Refreshes all valid cache lines.
    ub,     //!< Refreshes valid cache lines containing Unified Buffer addresses.
    gm,     //!< Refreshes valid cache lines containing Global Memory addresses.
    atomic, //!< Refreshes valid cache lines related to scalar atomic operations.
};

/** @brief Preload data into the DCache. */
__aicore__ inline void datacache_preload(__gm__ uint64_t* address, int64_t offset);

/**
 * @brief Clean and invalidate DCache lines in the selected target domain.
 *
 * @param target DCache target domain to clean and invalidate.
 */
__aicore__ inline void datacache_clean_and_invalid(clean_target target);

/** @brief Clean and invalidate the DCache line containing the input address. */
template <typename T>
__aicore__ inline void datacache_clean_and_invalid_line(T address);

/** @brief Invalidate the entire DCache without writing back dirty lines. */
__aicore__ inline void datacache_clean();

/** @brief Return the ICache preload status. */
__aicore__ inline int64_t get_icache_preload_status();

/** @brief Return the scalar load L2 cache mode. */
__aicore__ inline load_l2_cache_mode get_scalar_load_cache_mode();

/** @brief Return the scalar store L2 cache mode. */
__aicore__ inline store_l2_cache_mode get_scalar_store_cache_mode();

/** @brief Preload instructions into the ICache. */
__aicore__ inline void icache_preload(const void* addr, int64_t prefetch_len);

/** @brief Set the scalar load L2 cache mode. */
__aicore__ inline void set_scalar_cache_mode(load_l2_cache_mode mode);

/** @brief Set the scalar store L2 cache mode. */
__aicore__ inline void set_scalar_cache_mode(store_l2_cache_mode mode);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/cache_control/cache_control_impl.h"

#endif // INCLUDE_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CONTROL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CONTROL_H
#endif
