/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "sim_task_queue.h"

namespace HcclSim {
SimTaskQueue* SimTaskQueue::Global()
{
    static SimTaskQueue* globalSimTaskQueue = new SimTaskQueue;
    return globalSimTaskQueue;
}

void SimTaskQueue::Clear() { allRankTask_.clear(); }

void SimTaskQueue::AppendTask(NpuPos npuPos, SimStream* stream, std::shared_ptr<TaskStub> task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SingleTaskQueue& taskQueues = allRankTask_[npuPos.superpodId][npuPos.serverId][npuPos.phyId];
    uint32_t streamId_ = stream->GetStreamId();
    if (taskQueues.size() < (streamId_ + 1)) {
        taskQueues.resize(streamId_ + 1);
    }
    taskQueues[streamId_].push_back(task);
    return;
}

AllRankTaskQueues& SimTaskQueue::GetAllRankTaskQueues() { return allRankTask_; }

} // namespace HcclSim
