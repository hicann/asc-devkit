/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file aicpu_rt.cpp
 * \brief
 */

#include "acl/acl.h"
#include "securec.h"
#include "aicpu_rt.h"
#include "ascendc_tool_log.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cstdint>
#include <memory>
#include <mutex>
#include <chrono>

#define CHECK_ACL_PTR(x)                                \
    do {                                                \
        aclError __ret = x;                             \
        if (__ret != ACL_ERROR_NONE) {                  \
            ASCENDLOGE("[Check]acl Error: %d.", __ret); \
            return nullptr;                             \
        }                                               \
    } while (0)

#define CHECK_ACL_FREE(x, ptr)                          \
    do {                                                \
        aclError __ret = x;                             \
        if (__ret != ACL_ERROR_NONE) {                  \
            ASCENDLOGE("[Check]acl Error: %d.", __ret); \
            free(ptr);                                  \
            return;                                     \
        }                                               \
    } while (0)

#define CHECK_ACL_ERR(x)                                \
    do {                                                \
        aclError __ret = x;                             \
        if (__ret != ACL_ERROR_NONE) {                  \
            ASCENDLOGE("[Check]acl Error: %d.", __ret); \
            ;                                           \
            return __ret;                               \
        }                                               \
    } while (0)

#define CHECK_ACL(x)                                    \
    do {                                                \
        aclError __ret = x;                             \
        if (__ret != ACL_ERROR_NONE) {                  \
            ASCENDLOGE("[Check]acl Error: %d.", __ret); \
            return;                                     \
        }                                               \
    } while (0)

constexpr size_t DUMP_SIZE = 1048576;
// DumpConfig struct contains 2 fields: offset (size_t) and size (size_t)
constexpr size_t DUMP_CONFIG_FIELD_COUNT = 2;

