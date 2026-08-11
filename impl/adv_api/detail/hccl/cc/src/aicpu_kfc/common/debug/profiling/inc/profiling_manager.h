/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef COMMON_PROFILING_PROFILING_MANAGER_H
#define COMMON_PROFILING_PROFILING_MANAGER_H

#include "rt_external.h"
#include "adapter_prof.h"
#include "task_profiling_pub.h"
#include "profiler_base_pub.h"
#include "externalinput_pub.h"

#include "profiling_manager_pub.h"
#include "aprof_pub.h"
#include "dispatcher.h"

#include <string>
#include <cstdio>
#include <atomic>

namespace hccl {
using Prof_Status = uint32_t;
const Prof_Status SUCCESS = 0x0;
const Prof_Status FAILED = 0xFFFFFFFF;

struct EsLoopUpPara {
    s32 tag;
    void* srcAddr;
    void* dstAddr;
    u32 dataSize;
};

struct EsUpdatePara {
    s32 tag;
    char groupName[GROUP_NAME_MAX_LEN] = {0};
    void* srcAddr;
    void* dstAddr;
    u32 dataSize;
};

class ProfilingManager {
public:
    static constexpr u32 aging = 1;

    ProfilingManager();
    virtual ~ProfilingManager();
    static ProfilingManager& Instance();
    HcclResult CallMsprofRegFftsLaunch() const;
    HcclResult CallMsprofRegHcclOpApi() const;
    HcclResult CallMsprofRegHostApi() const;
    HcclResult RegEsTaskType(ProfTaskType taskType) const;
    HcclResult CallMsprofRegEsTaskTypeApi() const;
    HcclResult CallMsprofRegTaskTypeApi() const;
    HcclResult CallMsprofReportHostApi(
        HcclCMDType cmdType, uint64_t beginTime, u64 count, HcclDataType dataType, AlgType algType, uint64_t groupName,
        u32 numBlocks = 0) const;
    HcclResult CallMsprofReportHostHcclOpApi(
        uint64_t beginTime, uint64_t endTime, uint64_t itemId, uint32_t threadId) const;
    HcclResult CallMsprofReportTaskApi(bool isMainStrem, uint64_t beginTime, ProfTaskType taskType) const;
    HcclResult ReportTaskApi(bool isMainStrem, uint64_t beginTime, ProfTaskType taskType, uint32_t agingFlag) const;
    HcclResult CallEsMsprofReportTaskApi(bool isMainStrem, uint64_t beginTime, ProfTaskType taskType) const;
    HcclResult CallMsprofReportAdditionInfo(uint32_t type, uint64_t timeStamp, const void* data, int len) const;
    HcclResult CallMsprofReportMultiThreadInfo(const std::vector<uint32_t>& tidInfo) const;
    HcclResult CallMsprofReportContextIdInfo(u32 ctxIdMax) const;
    HcclResult CallMsprofReportHostAclApi(
        uint32_t type, uint64_t beginTime, uint64_t endTime, uint64_t itemId, uint32_t threadId) const;
    HcclResult CallMsprofReportHostNodeApi(
        uint64_t beginTime, uint64_t endTime, uint64_t itemId, uint32_t threadId) const;
    HcclResult CallMsprofReportHostNodeBasicInfo(
        uint64_t timeStamp, uint64_t itemId, uint32_t threadId, u32 numBlocks = 0) const;
    HcclResult CallMsprofReportHostHcclOpInfo(
        uint64_t timeStamp, uint32_t threadId, u64 count, HcclDataType dataType, std::string& algTypeStr,
        uint64_t groupName) const;
    HcclResult CallMsprofReportAdditionInfoForEsLookup(EsLoopUpPara& para, ProfTaskType type);
    HcclResult CallMsprofReportAdditionInfoForEsUpdate(const EsUpdatePara& para, ProfTaskType type);
    HcclResult ReportAdditionInfo(
        uint32_t type, uint64_t timeStamp, const void* data, int len, uint32_t agingFlag) const;
    HcclResult CallMsprofReportMc2CommInfo(uint64_t timeStamp, const void* data, int len);
    HcclResult CallMsprofReportEsAdditionInfo(uint32_t type, uint64_t timeStamp, const void* data, int len) const;

