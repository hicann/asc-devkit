/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file sys_macros.h
 * \brief
 */
#ifndef IMPL_UTILS_SYS_MACROS_H
#define IMPL_UTILS_SYS_MACROS_H

#include <cstdint>
#include "utils/base/sys_constants.h"

#if (defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1))
#include "stub_def.h"
#endif

#if !defined(ASCENDC_CPU_DEBUG) || ASCENDC_CPU_DEBUG != 1
// For ascc preprocess: __global__ should not be replaced
#ifdef __ASCC_PRE__
#ifdef __global__
#undef __global__
#endif
#else

#ifndef __aicore__
#define __aicore__ [aicore]
#endif // __aicore__

#ifndef __host__
#define __host__ __attribute__((cce_host))
#endif // __host__

#ifndef __host_aicore__
#define __host_aicore__ [ host, aicore ]
#endif // __host_aicore__

#ifndef __disable_kernel_type_autoinfer__
#define __disable_kernel_type_autoinfer__
#endif // __disable_kernel_type_autoinfer__

#endif // __ASCC_PRE__
#endif

#ifndef K_MAX_SHAPE_DIM
#define K_MAX_SHAPE_DIM 8
#endif

#if (defined(__DAV_CUBE__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510))
#define SPLIT_CORE_CUBE
#endif

#if (defined(__DAV_VEC__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510))
#define SPLIT_CORE_VEC
#endif

#if defined(ASCENDC_CPU_DEBUG)
extern int32_t g_coreType;
#define ASCEND_IS_AIV (g_coreType == AscendC::AIV)
#define ASCEND_IS_AIC (g_coreType == AscendC::AIC)
#define ASCEND_IS_NOT_AIV (g_coreType != AscendC::AIV)
#define ASCEND_IS_NOT_AIC (g_coreType != AscendC::AIC)
#else
#if defined(SPLIT_CORE_CUBE)
constexpr int32_t g_coreType = AscendC::AIC;
#elif defined(SPLIT_CORE_VEC)
constexpr int32_t g_coreType = AscendC::AIV;
#else
constexpr int32_t g_coreType = AscendC::MIX;
#endif
#define ASCEND_IS_AIV constexpr(g_coreType == AscendC::AIV)
#define ASCEND_IS_AIC constexpr(g_coreType == AscendC::AIC)
#define ASCEND_IS_NOT_AIV constexpr(g_coreType != AscendC::AIV)
#define ASCEND_IS_NOT_AIC constexpr(g_coreType != AscendC::AIC)
#endif

#if defined(__ASC_DISABLE_RESERVED_UBUF__)
#define __ASC_RESERVED_UBUF_UNAVAILABLE_ATTR__(msg) __attribute__((unavailable(msg)))
#else
#define __ASC_RESERVED_UBUF_UNAVAILABLE_ATTR__(msg)
#endif

#define __ASC_RESERVED_UBUF_CAT__(a, b) __ASC_RESERVED_UBUF_CAT_IMPL__(a, b)
#define __ASC_RESERVED_UBUF_CAT_IMPL__(a, b) a##b
// Probe whether an architecture has been registered below. A registered marker expands to
// "~, 1", so CHECK_N returns 1; an unregistered marker stays as one token and returns 0.
#define __ASC_RESERVED_UBUF_PROBE__() ~, 1
#define __ASC_RESERVED_UBUF_CHECK_N__(_0, n, ...) n
#define __ASC_RESERVED_UBUF_CHECK__(...) __ASC_RESERVED_UBUF_CHECK_N__(__VA_ARGS__, 0)
#define __ASC_RESERVED_UBUF_IS_PROBE__(...) __ASC_RESERVED_UBUF_CHECK__(__VA_ARGS__)

// Architecture whitelist for __ASC_USE_RESERVED_UBUF__. Add new chips here first so typos
// report a clear unsupported-architecture error even when reserved UBUF disabling is off.
#define __ASC_RESERVED_UBUF_ARCH_2201 __ASC_RESERVED_UBUF_PROBE__()
#define __ASC_RESERVED_UBUF_ARCH_3510 __ASC_RESERVED_UBUF_PROBE__()
#define __ASC_RESERVED_UBUF_IS_SUPPORTED_ARCH__(arch) \
    __ASC_RESERVED_UBUF_IS_PROBE__(__ASC_RESERVED_UBUF_CAT__(__ASC_RESERVED_UBUF_ARCH_, arch))

