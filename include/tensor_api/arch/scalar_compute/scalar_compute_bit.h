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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_H
#endif
#ifndef INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_H
#define INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

namespace asc {
namespace te {

/** @brief Clear the specified bit of a 64-bit unsigned integer. */
__aicore__ inline uint64_t clear_nthbit(uint64_t bits, uint32_t bit_index);

/** @brief Set the specified bit of a 64-bit unsigned integer. */
__aicore__ inline uint64_t set_nthbit(uint64_t bits, uint32_t bit_index);

/** @brief Return the number of leading zero bits in a 64-bit unsigned integer. */
__aicore__ inline int64_t clz(uint64_t value);

/** @brief Return the index of the first set bit in a 64-bit unsigned integer. */
__aicore__ inline int64_t ffs(uint64_t value);

/** @brief Return the index of the first zero bit in a 64-bit unsigned integer. */
__aicore__ inline int64_t ffz(uint64_t value);

/** @brief Return the number of set bits in a 64-bit unsigned integer. */
__aicore__ inline int64_t popc(uint64_t value);

/** @brief Return the count of bits following the sign bit of a 64-bit signed integer. */
__aicore__ inline int64_t sflbits(int64_t value);

/** @brief Return the number of zero bits in a 64-bit unsigned integer. */
__aicore__ inline int64_t zero_bits_cnt(uint64_t value);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/scalar_compute/scalar_compute_bit_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_H
#endif
