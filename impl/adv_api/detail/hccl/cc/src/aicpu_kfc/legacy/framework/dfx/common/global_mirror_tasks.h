/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef GLOBAL_MIRROR_TASKS_H
#define GLOBAL_MIRROR_TASKS_H

#include <map>
#include <memory>
#include <array>
#include "dfx_common.h"
#include "exception_util.h"
#include "internal_exception.h"
#include <array>

namespace Hccl {

class GlobalMirrorTasks {
public:
    static GlobalMirrorTasks& Instance();

    u32 DevSize() const;

public:
    TaskInfoQueue* GetQueue(u32 devId, u32 streamId) const;
    TaskInfo* GetTaskInfo(u32 devId, u32 streamId, u32 taskId) const;

    TaskInfoQueue& CreateQueue(u32 devId, u32 streamId, QueueType type);
    void DestroyQueue(u32 devId, u32 streamId);

    TaskInfoQueueMap::iterator Begin(u32 devId);
    TaskInfoQueueMap::iterator End(u32 devId);

    HcclResult FindTaskInfo(u32 devId, u32 streamId, u32 taskId, TaskInfo*& curTask) const;

private:
    static GlobalMirrorTasks ins_;
    std::array<TaskInfoQueueMap, DEVICE_MAX_NUM> taskMaps_;

private:
    GlobalMirrorTasks();
    ~GlobalMirrorTasks();
};

} // namespace Hccl

#endif // GLOBAL_MIRROR_TASKS_H
