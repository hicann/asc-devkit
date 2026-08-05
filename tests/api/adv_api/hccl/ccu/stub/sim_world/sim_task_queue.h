/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_TASK_QUEUE_H
#define SIM_TASK_QUEUE_H
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include "sim_common.h"
#include "sim_task.h"
#include "sim_stream.h"

namespace HcclSim {
// taskQueue[0]: master queue; taskQueue[1:]: slave queues
using SingleTaskQueue = std::vector<std::vector<std::shared_ptr<TaskStub>>>;

using AllRankTaskQueues = std::map<PodId, std::map<SerId, std::map<PhyId, SingleTaskQueue>>>;

class SimTaskQueue {
public:
    static SimTaskQueue* Global();
    void Clear();
    void AppendTask(NpuPos npuPos, SimStream* stream, std::shared_ptr<TaskStub> task);

    AllRankTaskQueues& GetAllRankTaskQueues();

private:
    std::mutex mutex_;
    AllRankTaskQueues allRankTask_;
};
} // namespace HcclSim
#endif
