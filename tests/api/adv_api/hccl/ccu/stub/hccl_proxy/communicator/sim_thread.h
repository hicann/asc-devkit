/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_THREAD_H
#define SIM_THREAD_H

#include "hccl_sim_pub.h"
#include "topo_model.h"
#include "hccl_common.h"
#include "sim_stream.h"
#include "sim_notify.h"

namespace HcclSim {
class SimHcclThread {
public:
    SimHcclThread(rtStream_t rtStream, u32 notifyNum, const NotifyLoadType notifyLoadType);
    SimHcclThread(StreamType streamType, u32 notifyNum, const NotifyLoadType notifyLoadType);
    ~SimHcclThread();

    HcclResult Init();

    std::string ToString() const;

    void SetCurRank(uint32_t rankId) { curRank_ = rankId; }
    uint32_t GetCurRank() const { return curRank_; };
    void SetCtxIndex(uint32_t index) { ctxIndex_ = index; }
    uint32_t GetNotifyNum() const { return notifyNum_; }
    uint32_t GetNotifyIdByIndex(uint32_t notifyIndex) const;
    SimStream* GetStream() const { return stream_; }

private:
    uint32_t curRank_;
    uint32_t ctxIndex_;

    StreamType streamType_ = StreamType::STREAM_TYPE_RESERVED;
    SimStream* stream_ = nullptr;

    NotifyLoadType notifyLoadType_ = NotifyLoadType::HOST_NOTIFY;
    uint32_t notifyNum_ = 0;
    std::vector<SimNotify*> notifys_;
}; // class SimHcclThread

}; // namespace HcclSim
#endif // SIM_COMMUNICATOR_H
