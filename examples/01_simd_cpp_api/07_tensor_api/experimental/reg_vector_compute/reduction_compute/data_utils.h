/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef REDUCTION_COMPUTE_DATA_UTILS_H
#define REDUCTION_COMPUTE_DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>

#define ERROR_LOG(fmt, args...) std::fprintf(stdout, "[ERROR]  " fmt "\n", ##args)

inline bool read_file(const std::string& file_path, size_t& file_size, void* buffer, size_t buffer_size)
{
    struct stat stat_buffer;
    if (stat(file_path.data(), &stat_buffer) == -1) {
        ERROR_LOG("failed to get file: %s", file_path.c_str());
        return false;
    }
    if (S_ISREG(stat_buffer.st_mode) == 0) {
        ERROR_LOG("%s is not a file", file_path.c_str());
        return false;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("open file failed: %s", file_path.c_str());
        return false;
    }

    std::filebuf* file_buffer = file.rdbuf();
    size_t size = file_buffer->pubseekoff(0, std::ios::end, std::ios::in);
    if (size == 0 || size > buffer_size) {
        ERROR_LOG("invalid file size: %s", file_path.c_str());
        return false;
    }
    file_buffer->pubseekpos(0, std::ios::in);
    file_buffer->sgetn(static_cast<char*>(buffer), size);
    file_size = size;
    return true;
}

inline bool write_file(const std::string& file_path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("write file failed: buffer is nullptr");
        return false;
    }

    int fd = open(file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWRITE);
    if (fd < 0) {
        ERROR_LOG("open file failed: %s", file_path.c_str());
        return false;
    }
    size_t write_size = write(fd, buffer, size);
    (void)close(fd);
    if (write_size != size) {
        ERROR_LOG("write file failed: %s", file_path.c_str());
        return false;
    }
    return true;
}

#endif // REDUCTION_COMPUTE_DATA_UTILS_H
