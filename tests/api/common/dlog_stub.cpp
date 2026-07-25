/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <stdio.h>
#include <atomic>
#include <cstring>
#include <cstdint>
#include <cstdarg>

extern int32_t logLevel;

namespace {
constexpr int32_t DLOG_ERROR_LEVEL = 3;
std::atomic<uint64_t> g_dlogErrorCount{0};
} // namespace

#ifdef __cplusplus
extern "C" {
#endif

int32_t CheckLogLevel(uint32_t moduleId, int32_t level)
{
    if (logLevel <= level) {
        return 1;
    }
    return 0;
}

void DlogRecord(uint32_t moduleId, int32_t level, const char* fmt, ...)
{
    if (level == DLOG_ERROR_LEVEL) {
        g_dlogErrorCount++;
    }
    if (fmt != nullptr) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

uint64_t GetDlogErrorCountForTest() { return g_dlogErrorCount.load(); }

#ifdef __cplusplus
}
#endif
