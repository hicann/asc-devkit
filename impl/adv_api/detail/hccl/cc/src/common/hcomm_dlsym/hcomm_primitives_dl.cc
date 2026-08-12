/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hcomm_primitives_dl.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

using HcclHcommBatchTransferOnThreadFunc =
    int32_t (*)(ThreadHandle, ChannelHandle, const HcclHcommBatchTransferDesc*, uint32_t);
using HcommAicpuTsTaskCacheLookupFunc = int32_t (*)(const char*, bool*);
using HcommAicpuTsTaskCacheStartFunc = int32_t (*)(const char*, void**, uint64_t*, uint64_t);
using HcommAicpuTsTaskCacheEndFunc = int32_t (*)(const char*);
using HcommAicpuTsTaskCacheExecuteFunc = int32_t (*)(const char*, void**, uint64_t*, uint64_t);
using HcommAicpuTsTaskCacheClearFunc = int32_t (*)(const char*);

static bool g_HcommBatchTransferOnThreadSupported = false;
static HcclHcommBatchTransferOnThreadFunc g_HcommBatchTransferOnThread = nullptr;
static HcommAicpuTsTaskCacheLookupFunc g_HcommAicpuTsTaskCacheLookup = nullptr;
static HcommAicpuTsTaskCacheStartFunc g_HcommAicpuTsTaskCacheStart = nullptr;
static HcommAicpuTsTaskCacheEndFunc g_HcommAicpuTsTaskCacheEnd = nullptr;
static HcommAicpuTsTaskCacheExecuteFunc g_HcommAicpuTsTaskCacheExecute = nullptr;
static HcommAicpuTsTaskCacheClearFunc g_HcommAicpuTsTaskCacheClear = nullptr;

#ifdef UT_TEST
extern "C" void HcommAicpuTsTaskCacheSetTestFunctions(
    HcommAicpuTsTaskCacheLookupFunc lookup, HcommAicpuTsTaskCacheStartFunc start, HcommAicpuTsTaskCacheEndFunc end,
    HcommAicpuTsTaskCacheExecuteFunc execute, HcommAicpuTsTaskCacheClearFunc clear)
{
    g_HcommAicpuTsTaskCacheLookup = lookup;
    g_HcommAicpuTsTaskCacheStart = start;
    g_HcommAicpuTsTaskCacheEnd = end;
    g_HcommAicpuTsTaskCacheExecute = execute;
    g_HcommAicpuTsTaskCacheClear = clear;
}

extern "C" void HcommAicpuTsTaskCacheResetTestFunctions(void)
{
    HcommAicpuTsTaskCacheSetTestFunctions(nullptr, nullptr, nullptr, nullptr, nullptr);
}
#endif

extern "C" bool HcommIsSupportHcommBatchTransferOnThread(void) { return g_HcommBatchTransferOnThreadSupported; }

extern "C" int32_t HcclHcommBatchTransferOnThread(
    ThreadHandle thread, ChannelHandle channel, const HcclHcommBatchTransferDesc* transferDescs,
    uint32_t transferDescNum)
{
    if (g_HcommBatchTransferOnThread == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommBatchTransferOnThread not supported");
        return -1;
    }
    return g_HcommBatchTransferOnThread(thread, channel, transferDescs, transferDescNum);
}

DEFINE_WEAK_FUNC(
    int32_t, HcommWriteWithNotifyOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t len, uint32_t remoteNotifyIdx);
DEFINE_WEAK_FUNC(
    int32_t, HcommWriteReduceWithNotifyOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t count, HcommDataType dataType, HcommReduceOp reduceOp, uint32_t remoteNotifyIdx);
DEFINE_WEAK_FUNC(
    int32_t, HcommWriteNbiOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t len);
DEFINE_WEAK_FUNC(int32_t, HcommWriteNbi, ChannelHandle channel, void* dst, const void* src, uint64_t len);
DEFINE_WEAK_FUNC(
    int32_t, HcommWriteWithNotifyNbiOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t len, uint32_t remoteNotifyIdx);
DEFINE_WEAK_FUNC(
    int32_t, HcommWriteWithNotifyNbi, ChannelHandle channel, void* dst, const void* src, uint64_t len,
    uint32_t remoteNotifyIdx);
