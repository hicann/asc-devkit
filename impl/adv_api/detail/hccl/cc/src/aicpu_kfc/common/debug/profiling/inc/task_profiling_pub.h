/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TASK_PROFILING_PUB_H
#define TASK_PROFILING_PUB_H

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <map>
#include <queue>
#include <hccl/hccl_types.h>

#include "hccl/base.h"
#include "profiler_base_pub.h"
#include "adapter_prof.h"
#include "prof_common.h"

namespace hccl {
constexpr u64 SHIFT_BITS_PLANE_ID = 28;
constexpr u64 SHIFT_BITS_RANK_SIZE = 16;
constexpr u64 SHIFT_BITS_RANK = 0;

constexpr u64 MASK_PLANE_ID = 0xF;
constexpr u64 MASK_RANK_SIZE = 0xFFF;
constexpr u64 MASK_RANK = 0xFFFF;
constexpr u64 INVALID_U64_PROF = 0xFFFFFFFFFFFFFFFF;
constexpr u64 OP_CALLED_COUNT_LIMIT = 1000000;

#define PARSE_PLANE_ID(id) (((static_cast<u64>(id)) >> SHIFT_BITS_PLANE_ID) & MASK_PLANE_ID)
#define PARSE_RANK(id) (((static_cast<u64>(id)) >> SHIFT_BITS_RANK) & MASK_RANK)
#define PARSE_RANK_SIZE(id) (((static_cast<u64>(id)) >> SHIFT_BITS_RANK_SIZE) & MASK_RANK_SIZE)

constexpr u64 ENGINE_MAX_TAG_LEN = 31;
/* *
 * @name  ProfReporterData
 * @brief struct of data to report
 */
struct ProfReporterData {
    char tag[ENGINE_MAX_TAG_LEN + 1]; // the sub-type of the module, data with different tag will be written
    u32 deviceId;                     // the index of device
    size_t dataLen;                   // the length of send data
    u8* data;                         // the data content
};

struct TaskData {
    u32 streamID;
    u32 taskID;

    TaskType taskType;

    TaskParaDMA DMA;       // taskType = SDMA/RDMA使用, 包括rtRDMASend写notify
    TaskParaReduce Reduce; // taskType = inline/CCE Reduce使用
    TaskParaNotify Notify; // taskType = Notify Record/Wait使用
    TaskParaAiv Aiv;       // taskType = Aiv   使用

    TaskData() : streamID(-1), taskID(-1), taskType(TaskType::TASK_SDMA) {}
    TaskData(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaDMA& para)
        : streamID(streamID), taskID(taskID), taskType(taskType), DMA(para)
    {}
    TaskData(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaReduce& para)
        : streamID(streamID), taskID(taskID), taskType(taskType), Reduce(para)
    {}
    TaskData(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaNotify& para)
        : streamID(streamID), taskID(taskID), taskType(taskType), Notify(para)
    {}
    TaskData(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaAiv& para)
        : streamID(streamID), taskID(taskID), taskType(taskType), Aiv(para)
    {}
};

struct HCCLReportData {
    std::string fileTag;
    uint64_t ts;
    uint32_t type;
    MsprofHcclInfo profInfo;
    std::string tag;
    std::string groupName;
};

enum class ProfTaskType {
    TASK_HCCL_INFO = 0,
    TASK_SDMA,
    TASK_RDMA,
    TASK_REDUCE_INLINE,
    TASK_REDUCE_TBE,
    TASK_NOTIFY_RECORD,
    TASK_NOTIFY_WAIT,
    TASK_STAGEX_STEPX,
    TASK_FLAG,
    TASK_END,
    TASK_MULTI_THREAD,
    TASK_LAUNCH_FFTS_TASK,
    TASK_AIV,

    TASK_ISET_LOOKUP_RESPONSE,
    TASK_WAIT_SOME,
    TASK_GET_LOOKUP_REQUEST,
    TASK_COLL_RECV_LOOKUP_REQUEST,
    TASK_COLL_RECV_UPDATE_REQUEST,
    TASK_ISEND_UPDATE_RESPONSE,
    TASK_ISEND_LOOKUP_RESPONSE,

    // update
    TASK_UPDATE_IMRECV,
    TASK_UPDATE_GLOBAL_REDUCE,

