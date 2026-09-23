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
 * \brief Utility functions for reading and writing binary files
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <string>

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR]  " fmt "\n", ##args)

bool read_file(const std::string& file_path, size_t expected_size, void* buffer, size_t buffer_size)
{
    struct stat stat_buf;
    if (stat(file_path.c_str(), &stat_buf) != 0) {
        ERROR_LOG("failed to stat file: %s", file_path.c_str());
        return false;
    }
    if (S_ISREG(stat_buf.st_mode) == 0) {
        ERROR_LOG("%s is not a regular file", file_path.c_str());
        return false;
    }
    if (static_cast<size_t>(stat_buf.st_size) != expected_size) {
        ERROR_LOG(
            "%s size mismatch, expected %zu bytes, actual %zu bytes", file_path.c_str(), expected_size,
            static_cast<size_t>(stat_buf.st_size));
        return false;
    }
    if (expected_size > buffer_size) {
        ERROR_LOG("%s is larger than buffer", file_path.c_str());
        return false;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("failed to open file: %s", file_path.c_str());
        return false;
    }
    file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(expected_size));
    if (static_cast<size_t>(file.gcount()) != expected_size) {
        ERROR_LOG("failed to read complete file: %s", file_path.c_str());
        return false;
    }
    return true;
}

bool write_file(const std::string& file_path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("write file failed because buffer is nullptr");
        return false;
    }

    int fd = open(file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWRITE);
    if (fd < 0) {
        ERROR_LOG("failed to open file: %s", file_path.c_str());
        return false;
    }

    const ssize_t write_size = write(fd, buffer, size);
    (void)close(fd);
    if (write_size < 0 || static_cast<size_t>(write_size) != size) {
        ERROR_LOG("failed to write complete file: %s", file_path.c_str());
        return false;
    }
    return true;
}
#endif // DATA_UTILS_H
