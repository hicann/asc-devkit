/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MIRROR_TASK_MANAGER_H
#define MIRROR_TASK_MANAGER_H

#include <unordered_map>
#include <memory>
#include <functional>
#include "circular_queue.h"
#include "task_info.h"
#include "global_mirror_tasks.h"
#include <mutex>

namespace Hccl {

using TaskInfoQueue = Queue<std::unique_ptr<TaskInfo>>;

struct MirrorStreamQueueEntry {
    TaskInfoQueue* queue = nullptr;
    QueueType queueType = QueueType::Vector_Queue;
    u32 taskNum = 0;
};

class MirrorTaskManager {
public:
    MirrorTaskManager(u32 devId, GlobalMirrorTasks* globalMirrorTasks, bool devUsed);

    void RegFullyCallBack(std::function<void()> callBack);
    void AddTaskInfo(std::unique_ptr<TaskInfo>&& taskInfo);
    HcclResult AddTaskInfo(
        u32 streamId, u32 taskId, u32 remoteRankId, const TaskParam& taskParam, std::shared_ptr<DfxOpInfo> dfxOpInfo,
        bool isMaster);
    void SetCurrDfxOpInfo(std::shared_ptr<DfxOpInfo> dfxOpInfo);

    std::shared_ptr<DfxOpInfo> GetCurrDfxOpInfo() const;
    TaskInfoQueue* GetQueue(u32 streamId) const;

public:
    std::unordered_map<u32, MirrorStreamQueueEntry>::iterator Begin();
    std::unordered_map<u32, MirrorStreamQueueEntry>::iterator End();
    std::mutex& GetTaskMutex() { return profMutex; }
    ~MirrorTaskManager();

private:
    u32 devId_;
    GlobalMirrorTasks* globalMirrorTasks_{nullptr};
    bool devUsed_{false};
    bool isStaticGraphMode_{false};
    OpMode opMode_;
    std::unordered_map<u32, MirrorStreamQueueEntry> streamQueues_;
    std::shared_ptr<DfxOpInfo> currDfxOpInfo_;
    std::function<void()> fullyCallBack_;
    std::mutex profMutex;

private:
    bool IsStaticGraphMode(const CollOperator& collOperator) const;
    QueueType GetQueueType() const;
};

} // namespace Hccl

#endif // MIRROR_TASK_MANAGER_H