    Prof_Status CallMsprofReport(ReporterData& reporterData) const;
    Prof_Status GetHashKey(MsprofHashData& data) const;
    Prof_Status PluginInit() const;
    Prof_Status PluginUnInit() const;
    HcclResult ReportStoragedTaskApi();
    HcclResult ReportStoragedAdditionInfo();
    HcclResult ReportStoragedCompactInfo();
    HcclResult ClearStoragedProfilingInfo();
    HcclResult ReportStoragedFftsInfo();
    HcclResult ReportStoragedOpApi();
    HcclResult CallMsprofReportNodeInfo(
        uint64_t beginTime, uint64_t endTime, const std::string profName, uint32_t threadId);
    void SetMsprofReporterCallback(MsprofReporterCallback func)
    {
        reporterCallback_ = func;
        HCCL_INFO("[Check][Param]SetMsprofReporterCallback.");
    }

    void StartFftsLaunchSubscribe()
    {
        isFftsLaunchSubscribe_ = HCCL_SUCCESS;
        CallMsprofRegFftsLaunch();
        HCCL_RUN_INFO("StartFftsLaunchSubscribe:[%d]", isFftsLaunchSubscribe_);
    }

    void StartHostHcclOpSubscribe()
    {
        isHostHcclOpSubscribe_ = HCCL_SUCCESS;
        HCCL_RUN_INFO("StartHostHcclOpSubscribe:[%d]", isHostHcclOpSubscribe_);
    }

    void StartHostApiSubscribe()
    {
        isHostApiSubscribe_ = HCCL_SUCCESS;
        CallMsprofRegHostApi();
        ReportStoragedCompactInfo();
        HCCL_RUN_INFO("SetHostApiSubscribe:[%d]", isHostApiSubscribe_);
    }

    void StartTaskApiSubscribe()
    {
        isTaskApiSubscribe_ = HCCL_SUCCESS;
        CallMsprofRegTaskTypeApi();
        HCCL_RUN_INFO("SetTaskApiSubscribe:[%d]", isTaskApiSubscribe_);
    }

    void StartAddtionInfoSubscribe()
    {
        isAddtionInfoSubscribe_ = HCCL_SUCCESS;
        ReportStoragedAdditionInfo();
        HCCL_RUN_INFO("StartAddtionInfoSubscribe:[%d]", isAddtionInfoSubscribe_);
    }

    void StartSubscribe(uint64_t profconfig)
    {
        // profconfig同步到platform
        SetProfConfig(profconfig);
        // HostApi粒度的打点控制
        if ((profconfig & PROF_ACL_API_MASK) != 0) {
            StartHostApiSubscribe();
        }

        // aicpu模式下 开启L0就上报task打点; 其他场景开启L1才上报
        if ((GetExternalInputHcclAicpuUnfold() && (profconfig & PROF_TASK_TIME_MASK) != 0) ||
            ((profconfig & PROF_TASK_TIME_L1_MASK) != 0) || ((profconfig & PROF_HCCL_TRACE_MASK) != 0)) {
            StartTaskApiSubscribe();
        }

        // 集合通信算子粒度的打点 只有L0打开的时候才上报 L1打开的时候不上报; AICPU也不上报算子粒度的打点
        if (((profconfig & PROF_TASK_TIME_MASK) != 0) && ((profconfig & PROF_TASK_TIME_L1_MASK) == 0)) {
            StartHostHcclOpSubscribe();
        }

        if (GetExternalInputHcclEnableFfts()) {
            // FFTS打开的时候 L0和L1都上报FFTSLaunch和contextID
            if (((profconfig & PROF_TASK_TIME_MASK) != 0) || ((profconfig & PROF_HCCL_TRACE_MASK) != 0)) {
                StartFftsLaunchSubscribe();
            }
        }
        // L1打开时, 上报task粒度的打点和子task的详细信息
        if (((profconfig & PROF_TASK_TIME_L1_MASK) != 0) || ((profconfig & PROF_HCCL_TRACE_MASK) != 0)) {
            StartAddtionInfoSubscribe();
        } else {
            HCCL_RUN_INFO("[Profiling][CommandHandle] profSwitch is[%u]", profconfig);
        }
    }

    void EsStartTaskApiSubscribe()
    {
        isTaskApiSubscribe_ = HCCL_SUCCESS;
        CallMsprofRegEsTaskTypeApi();
        HCCL_INFO("EsStartTaskApiSubscribe:[%d]", isTaskApiSubscribe_);
    }

    void EsStartAddtionInfoSubscribe()
    {
        isAddtionInfoSubscribe_ = HCCL_SUCCESS;
        HCCL_INFO("[Check][Param]EsStartAddtionInfoSubscribe.");
    }