    // new
    TASK_LOOKUP_RESPONSE_MEMCPY,
    TASK_LOOKUP_RESPONSE_ISEND,
    TASK_SHARE_MEMORY_ISEND_RECORD,

    TASK_ABORT_SELF,
    TASK_SERVICE_CANCEL,
    TASK_DESTROY_RESOURCE,
    TASK_EVENT_WAIT,

    // npu lookup
    TASK_KEY_DROP_DUPLICATES,
    TASK_SEND_KEYS,
    TASK_SEND_KEYS_RECORD,
    TASK_EVENT_WAIT_RECV_DONE,
    TASK_RESET_UNIQUE_HANDLE,
    TASK_EVENT_WAIT_SEND_DONE,
    TASK_RECV_VALUES,
    TASK_RECOVER_VALUE_AICORE,
    TASK_GATHER_FINISH,

    // npu update
    TASK_REMOTE_UPDATE_KEY_REDUCE,
    TASK_VALUE_CLEAR_AICORE,
    TASK_VALUE_REDUCE_SUM_AICORE,
    TASK_REMOTE_UPDATE_SEND_REQUEST,
    TASK_UPDATE_RESET_UNIQUE_HANDLE,
    TASK_NOTIFY_REMOTE_IMRECV_DONE_SIGNAL_KEY,
    TASK_NOTIFY_REMOTE_IMRECV_DONE_SIGNAL_VALUE,
    TASK_REMOTE_UPDATE_RECV_RESPONSE,

