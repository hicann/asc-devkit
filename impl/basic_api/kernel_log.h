/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_log.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_log.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_common.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_LOG_H__
#endif

#ifndef ASCENDC_MODULE_KERNEL_LOG_INTF_H
#define ASCENDC_MODULE_KERNEL_LOG_INTF_H
#if defined(__NPU_ARCH__) &&                                                                                  \
    ((__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510)) && \
    !(defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1))
#include "impl/utils/debug/asc_aicore_printf_impl.h"
#endif
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <string>
#include <map>
#include <csignal>
#include <cstdio>
#include <unistd.h>
#include "stub_def.h"

namespace AscendC {
#if !defined(__NPU_DEVICE__) && !defined(__ASCC_DEVICE__)

#if defined(UT_TEST) || defined(ST_TEST)
#define ASCENDC_ASSERT(cond, behavior)                          \
    do {                                                        \
        if (!(cond)) {                                          \
            behavior;                                           \
            AscendC::KernelRaise::GetInstance().Raise(SIGABRT); \
        }                                                       \
    } while (0)
#else
#define ASCENDC_ASSERT(cond, behavior) \
    do {                               \
        if (!(cond)) {                 \
            behavior;                  \
            raise(SIGABRT);            \
        }                              \
    } while (0)
#endif

// intend to give warning when cond is false instead of abort
#define ASCENDC_ASSERT_WARNING(cond, behavior) \
    do {                                       \
        if (!(cond)) {                         \
            behavior;                          \
        }                                      \
    } while (0)

#else // defined(__NPU_DEVICE__) || defined(__ASCC_DEVICE__)

#ifndef ASCC_ASCENDC_ASSERT
#define ASCC_ASCENDC_ASSERT
#define ASCENDC_ASSERT(cond, behavior)
#endif

#endif // !defined(__NPU_DEVICE__) && !defined(__ASCC_DEVICE__)

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
#define ASCENDC_REPORT_OVERFLOW_MEM(cond) \
    do {                                  \
        if (!(cond)) {                    \
            raise(SIGABRT);               \
        }                                 \
    } while (0)
#else
#define ASCENDC_REPORT_OVERFLOW_MEM(cond)
#endif

// Check xxx api is not supported in cpu
#define ASCENDC_REPORT_NOT_SUPPORT(cond, apiMsg)                                        \
    do {                                                                                \
        if (!(cond)) {                                                                  \
            KERNEL_LOG(KERNEL_ERROR, "%s is not supported on current device.", apiMsg); \
            raise(SIGABRT);                                                             \
        }                                                                               \
    } while (0)

// Check value in range [low, high]
#define ASCENDC_CHECK_VALUE_RANGE(value, rangeLow, rangeHigh, paramName, apiMsg)                        \
    do {                                                                                                \
        if (value < rangeLow || value > rangeHigh) {                                                    \
            KERNEL_LOG(                                                                                 \
                KERNEL_ERROR,                                                                           \
                "Failed to check %s value in %s, its valid range is "                                   \
                "%s ~ %s, current value is %s.",                                                        \
                paramName, apiMsg, std::to_string(rangeLow).c_str(), std::to_string(rangeHigh).c_str(), \
                std::to_string(value).c_str());                                                         \
            raise(SIGABRT);                                                                             \
        }                                                                                               \
    } while (0)

enum class TPosition : uint8_t;

template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr);

#define ASCENDC_CHECK_TENSOR_PTR_ALIGN(tensorPtr, tPos, alignBytes, tensorName, apiMsg) \
    do {                                                                                \
        uint64_t tensorAddr = TransUBAddr<tPos>(reinterpret_cast<uint64_t>(tensorPtr)); \
        if (tensorAddr % alignBytes != 0) {                                             \
            KERNEL_LOG(                                                                 \
                KERNEL_ERROR,                                                           \
                "Failed to check %s start address alignment in %s, "                    \
                "its start address must align with %dB.",                               \
                tensorName, apiMsg, alignBytes);                                        \
            raise(SIGABRT);                                                             \
        }                                                                               \
    } while (0)

// Check tensor tposition with condition judgement
#define ASCENDC_CHECK_TPOSITION(cond, tensorName, tPosName, apiMsg, curPos)      \
    do {                                                                         \
        if (!(cond)) {                                                           \
            KERNEL_LOG(                                                          \
                KERNEL_ERROR,                                                    \
                "Failed to check %s tensor position in %s, supported positions " \
                "are %s, current position is %s.",                               \
                tensorName, apiMsg, tPosName, curPos.c_str());                   \
            raise(SIGABRT);                                                      \
        }                                                                        \
    } while (0)

