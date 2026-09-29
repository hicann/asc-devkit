/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/**
 * npuops_exception_dump - 工具函数实现 (log/dump/文件/时间)
 */
#include "utils.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>

namespace npuops {

void LogInfo(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("[NPUOPS][INFO] ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
    fflush(stdout);
}

void LogWarn(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("[NPUOPS][WARN] ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
    fflush(stdout);
}

void LogError(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("[NPUOPS][ERROR] ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
    fflush(stdout);
}

std::string GetTimestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);
    struct tm tmBuf;
    localtime_r(&t, &tmBuf);
    char buf[64] = {0};
    snprintf(
        buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d_%03d", tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday,
        tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec, static_cast<int>(ms.count()));
    return std::string(buf);
}

std::string GetDumpDir()
{
    const char* dir = getenv("NPUOPS_DUMP_DIR");
    if (dir != nullptr && dir[0] != '\0') {
        return std::string(dir);
    }
    return std::string("./exception_dump/");
}

int GetDumpLevel()
{
    const char* lv = getenv("NPUOPS_DUMP_LEVEL");
    if (lv != nullptr && lv[0] != '\0') {
        return atoi(lv);
    }
    return 1; // 默认 level=1：公共信息 + args
}

int EnsureDir(const std::string& dir)
{
    if (dir.empty()) {
        return -1;
    }
    std::string cur;
    for (size_t i = 0; i < dir.size(); i++) {
        cur.push_back(dir[i]);
        if (dir[i] == '/' && i > 0) {
            mkdir(cur.c_str(), 0755);
        }
    }
    mkdir(dir.c_str(), 0755);
    struct stat st;
    return stat(dir.c_str(), &st) == 0 ? 0 : -1;
}

bool WriteBinFile(const std::string& path, const void* data, size_t size)
{
    FILE* fp = fopen(path.c_str(), "wb");
    if (fp == nullptr) {
        LogError("open file for write failed: %s", path.c_str());
        return false;
    }
    bool ok = true;
    if (size > 0 && data != nullptr) {
        ok = (fwrite(data, 1, size, fp) == size);
    }
    fclose(fp);
    return ok;
}

bool AppendTextFile(const std::string& path, const std::string& text)
{
    FILE* fp = fopen(path.c_str(), "a");
    if (fp == nullptr) {
        LogError("open file for append failed: %s", path.c_str());
        return false;
    }
    bool ok = (fwrite(text.data(), 1, text.size(), fp) == text.size());
    fclose(fp);
    return ok;
}

void LogHexDump(const char* tag, const void* data, size_t size, size_t maxBytes)
{
    const uint8_t* p = static_cast<const uint8_t*>(data);
    size_t n = size < maxBytes ? size : maxBytes;
    LogInfo("%s hexdump (total %zu bytes, show %zu):", tag, size, n);
    char line[128];
    for (size_t i = 0; i < n; i += 16) {
        int off = 0;
        off += snprintf(line + off, sizeof(line) - off, "  %08zx:", i);
        for (size_t j = 0; j < 16 && (i + j) < n; j++) {
            off += snprintf(line + off, sizeof(line) - off, " %02x", p[i + j]);
        }
        LogInfo("%s", line);
    }
}

} // namespace npuops