    TASK_BUILD_CS_TRANSPORT,
    TASK_UPDATE_ALG_GLOBAL_REDUCE,
    TASK_INTER_PROCESSOR_SYNC,
    TASK_INTER_RANK_RECORD,
    TASK_INVALID
};

const std::map<ProfTaskType, std::string> PROF_TASK_OP_NAME = {
    {ProfTaskType::TASK_HCCL_INFO, "hccl_info"},
    {ProfTaskType::TASK_SDMA, "Memcpy"},
    {ProfTaskType::TASK_RDMA, "RDMASend"},
    {ProfTaskType::TASK_REDUCE_INLINE, "Reduce_Inline"},
    {ProfTaskType::TASK_REDUCE_TBE, "Reduce_TBE"},
    {ProfTaskType::TASK_NOTIFY_RECORD, "Notify_Record"},
    {ProfTaskType::TASK_NOTIFY_WAIT, "Notify_Wait"},
    {ProfTaskType::TASK_STAGEX_STEPX, "StageX_StepX"},
    {ProfTaskType::TASK_FLAG, "Flag"},
    {ProfTaskType::TASK_END, "End"},
    {ProfTaskType::TASK_MULTI_THREAD, "Multi_Thread"},
    {ProfTaskType::TASK_LAUNCH_FFTS_TASK, "Launch_Ffts"},
    {ProfTaskType::TASK_AIV, "AivKernel"},

    {ProfTaskType::TASK_WAIT_SOME, "Wait_Some"},
    {ProfTaskType::TASK_COLL_RECV_LOOKUP_REQUEST, "Coll_Recv_Lookup_Request"},
    {ProfTaskType::TASK_COLL_RECV_UPDATE_REQUEST, "Coll_Recv_Update_Request"},
    {ProfTaskType::TASK_ISEND_UPDATE_RESPONSE, "Isend_Update_Response"},
    {ProfTaskType::TASK_ISEND_LOOKUP_RESPONSE, "Isend_Lookup_Response"},

    // RemoteUpdate 使用
    {ProfTaskType::TASK_UPDATE_IMRECV, "Update_Imrecv"},
    {ProfTaskType::TASK_UPDATE_GLOBAL_REDUCE, "Update_Global_Reduce"},

    // RemoteLookup 使用
    {ProfTaskType::TASK_LOOKUP_RESPONSE_MEMCPY, "Lookup_Response_Memcpy"},
    {ProfTaskType::TASK_LOOKUP_RESPONSE_ISEND, "Lookup_Response_Isend"},

    // SHM 使用
    {ProfTaskType::TASK_SHARE_MEMORY_ISEND_RECORD, "Share_Memory_Isend_Record"},

    {ProfTaskType::TASK_ABORT_SELF, "Abort_Self"},
    {ProfTaskType::TASK_SERVICE_CANCEL, "Service_Cancel"},
    {ProfTaskType::TASK_DESTROY_RESOURCE, "Destroy_Resource"},
    {ProfTaskType::TASK_EVENT_WAIT, "Event_Wait"},

    {ProfTaskType::TASK_KEY_DROP_DUPLICATES, "Key_Drop_Duplicates"},
    {ProfTaskType::TASK_SEND_KEYS, "Send_Keys"},
    {ProfTaskType::TASK_SEND_KEYS_RECORD, "Send_Keys_Record"},
    {ProfTaskType::TASK_EVENT_WAIT_RECV_DONE, "Event_Wait_Recv_Done"},
    {ProfTaskType::TASK_RESET_UNIQUE_HANDLE, "Reset_Unique_Handle"},
    {ProfTaskType::TASK_EVENT_WAIT_SEND_DONE, "Event_Wait_Send_Done"},
    {ProfTaskType::TASK_RECV_VALUES, "Recv_Values"},
    {ProfTaskType::TASK_RECOVER_VALUE_AICORE, "Recover_value_Aicore"},
    {ProfTaskType::TASK_GATHER_FINISH, "Gather_Finish"},

    {ProfTaskType::TASK_REMOTE_UPDATE_KEY_REDUCE, "Remote_Update_Key_Reduce"},
    {ProfTaskType::TASK_VALUE_CLEAR_AICORE, "Value_Clear_Aicore"},
    {ProfTaskType::TASK_VALUE_REDUCE_SUM_AICORE, "Value_Reduce_Sum_Aicore"},
    {ProfTaskType::TASK_REMOTE_UPDATE_SEND_REQUEST, "Remote_Update_Send_Request"},
    {ProfTaskType::TASK_UPDATE_RESET_UNIQUE_HANDLE, "Update_Reset_Unique_Handle"},
    {ProfTaskType::TASK_NOTIFY_REMOTE_IMRECV_DONE_SIGNAL_KEY, "Notify_Remote_Imrecv_Done_Signal_Key"},
    {ProfTaskType::TASK_NOTIFY_REMOTE_IMRECV_DONE_SIGNAL_VALUE, "Notify_Remote_Imrecv_Done_Signal_Value"},
    {ProfTaskType::TASK_REMOTE_UPDATE_RECV_RESPONSE, "Remote_Update_Recv_Response"},

    {ProfTaskType::TASK_BUILD_CS_TRANSPORT, "Build_Cs_Transport"},
    {ProfTaskType::TASK_UPDATE_ALG_GLOBAL_REDUCE, "Update_Glg_Global_Reduce"},
    {ProfTaskType::TASK_INTER_PROCESSOR_SYNC, "Inter_Processor_Sync"},
    {ProfTaskType::TASK_INTER_RANK_RECORD, "Inter_Rank_Record"},
    {ProfTaskType::TASK_INVALID, "unknown"}};

inline std::string GetProfTaskOpName(ProfTaskType type)
{
    CHK_PRT_RET(PROF_TASK_OP_NAME.empty(), HCCL_ERROR("PROF_OP_NAME has not inited."), "invalid");
    auto it = PROF_TASK_OP_NAME.find(type);
    if (it != PROF_TASK_OP_NAME.end()) {
        return it->second;
    }
    return "unknown";
}

const std::map<HcclCMDType, std::string> PROF_OP_NAME = {
    {HcclCMDType::HCCL_CMD_INVALID, "hcom_invalid_"},
    {HcclCMDType::HCCL_CMD_BROADCAST, "hcom_broadcast_"},
    {HcclCMDType::HCCL_CMD_ALLREDUCE, "hcom_allReduce_"},
    {HcclCMDType::HCCL_CMD_REDUCE, "hcom_reduce_"},
    {HcclCMDType::HCCL_CMD_SEND, "hcom_send_"},
    {HcclCMDType::HCCL_CMD_RECEIVE, "hcom_receive_"},
    {HcclCMDType::HCCL_CMD_ALLGATHER, "hcom_allGather_"},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "hcom_reduceScatter_"},
    {HcclCMDType::HCCL_CMD_SCATTER, "hcom_scatter_"},
    {HcclCMDType::HCCL_CMD_ALLTOALL, "hcom_alltoall_"},
    {HcclCMDType::HCCL_CMD_ALLTOALLV, "hcom_alltoallv_"},
    {HcclCMDType::HCCL_CMD_ALLGATHER_V, "hcom_allGatherv_"},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V, "hcom_reduceScatterv_"},
    {HcclCMDType::HCCL_CMD_ALLTOALLVC, "hcom_alltoallvc_"},
    {HcclCMDType::HCCL_CMD_BATCH_SEND_RECV, "hcom_batchSendRecv_"},
    {HcclCMDType::HCCL_CMD_BATCH_PUT, "hccl_batchPut_"},
    {HcclCMDType::HCCL_CMD_BATCH_GET, "hccl_batchGet_"}};