// Report error when failed cpu check
#define ASCENDC_REPORT_CHECK_ERROR(apiMsg, funcType)                                      \
    do {                                                                                  \
        if (funcType == KernelFuncType::MASK_COUNT_MODE) {                                \
            KERNEL_LOG(KERNEL_ERROR, "Failed to pass %s mask count mode check.", apiMsg); \
        } else if (funcType == KernelFuncType::MASK_BIT_MODE) {                           \
            KERNEL_LOG(KERNEL_ERROR, "Failed to pass %s mask bit mode check.", apiMsg);   \
        } else if (funcType == KernelFuncType::CALCOUNT_MODE) {                           \
            KERNEL_LOG(KERNEL_ERROR, "Failed to pass %s calcount mode check.", apiMsg);   \
        } else {                                                                          \
            KERNEL_LOG(KERNEL_ERROR, "Failed to pass %s check.", apiMsg);                 \
        }                                                                                 \
        raise(SIGABRT);                                                                   \
    } while (0)

enum KernelFuncType : uint8_t {
    NONE_MODE,
    MASK_COUNT_MODE, // mask
    MASK_BIT_MODE,   // mask[]
    CALCOUNT_MODE    // calcount
};

enum class LogLevel : uint8_t {
    KERNEL_DEBUG = 0,
    KERNEL_INFO = 1,
    KERNEL_WARN = 2,
    KERNEL_ERROR = 3,
};
} // namespace AscendC

#define KERNEL_LOG(level, format, ...) KERNEL_LOG_##level(format, ##__VA_ARGS__)

#if __NPU_ARCH__ == 2201

namespace AscendC {
inline std::string GenCoreTypeStr()
{
    std::string coreTypeStr = "";
    if (g_coreType == AscendC::AIC_TYPE) {
        coreTypeStr = "AIC_";
    } else if (g_coreType == AscendC::AIV_TYPE) {
        coreTypeStr = "AIV_";
    } else {
        coreTypeStr = "MIX_";
    }
    coreTypeStr += std::to_string(sub_block_idx);
    return coreTypeStr;
}

inline std::string GenBlockStr()
{
    std::string blockStr = "Block_";
    blockStr += std::to_string(block_idx);
    return blockStr;
}
} // namespace AscendC

