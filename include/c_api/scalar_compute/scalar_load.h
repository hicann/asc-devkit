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
#error "scalar_load.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_LOAD_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_LOAD_H
#define INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_LOAD_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/scalar_compute_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/scalar_compute_impl.h"
#endif

__aicore__ inline int8_t asc_load_dev(__gm__ int8_t* addr);
__aicore__ inline uint8_t asc_load_dev(__gm__ uint8_t* addr);
__aicore__ inline int16_t asc_load_dev(__gm__ int16_t* addr);
__aicore__ inline uint16_t asc_load_dev(__gm__ uint16_t* addr);
__aicore__ inline int32_t asc_load_dev(__gm__ int32_t* addr);
__aicore__ inline uint32_t asc_load_dev(__gm__ uint32_t* addr);
__aicore__ inline int64_t asc_load_dev(__gm__ int64_t* addr);
__aicore__ inline uint64_t asc_load_dev(__gm__ uint64_t* addr);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_LOAD_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_LOAD_H
#endif
