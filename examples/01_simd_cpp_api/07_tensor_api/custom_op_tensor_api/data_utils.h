/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CUSTOM_OP_TENSOR_API_DATA_UTILS_H
#define CUSTOM_OP_TENSOR_API_DATA_UTILS_H

#include <cstddef>
#include <fstream>
#include <string>

inline bool read_file(const std::string& file_path, size_t size, void* buffer)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
    return file.gcount() == static_cast<std::streamsize>(size);
}

inline bool write_file(const std::string& file_path, const void* buffer, size_t size)
{
    std::ofstream file(file_path, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file.write(static_cast<const char*>(buffer), static_cast<std::streamsize>(size));
    return file.good();
}

#endif // CUSTOM_OP_TENSOR_API_DATA_UTILS_H
