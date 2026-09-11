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
    "impl/c_api/memory_base_impl/scalar_bit_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_SCALAR_BIT_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_SCALAR_BIT_INTF_IMPL_H

__aicore__ inline int64_t asc_clz(uint64_t value_in) { return clz(value_in); }

__aicore__ inline int64_t asc_ffs(uint64_t value) { return sff1(value); }

__aicore__ inline int64_t asc_sflbits(int64_t value) { return sflbits(value); }

__aicore__ inline int64_t asc_zero_bits_cnt(uint64_t value) { return bcnt0(value); }

__aicore__ inline int64_t asc_ffz(uint64_t value) { return sff0(value); }

__aicore__ inline int64_t asc_popc(uint64_t value) { return bcnt1(value); }

__aicore__ inline uint64_t asc_set_nthbit(uint64_t bits, int64_t idx) { return sbitset1(bits, idx); }

__aicore__ inline uint64_t asc_clear_nthbit(uint64_t bits, int64_t idx) { return sbitset0(bits, idx); }

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
