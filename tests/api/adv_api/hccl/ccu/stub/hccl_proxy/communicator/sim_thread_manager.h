/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_THREAD_MANAGER_H
#define SIM_THREAD_MANAGER_H

#include "hccl_sim_pub.h"
#include "topo_model.h"
#include "hccl_common.h"
#include "sim_thread.h"
#include <mutex>

namespace HcclSim {

class SimThreadMgr {
public:
    SimThreadMgr(std::string commId, u32 curRank);
    ~SimThreadMgr() = default;
    HcclResult HcclThreadAcquire(
        CommEngine engine, uint32_t threadNum, uint32_t notifyNumPerThread, ThreadHandle* threads);
    HcclResult HcclThreadAcquireWithStream(
        CommEngine engine, rtStream_t stream, uint32_t notifyNum, ThreadHandle* thread);
    HcclResult CommGetNotifyNumInThread(ThreadHandle thread, uint32_t* notifyNum);

private:
    HcclResult CommEngineToNotifyLoadType(CommEngine engine, NotifyLoadType& type);

    std::string commId_;
    u32 curRank_;

    std::mutex threadMutex_;
    std::vector<std::shared_ptr<SimHcclThread>> threads_;

    std::mutex mainThreadMutex_;
    std::map<rtStream_t, std::unique_ptr<SimHcclThread>> mainThread_;
}; // class SimThreadMgr

}; // namespace HcclSim
#endif // SIM_COMMUNICATOR_H
