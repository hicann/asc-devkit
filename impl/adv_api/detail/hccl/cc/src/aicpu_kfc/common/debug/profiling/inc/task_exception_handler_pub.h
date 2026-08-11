/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_TASK_EXCEPTION_HANDLER_PUB_H
#define HCCL_TASK_EXCEPTION_HANDLER_PUB_H
#include <map>
#include <queue>
#include <mutex>
#include <array>
#include <list>
#include <functional>
#include "rt_external.h"
#include "acl/acl_rt.h"
#include "profiler_base_pub.h"
#include "aicpu_operator_pub.h"
namespace hccl {
inline void PrintBaseErrorLog(const std::string& stageErrInfo, const std::string& baseInfo)
{
    HCCL_ERROR("%sTask run failed, base information is %s", stageErrInfo.c_str(), baseInfo.c_str());
}

inline void PrintParaErrorLog(const std::string& stageErrInfo, const std::string& paraInfoStr, const std::string& tag)
{
    HCCL_ERROR(
        "%sTask run failed, para information is %s, tag[%s].", stageErrInfo.c_str(), paraInfoStr.c_str(), tag.c_str());
}

inline void PrintOpDataErrorLog(const std::string& stageErrInfo, const std::string& opDataContent)
{
    HCCL_ERROR("%sTask run failed, opData information is %s", stageErrInfo.c_str(), opDataContent.c_str());
}

inline void PrintGroupErrorLog(
    const std::string& stageErrInfo, const std::string& groupRankContent, const std::string& tag)
{
    HCCL_ERROR(
        "%sTask run failed, groupRank information is %s, tag[%s].", stageErrInfo.c_str(), groupRankContent.c_str(),
        tag.c_str());
}

inline void PrintContextErrorLog(const std::string& stageErrInfo, const std::string& ctxBaseInfo)
{
    HCCL_ERROR("%sTask run failed, context base information is %s", stageErrInfo.c_str(), ctxBaseInfo.c_str());
}

struct ParaDMA {
    const void* src;
    const void* dst;
    std::size_t size;
    u64 notifyID;
    LinkType linkType;
    u32 remoteUserRank;
};

struct ParaReduce {
    const void* src;
    const void* dst;
    std::size_t size;
    HcclReduceOp op;
    HcclDataType dataType;
    LinkType linkType;
    u32 remoteUserRank;
};
struct ParaNotify {
    u64 notifyID;
    s32 stage; // 用于标识stream间同步时所在的stage， 非用于stream同步的默认为-1
    u32 remoteUserRank;
};
struct ParaAiv {
    HcclCMDType cmdType;
    u32 tag;
    u64 size;
    u32 numBlocks;
    u32 rankSize;
    s32 aivRdmaStep;
    void* flagMem;
    u32 rank;
    bool isOpbase;
};
struct TaskInfo {
    u32 streamID;
    u32 taskID;
    std::string tag;
    TaskType taskType;
    bool isAlgInfo;
    AlgType algType;
    u32 index;
    union {
        ParaDMA DMA;       // taskType = SDMA/RDMA使用, 包括rtRDMASend写notify
        ParaReduce Reduce; // taskType = inline/CCE Reduce使用
        ParaNotify Notify; // taskType = Notify Record/Wait使用
        ParaAiv Aiv;       // taskType = Aiv 使用
    } taskPara;
    TaskInfo(
        u32& streamID, u32& taskID, std::string& tag, TaskType& taskType, AlgType& algType, u32& index,
        const TaskParaDMA& para);
    TaskInfo(
        u32& streamID, u32& taskID, std::string& tag, TaskType& taskType, AlgType& algType, u32& index,
        const TaskParaReduce& para);
    TaskInfo(
        u32& streamID, u32& taskID, std::string& tag, TaskType& taskType, AlgType& algType, u32& index,
        const TaskParaNotify& para);
    TaskInfo(u32& streamID, u32& taskID, std::string& tag, const TaskParaAiv& para);
    std::string GetBaseInfoStr(); // 防止tag字符串过长，base信息和para信息分开打印
    std::string GetParaInfoStr();
    std::string GetParaDMA();
    std::string GetParaReduce();
    std::string GetParaNotify();
    std::string GetParaAiv();
    std::string GetRankInfo();
    std::string GetNotifyInfo();
    u32 GetRemoteUserRank();
};
struct FFTSOpInfo {
    u32 streamID;
    u32 taskID;
    std::shared_ptr<char> tag;
    AlgType algType;
    u32 index;
    std::string GetBaseInfoStr();
};
struct CtxInfo {
    TaskType taskType;
    AlgType algType;
    u32 index;
    union {
        ParaDMA DMA;       // taskType = SDMA/RDMA使用, 包括rtRDMASend写notify
        ParaReduce Reduce; // taskType = inline/CCE Reduce使用
        ParaNotify Notify; // taskType = Notify Record/Wait使用
    } ctxPara;
    CtxInfo(TaskType& taskType, const TaskParaDMA& para);
    CtxInfo(TaskType& taskType, const TaskParaReduce& para);
    CtxInfo(TaskType& taskType, const TaskParaNotify& para);
    std::string GetCtxBaseInfoStr(); // 防止tag字符串过长，base信息和para信息分开打印
    std::string GetCtxParaInfoStr();
    std::string GetCtxParaDMA();
    std::string GetCtxParaReduce();
    std::string GetCtxParaNotify();
    std::string GetCtxRankInfo();
    std::string GetCtxNotifyInfo();
    u32 GetCtxRemoteUserRank();
};
class TaskExceptionHandler : public ProfilerBase {
public:
    explicit TaskExceptionHandler(u32 deviceLogicId);
    ~TaskExceptionHandler() override;
    static HcclResult Init();
    static HcclResult DeInit();
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaDMA& para) override;
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaReduce& para) override;
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaNotify& para) override;
    HcclResult Save(u32 streamID, u32 taskID, const TaskParaAiv& para) override;
    HcclResult Save(u32& streamID, u32& taskID) override;
    HcclResult SaveToLog(const TaskParaHost& paraHost) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaDMA& para) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaReduce& para) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaNotify& para) override;
    HcclResult Save(u32 captureStreamID, u32 streamID, u32 taskID) override;
    HcclResult Save(u32 captureStreamID, u32 streamID, u32 taskID, const TaskParaAiv& para) override;
    static void Callback(rtExceptionInfo* exceptionInfo);
    HcclResult Run(const StepData& stepData) override;
    HcclResult Flush() override;