#define KERNEL_LOG_KERNEL_DEBUG(format, ...)                                                                          \
    do {                                                                                                              \
        std::string coreTypeStr = AscendC::GenCoreTypeStr();                                                          \
        std::string blockStr = AscendC::GenBlockStr();                                                                \
        printf(                                                                                                       \
            "[DEBUG][%s][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), coreTypeStr.c_str(), __FILE__, __LINE__, \
            __FUNCTION__, static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                            \
    } while (0)

#define KERNEL_LOG_KERNEL_INFO(format, ...)                                                                          \
    do {                                                                                                             \
        std::string coreTypeStr = AscendC::GenCoreTypeStr();                                                         \
        std::string blockStr = AscendC::GenBlockStr();                                                               \
        printf(                                                                                                      \
            "[INFO][%s][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), coreTypeStr.c_str(), __FILE__, __LINE__, \
            __FUNCTION__, static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                           \
    } while (0)

#define KERNEL_LOG_KERNEL_WARN(format, ...)                                                                          \
    do {                                                                                                             \
        std::string coreTypeStr = AscendC::GenCoreTypeStr();                                                         \
        std::string blockStr = AscendC::GenBlockStr();                                                               \
        printf(                                                                                                      \
            "[WARN][%s][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), coreTypeStr.c_str(), __FILE__, __LINE__, \
            __FUNCTION__, static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                           \
    } while (0)

#define KERNEL_LOG_KERNEL_ERROR(format, ...)                                                                          \
    do {                                                                                                              \
        std::string coreTypeStr = AscendC::GenCoreTypeStr();                                                          \
        std::string blockStr = AscendC::GenBlockStr();                                                                \
        printf(                                                                                                       \
            "[ERROR][%s][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), coreTypeStr.c_str(), __FILE__, __LINE__, \
            __FUNCTION__, static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                            \
    } while (0)

#else

#define KERNEL_LOG_KERNEL_DEBUG(format, ...)                                                               \
    do {                                                                                                   \
        std::string blockStr = "Core_";                                                                    \
        blockStr += std::to_string(block_idx);                                                             \
        printf(                                                                                            \
            "[DEBUG][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), __FILE__, __LINE__, __FUNCTION__, \
            static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                               \
    } while (0)

#define KERNEL_LOG_KERNEL_INFO(format, ...)                                                               \
    do {                                                                                                  \
        std::string blockStr = "Core_";                                                                   \
        blockStr += std::to_string(block_idx);                                                            \
        printf(                                                                                           \
            "[INFO][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), __FILE__, __LINE__, __FUNCTION__, \
            static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                              \
    } while (0)

#define KERNEL_LOG_KERNEL_WARN(format, ...)                                                               \
    do {                                                                                                  \
        std::string blockStr = "Core_";                                                                   \
        blockStr += std::to_string(block_idx);                                                            \
        printf(                                                                                           \
            "[WARN][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), __FILE__, __LINE__, __FUNCTION__, \
            static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                              \
    } while (0)

#define KERNEL_LOG_KERNEL_ERROR(format, ...)                                                               \
    do {                                                                                                   \
        std::string blockStr = "Core_";                                                                    \
        blockStr += std::to_string(block_idx);                                                             \
        printf(                                                                                            \
            "[ERROR][%s][%s:%d][%s][%u] " format "\n", blockStr.c_str(), __FILE__, __LINE__, __FUNCTION__, \
            static_cast<uint32_t>(getpid()), ##__VA_ARGS__);                                               \
    } while (0)

#endif

#else

#define KERNEL_LOG(level, format, ...)
#if !defined(__NPU_HOST__) && !defined(__ASCC_HOST__)

#define ASCENDC_ASSERT(cond, behavior)

#else // define (__NPU_HOST__) || defined(__ASCC_HOST__)

#ifndef ASCC_ASCENDC_ASSERT
#define ASCC_ASCENDC_ASSERT
#define ASCENDC_ASSERT(cond, behavior) \
    do {                               \
        if (!(cond)) {                 \
            behavior;                  \
            raise(SIGABRT);            \
        }                              \
    } while (0)
#endif

#endif // !defined(__NPU_HOST__) && !defined(__ASCC_HOST__)
#define ASCENDC_REPORT_NOT_SUPPORT(cond, apiMsg)
#define ASCENDC_CHECK_VALUE_RANGE(value, rangeLow, rangeHigh, paramName, apiMsg)
#define ASCENDC_CHECK_TENSOR_PTR_ALIGN(tensorPtr, tPos, alignBytes, tensorName, apiMsg)
#define ASCENDC_CHECK_TPOSITION(cond, tensorName, tPosName, apiMsg, curPos)
#define ASCENDC_REPORT_CHECK_ERROR(apiMsg, funcType)
#define ASCENDC_REPORT_OVERFLOW_MEM(cond)

#endif

namespace AscendC {
template <class... Args>
__aicore__ inline void AssertImpl(__gm__ const char* fmt, Args&&... args);
}

namespace AscendC {
template <class... Args>
__aicore__ static __attribute__((noinline)) void AssertPrint(__gm__ const char* fmt, Args&&... args)
{
    AscendC::AssertImpl(fmt, args...);
}

// assert define
#ifdef ASCENDC_CPU_DEBUG
#define ASSERT_MSG(expr, fmt, ...)                                                                            \
    do {                                                                                                      \
        if (!(expr)) {                                                                                        \
            fprintf(stderr, "[ASSERT] %s:%u: Assertion `%s' " fmt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
            abort();                                                                                          \
        }                                                                                                     \
    } while (0)
#define ASC_ASSERT_MSG__(prompt, expr, fmt, ...)                                                                    \
    do {                                                                                                            \
        if (!(expr)) {                                                                                              \
            fprintf(                                                                                                \
                stderr, "%s[ASSERT] %s:%u: Assertion `%s' " fmt, prompt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
            abort();                                                                                                \
        }                                                                                                           \
    } while (0)
#define ASC_WARNING_MSG__(prompt, expr, fmt, ...)                                                                    \
    do {                                                                                                             \
        if (!(expr)) {                                                                                               \
            fprintf(                                                                                                 \
                stderr, "%s[WARNING] %s:%u: Assertion `%s' " fmt, prompt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
        }                                                                                                            \
    } while (0)
#else
#if defined(ASC_DEVKIT_VERSION_STR) && defined(ASC_DEVKIT_TIMESTAMP)
#define ASSERT_MSG(expr, fmt, ...)                                                                                   \
    do {                                                                                                             \
        if (!(expr)) {                                                                                               \
            AscendC::AssertPrint(                                                                                    \
                "[ASSERT] [CANN_VERSION : %s][TimeStamp : %u] %s:%u: Assertion `%s' " fmt,                           \
                (__gm__ const char*)(ASC_DEVKIT_VERSION_STR), static_cast<uint64_t>(ASC_DEVKIT_TIMESTAMP), __FILE__, \
                __LINE__, #expr, ##__VA_ARGS__);                                                                     \
            trap();                                                                                                  \
        }                                                                                                            \
    } while (0)
#define ASC_ASSERT_MSG__(prompt, expr, fmt, ...)                                                                     \
    do {                                                                                                             \
        if (!(expr)) {                                                                                               \
            AscendC::AssertPrint(                                                                                    \
                "%s[ASSERT] [CANN_VERSION : %s][TimeStamp : %u] %s:%u: Assertion `%s' " fmt, prompt,                 \
                (__gm__ const char*)(ASC_DEVKIT_VERSION_STR), static_cast<uint64_t>(ASC_DEVKIT_TIMESTAMP), __FILE__, \
                __LINE__, #expr, ##__VA_ARGS__);                                                                     \
            trap();                                                                                                  \
        }                                                                                                            \
    } while (0)
#define ASC_WARNING_MSG__(prompt, expr, fmt, ...)                                                                    \
    do {                                                                                                             \
        if (!(expr)) {                                                                                               \
            __asc_aicore::printf_impl(                                                                               \
                "%s[WARNING] [CANN_VERSION : %s][TimeStamp : %u] %s:%u: Assertion `%s' " fmt, prompt,                \
                (__gm__ const char*)(ASC_DEVKIT_VERSION_STR), static_cast<uint64_t>(ASC_DEVKIT_TIMESTAMP), __FILE__, \
                __LINE__, #expr, ##__VA_ARGS__);                                                                     \
        }                                                                                                            \
    } while (0)
#else
#define ASSERT_MSG(expr, fmt, ...)                                                                                 \
    do {                                                                                                           \
        if (!(expr)) {                                                                                             \
            AscendC::AssertPrint("[ASSERT] %s:%u: Assertion `%s' " fmt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
            trap();                                                                                                \
        }                                                                                                          \
    } while (0)
#define ASC_ASSERT_MSG__(prompt, expr, fmt, ...)                                                            \
    do {                                                                                                    \
        if (!(expr)) {                                                                                      \
            AscendC::AssertPrint(                                                                           \
                "%s[ASSERT] %s:%u: Assertion `%s' " fmt, prompt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
            trap();                                                                                         \
        }                                                                                                   \
    } while (0)
#define ASC_WARNING_MSG__(prompt, expr, fmt, ...)                                                            \
    do {                                                                                                     \
        if (!(expr)) {                                                                                       \
            __asc_aicore::printf_impl(                                                                       \
                "%s[WARNING] %s:%u: Assertion `%s' " fmt, prompt, __FILE__, __LINE__, #expr, ##__VA_ARGS__); \
        }                                                                                                    \
    } while (0)
#endif
#endif

#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
#define VA_ARGS_IS_EMPTY(...) (sizeof(#__VA_ARGS__) == 1)
#define ASCENDC_DEBUG_ASSERT_IMPL(expr, ...)                 \
    do {                                                     \
        __gm__ const char* prompt = "";                      \
        if (!(expr)) {                                       \
            if (VA_ARGS_IS_EMPTY(__VA_ARGS__)) {             \
                ASC_ASSERT_MSG__(prompt, expr, "\n");        \
            } else {                                         \
                ASC_ASSERT_MSG__(prompt, expr, __VA_ARGS__); \
            }                                                \
        }                                                    \
    } while (0)

#define ASCENDC_DEBUG_WARNING_IMPL(expr, ...)                 \
    do {                                                      \
        __gm__ const char* prompt = "";                       \
        if (!(expr)) {                                        \
            if (VA_ARGS_IS_EMPTY(__VA_ARGS__)) {              \
                ASC_WARNING_MSG__(prompt, expr, "\n");        \
            } else {                                          \
                ASC_WARNING_MSG__(prompt, expr, __VA_ARGS__); \
            }                                                 \
        }                                                     \
    } while (0)

#else
#define ASCENDC_DEBUG_ASSERT_IMPL(...)
#define ASCENDC_DEBUG_WARNING_IMPL(...)
#endif
} // namespace AscendC

namespace AscendC {
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
#define KERNEL_LOG_INTERNAL(level, format, ...) KERNEL_LOG_##level(format, ##__VA_ARGS__)
#define ASCENDC_DEBUG_ASSERT(...) ASCENDC_ASSERT(__VA_ARGS__)
#define ASCENDC_DEBUG_WARNING(...) ASCENDC_ASSERT_WARNING(__VA_ARGS__)
#else
#ifdef ASCENDC_DEBUG
#define KERNEL_LOG_INTERNAL(level, format, ...) format, ##__VA_ARGS__
#define ASCENDC_DEBUG_ASSERT(...) ASCENDC_DEBUG_ASSERT_IMPL(__VA_ARGS__)
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510))
#define ASCENDC_DEBUG_WARNING(...) ASCENDC_DEBUG_WARNING_IMPL(__VA_ARGS__)
#else
#define ASCENDC_DEBUG_WARNING(...)
#endif
#else
#define KERNEL_LOG_INTERNAL(level, format, ...)
#define ASCENDC_DEBUG_ASSERT(...)
#define ASCENDC_DEBUG_WARNING(...)
#endif
#endif
} // namespace AscendC

#endif // ASCENDC_MODULE_KERNEL_LOG_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_LOG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_LOG_H__
#endif
