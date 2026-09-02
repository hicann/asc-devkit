/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "scalar_store.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_STORE_H
#define INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_STORE_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/scalar_compute_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/scalar_compute_impl.h"
#endif

__aicore__ inline void asc_store_dev(__gm__ int8_t* addr, int8_t value);
__aicore__ inline void asc_store_dev(__gm__ uint8_t* addr, uint8_t value);
__aicore__ inline void asc_store_dev(__gm__ int16_t* addr, int16_t value);
__aicore__ inline void asc_store_dev(__gm__ uint16_t* addr, uint16_t value);
__aicore__ inline void asc_store_dev(__gm__ int32_t* addr, int32_t value);
__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr, uint32_t value);
__aicore__ inline void asc_store_dev(__gm__ int64_t* addr, int64_t value);
__aicore__ inline void asc_store_dev(__gm__ uint64_t* addr, uint64_t value);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H
#endif