extern "C" {
int32_t ElfGetSymbolOffset(uint8_t* elf, size_t elfSize, const char* symbolName, size_t* offset, size_t* size);

void AicpuDumpPrintBuffer(const void* dumpBuffer, const size_t bufSize)
{
    if (dumpBuffer == nullptr || bufSize == 0) {
        ASCENDLOGE("AicpuDumpPrintBuffer: empty buffer.");
        return;
    }
    void* bufHost = malloc(bufSize);
    if (bufHost == nullptr) {
        ASCENDLOGE("Failed to allocate host buffer of size %zu", bufSize);
        return;
    }
    memset_s(bufHost, bufSize, 0, bufSize);
    // support CAPTURE MODE GLOBAL on host, when using device printf
    aclmdlRICaptureMode mode = ACL_MODEL_RI_CAPTURE_MODE_RELAXED;
    CHECK_ACL_FREE(aclmdlRICaptureThreadExchangeMode(&mode), bufHost);
    CHECK_ACL_FREE(aclrtMemcpy(bufHost, bufSize, dumpBuffer, bufSize, ACL_MEMCPY_DEVICE_TO_HOST), bufHost);
    CHECK_ACL_FREE(aclmdlRICaptureThreadExchangeMode(&mode), bufHost);
    static thread_local size_t lastOffSet = 8;
    size_t curOffSet = *reinterpret_cast<size_t*>(bufHost);
    if (curOffSet > lastOffSet) {
        printf("%s", reinterpret_cast<char*>(bufHost) + lastOffSet);
        lastOffSet = curOffSet;
    }
    free(bufHost);
}

AicpuDumpThreadRes::AicpuDumpThreadRes(const void* dumpAddr, const size_t dumpSize, const int32_t deviceId)
    : thread_([dumpAddr, dumpSize, deviceId, args = &mutex_]() {
          ASCENDLOGI("Aicpu dump thread started, deviceId: %d.", deviceId);
          CHECK_ACL(aclrtSetDevice(deviceId));
          while (!args->stop.load(std::memory_order_relaxed)) {
              AicpuDumpPrintBuffer(dumpAddr, dumpSize);
              std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 means 100 ms
          }
          CHECK_ACL(aclrtResetDevice(deviceId));
      })
{}

AicpuDumpThreadRes::~AicpuDumpThreadRes()
{
    mutex_.stop.store(true, std::memory_order_relaxed);
    thread_.join();
}

int AicpuGetDumpConfig(void** addr, size_t* size)
{
    static void* dumpAddr[16] = {nullptr};
    static std::unique_ptr<AicpuDumpThreadRes> dumpThreads[16];
    static std::mutex dumpMutex;
    int32_t deviceId = -1;
    CHECK_ACL_ERR(aclrtGetDevice(&deviceId));

    if (dumpAddr[deviceId] != nullptr) {
        *addr = dumpAddr[deviceId];
        *size = DUMP_SIZE;
        return 0;
    }
    std::lock_guard<std::mutex> lock(dumpMutex);
    if (dumpAddr[deviceId] == nullptr) {
        void* deviceAddr = nullptr;
        CHECK_ACL_ERR(aclrtMalloc(reinterpret_cast<void**>(&deviceAddr), DUMP_SIZE, ACL_MEM_MALLOC_HUGE_FIRST));
        dumpAddr[deviceId] = deviceAddr;
        uint64_t bufferOffSet = 8;
        // support CAPTURE MODE GLOBAL on host, when using device printf
        aclmdlRICaptureMode mode = ACL_MODEL_RI_CAPTURE_MODE_RELAXED;
        CHECK_ACL_ERR(aclmdlRICaptureThreadExchangeMode(&mode));
        CHECK_ACL_ERR(aclrtMemcpy(deviceAddr, 8, &bufferOffSet, 8, ACL_MEMCPY_HOST_TO_DEVICE));
        CHECK_ACL_ERR(aclmdlRICaptureThreadExchangeMode(&mode));
        *addr = dumpAddr[deviceId];
        *size = DUMP_SIZE;
        dumpThreads[deviceId] = std::make_unique<AicpuDumpThreadRes>(*addr, *size, deviceId);
    }
    return 0;
}

size_t* AicpuSetDumpConfig(const unsigned long* aicpuFileBuf, size_t fileSize)
{
    if (aicpuFileBuf == nullptr) {
        ASCENDLOGE("aicpuFileBuf is nullptr");
        return nullptr;
    }
    if (fileSize == 0ULL) {
        ASCENDLOGE("Invalid fileSize: %zu", fileSize);
        return nullptr;
    }
    size_t* kernelBuf = reinterpret_cast<size_t*>(malloc(fileSize));
    if (kernelBuf == nullptr) {
        ASCENDLOGE("Failed to allocate memory for kernel buffer, size: %zu", fileSize);
        return nullptr;
    }
    memcpy_s(kernelBuf, fileSize, aicpuFileBuf, fileSize);
    size_t startIndex = 0, symbolSize = 0;
    int32_t ret = ElfGetSymbolOffset(
        reinterpret_cast<uint8_t*>(kernelBuf), fileSize, "g_aicpuDumpConfig", &startIndex, &symbolSize);
    if (ret != 0) {
        if (ret == 1) {
            free(kernelBuf);
            ASCENDLOGE("elf is not legal, please check log!");
            return nullptr;
        } else if (ret == 2) { // 2 means no symbol: g_aicpuDumpConfig
            ASCENDLOGI("dump switch is off.");
        }
        return kernelBuf;
    }
    // validate startIndex and symbolSize to avoid out-of-bounds access
    if (symbolSize < sizeof(size_t) * DUMP_CONFIG_FIELD_COUNT ||
        startIndex + sizeof(size_t) * DUMP_CONFIG_FIELD_COUNT > fileSize) {
        ASCENDLOGE(
            "Invalid symbol offset or size: startIndex=%zu, symbolSize=%zu, fileSize=%zu", startIndex, symbolSize,
            fileSize);
        return kernelBuf;
    }
    if (startIndex % sizeof(size_t) != 0) {
        ASCENDLOGE("Symbol offset is not aligned to size_t: startIndex=%zu", startIndex);
        return kernelBuf;
    }
    void* dumpAddr = nullptr;
    size_t dumpSize = 0;
    AicpuGetDumpConfig(&dumpAddr, &dumpSize);
    startIndex /= sizeof(size_t);
    kernelBuf[startIndex] = static_cast<size_t>(reinterpret_cast<uintptr_t>(dumpAddr));
    kernelBuf[startIndex + 1] = dumpSize;
    return kernelBuf;
}
}
