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
 * \brief Binary input and golden-data reader for the compare sample.
 */

#ifndef COMPARE_DATA_UTILS_H
#define COMPARE_DATA_UTILS_H

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

bool ReadFile(const std::string& file_path, void* buffer, size_t expected_size)
{
    if (buffer == nullptr) {
        std::cerr << "[ERROR] Buffer is null: " << file_path << std::endl;
        return false;
    }

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open file: " << file_path << std::endl;
        return false;
    }

    const std::streamsize file_size = file.tellg();
    if (file_size != static_cast<std::streamsize>(expected_size)) {
        std::cerr << "[ERROR] Unexpected file size for " << file_path << ": expected " << expected_size << ", got "
                  << file_size << std::endl;
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read(static_cast<char*>(buffer), file_size);
    if (!file) {
        std::cerr << "[ERROR] Failed to read file: " << file_path << std::endl;
        return false;
    }
    return true;
}

#endif // COMPARE_DATA_UTILS_H
