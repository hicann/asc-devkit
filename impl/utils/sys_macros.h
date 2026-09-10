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

#ifndef __PLUGIN__KERNEL_META_TYPE_ENUME_DEFINED__
#define __PLUGIN__KERNEL_META_TYPE_ENUME_DEFINED__
enum KernelMetaType : uint8_t {
    KERNEL_TYPE_AIV_ONLY,
    KERNEL_TYPE_AIC_ONLY,
    KERNEL_TYPE_MIX_AIV_1_0,
    KERNEL_TYPE_MIX_AIC_1_0,
    KERNEL_TYPE_MIX_AIC_1_1,
    KERNEL_TYPE_MIX_AIC_1_2,
    KERNEL_TYPE_AICORE,
    KERNEL_TYPE_VECTORCORE,
    KERNEL_TYPE_MIX_AICORE,
    KERNEL_TYPE_MIX_VECTOR_CORE,
    KERNEL_TYPE_MAX,
};
#endif

#if defined(ASCENDC_CPU_DEBUG)
#define ASCENDC_PRELOAD_IMPL(len) \
    {}
#else
#define ASCENDC_PRELOAD_IMPL(len)                     \
    do {                                              \
        uint64_t pc;                                  \
        asm volatile("mov %0, pc \n" : "=l"(pc) : :); \
        preload((void*)pc, len);                      \
    } while (0)
#endif

// In order to pass __COUNTER__ to variable name, need 3 times of MACRO to pass argument
#define ASCENDC_TILING_STRUCT_SECTION_INIT_BASE_IMPL(counter, val)                                                    \
    static const uint64_t __ascendc_tiling_struct_##counter __attribute__((used, section(".ascendc_tiling." #val))) = \
        sizeof(val)
#define ASCENDC_TILING_STRUCT_SECTION_INIT_IMPL(counter, val) ASCENDC_TILING_STRUCT_SECTION_INIT_BASE_IMPL(counter, val)

#ifdef __CHECK_FEATURE_AT_PRECOMPILE
#define ASCENDC_ENABLE_FEATURE_FOR_COMPILE_IMPL(f, val) auto __enable_feature_for_compile_##f = val
#define ASCENDC_ENABLE_FEATURE_FOR_TILING_IMPL(expression, val) auto __enable_custom_tiling val = expression
#define ASCENDC_REGISTER_NONE_TILING_IMPL \
    auto __enable_no_register_custom_tiling ascendc_trigger_tiling_struct = default
#else
#define ASCENDC_ENABLE_FEATURE_FOR_COMPILE_IMPL(f, val)
#define ASCENDC_ENABLE_FEATURE_FOR_TILING_IMPL(expression, val) \
    ASCENDC_TILING_STRUCT_SECTION_INIT_IMPL(__COUNTER__, val)
#define ASCENDC_REGISTER_NONE_TILING_IMPL
#endif

#ifndef ASCENDC_TILING_KEY_VAR_IMPL
#ifndef TILING_KEY_VAR
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#if defined(ASCENDC_CPU_DEBUG)
extern uint64_t g_tilingKey;
#else
#if __NPU_ARCH__ == 2002
[[block_local]] uint64_t g_tilingKey;
#else
[[workgroup_local]] __gm__ uint64_t g_tilingKey;
#endif
#endif
#endif
#define ASCENDC_TILING_KEY_VAR_IMPL g_tilingKey
#else
#define ASCENDC_TILING_KEY_VAR_IMPL TILING_KEY_VAR
#endif
#endif

#define ASCENDC_KERNEL_TASK_TYPE_IMPL(key, value) ASCENDC_ENABLE_FEATURE_FOR_COMPILE_IMPL(key, value)
#define ASCENDC_KERNEL_TASK_TYPE_DEFAULT_IMPL(value) ASCENDC_ENABLE_FEATURE_FOR_COMPILE_IMPL(default, value)
#define ASCENDC_REGISTER_TILING_DEFAULT_IMPL(tiling_struct) \
    ASCENDC_ENABLE_FEATURE_FOR_TILING_IMPL(default, tiling_struct)
#define ASCENDC_REGISTER_TILING_FOR_TILINGKEY_IMPL(expression, tiling_struct) \
    ASCENDC_ENABLE_FEATURE_FOR_TILING_IMPL(expression, tiling_struct)
#define ASCENDC_TILING_KEY_IS_IMPL(k) (ASCENDC_TILING_KEY_VAR_IMPL == (k))

#define ASCENDC_TILING_KEY_LIST_INOUT_IMPL(...) ASCENDC_TILING_KEY_LIST_INOUT_BASE_IMPL(__VA_ARGS__)
#define ASCENDC_TILING_KEY_LIST_INOUT_BASE_IMPL(...) \
    ASCENDC_TILING_KEY_ARGS_CONCAT_IMPL(             \
        ASCENDC_TILING_KEY_INDEX_INOUT_, ASCENDC_TILING_KEY_ARG_COUNT_IMPL(__VA_ARGS__)(__VA_ARGS__))

#define ASCENDC_TILING_KEY_INDEX_INOUT_1(a) ASCENDC_TILING_KEY_VAR_IMPL == (a)
#define ASCENDC_TILING_KEY_INDEX_INOUT_2(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_1(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_3(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_2(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_4(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_3(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_5(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_4(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_6(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_5(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_7(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_6(__VA_ARGS__)
#define ASCENDC_TILING_KEY_INDEX_INOUT_8(a, ...) \
    ASCENDC_TILING_KEY_INDEX_INOUT_1(a) || ASCENDC_TILING_KEY_INDEX_INOUT_7(__VA_ARGS__)

#define ASCENDC_TILING_KEY_ARG_COUNT_IMPL(...) \
    ASCENDC_TILING_KEY_ARG_COUNT_BASE_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ASCENDC_TILING_KEY_ARG_COUNT_BASE_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

#define ASCENDC_TILING_KEY_ARGS_CONCAT_IMPL(a, b) ASCENDC_TILING_KEY_ARGS_CONCAT_BASE_IMPL(a, b)
#define ASCENDC_TILING_KEY_ARGS_CONCAT_BASE_IMPL(a, b) a##b

#ifdef __CHECK_FEATURE_AT_PRECOMPILE
#define ASCENDC_TILING_KEY_LIST_IMPL(...) (ASCENDC_TILING_KEY_LIST_INOUT_IMPL(__VA_ARGS__)) "TILING_KEY_LIST"
#else
#define ASCENDC_TILING_KEY_LIST_IMPL(...) (ASCENDC_TILING_KEY_LIST_INOUT_IMPL(__VA_ARGS__))
#endif

#endif
