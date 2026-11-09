/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "acl/acl_rt.h"
#include <cstdlib>

extern "C" {
uint32_t g_aclrtLaunchKernelWithHostArgsCallCount = 0;
uint32_t g_aclrtLaunchKernelWithHostArgsLastNumBlocks = 0;
aclrtStream g_aclrtLaunchKernelWithHostArgsLastStream = nullptr;
size_t g_aclrtLaunchKernelWithHostArgsLastArgsSize = 0;
uint32_t g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum = 0;
aclrtLaunchKernelAttr g_aclrtLaunchKernelWithHostArgsLastAttrs[4] = {};
const char* g_aclrtRegStreamStateCallbackLastName = nullptr;
aclrtStreamStateCallback g_aclrtRegStreamStateCallbackLastCallback = nullptr;
void* g_aclrtRegStreamStateCallbackLastArg = nullptr;
aclrtStream g_aclrtCreateStreamWithConfigLastStream = reinterpret_cast<aclrtStream>(0x1010);
aclrtEvent g_aclrtCreateEventExWithFlagLastEventA = reinterpret_cast<aclrtEvent>(0x2020);
aclrtEvent g_aclrtCreateEventExWithFlagLastEventB = reinterpret_cast<aclrtEvent>(0x3030);
uint32_t g_aclrtCreateEventExWithFlagCallCount = 0;
uint32_t g_aclrtCreateStreamWithConfigCallCount = 0;
uint32_t g_aclrtDestroyStreamCallCount = 0;
uint32_t g_aclrtDestroyEventCallCount = 0;
aclrtStream g_aclrtDestroyStreamLastStream = nullptr;
aclrtEvent g_aclrtDestroyEventLastEvent = nullptr;
aclrtEvent g_aclrtRecordEventLastEvent = nullptr;
aclrtStream g_aclrtRecordEventLastStream = nullptr;
aclrtStream g_aclrtStreamWaitEventLastStream = nullptr;
aclrtEvent g_aclrtStreamWaitEventLastEvent = nullptr;
int64_t g_aclrtGetDeviceInfoAicoreNum = 0;
int64_t g_aclrtGetDeviceInfoVectorNum = 0;
}

int aclrtMemcpy(void* dst, size_t destMax, const void* src, size_t count, aclrtMemcpyKind kind) { return 0; }

int aclrtRegisterCpuFunc(
    const aclrtBinHandle handle, const char* funcName, const char* kernelName, aclrtFuncHandle* funcHandle)
{
    return 0;
}

int aclrtKernelArgsInit(aclrtFuncHandle funcHandle, aclrtArgsHandle* argsHandle) { return 0; }

int aclrtKernelArgsAppend(aclrtArgsHandle argsHandle, void* param, size_t paramSize, aclrtParamHandle* paramHandle)
{
    return 0;
}

int aclrtKernelArgsFinalize(aclrtArgsHandle argsHandl) { return 0; }

int aclrtGetDevice(int32_t* deviceId) { return 0; }

int aclrtSetDevice(int32_t deviceId) { return 0; }

int aclrtResetDevice(int32_t deviceId) { return 0; }

int aclrtMalloc(void** devPtr, size_t size, aclrtMemMallocPolicy policy) { return 0; }

int aclrtGetFunctionName(aclrtFuncHandle funcHandle, uint32_t maxLen, char* name) { return 0; }

aclError aclrtRecordEvent(aclrtEvent event, aclrtStream stream)
{
    g_aclrtRecordEventLastEvent = event;
    g_aclrtRecordEventLastStream = stream;
    return ACL_SUCCESS;
}

aclError aclrtStreamWaitEvent(aclrtStream stream, aclrtEvent event)
{
    g_aclrtStreamWaitEventLastStream = stream;
    g_aclrtStreamWaitEventLastEvent = event;
    return ACL_SUCCESS;
}

aclError aclrtResetEvent(aclrtEvent event, aclrtStream stream) { return ACL_SUCCESS; }

aclError aclrtGetDeviceInfo(uint32_t deviceId, aclrtDevAttr attr, int64_t* value)
{
    if (value == nullptr) {
        return 1;
    }
    if (attr == ACL_DEV_ATTR_AICORE_CORE_NUM) {
        *value = g_aclrtGetDeviceInfoAicoreNum;
    } else if (attr == ACL_DEV_ATTR_VECTOR_CORE_NUM) {
        *value = g_aclrtGetDeviceInfoVectorNum;
    }
    return ACL_SUCCESS;
}