#define __ASC_RESERVED_UBUF_PRAGMA__(x) _Pragma(#x)
#define __ASC_RESERVED_UBUF_UNSUPPORTED_ARCH_ERROR__() \
    __ASC_RESERVED_UBUF_PRAGMA__(                      \
        GCC error                                      \
        "unsupported chip architecture in __ASC_USE_RESERVED_UBUF__; supported architectures are 2201 and 3510")

#if defined(__ASC_DISABLE_RESERVED_UBUF__) && defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#define __ASC_USE_RESERVED_UBUF_2201(msg) __ASC_RESERVED_UBUF_UNAVAILABLE_ATTR__(msg)
#else
#define __ASC_USE_RESERVED_UBUF_2201(msg)
#endif

#if defined(__ASC_DISABLE_RESERVED_UBUF__) && defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#define __ASC_USE_RESERVED_UBUF_3510(msg) __ASC_RESERVED_UBUF_UNAVAILABLE_ATTR__(msg)
#else
#define __ASC_USE_RESERVED_UBUF_3510(msg)
#endif

// Dispatch one architecture: unsupported chips fail fast, supported chips expand to the
// architecture-specific unavailable attribute only when __ASC_DISABLE_RESERVED_UBUF__ matches.
#define __ASC_USE_RESERVED_UBUF_SUPPORTED_0(arch, msg) __ASC_RESERVED_UBUF_UNSUPPORTED_ARCH_ERROR__()
#define __ASC_USE_RESERVED_UBUF_SUPPORTED_1(arch, msg) __ASC_RESERVED_UBUF_CAT__(__ASC_USE_RESERVED_UBUF_, arch)(msg)
#define __ASC_USE_RESERVED_UBUF_CALL__(supported, arch, msg) __ASC_USE_RESERVED_UBUF_CALL_IMPL__(supported, arch, msg)
#define __ASC_USE_RESERVED_UBUF_CALL_IMPL__(supported, arch, msg) \
    __ASC_RESERVED_UBUF_CAT__(__ASC_USE_RESERVED_UBUF_SUPPORTED_, supported)(arch, msg)
#define __ASC_USE_RESERVED_UBUF_IMPL__(arch, msg) \
    __ASC_USE_RESERVED_UBUF_CALL__(__ASC_RESERVED_UBUF_IS_SUPPORTED_ARCH__(arch), arch, msg)

// Public entry supports one to five architecture arguments followed by the diagnostic message.
// Example: __ASC_USE_RESERVED_UBUF__(2201, 3510, "API is forbidden ...")
#define __ASC_USE_RESERVED_UBUF_1(arch1, msg) __ASC_USE_RESERVED_UBUF_IMPL__(arch1, msg)
#define __ASC_USE_RESERVED_UBUF_2(arch1, arch2, msg) \
    __ASC_USE_RESERVED_UBUF_IMPL__(arch1, msg)       \
    __ASC_USE_RESERVED_UBUF_IMPL__(arch2, msg)
#define __ASC_USE_RESERVED_UBUF_3(arch1, arch2, arch3, msg) \
    __ASC_USE_RESERVED_UBUF_2(arch1, arch2, msg)            \
    __ASC_USE_RESERVED_UBUF_IMPL__(arch3, msg)
#define __ASC_USE_RESERVED_UBUF_4(arch1, arch2, arch3, arch4, msg) \
    __ASC_USE_RESERVED_UBUF_3(arch1, arch2, arch3, msg)            \
    __ASC_USE_RESERVED_UBUF_IMPL__(arch4, msg)
#define __ASC_USE_RESERVED_UBUF_5(arch1, arch2, arch3, arch4, arch5, msg) \
    __ASC_USE_RESERVED_UBUF_4(arch1, arch2, arch3, arch4, msg)            \
    __ASC_USE_RESERVED_UBUF_IMPL__(arch5, msg)

#define __ASC_USE_RESERVED_UBUF_PICK__(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define __ASC_USE_RESERVED_UBUF__(...)                                                                \
    __ASC_USE_RESERVED_UBUF_PICK__(                                                                   \
        __VA_ARGS__, __ASC_USE_RESERVED_UBUF_5, __ASC_USE_RESERVED_UBUF_4, __ASC_USE_RESERVED_UBUF_3, \
        __ASC_USE_RESERVED_UBUF_2, __ASC_USE_RESERVED_UBUF_1)                                         \
    (__VA_ARGS__)

#endif
