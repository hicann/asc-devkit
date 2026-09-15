/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hccl_res_dl.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

// ---------- 桩函数定义（签名与真实API完全一致）----------
DEFINE_WEAK_FUNC(HcclResult, HcclGetRemoteIpcHcclBuf, HcclComm comm, uint64_t remoteRank, void** addr, uint64_t* size);
DEFINE_WEAK_FUNC(int32_t, HcclTaskRegister, HcclComm comm, const char* msgTag, Callback cb);
DEFINE_WEAK_FUNC(int32_t, HcclTaskUnRegister, HcclComm comm, const char* msgTag);
DEFINE_WEAK_FUNC(
    HcclResult, HcclDevMemAcquire, HcclComm comm, const char* memTag, uint64_t* size, void** addr, bool* newCreated);
DEFINE_WEAK_FUNC(
    HcclResult, HcclThreadExportToCommEngine, HcclComm comm, uint32_t threadNum, const ThreadHandle* threads,
    CommEngine dstCommEngine, ThreadHandle* exportedThreads);
DEFINE_WEAK_FUNC(
    HcclResult, HcclChannelGetRemoteMems, HcclComm comm, ChannelHandle channel, uint32_t* memNum, CommMem** remoteMems,
    char*** memTags);
DEFINE_WEAK_FUNC(
    HcclResult, HcclCommMemReg, HcclComm comm, const char* memTag, const CommMem* mem, HcclMemHandle* memHandle);
DEFINE_WEAK_FUNC(HcclResult, HcclEngineCtxDestroy, HcclComm comm, const char* ctxTag, CommEngine engine);
DEFINE_WEAK_FUNC(
    HcclResult, HcclChannelQuery, HcclComm comm, CommEngine engine, const HcclChannelDesc* channelDescs,
    uint32_t channelNum, ChannelHandle* channels);
DEFINE_WEAK_FUNC(HcclResult, HcclChannelDestroy, HcclComm comm, const ChannelHandle* channels, uint32_t channelNum);

// 初始化
void HcclResDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcclGetRemoteIpcHcclBuf);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclTaskRegister);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclTaskUnRegister);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclDevMemAcquire);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclThreadExportToCommEngine);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclChannelGetRemoteMems);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclCommMemReg);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclEngineCtxDestroy);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclChannelQuery);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclChannelDestroy);
}
