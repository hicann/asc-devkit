/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCOMM_HOST_PROFILING_DL_H
#define HCOMM_HOST_PROFILING_DL_H

#include "dlsym_common.h"
#include "hccl_res_dl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HcomProInfoTmp {
#define MAX_LENGTH 128
    uint8_t dataType;
    uint8_t cmdType;
    uint64_t dataCount;
    uint32_t rankSize;
    uint32_t userRank;
    uint32_t blockDim = 0;
    uint64_t beginTime;
    uint32_t root;
    uint32_t slaveThreadNum;
    uint64_t commNameLen;
    uint64_t algTypeLen;
    char tag[MAX_LENGTH];
    char commName[MAX_LENGTH];
    char algType[MAX_LENGTH];
    bool isCapture = false;
    bool isAiv = false;
    uint8_t reserved[MAX_LENGTH];
} HcomProInfoTmp;

DECL_WEAK_FUNC(HcclResult, HcommProfilingRegThread, HcomProInfoTmp profInfo, ThreadHandle* threads);
DECL_WEAK_FUNC(HcclResult, HcommProfilingUnRegThread, HcomProInfoTmp profInfo, ThreadHandle* threads);
DECL_WEAK_FUNC(HcclResult, HcommProfilingReportKernel, uint64_t beginTime, const char* profName);
DECL_WEAK_FUNC(HcclResult, HcommProfilingReportOp, HcomProInfoTmp profInfo);
DECL_WEAK_FUNC(uint64_t, HcommGetProfilingSysCycleTime);
DECL_WEAK_FUNC(HcclResult, HcclDfxRegOpInfo, HcclComm comm, void* dfxOpInfo);
DECL_WEAK_FUNC(HcclResult, HcclDfxRegOpInfoByCommId, char* commId, void* dfxOpInfo);
DECL_WEAK_FUNC(HcclResult, HcclProfilingReportOp, HcclComm comm, uint64_t beginTime);
DECL_WEAK_FUNC(HcclResult, HcclReportAicpuKernel, HcclComm comm, uint64_t beginTime, char* kernelName);

// 动态库管理接口
void HcommProfilingDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // HCOMM_PROFILING_DL_H
