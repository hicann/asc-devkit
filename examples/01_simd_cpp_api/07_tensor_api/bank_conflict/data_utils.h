/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef BANK_CONFLICT_DATA_UTILS_H
#define BANK_CONFLICT_DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

// Read exactly expectedSize bytes.  Tensor samples use binary files whose
// shape is fixed by the compile-time sample configuration, so accepting a
// truncated or stale file would otherwise make a run appear successful.
inline bool ReadFile(const std::string& path, void* buffer, size_t expectedSize)
{
    if (buffer == nullptr) {
        std::fprintf(stderr, "[ERROR] null input buffer for %s\n", path.c_str());
        return false;
    }

    struct stat fileStat {};
    if (stat(path.c_str(), &fileStat) != 0 || !S_ISREG(fileStat.st_mode)) {
        std::fprintf(stderr, "[ERROR] input file does not exist: %s\n", path.c_str());
        return false;
    }
    const size_t fileSize = static_cast<size_t>(fileStat.st_size);
    if (fileSize != expectedSize) {
        std::fprintf(
            stderr, "[ERROR] invalid file size for %s: expected %zu, got %zu\n", path.c_str(), expectedSize, fileSize);
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(expectedSize))) {
        std::fprintf(stderr, "[ERROR] failed to read file: %s\n", path.c_str());
        return false;
    }
    return true;
}

inline bool WriteFile(const std::string& path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        std::fprintf(stderr, "[ERROR] null output buffer for %s\n", path.c_str());
        return false;
    }

    const int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        std::fprintf(stderr, "[ERROR] failed to open output file: %s\n", path.c_str());
        return false;
    }

    const char* data = static_cast<const char*>(buffer);
    size_t written = 0;
    while (written < size) {
        const ssize_t count = write(fd, data + written, size - written);
        if (count <= 0) {
            std::fprintf(stderr, "[ERROR] failed to write output file: %s\n", path.c_str());
            (void)close(fd);
            return false;
        }
        written += static_cast<size_t>(count);
    }
    (void)close(fd);
    return true;
}

#endif // BANK_CONFLICT_DATA_UTILS_H