DEFINE_WEAK_FUNC(
    int32_t, HcommReadNbiOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t len);
DEFINE_WEAK_FUNC(int32_t, HcommReadNbi, ChannelHandle channel, void* dst, const void* src, uint64_t len);
DEFINE_WEAK_FUNC(
    int32_t, HcommReadReduceOnThread, ThreadHandle thread, ChannelHandle channel, void* dst, const void* src,
    uint64_t count, HcommDataType dataType, HcommReduceOp reduceOp);
DEFINE_WEAK_FUNC(int32_t, HcommChannelNotifyRecord, ChannelHandle channel, uint32_t remoteNotifyIdx);
DEFINE_WEAK_FUNC(int32_t, HcommChannelNotifyWait, ChannelHandle channel, uint32_t localNotifyIdx, uint32_t timeout);
DEFINE_WEAK_FUNC(
    HcclResult, HcommSymWinGetPeerPointer, HcclCommSymWindow winHandle, size_t offset, uint32_t peerRank, void** ptr);
DEFINE_WEAK_FUNC(int32_t, HcommThreadSynchronize, ThreadHandle thread);
DEFINE_WEAK_FUNC(
    int32_t, HcommSendRequest, uint64_t handle, const char* msgTag, const void* src, size_t sizeByte, uint32_t* msgId);
DEFINE_WEAK_FUNC(int32_t, HcommWaitResponse, uint64_t handle, void* dst, size_t sizeByte, uint32_t* msgId);
DEFINE_WEAK_FUNC(int32_t, HcommFlush);
DEFINE_WEAK_FUNC(int32_t, HcommChannelFence, ChannelHandle channel);
DEFINE_WEAK_FUNC(int32_t, HcommFenceOnThread, ThreadHandle thread);
DEFINE_WEAK_FUNC(int32_t, HcommChannelFenceOnThread, ThreadHandle thread, ChannelHandle channel);
DEFINE_WEAK_FUNC(HcclResult, HcommThreadJoin, ThreadHandle thread, uint32_t timeout);
DEFINE_WEAK_FUNC(
    HcclResult, HcclSymWinGetPeerPointer, HcclCommSymWindow winHandle, size_t offset, uint32_t peerRank, void** ptr);
DEFINE_WEAK_FUNC(
    HcclResult, HcclCommSymWinGet, HcclComm comm, void* ptr, size_t size, HcclCommSymWindow* winHandle, size_t* offset);
extern "C" bool HcommIsSupportAicpuTsTaskCache(void)
{
    return HcommIsSupportHcommAicpuTsTaskCacheLookup() && HcommIsSupportHcommAicpuTsTaskCacheStart() &&
           HcommIsSupportHcommAicpuTsTaskCacheEnd() && HcommIsSupportHcommAicpuTsTaskCacheExecute() &&
           HcommIsSupportHcommAicpuTsTaskCacheClear();
}

extern "C" bool HcommIsSupportHcommAicpuTsTaskCacheLookup(void) { return g_HcommAicpuTsTaskCacheLookup != nullptr; }

extern "C" bool HcommIsSupportHcommAicpuTsTaskCacheStart(void) { return g_HcommAicpuTsTaskCacheStart != nullptr; }

extern "C" bool HcommIsSupportHcommAicpuTsTaskCacheEnd(void) { return g_HcommAicpuTsTaskCacheEnd != nullptr; }

extern "C" bool HcommIsSupportHcommAicpuTsTaskCacheExecute(void) { return g_HcommAicpuTsTaskCacheExecute != nullptr; }

extern "C" bool HcommIsSupportHcommAicpuTsTaskCacheClear(void) { return g_HcommAicpuTsTaskCacheClear != nullptr; }

extern "C" int32_t HcclHcommAicpuTsTaskCacheLookup(const char* tag, bool* isHit)
{
    if (g_HcommAicpuTsTaskCacheLookup == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommAicpuTsTaskCacheLookup not supported");
        return HCCL_E_NOT_SUPPORT;
    }
    return g_HcommAicpuTsTaskCacheLookup(tag, isHit);
}

