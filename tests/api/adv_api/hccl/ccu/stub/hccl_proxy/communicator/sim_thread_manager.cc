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
        default:
            HCCL_ERROR("[ThreadMgr] Unknown comm engine type: %d", engine);
            return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::HcclThreadAcquireWithStream(
    CommEngine engine, rtStream_t stream, uint32_t notifyNum, ThreadHandle* thread)
{
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::HcclThreadAcquire(
    CommEngine engine, uint32_t threadNum, uint32_t notifyNumPerThread, ThreadHandle* threads)
{
    return HCCL_SUCCESS;
}

HcclResult SimThreadMgr::CommGetNotifyNumInThread(ThreadHandle thread, uint32_t* notifyNum) { return HCCL_SUCCESS; }

}; // namespace HcclSim
