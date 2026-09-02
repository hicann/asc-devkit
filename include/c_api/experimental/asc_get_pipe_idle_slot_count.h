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
#error "asc_get_pipe_idle_slot_count.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_EXPERIMENTAL_ASC_GET_PIPE_IDLE_SLOT_COUNT_H
#endif

#ifndef INCLUDE_C_API_EXPERIMENTAL_ASC_GET_PIPE_IDLE_SLOT_COUNT_H
#define INCLUDE_C_API_EXPERIMENTAL_ASC_GET_PIPE_IDLE_SLOT_COUNT_H

#ifndef CANN_ASC_USE_EXPERIMENTAL
#error "this is experimental feature, pls set CANN_ASC_USE_EXPERIMENTAL = ON to use it"
#endif
// WARNING: The APIs in this header are unstable and may change or be removed in future releases.

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_pipe_idle_slot_count_impl.h"
#endif

__aicore__ inline uint8_t asc_get_pipe_mte2_idle_slot_count();

__aicore__ inline uint8_t asc_get_pipe_mte3_idle_slot_count();

__aicore__ inline uint8_t asc_get_pipe_v_idle_slot_count();

__aicore__ inline uint8_t asc_get_pipe_m_idle_slot_count();

__aicore__ inline uint8_t asc_get_pipe_mte1_idle_slot_count();

__aicore__ inline uint8_t asc_get_pipe_fix_idle_slot_count();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_EXPERIMENTAL_ASC_GET_PIPE_IDLE_SLOT_COUNT_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_EXPERIMENTAL_ASC_GET_PIPE_IDLE_SLOT_COUNT_H
#endif
