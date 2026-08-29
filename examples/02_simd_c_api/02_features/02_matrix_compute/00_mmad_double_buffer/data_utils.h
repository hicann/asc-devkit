/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef MMAD_DOUBLE_BUFFER_DATA_UTILS_H
#define MMAD_DOUBLE_BUFFER_DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR] " fmt "\n", ##args)

inline bool ReadFile(const std::string& file_path, void* buffer, size_t expected_size)
{
    if (buffer == nullptr || expected_size == 0) {
        ERROR_LOG("failed to read input file: buffer is nullptr or expected size is 0");
        return false;
    }

    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1 || S_ISREG(file_stat.st_mode) == 0) {
        ERROR_LOG("invalid input file: %s", file_path.c_str());
        return false;
    }
    if (static_cast<size_t>(file_stat.st_size) != expected_size) {
        ERROR_LOG("unexpected file size: %s", file_path.c_str());
        return false;
    }
    if (expected_size > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
        ERROR_LOG("input file exceeds stream size limit: %s", file_path.c_str());
        return false;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("failed to open input file: %s", file_path.c_str());
        return false;
    }
    file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(expected_size));
    if (file.gcount() != static_cast<std::streamsize>(expected_size) || file.bad()) {
        ERROR_LOG("failed to read input file: %s", file_path.c_str());
        return false;
    }
    return true;
}

inline bool WriteFile(const std::string& file_path, const void* buffer, size_t size)
{
    if (buffer == nullptr || size == 0) {
        ERROR_LOG("failed to write output file: buffer is nullptr or size is 0");
        return false;
    }

    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        ERROR_LOG("failed to open output file: %s", file_path.c_str());
        return false;
    }
    size_t written_size = 0;
    const char* src = static_cast<const char*>(buffer);
    while (written_size < size) {
        ssize_t written = write(fd, src + written_size, size - written_size);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            ERROR_LOG("failed to write output file: %s", file_path.c_str());
            (void)close(fd);
            (void)unlink(file_path.c_str());
            return false;
        }
        if (written == 0) {
            ERROR_LOG("failed to write output file: no data was written");
            (void)close(fd);
            (void)unlink(file_path.c_str());
            return false;
        }
        written_size += static_cast<size_t>(written);
    }

    if (close(fd) != 0) {
        ERROR_LOG("failed to close output file: %s", file_path.c_str());
        (void)unlink(file_path.c_str());
        return false;
    }
    return true;
}

#endif
