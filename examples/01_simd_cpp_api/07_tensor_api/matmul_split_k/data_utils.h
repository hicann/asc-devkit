/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef MATMUL_SPLIT_K_DATA_UTILS_H
#define MATMUL_SPLIT_K_DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>

inline bool ReadFile(const std::string& path, void* buffer, size_t expectedSize)
{
    struct stat fileStat {};
    if (stat(path.c_str(), &fileStat) != 0 || !S_ISREG(fileStat.st_mode)) {
        std::fprintf(stderr, "[ERROR] input file does not exist: %s\n", path.c_str());
        return false;
    }
    if (static_cast<size_t>(fileStat.st_size) != expectedSize) {
        std::fprintf(
            stderr, "[ERROR] invalid file size for %s: expected %zu, got %zu\n", path.c_str(), expectedSize,
            static_cast<size_t>(fileStat.st_size));
        return false;
    }
    std::ifstream file(path, std::ios::binary);
    if (!file.read(static_cast<char*>(buffer), expectedSize)) {
        std::fprintf(stderr, "[ERROR] failed to read file: %s\n", path.c_str());
        return false;
    }
    return true;
}

inline bool WriteFile(const std::string& path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        std::fprintf(stderr, "[ERROR] output buffer is null\n");
        return false;
    }
    const int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        std::fprintf(stderr, "[ERROR] failed to open output file: %s\n", path.c_str());
        return false;
    }
    const ssize_t written = write(fd, buffer, size);
    close(fd);
    if (written < 0 || static_cast<size_t>(written) != size) {
        std::fprintf(stderr, "[ERROR] failed to write output file: %s\n", path.c_str());
        return false;
    }
    return true;
}

#endif // MATMUL_SPLIT_K_DATA_UTILS_H
