/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_EXCEPTION_UTIL_H
#define HCCLV2_EXCEPTION_UTIL_H

#include "../exception/null_ptr_exception.h"
#include "log.h"
#include "string_util.h"
#include <string>
#include <exception>

#define MACRO_THROW(EXCEPTION, MSG)      \
    do {                                 \
        HCCL_ERROR("%s", (MSG).c_str()); \
        throw EXCEPTION(MSG);            \
    } while (0)

#define DECTOR_TRY_CATCH(classname, expr)                                                     \
    try {                                                                                     \
        expr;                                                                                 \
    } catch (HcclException & e) {                                                             \
        HCCL_ERROR("Exception occurred: %s", e.what());                                       \
    } catch (std::exception & e) {                                                            \
        HCCL_ERROR("Exception occurred: %s", e.what());                                       \
    } catch (...) {                                                                           \
        HCCL_ERROR("Unknown error occurs when destructor object of class \"%s\"", classname); \
    }

#define TRY_CATCH_THROW(EXCEPTION, MSG, EXPR)                                        \
    do {                                                                             \
        try {                                                                        \
            EXPR;                                                                    \
        } catch (HcclException & e) {                                                \
            THROW<EXCEPTION>(StringFormat("%s, %s", e.what(), MSG.c_str()));         \
        } catch (std::exception & e) {                                               \
            THROW<EXCEPTION>(StringFormat("%s, %s", e.what(), MSG.c_str()));         \
        } catch (...) {                                                              \
            THROW<EXCEPTION>(StringFormat("Unknown error occurs, %s", MSG.c_str())); \
        }                                                                            \
    } while (0)

#define TRY_CATCH_RETURN(expr)                                                    \
    do {                                                                          \
        try {                                                                     \
            expr;                                                                 \
        } catch (HcclException & e) {                                             \
            HCCL_ERROR("Exception occurred: %s", e.what());                       \
            auto backTraces = e.GetBackTraceStrings();                            \
            std::for_each(backTraces.begin(), backTraces.end(), [](string item) { \
                HCCL_INFO("backTraces item: %s", item.c_str());                   \
            });                                                                   \
            return e.GetErrorCode();                                              \
        } catch (exception & e) {                                                 \
            HCCL_ERROR("Exception occurred: %s", e.what());                       \
            return HcclResult::HCCL_E_INTERNAL;                                   \
        } catch (...) {                                                           \
            HCCL_ERROR("Unknown error occurs!");                                  \
            return HcclResult::HCCL_E_INTERNAL;                                   \
        }                                                                         \
    } while (0)

#define TRY_CATCH_PROCESS_THROW(EXCEPTION, EXPR, MSG, PROCESS)                             \
    do {                                                                                   \
        try {                                                                              \
            EXPR;                                                                          \
        } catch (HcclException & e) {                                                      \
            PROCESS;                                                                       \
            HCCL_ERROR("%s due to %s", MSG, e.what());                                     \
            throw e;                                                                       \
        } catch (std::exception & e) {                                                     \
            PROCESS;                                                                       \
            MACRO_THROW(EXCEPTION, StringFormat("%s due to %s", MSG, e.what()));           \
        } catch (...) {                                                                    \
            PROCESS;                                                                       \
            MACRO_THROW(EXCEPTION, StringFormat("%s due to: Unknown error occurs!", MSG)); \
        }                                                                                  \
    } while (0)

#define TRY_CATCH_PRINT_ERROR(expr)                                               \
    do {                                                                          \
        try {                                                                     \
            expr;                                                                 \
        } catch (HcclException & e) {                                             \
            HCCL_ERROR("Exception occurred: %s", e.what());                       \
            auto backTraces = e.GetBackTraceStrings();                            \
            std::for_each(backTraces.begin(), backTraces.end(), [](string item) { \
                HCCL_INFO("backTraces item: %s", item.c_str());                   \
            });                                                                   \
        } catch (exception & e) {                                                 \
            HCCL_ERROR("Exception occurred: %s", e.what());                       \
        } catch (...) {                                                           \
            HCCL_ERROR("Unknown error occurs!");                                  \
        }                                                                         \
    } while (0)

#define CHK_RET_THROW(EXCEPTION, MSG, expr)              \
    do {                                                 \
        auto ret = (expr);                               \
        if (UNLIKELY(ret != HcclResult::HCCL_SUCCESS)) { \
            THROW<EXCEPTION>(MSG);                       \
        }                                                \
    } while (0)

#define CHK_PRT_THROW(expr, exeLog, EXCEPTION, MSG) \
    do {                                            \
        if (UNLIKELY(expr)) {                       \
            exeLog;                                 \
            throw EXCEPTION(MSG);                   \
        }                                           \
    } while (0)

namespace Hccl {

template <typename EXCEPTION>
inline void THROW(const std::string& msg)
{
    HCCL_ERROR("%s", msg.c_str());
    throw EXCEPTION(msg);
}

template <typename EXCEPTION, typename... Args>
inline void THROW(const char* format, Args... args)
{
    auto msg = StringFormat(format, args...);
    HCCL_ERROR("%s", msg.c_str());
    throw EXCEPTION(msg);
}

template <typename POINTER>
inline void CHECK_NULLPTR(const POINTER& p, const std::string& msg)
{
    if (UNLIKELY(p == nullptr)) {
        THROW<NullPtrException>(msg);
    }
}

} // namespace Hccl

#endif // HCCLV2_EXCEPTION_UTIL_H
