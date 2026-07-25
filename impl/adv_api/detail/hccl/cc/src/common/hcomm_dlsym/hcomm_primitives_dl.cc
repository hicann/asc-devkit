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

static bool g_HcommBatchTransferOnThreadSupported = false;
static HcclHcommBatchTransferOnThreadFunc g_HcommBatchTransferOnThread = nullptr;

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
