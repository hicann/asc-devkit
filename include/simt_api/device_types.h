/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_DEVICE_TYPES_H
#define INCLUDE_SIMT_API_DEVICE_TYPES_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_TYPES_H__
#endif

#if defined(ASCENDC_CPU_DEBUG)
#include "simt_stub.h"
#endif
#include "utils/base/sys_macros.h"

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#define __SIMT_DEVICE_FUNCTIONS_DECL__ __aicore__
#else
#define __SIMT_DEVICE_FUNCTIONS_DECL__ __simt_callee__
#endif
// Keep the public API flexible: one to five architecture arguments followed by the message.
// Each overload maps directly to one diagnose_if expression to keep macro diagnostics short.
#if defined(ASC_SIMT_SIMULATION_CHECK) && defined(__NPU_ARCH__)
#define __ASC_SIMT_SIMULATION_CHECK_ARCH_MATCH_4__(arch1, arch2, arch3, arch4) \
    (__NPU_ARCH__ == arch1 || __NPU_ARCH__ == arch2 || __NPU_ARCH__ == arch3 || __NPU_ARCH__ == arch4)
#define __ASC_SIMT_SIMULATION_CHECK_1(arch1, msg) __attribute__((diagnose_if(__NPU_ARCH__ == arch1, msg, "warning")))
#define __ASC_SIMT_SIMULATION_CHECK_2(arch1, arch2, msg) \
    __attribute__((diagnose_if(__NPU_ARCH__ == arch1 || __NPU_ARCH__ == arch2, msg, "warning")))
#define __ASC_SIMT_SIMULATION_CHECK_3(arch1, arch2, arch3, msg) \
    __attribute__((                                             \
        diagnose_if(__NPU_ARCH__ == arch1 || __NPU_ARCH__ == arch2 || __NPU_ARCH__ == arch3, msg, "warning")))
#define __ASC_SIMT_SIMULATION_CHECK_4(arch1, arch2, arch3, arch4, msg) \
    __attribute__((diagnose_if(__ASC_SIMT_SIMULATION_CHECK_ARCH_MATCH_4__(arch1, arch2, arch3, arch4), msg, "warning")))
#define __ASC_SIMT_SIMULATION_CHECK_5(arch1, arch2, arch3, arch4, arch5, msg)                               \
    __attribute__((diagnose_if(                                                                             \
        __NPU_ARCH__ == arch1 || __NPU_ARCH__ == arch2 || __NPU_ARCH__ == arch3 || __NPU_ARCH__ == arch4 || \
            __NPU_ARCH__ == arch5,                                                                          \
        msg, "warning")))
#else
#define __ASC_SIMT_SIMULATION_CHECK_1(arch1, msg)
#define __ASC_SIMT_SIMULATION_CHECK_2(arch1, arch2, msg)
#define __ASC_SIMT_SIMULATION_CHECK_3(arch1, arch2, arch3, msg)
#define __ASC_SIMT_SIMULATION_CHECK_4(arch1, arch2, arch3, arch4, msg)
#define __ASC_SIMT_SIMULATION_CHECK_5(arch1, arch2, arch3, arch4, arch5, msg)
#endif

#define __ASC_SIMT_SIMULATION_CHECK_PICK__(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define __ASC_SIMT_SIMULATION_CHECK__(...)                                                                        \
    __ASC_SIMT_SIMULATION_CHECK_PICK__(                                                                           \
        __VA_ARGS__, __ASC_SIMT_SIMULATION_CHECK_5, __ASC_SIMT_SIMULATION_CHECK_4, __ASC_SIMT_SIMULATION_CHECK_3, \
        __ASC_SIMT_SIMULATION_CHECK_2, __ASC_SIMT_SIMULATION_CHECK_1)                                             \
    (__VA_ARGS__)

#if defined(__ASC_FTZ__)
#define __ASC_SIMT_SIMULATION_EXP_CHECK__(...)
#else
#define __ASC_SIMT_SIMULATION_EXP_CHECK__(...) __ASC_SIMT_SIMULATION_CHECK__(__VA_ARGS__)
#endif

#if defined(__ASC_FTZ__) && !defined(__ASC_PREC_SQRT__)
#define __ASC_SIMT_SIMULATION_SQRT_CHECK__(...)
#else
#define __ASC_SIMT_SIMULATION_SQRT_CHECK__(...) __ASC_SIMT_SIMULATION_CHECK__(__VA_ARGS__)
#endif

#if defined(ASCENDC_USE_LEGACY_PRECISION)
#define __ASC_SIMT_SIMULATION_POW_CHECK__(...)
#else
#define __ASC_SIMT_SIMULATION_POW_CHECK__(...) __ASC_SIMT_SIMULATION_CHECK__(__VA_ARGS__)
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_TYPES_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_TYPES_H__
#endif

#endif // INCLUDE_SIMT_API_DEVICE_TYPES_H
