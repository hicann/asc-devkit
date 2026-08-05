/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "log.h"
#include "hcomm_host_profiling_dl.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

DEFINE_WEAK_FUNC(HcclResult, HcommProfilingRegThread, HcomProInfoTmp profInfo, ThreadHandle* threads);
DEFINE_WEAK_FUNC(HcclResult, HcommProfilingUnRegThread, HcomProInfoTmp profInfo, ThreadHandle* threads);
DEFINE_WEAK_FUNC(HcclResult, HcommProfilingReportKernel, uint64_t beginTime, const char* profName);
DEFINE_WEAK_FUNC(HcclResult, HcommProfilingReportOp, HcomProInfoTmp profInfo);
DEFINE_WEAK_FUNC(uint64_t, HcommGetProfilingSysCycleTime);
DEFINE_WEAK_FUNC(HcclResult, HcclDfxRegOpInfo, HcclComm comm, void* dfxOpInfo);
DEFINE_WEAK_FUNC(HcclResult, HcclDfxRegOpInfoByCommId, char* commId, void* dfxOpInfo);
DEFINE_WEAK_FUNC(HcclResult, HcclProfilingReportOp, HcclComm comm, uint64_t beginTime);
DEFINE_WEAK_FUNC(HcclResult, HcclReportAicpuKernel, HcclComm comm, uint64_t beginTime, char* kernelName);

// 初始化
void HcommProfilingDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, HcommProfilingRegThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommProfilingUnRegThread);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommProfilingReportKernel);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommProfilingReportOp);
    INIT_SUPPORT_FLAG(libHcommHandle, HcommGetProfilingSysCycleTime);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclDfxRegOpInfo);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclDfxRegOpInfoByCommId);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclProfilingReportOp);
    INIT_SUPPORT_FLAG(libHcommHandle, HcclReportAicpuKernel);
}
