/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_PROFILING_REPORTER_LITE_H
#define HCCL_PROFILING_REPORTER_LITE_H
#include "profiling_handler_lite.h"
#include "mirror_task_manager_lite.h"
#include "circular_queue.h"

namespace Hccl {
class ProfilingReporterLite {
public:
    explicit ProfilingReporterLite(MirrorTaskManagerLite *mirrorTaskMgrLite, ProfilingHandlerLite *profilingHandlerLite, bool isIndop = false);
    virtual ~ProfilingReporterLite();
    HcclResult Init();
    void ReportAllTasks();
    void UpdateProfStat() const;

private:
    void ReportAllTasksLog();
    void UpdateAllLastPos();
    MirrorTaskManagerLite                                                         *mirrorTaskMgrLite_{nullptr};
    ProfilingHandlerLite                                                      *profilingHandlerLite_{nullptr};
    std::map<u32, std::shared_ptr<Queue<std::unique_ptr<TaskInfo>>::Iterator>> lastPoses_{};
    bool initializedFlag_{false};
};
} // namespace Hccl

#endif // HCCL_PROFILING_REPORTER_LITE_H