    void EsStartSubscribe(uint64_t profconfig)
    {
        HCCL_INFO("[Profiling][CommandHandle] EsStartSubscribe profSwitch is[%u]", profconfig);
        // profconfig同步到platform
        SetProfConfig(profconfig);
        if (((profconfig & PROF_TASK_TIME_MASK) != 0) || ((profconfig & PROF_HCCL_TRACE_MASK) != 0)) {
            EsStartTaskApiSubscribe();
        }

        if (((profconfig & PROF_TASK_TIME_L1_MASK) != 0) || ((profconfig & PROF_HCCL_TRACE_MASK) != 0)) {
            EsStartAddtionInfoSubscribe();
        }

        HCCL_INFO("EsStartSubscribe");
    }

    void StopSubscribe(uint64_t profconfig)
    {
        // profconfig同步到platform
        SetProfConfig(profconfig);
        CallMsprofRegHcclOpApi();
        ReportStoragedOpApi();
        ReportStoragedTaskApi();
        isHostApiSubscribe_ = HCCL_E_NOT_SUPPORT;
        isHostHcclOpSubscribe_ = HCCL_E_NOT_SUPPORT;
        isTaskApiSubscribe_ = HCCL_E_NOT_SUPPORT;
        isAddtionInfoSubscribe_ = HCCL_E_NOT_SUPPORT;
        isFftsLaunchSubscribe_ = HCCL_E_NOT_SUPPORT;
        HCCL_RUN_INFO("[ProfilingManage]StopSubscribe.");
    }

    void EsStopSubscribe(uint64_t profconfig)
    {
        // profconfig同步到platform
        SetProfConfig(profconfig);
        isTaskApiSubscribe_ = HCCL_E_NOT_SUPPORT;
        isAddtionInfoSubscribe_ = HCCL_E_NOT_SUPPORT;
        HCCL_INFO("[Check][Param]EsStopSubscribe.");
    }

    HcclResult GetFftsLaunchApiState() { return isFftsLaunchSubscribe_; }
    HcclResult GetAddtionInfoState() { return isAddtionInfoSubscribe_; }
    HcclResult GetTaskApiState() { return isTaskApiSubscribe_; }
    bool GetAllState()
    {
        return (isHostApiSubscribe_ == HCCL_E_NOT_SUPPORT) && (isTaskApiSubscribe_ == HCCL_E_NOT_SUPPORT) &&
               (isAddtionInfoSubscribe_ == HCCL_E_NOT_SUPPORT) && (isHostHcclOpSubscribe_ == HCCL_E_NOT_SUPPORT) &&
               (isFftsLaunchSubscribe_ == HCCL_E_NOT_SUPPORT);
    }
    void SetFftsDispatcherMode();
    void ReSetFftsDispatcherMode();
    static void SetThreadCaptureStatus(s32 threadID, bool isCapture);
    static bool GetThreadCaptureStatus();
    static void DeleteThreadCaptureStatus(s32 threadID);

private:
    MsprofReporterCallback reporterCallback_;
    HcclResult isHostApiSubscribe_ = HCCL_E_NOT_SUPPORT;
    HcclResult isTaskApiSubscribe_ = HCCL_E_NOT_SUPPORT;
    HcclResult isAddtionInfoSubscribe_ = HCCL_E_NOT_SUPPORT;
    HcclResult isHostHcclOpSubscribe_ = HCCL_E_NOT_SUPPORT;
    HcclResult isFftsLaunchSubscribe_ = HCCL_E_NOT_SUPPORT;
    static std::queue<MsprofApi> storageTaskApi_;
    static std::array<std::queue<MsprofAdditionalInfo>, MAX_MODULE_DEVICE_NUM> storageAdditionInfo_;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> reportAddInfoMutex_;
    static std::array<std::queue<MsprofCompactInfo>, MAX_MODULE_DEVICE_NUM> storageCompactInfo_;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> reportCompactInfoMutex_;
    static std::mutex reportDataQueueMutex_;
    static std::array<std::queue<MsprofAdditionalInfo>, MAX_MODULE_DEVICE_NUM> storageAdditionInfoFftsCapture_;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> reportAddInfoFftsCaptureMutex_;
    std::atomic<bool> isFftsDispatcher_{false};
    static std::unordered_map<s32, bool> captureStatusThreadIDMap_;
    static std::mutex captureStatusMapMutex_;
    static std::queue<MsprofApi> storageOpApi_;
    static std::mutex reportDataOpQueueMutex_;
};
} // namespace hccl
#endif // COMMON_PROFILING_PROFILING_MANAGER_H