aclError aclrtDestroyEvent(aclrtEvent event)
{
    ++g_aclrtDestroyEventCallCount;
    g_aclrtDestroyEventLastEvent = event;
    return ACL_SUCCESS;
}

const char* aclrtGetSocName() { return " "; }

aclError aclrtDestroyStream(aclrtStream stream)
{
    ++g_aclrtDestroyStreamCallCount;
    g_aclrtDestroyStreamLastStream = stream;
    return ACL_SUCCESS;
}

aclError aclrtCreateEventWithFlag(aclrtEvent* event, uint32_t flag) { return ACL_SUCCESS; }

aclError aclrtCreateEventExWithFlag(aclrtEvent* event, uint32_t flag)
{
    ++g_aclrtCreateEventExWithFlagCallCount;
    if (event == nullptr) {
        return 1;
    }
    if (g_aclrtCreateEventExWithFlagCallCount == 1) {
        *event = g_aclrtCreateEventExWithFlagLastEventA;
    } else {
        *event = g_aclrtCreateEventExWithFlagLastEventB;
    }
    return ACL_SUCCESS;
}

aclError aclrtCreateStreamWithConfig(aclrtStream* stream, uint32_t priority, uint32_t flag)
{
    ++g_aclrtCreateStreamWithConfigCallCount;
    if (stream == nullptr) {
        return 1;
    }
    *stream = g_aclrtCreateStreamWithConfigLastStream;
    return ACL_SUCCESS;
}

aclError aclrtFree(void* devPtr)
{
    if (devPtr == nullptr) {
        return 1;
    }
    return ACL_SUCCESS;
}

aclError aclrtMallocHost(void** hostPtr, size_t size)
{
    if (hostPtr == nullptr || size == 0) {
        return 1;
    }
    *hostPtr = std::malloc(size);
    return *hostPtr == nullptr ? 1 : ACL_SUCCESS;
}

aclError aclrtFreeHost(void* hostPtr)
{
    if (hostPtr == nullptr) {
        return 1;
    }
    std::free(hostPtr);
    return ACL_SUCCESS;
}

aclError aclrtMallocWithCfg(void** devPtr, size_t size, aclrtMemMallocPolicy policy, aclrtMallocConfig* cfg)
{
    if (devPtr == nullptr) {
        return 1;
    }
    return ACL_SUCCESS;
}

aclError aclrtBinaryGetFunction(const aclrtBinHandle binHandle, const char* kernelName, aclrtFuncHandle* funcHandle)
{
    return 0;
}

aclError aclrtLaunchKernelWithHostArgs(
    aclrtFuncHandle funcHandle, uint32_t numBlocks, aclrtStream stream, aclrtLaunchKernelCfg* cfg, void* hostArgs,
    size_t argsSize, aclrtPlaceHolderInfo* placeHolderArray, size_t placeHolderNum)
{
    ++g_aclrtLaunchKernelWithHostArgsCallCount;
    g_aclrtLaunchKernelWithHostArgsLastNumBlocks = numBlocks;
    g_aclrtLaunchKernelWithHostArgsLastStream = stream;
    g_aclrtLaunchKernelWithHostArgsLastArgsSize = argsSize;
    g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum = 0;
    if (cfg != nullptr && cfg->attrs != nullptr) {
        g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum = static_cast<uint32_t>(cfg->numAttrs);
        for (size_t i = 0; i < cfg->numAttrs && i < 4; ++i) {
            g_aclrtLaunchKernelWithHostArgsLastAttrs[i] = cfg->attrs[i];
        }
    }
    return 0;
}

aclError aclmdlRICaptureThreadExchangeMode(aclmdlRICaptureMode* mode) { return 0; }

aclError aclrtBinaryGetFunctionByEntry(const aclrtBinHandle binHandle, uint64_t entry, aclrtFuncHandle* funcHandle)
{
    return 0;
}

aclError aclrtGetHardwareSyncAddr(void** addr)
{
    *addr = nullptr;
    return 0;
}

aclError aclrtRegStreamStateCallback(const char* streamName, aclrtStreamStateCallback callback, void* arg)
{
    g_aclrtRegStreamStateCallbackLastName = streamName;
    g_aclrtRegStreamStateCallbackLastCallback = callback;
    g_aclrtRegStreamStateCallbackLastArg = arg;
    return 0;
}