protected:
private:
    HcclResult InsertTaskMap(u32& streamID, TaskInfo& tmpTaskInfo) const;
    HcclResult InsertOpMap(u32& streamID, u32& taskID, std::string& tag, AlgType& algType, u32& index) const;
    HcclResult InsertOpCtxInfo(u32& streamID, u32& taskID, std::string& tag, AlgType& algType, u32& index) const;
    HcclResult InsertRankInfo(std::string& tag) const;
    HcclResult InsertOpData(std::string& tag) const;
    static void DumpAivPrintWorkSpace(const std::shared_ptr<std::deque<TaskInfo>>& taskQue);
    static void PrintTaskContextInfo(
        const std::shared_ptr<std::vector<CtxInfo>>& taskList, u32 contextId, std::string& stageErrInfo);
    static void PrintTaskContextInfo(const std::shared_ptr<std::deque<TaskInfo>>& taskQue, std::string& stageErrInfo);
    static void PrintTaskAivBuffer(const std::shared_ptr<std::deque<TaskInfo>>& taskQue);
    static void PrintTaskAivInfo(const std::shared_ptr<std::deque<TaskInfo>>& taskQue);
    static HcclResult PrintCommAivInfo();
    static void ParseTaskSyncFlag(s32* flagMem, u32 flagMemSize, u32 rankSize, u32 rank, u32 index);
    static std::string SerializeSyncFlag(s32* buf, u32 num, u32 interval);
    static void PrintOpDataInfo(OpDataInfo& opDataInfo, bool isFftsPlus, std::string& stageErrInfo);
    static void TimeStruct2Str(struct timeval& tv, std::string& opDataContent);
    static bool DealExceptionOp(rtExceptionInfo* exceptionInfo);
    static bool DealExceptionTask(rtExceptionInfo* exceptionInfo);
    static bool DealExceptionCtx(rtExceptionInfo* exceptionInfo);
    static bool DealExceptionOpData(
        rtExceptionInfo* exceptionInfo, std::string& tag, bool isFftsPlus, u32 index, std::string& stageErrInfo);
    static bool DealExceptionGroupRank(
        rtExceptionInfo* exceptionInfo, std::string& tag, bool isFftsPlus, std::string& groupRankContentInfo,
        std::string& stageErrInfo);
    static bool FindAndValidateContext(rtExceptionInfo* exceptionInfo);
    static bool ProcessContext(rtExceptionInfo* exceptionInfo, std::string& stageErrInfo);
    static void PrintAicpuErrorMessage(rtExceptionInfo* exceptionInfo, bool& isExistAicpuError);
    static void PrintGroupErrorMessage(
        ErrorMessageReport& errorMessage, TaskInfo& exceptionTaskInfo, std::string& groupRankContent,
        std::string& stageErrInfo);
    static void PrintOpDataErrorMessage(u32 deviceId, ErrorMessageReport& errorMessage, std::string& stageErrInfo);
    static std::array<std::map<int, std::shared_ptr<std::deque<TaskInfo>>>, MAX_MODULE_DEVICE_NUM> taskMap;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> taskMapMutex;
    static std::array<std::map<int, std::shared_ptr<std::deque<FFTSOpInfo>>>, MAX_MODULE_DEVICE_NUM> opMap;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> opMapMutex;
    static std::array<
        std::map<
            int,
            std::shared_ptr<std::deque<std::pair<std::shared_ptr<FFTSOpInfo>, std::shared_ptr<std::vector<CtxInfo>>>>>>,
        MAX_MODULE_DEVICE_NUM>
        opCtxInfo;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> opCtxInfoMutex;
    static std::array<std::vector<CtxInfo>, MAX_MODULE_DEVICE_NUM> ctxInfoArray;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> ctxInfoVectorMutex;
    static std::array<
        std::map<const std::string, std::pair<const std::string, std::shared_ptr<GroupRankInfo>>>,
        MAX_MODULE_DEVICE_NUM>
        groupRankMap;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> groupRankMapMutex;
    static std::array<std::map<const std::string, std::shared_ptr<std::queue<OpDataInfo>>>, MAX_MODULE_DEVICE_NUM>
        tagOpDataMap;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> tagOpDataMapMutex;
    static std::array<std::map<const std::string, std::string>, MAX_MODULE_DEVICE_NUM> groupUdiMap;
    static std::array<std::mutex, MAX_MODULE_DEVICE_NUM> groupUdiMapMutex;
    static std::atomic<int> communicatorCount_;
};

using GetErrStatusVecCallBack = std::vector<std::string> (*)(s32 deviceLogicID, const std::string& group);
using GetAicpuTaskExceptionCallBack = std::function<ErrorMessageReport()>;
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
extern void RegisterGetErrStatusVecCallBack(GetErrStatusVecCallBack);
extern void RegisterGetAicpuTaskExceptionCallBack(s32 streamId, u32 deviceLogicId, GetAicpuTaskExceptionCallBack p1);
#ifdef __cplusplus
}
#endif // __cplusplus
} // namespace hccl

#endif
