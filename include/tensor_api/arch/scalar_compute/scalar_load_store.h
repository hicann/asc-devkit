/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_H
#endif
#ifndef INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_H
#define INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_H

namespace asc {
namespace te {

/**
 * @brief Load a value from Global Memory without passing through the DCache.
 *
 * @tparam T Data type of the value.
 * @param addr Source Global Memory address.
 * @return Value loaded from the source address.
 */
template <typename T>
__aicore__ inline T load_dev(__gm__ T* addr);

/**
 * @brief Store a value to Global Memory without passing through the DCache.
 *
 * @tparam T Data type of the value.
 * @param addr Destination Global Memory address.
 * @param value Value written to the destination address.
 */
template <typename T>
__aicore__ inline void store_dev(__gm__ T* addr, T value);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/scalar_compute/scalar_load_store_impl.h"

#endif // INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_H
#endif
