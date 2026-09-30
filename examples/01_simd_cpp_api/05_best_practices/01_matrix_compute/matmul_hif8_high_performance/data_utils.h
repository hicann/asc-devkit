/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR] " fmt "\n", ##args)

inline bool ReadFile(const std::string& filePath, size_t& fileSize, void* buffer, size_t bufferSize)
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0 || !S_ISREG(fileStat.st_mode)) {
        ERROR_LOG("failed to open input file: %s", filePath.c_str());
        return false;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("failed to open input file: %s", filePath.c_str());
        return false;
    }
    file.seekg(0, std::ios::end);
    const std::streamoff size = file.tellg();
    if (size <= 0 || static_cast<size_t>(size) > bufferSize) {
        ERROR_LOG("invalid input file size: %s", filePath.c_str());
        return false;
    }
    file.seekg(0, std::ios::beg);
    file.read(static_cast<char*>(buffer), size);
    if (!file) {
        ERROR_LOG("failed to read input file: %s", filePath.c_str());
        return false;
    }
    fileSize = static_cast<size_t>(size);
    return true;
}

inline bool WriteFile(const std::string& filePath, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("output buffer is null");
        return false;
    }
    const int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        ERROR_LOG("failed to open output file: %s", filePath.c_str());
        return false;
    }
    const ssize_t written = write(fd, buffer, size);
    close(fd);
    if (written != static_cast<ssize_t>(size)) {
        ERROR_LOG("failed to write output file: %s", filePath.c_str());
        return false;
    }
    return true;
}

#endif
