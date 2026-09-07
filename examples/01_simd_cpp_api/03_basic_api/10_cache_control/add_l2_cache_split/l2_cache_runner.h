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
 * \file l2_cache_runner.h
 * \brief Host runtime shared by the negative and positive examples.
 */

#ifndef ADD_L2_CACHE_SPLIT_L2_CACHE_RUNNER_H
#define ADD_L2_CACHE_SPLIT_L2_CACHE_RUNNER_H

#include "acl/acl.h"
#include "data_utils.h"
#include "l2_cache_kernel.h"
#include <cerrno>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <sys/stat.h>

namespace AddL2Cache {

using KernelLauncher = void (*)(aclrtStream, uint8_t*);

inline bool CheckAcl(const char* action, aclError ret)
{
    if (ret == ACL_SUCCESS) {
        return true;
    }
    std::fprintf(stderr, "[ERROR] %s failed, aclError=%d\n", action, static_cast<int32_t>(ret));
    return false;
}

inline uint16_t GetHalfBits(const aclFloat16& value)
{
    uint16_t bits = 0;
    static_assert(sizeof(bits) == sizeof(value), "aclFloat16 must use 16 bits");
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

inline bool VerifyOutput(const uint8_t* inputHost, const uint8_t* outputHost, size_t dataSizeBytes)
{
    const aclFloat16* input = reinterpret_cast<const aclFloat16*>(inputHost);
    const aclFloat16* output = reinterpret_cast<const aclFloat16*>(outputHost);
    const size_t elementCount = dataSizeBytes / sizeof(aclFloat16);

    for (size_t idx = 0; idx < elementCount; ++idx) {
        const float inputValue = aclFloat16ToFloat(input[idx]);
        const aclFloat16 expected = aclFloatToFloat16(inputValue + static_cast<float>(computeRoundCount));
        if (std::memcmp(&output[idx], &expected, sizeof(aclFloat16)) == 0) {
            continue;
        }

        std::fprintf(
            stderr, "[ERROR] result mismatch at index %zu: input=%f, output=%f(0x%04x), expected=%f(0x%04x)\n", idx,
            inputValue, aclFloat16ToFloat(output[idx]), GetHalfBits(output[idx]), aclFloat16ToFloat(expected),
            GetHalfBits(expected));
        return false;
    }
    return true;
}

inline bool RunKernel(const char* caseName, const char* outputPath, KernelLauncher launchKernel, aclrtStream stream)
{
    constexpr size_t dataSizeBytes = static_cast<size_t>(totalElementCount) * sizeof(aclFloat16);

    uint8_t* inputHost = nullptr;
    uint8_t* outputHost = nullptr;
    uint8_t* dataDevice = nullptr;

    bool success =
        CheckAcl("aclrtMallocHost(inputHost)", aclrtMallocHost(reinterpret_cast<void**>(&inputHost), dataSizeBytes));
    success =
        success &&
        CheckAcl("aclrtMallocHost(outputHost)", aclrtMallocHost(reinterpret_cast<void**>(&outputHost), dataSizeBytes));
    success =
        success && CheckAcl(
                       "aclrtMalloc(dataDevice)",
                       aclrtMalloc(reinterpret_cast<void**>(&dataDevice), dataSizeBytes, ACL_MEM_MALLOC_HUGE_FIRST));

    size_t fileSize = 0;
    if (success) {
        success = ReadFile("./input/input.bin", fileSize, inputHost, dataSizeBytes) && fileSize == dataSizeBytes;
        if (!success) {
            std::fprintf(stderr, "[ERROR] invalid input file (expected %zu bytes, got %zu)\n", dataSizeBytes, fileSize);
        }
    }
    if (success && mkdir("./output", 0755) != 0 && errno != EEXIST) {
        std::fprintf(stderr, "[ERROR] failed to create output directory: %s\n", std::strerror(errno));
        success = false;
    }
    if (success) {
        success = CheckAcl(
            "aclrtMemcpy(HOST_TO_DEVICE)",
            aclrtMemcpy(dataDevice, dataSizeBytes, inputHost, dataSizeBytes, ACL_MEMCPY_HOST_TO_DEVICE));
    }
    if (success) {
        launchKernel(stream, dataDevice);
        success = CheckAcl("aclrtSynchronizeStream", aclrtSynchronizeStream(stream));
    }
    if (success) {
        success = CheckAcl(
            "aclrtMemcpy(DEVICE_TO_HOST)",
            aclrtMemcpy(outputHost, dataSizeBytes, dataDevice, dataSizeBytes, ACL_MEMCPY_DEVICE_TO_HOST));
    }
    if (success) {
        success = VerifyOutput(inputHost, outputHost, dataSizeBytes);
    }
    if (success) {
        success = WriteFile(outputPath, outputHost, dataSizeBytes);
    }
    if (success) {
        std::printf("%s output: %s\n", caseName, outputPath);
    }

    if (dataDevice != nullptr) {
        (void)aclrtFree(dataDevice);
    }
    if (outputHost != nullptr) {
        (void)aclrtFreeHost(outputHost);
    }
    if (inputHost != nullptr) {
        (void)aclrtFreeHost(inputHost);
    }
    return success;
}

inline int32_t RunSample(
    int32_t argc, char* argv[], const char* caseName, const char* outputPath, KernelLauncher launchKernel,
    bool enableL2Split)
{
    constexpr uint32_t bytesPerMiB = 1024 * 1024;
    if (argc != 1) {
        std::fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }
    const uint32_t activeWorkingSetBytes = enableL2Split ? l2BatchSizeBytes : totalByteSize;
    const uint32_t activeBatchCount = enableL2Split ? l2BatchCount : 1U;
    std::printf(
        "case=%s, total_data_elements=%u, total_data=%u MB, "
        "l2_split=%s, l2_working_set_threshold=%u MB, active_working_set=%u MB, "
        "logical_gm_to_ub=%u MB, vector_cores=%u, l2_batches=%u, compute_rounds=%u\n",
        caseName, totalElementCount, totalByteSize / bytesPerMiB, enableL2Split ? "enabled" : "disabled",
        L2_TILE_THRESHOLD / bytesPerMiB, activeWorkingSetBytes / bytesPerMiB, logicalGmToUbBytesPerKernel / bytesPerMiB,
        vectorCoreCount, activeBatchCount, computeRoundCount);
    std::fflush(stdout);

    bool aclInitialized = CheckAcl("aclInit", aclInit(nullptr));
    bool success = aclInitialized;
    constexpr int32_t deviceId = 0;
    bool deviceSet = false;
    if (success) {
        deviceSet = CheckAcl("aclrtSetDevice", aclrtSetDevice(deviceId));
        success = deviceSet;
    }
    aclrtStream stream = nullptr;
    if (success) {
        success = CheckAcl("aclrtCreateStream", aclrtCreateStream(&stream));
    }

    if (success) {
        success = RunKernel(caseName, outputPath, launchKernel, stream);
    }
    if (stream != nullptr) {
        success = CheckAcl("aclrtDestroyStream", aclrtDestroyStream(stream)) && success;
    }
    if (deviceSet) {
        success = CheckAcl("aclrtResetDevice", aclrtResetDevice(deviceId)) && success;
    }
    if (aclInitialized) {
        success = CheckAcl("aclFinalize", aclFinalize()) && success;
    }
    std::printf("test %s!\n", success ? "pass" : "failed");
    return success ? 0 : 1;
}

} // namespace AddL2Cache

#endif