inline std::string GetProfOpName(HcclCMDType cmdType)
{
    CHK_PRT_RET(PROF_OP_NAME.empty(), HCCL_ERROR("PROF_OP_NAME has not inited."), "hcom_invalid_");
    auto it = PROF_OP_NAME.find(cmdType);
    if (it != PROF_OP_NAME.end()) {
        return it->second;
    }
    return PROF_OP_NAME.begin()->second;
}

class TaskProfiling : public ProfilerBase {
public:
    /* * 当前Profiling只有注册接口, 生命期需要贯穿整个进程, 故选择静态成员变量
        多线程操作相同reporter_对象需要加锁 */
    static std::mutex mutex_;

public:
    explicit TaskProfiling(u32 deviceLogicId_, u32 localRank_, bool profilingOn = true);
    ~TaskProfiling() override;

public:
    HcclResult Run(const std::string& opName, const std::string& tag) const;
    HcclResult Run(const StepData& stepData) override;
    HcclResult Flush() override;
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaDMA& paraDMA) override;
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaReduce& paraReduce) override;
    HcclResult Save(u32& streamID, u32& taskID, TaskType& taskType, const TaskParaNotify& paraNotify) override;
    HcclResult Save(u32 streamID, u32 taskID, const TaskParaAiv& paraAiv) override;
    HcclResult Save(u32& streamID, u32& taskID) override;
    HcclResult SaveToLog(const TaskParaHost& paraHost) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaDMA& para) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaReduce& para) override;
    HcclResult Save(
        u32 captureStreamID, u32 streamID, u32 taskID, TaskType& taskType, const TaskParaNotify& para) override;
    HcclResult Save(u32 captureStreamID, u32 streamID, u32 taskID) override;
    HcclResult Save(u32 captureStreamID, u32 streamID, u32 taskID, const TaskParaAiv& paraAiv) override;

    static void DumpReportDataInfo(uint32_t type, const MsprofHcclInfo& profInfo);

    static HcclResult ReportMsprofData(HCCLReportData& hcclReportData);

protected:
private:
    HcclResult Run(const TaskData& taskData, bool isCapture = false);
    u64 TimestampNanosecond() const;

    HcclResult Report(struct ProfReporterData& data);

    ProfTaskType GetProfTaskType(TaskType taskType) const;
    double GetTaskTime(TaskType taskType, const TaskData& taskData) const;
    void GetTaskData(TaskType taskType, const TaskData& taskData, struct MsprofHcclInfo& taskInfo);
    void GetSdmaTaskData(TaskType taskType, const TaskData& taskData, struct MsprofHcclInfo& taskInfo) const;
    void GetRdmaTaskData(TaskType taskType, const TaskData& taskData, struct MsprofHcclInfo& taskInfo) const;
    void GetReduceTaskData(TaskType taskType, const TaskData& taskData, struct MsprofHcclInfo& taskInfo) const;
    void GetNotifyTaskData(TaskType taskType, const TaskData& taskData, struct MsprofHcclInfo& taskInfo) const;
    uint32_t GetTransportType(TaskType taskType) const;
    uint32_t GetTaskRole(TaskType taskType) const;

private:
    const u32 localRank_;
    bool profilingOn_; // 当前无条件启动profiling
};
} // namespace hccl

#endif /* TASK_PROFILING_PUB_H */
