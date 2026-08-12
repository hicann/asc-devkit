/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MIRROR_TASK_MANAGER_LITE_H
#define MIRROR_TASK_MANAGER_LITE_H

#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include "dfx_common.h"
#include "circular_queue.h"
#include "task_info.h"

namespace Hccl {

struct StreamQueueEntry {
    std::unique_ptr<CircularQueue<std::unique_ptr<TaskInfo>>> queue;
    u32 capacity = 0;
    u32 taskNum = 0;
};

// NOTE: This class is designed for AICPU single-threaded environments.
// All methods (AddTaskInfo, SetCurrDfxOpInfo, etc.) are NOT thread-safe.
// If used in a multi-threaded environment, external synchronization is required.
class MirrorTaskManagerLite {
public:
    MirrorTaskManagerLite();

    void RegFullyCallBack(std::function<void()> callBack);
    void RegGetRemoteRankCallBack(std::function<u32(u64)> callBack);
    HcclResult AddTaskInfo(u32 streamId, u32 taskId, const Hccl::TaskParam &taskParam, u64 handle);
    void AddTaskInfo(std::unique_ptr<TaskInfo> &&taskInfo);
    HcclResult SetCurrDfxOpInfo(std::shared_ptr<DfxOpInfo> dfxOpInfo);

    std::shared_ptr<DfxOpInfo> GetCurrDfxOpInfo() const;
    TaskInfo* GetTaskInfo(u32 streamId, u32 taskId) const;
    TaskInfoQueue             *GetQueue(u32 streamId) const;

public:
    std::unordered_map<u32, StreamQueueEntry>::iterator Begin();
    std::unordered_map<u32, StreamQueueEntry>::iterator End();

    ~MirrorTaskManagerLite();

private:
    std::unordered_map<u32, StreamQueueEntry> streamQueues_;
    std::shared_ptr<DfxOpInfo>     currDfxOpInfo_;
    std::function<void()>          fullyCallBack_;
    std::function<u32(u64)> getRemoteRankCallback_;
};

} // namespace Hccl

#endif // MIRROR_TASK_MANAGER_LITE_H
