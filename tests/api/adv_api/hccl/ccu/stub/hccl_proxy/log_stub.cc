/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <map>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <securec.h>

uint32_t logLevel = 0x03; // ERROR级别
constexpr int TIME_FROM_1900 = 1900;
constexpr int LOG_STUB_BUFFER_SIZE = 1024;
std::map<int, std::string> LOG_LEVEL_STR_MAP = {
    {0x00, "[DEBUG]"}, {0x01, "[INFO]"}, {0x02, "[WARNING]"}, {0x03, "[ERROR]"}, {0x10, "[EVENT]"}};

#ifdef __cplusplus
extern "C" {
#endif

int32_t CheckLogLevel(int32_t moduleId, int32_t logLevel) { return 1; }

void GetCurTimeStr(char* timeStr, int len)
{
    struct timeval tv;
    time_t tmpt;
    struct tm* now;

    if (timeStr == nullptr) {
        return;
    }

    if (0 > gettimeofday(&tv, nullptr)) {
        return;
    }

    tmpt = (time_t)tv.tv_sec;
    now = localtime(&tmpt);
    if (now == nullptr) {
        return;
    }

    int iLen = snprintf_s(
        timeStr, len, len, "%04d-%02d-%02d %02d:%02d:%02d.%06u", now->tm_year + TIME_FROM_1900, now->tm_mon + 1,
        now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, (uint32_t)tv.tv_usec);
    if (iLen == -1) {
        printf("Print time failed\n");
    }
}

void DlogPrintStub(int level, char* logBuffer)
{
    std::string logLevelStr = "[INFO]";
    if (LOG_LEVEL_STR_MAP.find(level) != LOG_LEVEL_STR_MAP.end()) {
        logLevelStr = LOG_LEVEL_STR_MAP[level];
    }

    char timeBuffer[LOG_STUB_BUFFER_SIZE] = {0};
    GetCurTimeStr(timeBuffer, LOG_STUB_BUFFER_SIZE);
    printf("[%-26s][pid:%u]%s%s\n", timeBuffer, getpid(), logLevelStr.c_str(), logBuffer);
}

void DlogInner(int moduleId, int level, const char* fmt, ...)
{
    if (level < logLevel) {
        return;
    }

    // 定义一个缓冲区，用于存储格式化后的字符串
    char buffer[LOG_STUB_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf_s(buffer, sizeof(buffer), (sizeof(buffer) - 1), fmt, args);
    va_end(args);

    DlogPrintStub(level, buffer);
}

void DlogRecord(int moduleId, int level, const char* fmt, ...)
{
    if (level < logLevel) {
        return;
    }

    // 定义一个缓冲区，用于存储格式化后的字符串
    char buffer[LOG_STUB_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf_s(buffer, sizeof(buffer), (sizeof(buffer) - 1), fmt, args);
    va_end(args);

    DlogPrintStub(level, buffer);
}

#ifdef __cplusplus
}
#endif
