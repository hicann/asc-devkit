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
#error "atomic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_ATOMIC_ATOMIC_H
#define INCLUDE_C_API_ATOMIC_ATOMIC_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/atomic_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/atomic_impl.h"
#endif

__aicore__ inline void asc_set_atomic_add_float();

__aicore__ inline void asc_set_atomic_max_float16();

__aicore__ inline void asc_set_atomic_add_int();

__aicore__ inline void asc_set_atomic_add_int8();

__aicore__ inline void asc_set_atomic_add_int16();

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

__aicore__ inline void asc_set_atomic_none();

__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config);

__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type, uint16_t op);

__aicore__ inline void asc_set_atomic_add_bfloat();

__aicore__ inline void asc_set_atomic_add_float16();

__aicore__ inline void asc_set_atomic_max_bfloat();

__aicore__ inline void asc_set_atomic_max_float();

__aicore__ inline void asc_set_atomic_max_int();

__aicore__ inline void asc_set_atomic_max_int8();

__aicore__ inline void asc_set_atomic_max_int16();

__aicore__ inline void asc_set_atomic_min_bfloat();

__aicore__ inline void asc_set_atomic_min_float();

__aicore__ inline void asc_set_atomic_min_float16();

__aicore__ inline void asc_set_atomic_min_int();

__aicore__ inline void asc_set_atomic_min_int8();

__aicore__ inline void asc_set_atomic_min_int16();

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

__aicore__ inline void asc_set_atomic_add_float16();

__aicore__ inline void asc_set_atomic_max_float();

[[deprecated("NOTICE: asc_get_store_atomic_config is deprecated on Ascend 950PR/Ascend 950DT."
             "Please use asc_atomic_add instead for atomic add operation.")]]
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config);

[[deprecated("NOTICE: asc_set_store_atomic_config_v2 is deprecated."
             "Please use asc_atomic_add instead for atomic add operation.")]]
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op);

__aicore__ inline void asc_set_atomic_max_int();

__aicore__ inline void asc_set_atomic_max_int8();

__aicore__ inline void asc_set_atomic_max_int16();

__aicore__ inline void asc_set_atomic_min_int();

__aicore__ inline void asc_set_atomic_min_int8();

__aicore__ inline void asc_set_atomic_min_int16();

#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
