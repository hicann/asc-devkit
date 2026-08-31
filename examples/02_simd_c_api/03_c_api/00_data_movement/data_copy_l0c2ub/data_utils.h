/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file data_utils.h
 * \brief File read/write helpers for the host program.
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <limits>

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR]  " fmt "\n", ##args)

bool ReadFile(const std::string& file_path, void* buffer, size_t expected_size)
{
    if (buffer == nullptr || expected_size == 0) {
        ERROR_LOG("Read file failed. buffer is nullptr or expected size is 0");
        return false;
    }

    struct stat stat_buffer;
    if (stat(file_path.data(), &stat_buffer) == -1) {
        ERROR_LOG("failed to get file");
        return false;
    }
    if (S_ISREG(stat_buffer.st_mode) == 0) {
        ERROR_LOG("%s is not a file, please enter a file", file_path.c_str());
        return false;
    }
    if (stat_buffer.st_size <= 0 ||
        static_cast<uintmax_t>(stat_buffer.st_size) != static_cast<uintmax_t>(expected_size)) {
        ERROR_LOG(
            "file size does not match expected size. expected = %zu, actual = %lld", expected_size,
            static_cast<long long>(stat_buffer.st_size));
        return false;
    }
    if (expected_size > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
        ERROR_LOG("file size exceeds stream size limit. size = %zu", expected_size);
        return false;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("Open file failed. path = %s", file_path.c_str());
        return false;
    }
    file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(expected_size));
    if (file.gcount() != static_cast<std::streamsize>(expected_size)) {
        ERROR_LOG(
            "Read file failed. expected = %zu, actual = %lld", expected_size, static_cast<long long>(file.gcount()));
        file.close();
        return false;
    }
    file.close();
    if (file.fail()) {
        ERROR_LOG("Close file failed. path = %s", file_path.c_str());
        return false;
    }
    return true;
}

bool WriteFile(const std::string& file_path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("Write file failed. buffer is nullptr");
        return false;
    }

    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWRITE);
    if (fd < 0) {
        ERROR_LOG("Open file failed. path = %s", file_path.c_str());
        return false;
    }

    size_t written_size = 0;
    const char* src = static_cast<const char*>(buffer);
    while (written_size < size) {
        ssize_t write_size = write(fd, src + written_size, size - written_size);
        if (write_size < 0) {
            if (errno == EINTR) {
                continue;
            }
            ERROR_LOG("Write file failed. path = %s", file_path.c_str());
            (void)close(fd);
            return false;
        }
        if (write_size == 0) {
            ERROR_LOG("Write file failed. no data was written");
            (void)close(fd);
            return false;
        }
        written_size += static_cast<size_t>(write_size);
    }

    if (close(fd) != 0) {
        ERROR_LOG("Close file failed. path = %s", file_path.c_str());
        return false;
    }
    return true;
}

#endif // DATA_UTILS_H
