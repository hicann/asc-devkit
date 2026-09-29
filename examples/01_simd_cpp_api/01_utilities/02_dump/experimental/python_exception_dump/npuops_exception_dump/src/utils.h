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
 * npuops_exception_dump 内部头文件 - 工具函数 (log/dump/文件/时间)
 */
#ifndef NPUOPS_UTILS_H_
#define NPUOPS_UTILS_H_

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace npuops {

// 日志输出（stdout，带统一前缀，便于筛选）
void LogInfo(const char* fmt, ...);
void LogWarn(const char* fmt, ...);
void LogError(const char* fmt, ...);

// 当前时间戳字符串: 20260626_010900_123
std::string GetTimestamp();

// 读环境变量配置
std::string GetDumpDir(); // NPUOPS_DUMP_DIR, 默认 ./exception_dump/
int GetDumpLevel();       // NPUOPS_DUMP_LEVEL, 默认 1 (0=仅公共信息, 1=公共信息+args)

// 递归建目录 (mkdir -p)
int EnsureDir(const std::string& dir);

// 二进制写文件（dump 产物落盘）
bool WriteBinFile(const std::string& path, const void* data, size_t size);

// 文本追加写（info 文件）
bool AppendTextFile(const std::string& path, const std::string& text);

// hex dump 日志（前 maxBytes 字节）
void LogHexDump(const char* tag, const void* data, size_t size, size_t maxBytes);

} // namespace npuops

#endif /* NPUOPS_UTILS_H_ */
