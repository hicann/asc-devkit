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

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

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

/**
 * @brief Clean and invalidate DCache lines in the selected target domain.
 *
 * @param target DCache target domain to clean and invalidate.
 */
__aicore__ inline void datacache_clean_and_invalid(clean_target target);

/** @brief Invalidate the entire DCache without writing back dirty lines. */
__aicore__ inline void datacache_clean();

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/cache_control/cache_control_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_CACHE_CONTROL_CACHE_CONTROL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CONTROL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CONTROL_H
#endif
