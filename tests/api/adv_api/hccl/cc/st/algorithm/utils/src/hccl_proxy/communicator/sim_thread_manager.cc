/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "sim_communicator.h"

using namespace std;

namespace HcclSim {

SimThreadMgr::SimThreadMgr(std::string commId, u32 curRank) : commId_(commId), curRank_(curRank) {}

HcclResult SimThreadMgr::CommEngineToNotifyLoadType(CommEngine engine, NotifyLoadType& type)
{
    switch (engine) {
        case 0: // COMM_ENGINE_CPU
        case 1: // COMM_ENGINE_CPU_TS
            type = NotifyLoadType::HOST_NOTIFY;
            break;
        case COMM_ENGINE_AICPU:
        case COMM_ENGINE_AICPU_TS:
            type = NotifyLoadType::DEVICE_NOTIFY;
            break;
        case COMM_ENGINE_CCU:
            type = NotifyLoadType::HOST_NOTIFY;
            break;
        default:
            HCCL_ERROR("[ThreadMgr] Unknown comm engine type: %d", engine);
            return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::HcclThreadAcquireWithStream(
    CommEngine engine, rtStream_t stream, uint32_t notifyNum, ThreadHandle* thread)
{
    NotifyLoadType notifyLoadType;
    CHK_RET(CommEngineToNotifyLoadType(engine, notifyLoadType));

    std::lock_guard<std::mutex> lock(mainThreadMutex_);
    std::unique_ptr<SimHcclThread> handle = std::make_unique<SimHcclThread>(stream, notifyNum, notifyLoadType);
    handle->SetCurRank(curRank_);
    handle->SetCtxIndex(0);
    CHK_RET(handle->Init());
    mainThread_.emplace(stream, std::move(handle));

    *thread = reinterpret_cast<ThreadHandle>(mainThread_[stream].get());
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::HcclThreadAcquire(
    CommEngine engine, uint32_t threadNum, uint32_t notifyNumPerThread, ThreadHandle* threads)
{
    std::lock_guard<std::mutex> lock(threadMutex_);
    for (uint32_t i = 0; i < threadNum; ++i) {
        auto simThread = std::make_shared<SimHcclThread>(
            StreamType::STREAM_TYPE_RESERVED, notifyNumPerThread, NotifyLoadType::HOST_NOTIFY);
        simThread->SetCurRank(curRank_);
        simThread->SetCtxIndex(i + 1);
        CHK_RET(simThread->Init());
        threads_.push_back(simThread);

        threads[i] = reinterpret_cast<ThreadHandle>(simThread.get());
    }
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::CommGetNotifyNumInThread(ThreadHandle thread, uint32_t* notifyNum)
{
    SimHcclThread* hcclThread = reinterpret_cast<SimHcclThread*>(thread);
    CHK_PTR_NULL(hcclThread);
    *notifyNum = hcclThread->GetNotifyNum();
    return HCCL_SUCCESS;
}

}; // namespace HcclSim
