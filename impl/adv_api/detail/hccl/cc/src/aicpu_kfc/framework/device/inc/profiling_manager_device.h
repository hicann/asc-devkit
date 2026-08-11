/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ASCEND_ACE_COMOP_HCCL_HCCL_AI_CPU_KERNEL_DFX_PROFILING_PROFILING_MANAGER_H_
#define ASCEND_ACE_COMOP_HCCL_HCCL_AI_CPU_KERNEL_DFX_PROFILING_PROFILING_MANAGER_H_
#include "hccl_types.h"
#include "stream_pub.h"
#include "hccl_common.h"
#include "common/aicpu_hccl_def.h"
#include "prof_common.h"

extern "C" {
__attribute__((weak)) int32_t
AdprofReportBatchAdditionalInfo(uint32_t nonPersistantFlag, const void* data, uint32_t length);
__attribute__((weak)) int32_t
MsprofReportBatchAdditionalInfo(uint32_t nonPersistantFlag, const VOID_PTR data, uint32_t length);
__attribute__((weak)) int32_t AdprofReportAdditionalInfo(uint32_t nonPersistantFlag, const void* data, uint32_t length);
__attribute__((weak)) int32_t
MsprofReportAdditionalInfo(uint32_t nonPersistantFlag, const VOID_PTR data, uint32_t length);
__attribute__((weak)) int32_t AdprofCheckFeatureIsOn(uint64_t feature);
__attribute__((weak)) int32_t MsprofRegisterCallback(uint32_t moduleId, ProfCommandHandle handle);
__attribute__((weak)) uint64_t AdprofGetHashId(const char* hashInfo, size_t length);
__attribute__((weak)) uint64_t MsprofStr2Id(const char* hashInfo, size_t length);
};
namespace dfx {

struct ProfCommInfo {
    uint64_t groupNameHashId{0};
    u32 rankNum{0};
    u32 rankId{0};

    ProfCommInfo() {};

    ProfCommInfo(uint64_t groupNameHashId, u32 rankNum, u32 rankId)
        : groupNameHashId(groupNameHashId), rankNum(rankNum), rankId(rankId) {};
};

class ProfilingManager {
public:
    static HcclResult CallMsprofReportAdditionInfo(uint32_t type, uint64_t timeStamp, const void* data, int len);
    static void SqeInfo2MsprofAicpuMC2HcclInfo(const SqeInfo& sqeInfo, MsprofAicpuHcclTaskInfo& msprofAicpuMC2HcclInfo);
    static bool IsProfOn(uint64_t feature);

    /* AICPU profiling*/
    static bool IsProfL1On();
    static bool IsProfL0On();
    static void SetProL0On(bool val);
    static void SetProL1On(bool val);
    static bool IsL1fromOffToOn();
    static HcclResult ReportTaskInfo(s32 streamId, void* ctxPtr);
    static HcclResult ReportHcclOpInfo(MsprofAicpuHCCLOPInfo& hcclOpInfo, std::string& algTypeStr);
    static uint64_t GetProfHashId(const char* name, uint32_t len);
    static HcclResult GetProfInfoByStreamId(s32 streamId, ProfCommInfo& profInfo);
    static HcclResult AddProfInfoByStreamId(s32 streamId, const std::string& tag, const ProfCommInfo& profInfo);
    static HcclResult UpdateStartReportSqeIdx(s32 streamId, u32 newSqeTailIdx);
    static uint32_t GetStartReportSqeIdx(s32 streamId);
    static bool GetProfL0State();
    static bool GetProfL1State();
    static HcclResult ReportMainStreamTask(hccl::Stream& stream, uint16_t taskId, uint16_t type);
    static HcclResult ReportFilpTask(s32 streamId, uint16_t taskId, uint32_t flipNum);
    static uint64_t TransferAlgType(AlgType algType);
    static void DumpHcclInfo(const MsprofAicpuHcclTaskInfo& taskInfo, u32 batchId, u32 idx);
    static void CommInfo2HcclInfo(const dfx::ProfCommInfo& profInfo, MsprofAicpuHcclTaskInfo& taskInfo);
    static HcclResult TaskInfo2Addition(const void* data, int len, MsprofAdditionalInfo& reporterData);

private:
    static std::mutex streamMutex_;
    static std::unordered_map<std::string, ProfCommInfo> tagOpInfoMap_;
    static std::unordered_map<s32, std::string> streamToTagMap_;

    static std::mutex startReportSqeIdxMutex_;
    static std::unordered_map<s32, u32> streamToSqeIdxMap_;
    static bool isL0Open_;
    static bool isL1Open_;
};
void TaskProfilingCallBack(void* userPtr, void* param, u32 length);
} // namespace dfx
#endif // ASCEND_ACE_COMOP_HCCL_HCCL_AI_CPU_KERNEL_DFX_PROFILING_PROFILING_MANAGER_H_
