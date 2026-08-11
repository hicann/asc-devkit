/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TASK_INFO_H
#define TASK_INFO_H
#include <string>
#include <memory>
#include "task_param.h"
#include "coll_operator.h"

namespace Hccl {

class DfxOpInfo {
public:
    CollOperator op_;
    std::string tag_; // Actually opType
    std::string algType_;
    u32 commIndex_;
    u64 beginTime_;
    u64 endTime_;
    void* comm_{nullptr};
    bool isIndop_{false};
    std::string algTag_;
    std::string groupName_;
    u32 rankSize_{0};
    u32 cpuWaitAicpuNotifyId_{0};
    std::string commId_{};
    u32 opIndex_{0};           // Total number of delivered ops (single op/graph mode/CCU fast delivery)
    u64 headOpCounterAddr_{0}; // Head counter address of the executed op (incremented before execution)
    u64 tailOpCounterAddr_{0}; // Tail counter address of the executed op (incremented after execution)
    CommEngine engine{CommEngine::COMM_ENGINE_RESERVED};

public:
    std::string Describe() const
    {
        return StringFormat(
            "DfxOpInfo: [collOperator:[%s], tag:[%s], algType:[%s], commIndex:[%u], commId[%s], "
            "beginTime:[%llu], endTime:[%llu], opIndex[%u], headOpCounterAddr[%llx], "
            "tailOpCounterAddr[%llx], groupName[%s], rankSize[%u], myRank[%d], dataType[%s]]",
            CollOpToString(op_).c_str(), tag_.c_str(), algType_.c_str(), commIndex_, commId_.c_str(), beginTime_,
            endTime_, opIndex_, headOpCounterAddr_, tailOpCounterAddr_, groupName_.c_str(), rankSize_, op_.myRank,
            DataTypeToSerialString(static_cast<uint32_t>(op_.dataType)).c_str());
    }
};

class TaskInfo {
public:
    u32 streamId_;
    u32 taskId_;
    u32 remoteRank_{0xffffffff};
    TaskParam taskParam_;
    std::shared_ptr<DfxOpInfo> dfxOpInfo_;
    bool isMaster_;
    u64 channelHandle_{INVALID_U64};
    std::function<u32(u64)> getRemoteRankByHandle_{nullptr};

public:
    TaskInfo(
        u32 streamId, u32 taskId, u32 remoteRank, const TaskParam& taskParam,
        const std::shared_ptr<DfxOpInfo>& dfxOpInfo = nullptr, bool isMaster = false);

    std::string Describe() const;

    std::string GetAlgTypeName() const;
    std::string GetBaseInfo() const;
    std::string GetConciseBaseInfo() const;
    std::string GetParaInfo() const;
    std::string GetOpInfo() const;

    std::string GetIndopDataInfo() const;
    std::string GetIndopBaseInfo() const;
    u32 GetRemoteRankId() const;

private:
    std::string GetParaDMA() const;
    std::string GetParaReduce() const;
    std::string GetParaNotify() const;
    std::string GetParaAiv() const;
    std::string GetRemoteRankInfo(bool needConcise = false) const;
    std::string GetTaskConciseName() const;
    std::string GetNotifyInfo() const;
};

} // namespace Hccl

#endif