extern "C" int32_t HcclHcommAicpuTsTaskCacheStart(const char* tag, void** addrs, uint64_t* sizes, uint64_t count)
{
    if (g_HcommAicpuTsTaskCacheStart == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommAicpuTsTaskCacheStart not supported");
        return HCCL_E_NOT_SUPPORT;
    }
    return g_HcommAicpuTsTaskCacheStart(tag, addrs, sizes, count);
}

extern "C" int32_t HcclHcommAicpuTsTaskCacheEnd(const char* tag)
{
    if (g_HcommAicpuTsTaskCacheEnd == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommAicpuTsTaskCacheEnd not supported");
        return HCCL_E_NOT_SUPPORT;
    }
    return g_HcommAicpuTsTaskCacheEnd(tag);
}

extern "C" int32_t HcclHcommAicpuTsTaskCacheExecute(const char* tag, void** addrs, uint64_t* sizes, uint64_t count)
{
    if (g_HcommAicpuTsTaskCacheExecute == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommAicpuTsTaskCacheExecute not supported");
        return HCCL_E_NOT_SUPPORT;
    }
    return g_HcommAicpuTsTaskCacheExecute(tag, addrs, sizes, count);
}

extern "C" int32_t HcclHcommAicpuTsTaskCacheClear(const char* tag)
{
    if (g_HcommAicpuTsTaskCacheClear == nullptr) {
        HCCL_ERROR("[HcclWrapper] HcommAicpuTsTaskCacheClear not supported");
        return HCCL_E_NOT_SUPPORT;
    }
    return g_HcommAicpuTsTaskCacheClear(tag);
}

// ---------- 初始化函数 ----------
void HcommPrimitivesDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteWithNotifyOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteReduceWithNotifyOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteNbiOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteNbi);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteWithNotifyNbiOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWriteWithNotifyNbi);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommReadNbiOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommReadNbi);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommReadReduceOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommChannelNotifyRecord);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommChannelNotifyWait);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommSymWinGetPeerPointer);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommThreadSynchronize);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommSendRequest);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommWaitResponse);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommFlush);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommChannelFence);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommFenceOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommChannelFenceOnThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommThreadJoin);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclSymWinGetPeerPointer);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommSymWinGet);
    g_HcommAicpuTsTaskCacheLookup =
        reinterpret_cast<HcommAicpuTsTaskCacheLookupFunc>(dlsym(libHcommHandle, "HcommAicpuTsTaskCacheLookup"));
    g_HcommAicpuTsTaskCacheStart =
        reinterpret_cast<HcommAicpuTsTaskCacheStartFunc>(dlsym(libHcommHandle, "HcommAicpuTsTaskCacheStart"));
    g_HcommAicpuTsTaskCacheEnd =
        reinterpret_cast<HcommAicpuTsTaskCacheEndFunc>(dlsym(libHcommHandle, "HcommAicpuTsTaskCacheEnd"));
    g_HcommAicpuTsTaskCacheExecute =
        reinterpret_cast<HcommAicpuTsTaskCacheExecuteFunc>(dlsym(libHcommHandle, "HcommAicpuTsTaskCacheExecute"));
    g_HcommAicpuTsTaskCacheClear =
        reinterpret_cast<HcommAicpuTsTaskCacheClearFunc>(dlsym(libHcommHandle, "HcommAicpuTsTaskCacheClear"));

    dlerror();
    g_HcommBatchTransferOnThread =
        reinterpret_cast<HcclHcommBatchTransferOnThreadFunc>(dlsym(libHcommHandle, "HcommBatchTransferOnThread"));
    const char* batchTransferError = dlerror();
    g_HcommBatchTransferOnThreadSupported = (g_HcommBatchTransferOnThread != nullptr && batchTransferError == nullptr);
    if (!g_HcommBatchTransferOnThreadSupported) {
        g_HcommBatchTransferOnThread = nullptr;
    }
    HCCL_DEBUG(
        "[MC2_BATCH_TRANSFER][RuntimeSupport] supported[%d], func[%p], error[%s].",
        static_cast<int>(g_HcommBatchTransferOnThreadSupported), reinterpret_cast<void*>(g_HcommBatchTransferOnThread),
        batchTransferError == nullptr ? "none" : batchTransferError);
}
