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
#error "spr/spr.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H
#endif

#ifndef INCLUDE_C_API_SPR_SPR_H
#define INCLUDE_C_API_SPR_SPR_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/sys_var_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl.h"
#endif

__aicore__ inline int64_t asc_get_ctrl();

__aicore__ inline void asc_set_ctrl(uint64_t config);

__aicore__ inline int64_t asc_get_squeeze_status();

[[deprecated("NOTICE: asc_get_ar_spr is deprecated. "
             "Please use asc_get_squeeze_status instead.")]]
__aicore__ inline int64_t asc_get_ar_spr();

__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat);
__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode);
__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy);
__aicore__ inline asc_override_strategy asc_get_saturation_strategy();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H
#endif
