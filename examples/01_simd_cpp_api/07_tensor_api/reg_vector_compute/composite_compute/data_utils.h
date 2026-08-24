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
 * \brief File input and output utilities.
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR]  " fmt "\\n", ##args)

inline bool ReadFile(const std::string& file_path, size_t& file_size, void* buffer, size_t buffer_size)
{
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1) {
        ERROR_LOG("Failed to get file information.");
        return false;
    }
    if (S_ISREG(file_stat.st_mode) == 0) {
        ERROR_LOG("%s is not a file.", file_path.c_str());
        return false;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("Failed to open file. path = %s", file_path.c_str());
        return false;
    }

    std::filebuf* file_buffer = file.rdbuf();
    size_t size = file_buffer->pubseekoff(0, std::ios::end, std::ios::in);
    if (size == 0 || size > buffer_size) {
        ERROR_LOG("Invalid file size.");
        return false;
    }
    file_buffer->pubseekpos(0, std::ios::in);
    file_buffer->sgetn(static_cast<char*>(buffer), size);
    file_size = size;
    return true;
}

inline bool WriteFile(const std::string& file_path, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("Failed to write file because buffer is null.");
        return false;
    }

    int fd = open(file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWRITE);
    if (fd < 0) {
        ERROR_LOG("Failed to open file. path = %s", file_path.c_str());
        return false;
    }

    size_t write_size = write(fd, buffer, size);
    (void)close(fd);
    if (write_size != size) {
        ERROR_LOG("Failed to write file.");
        return false;
    }
    return true;
}

#endif // DATA_UTILS_H
