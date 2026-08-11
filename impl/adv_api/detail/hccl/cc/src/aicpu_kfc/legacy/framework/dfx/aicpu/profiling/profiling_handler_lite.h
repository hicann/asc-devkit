/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_PROFILING_HANDLER_LITE_H
#define HCCL_PROFILING_HANDLER_LITE_H

#include <vector>
#include <unordered_map>
#include "hccl/hccl_types.h"
#include "prof_common.h"
#include "aprof_pub.h"
#include "task_info.h"
#include "profiling_common.h"

extern "C" {
__attribute__((weak)) int32_t
MsprofReportBatchAdditionalInfo(uint32_t nonPersistantFlag, const VOID_PTR data, uint32_t length);
__attribute__((weak)) int32_t
AdprofReportBatchAdditionalInfo(uint32_t nonPersistantFlag, const void* data, uint32_t length);
__attribute__((weak)) int32_t AdprofReportAdditionalInfo(uint32_t agingFlag, const void* data, uint32_t length);
__attribute__((weak)) int32_t
MsprofReportAdditionalInfo(uint32_t nonPersistantFlag, const VOID_PTR data, uint32_t length);
__attribute__((weak)) int32_t AdprofCheckFeatureIsOn(uint64_t feature);
__attribute__((weak)) int32_t MsprofRegisterCallback(uint32_t moduleId, ProfCommandHandle handle);
__attribute__((weak)) uint64_t AdprofGetHashId(const char* hashInfo, size_t length);
__attribute__((weak)) uint64_t MsprofStr2Id(const char* hashInfo, size_t length);
};

namespace aicpu {
MAKE_ENUM(status_t, AICPU_ERROR_NONE = 0, AICPU_ERROR_FAILED = 1)
status_t __attribute__((weak)) GetTaskAndStreamId(uint64_t& taskId, uint32_t& streamId);
} // namespace aicpu

namespace Hccl {
MAKE_ENUM(MainStreamTaskType, HEAD = 0, TAIL = 1)

MAKE_ENUM(ProfilingLevel, L0, L1)

struct FlagTaskInfo {
    uint32_t taskId;
    MainStreamTaskType type;
};

class ProfilingHandlerLite {
public:
    ~ProfilingHandlerLite();
    ProfilingHandlerLite(const ProfilingHandlerLite& that) = delete;
    ProfilingHandlerLite& operator=(const ProfilingHandlerLite& that) = delete;
    static ProfilingHandlerLite& GetInstance();
    HcclResult Init();
    void ReportHcclOpInfo(const DfxOpInfo& opInfo) const;
    void ReportHcclTaskDetails(const std::vector<TaskInfo*>& taskInfo) const;
    void ReportMainStreamTask(const FlagTaskInfo& flagTaskInfo) const;
    void SetCachedCclTag();
    void SetCachedGroupName(const std::string& groupName, u32 rankSize);
    uint64_t GetCachedAlgTypeHashId() const { return cachedAlgTypeHashId_; }
    void UpdateProfSwitch();
    void SetProL0On(bool val);
    void SetProL1On(bool val);
    inline bool GetProfL0State() const { return enableHcclL0_; }
    inline bool GetProfL1State() const { return enableHcclL1_; }
    uint64_t GetProfHashId(const char* name, uint32_t len) const;

private:
    explicit ProfilingHandlerLite();
    void ReportAdditionInfo(const MsprofAdditionalInfo& reporterData) const;

    bool IsProfOn(uint64_t feature) const;
    bool IsProfSwitchOn(ProfilingLevel level);
    bool IsL1fromOffToOn();

    void DumpTaskDetails(const MsprofAicpuHcclTaskInfo& taskDetailsInfos, const TaskInfo* taskInfo) const;
    void GetTaskDetailInfos(const TaskInfo* it, MsprofAicpuHcclTaskInfo& taskDetailsInfos) const;
    void FillTaskParamDetails(const TaskInfo* it, MsprofAicpuHcclTaskInfo& taskDetailsInfos) const;
    bool FillBatchReporterData(
        uint32_t batchId, const MsprofAicpuHcclTaskInfo* taskInfos, MsprofAdditionalInfo& addInfo) const;
    void ReportHcclTaskDetailsLog(const std::vector<TaskInfo*>& taskInfo) const;
    bool ReportBatchAddInfo(
        uint32_t batchId, MsprofAicpuHcclTaskInfo* taskInfos, MsprofAdditionalInfo* addInfoVec, uint32_t& addInfoIndx,
        uint32_t maxBatchNum, bool isLastBatch) const;

private:
    static ProfilingHandlerLite instance_;
    bool enableHcclL0_{false};
    bool enableHcclL1_{false};
    bool initializedFlag_{false};
    uint64_t cachedGroupName_{INVALID_U64};
    uint64_t cachedAlgTypeHashId_{0};
    u32 cachedRankSize_{0};
    uint32_t cachedTid_{0};
    std::unordered_map<uint32_t, uint64_t> taskTypeHashCache_;
    std::unordered_map<std::string, uint64_t> cachedCclTag_;
    using ReportAdditionalInfoHandle = int32_t (*)(uint32_t, const void*, uint32_t);
    ReportAdditionalInfoHandle reportAdditionalInfo_{nullptr};
    using ReportBatchAdditionalInfoHandle = int32_t (*)(uint32_t, const void*, uint32_t);
    ReportBatchAdditionalInfoHandle reportBatchAdditionalInfo_{nullptr};
    using GetProfHashIdHandle = uint64_t (*)(const char*, size_t);
    GetProfHashIdHandle getProfHashId_{nullptr};
};
} // namespace Hccl

#endif // HCCL_PROFILING_HANDLER_LITE_H
