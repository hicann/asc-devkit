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
#warning \\
    "impl/tensor_api/arch/scalar_compute/scalar_compute_bit_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\"#include \\"tensor_api/arch/scalar_compute/scalar_compute_bit.h\\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_IMPL_H
#endif
#ifndef IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_IMPL_H
#define IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_IMPL_H

#include "c_api/asc_simd.h"

namespace asc {
namespace te {

__aicore__ inline uint64_t clear_nthbit(uint64_t bits, uint32_t bit_index) { return asc_clear_nthbit(bits, bit_index); }

__aicore__ inline uint64_t set_nthbit(uint64_t bits, uint32_t bit_index) { return asc_set_nthbit(bits, bit_index); }

__aicore__ inline int64_t clz(uint64_t value) { return asc_clz(value); }

__aicore__ inline int64_t ffs(uint64_t value) { return asc_ffs(value); }

__aicore__ inline int64_t ffz(uint64_t value) { return asc_ffz(value); }

__aicore__ inline int64_t popc(uint64_t value) { return asc_popc(value); }

__aicore__ inline int64_t sflbits(int64_t value) { return asc_sflbits(value); }

__aicore__ inline int64_t zero_bits_cnt(uint64_t value) { return asc_zero_bits_cnt(value); }

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_COMPUTE_BIT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_BIT_IMPL_H
#endif
