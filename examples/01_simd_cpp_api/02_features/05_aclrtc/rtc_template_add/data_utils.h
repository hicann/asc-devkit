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
 * \brief
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "acl/acl.h"

#define ERROR_LOG(fmt, args...) fprintf(stdout, "[ERROR]  " fmt "\n", ##args)

#define ASCENDC_CHECK(x)                                                                    \
    do {                                                                                    \
        aclError __ret = x;                                                                 \
        if (__ret != ACL_SUCCESS) {                                                         \
            std::cerr << __FILE__ << ":" << __LINE__ << " aclError:" << __ret << std::endl; \
            exit(1);                                                                        \
        }                                                                                   \
    } while (0)

bool ReadFile(const std::string& filePath, size_t& fileSize, void* buffer, size_t bufferSize)
{
    struct stat sBuf;
    int fileStatus = stat(filePath.data(), &sBuf);
    if (fileStatus == -1) {
        ERROR_LOG("failed to get file");
        return false;
    }
    if (S_ISREG(sBuf.st_mode) == 0) {
        ERROR_LOG("%s is not a file, please enter a file", filePath.c_str());
        return false;
    }

    std::ifstream file;
    file.open(filePath, std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG("Open file failed. path = %s", filePath.c_str());
        return false;
    }

    std::filebuf* buf = file.rdbuf();
    size_t size = buf->pubseekoff(0, std::ios::end, std::ios::in);
    if (size == 0) {
        ERROR_LOG("file size is 0");
        file.close();
        return false;
    }
    if (size > bufferSize) {
        ERROR_LOG("file size is larger than buffer size");
        file.close();
        return false;
    }
    buf->pubseekpos(0, std::ios::in);
    buf->sgetn(static_cast<char*>(buffer), size);
    fileSize = size;
    file.close();
    return true;
}

bool WriteFile(const std::string& filePath, const void* buffer, size_t size)
{
    if (buffer == nullptr) {
        ERROR_LOG("Write file failed. buffer is nullptr");
        return false;
    }

    int fd = open(filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWRITE);
    if (fd < 0) {
        ERROR_LOG("Open file failed. path = %s", filePath.c_str());
        return false;
    }

    size_t writeSize = write(fd, buffer, size);
    (void)close(fd);
    if (writeSize != size) {
        ERROR_LOG("Write file Failed.");
        return false;
    }

    return true;
}

inline void GenTestData(float* x, float* y, float* golden, size_t len)
{
    srand((unsigned int)time(nullptr));
    for (size_t i = 0; i < len; i++) {
        x[i] = 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 99.0f;
        y[i] = 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 99.0f;
        golden[i] = x[i] + y[i];
    }
}

inline bool VerifyResult(const float* output, const float* golden, size_t len)
{
    constexpr float RELATIVE_TOL = 1e-4f;
    constexpr float ABSOLUTE_TOL = 1e-5f;
    constexpr float ERROR_TOL = 1e-4f;

    size_t errorCount = 0;
    for (size_t i = 0; i < len; i++) {
        float diff = fabsf(output[i] - golden[i]);
        float tolerance = ABSOLUTE_TOL + RELATIVE_TOL * fabsf(golden[i]);
        if (diff > tolerance) {
            if (errorCount < 100) {
                printf(
                    "data index: %06zu, expected: %-.9f, actual: %-.9f, rdiff: %-.6f\n", i, golden[i], output[i],
                    diff / fabsf(golden[i]));
            }
            errorCount++;
        }
    }
    float errorRatio = static_cast<float>(errorCount) / len;
    printf("error ratio: %.4f, tolerance: %.4f\n", errorRatio, ERROR_TOL);
    return errorRatio <= ERROR_TOL;
}
#endif // DATA_UTILS_H
