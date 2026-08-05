/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <sstream>
#include "sim_thread.h"
#include "sim_world.h"
#include "sim_npu.h"
#include "exception_util.h"
#include "invalid_params_exception.h"

using namespace std;

namespace HcclSim {

SimHcclThread::SimHcclThread(rtStream_t rtStream, u32 notifyNum, const NotifyLoadType notifyLoadType)
    : notifyNum_(notifyNum), notifyLoadType_(notifyLoadType)
{
    stream_ = reinterpret_cast<SimStream*>(rtStream);
}

SimHcclThread::SimHcclThread(StreamType streamType, u32 notifyNum, const NotifyLoadType notifyLoadType)
    : streamType_(streamType), notifyNum_(notifyNum), notifyLoadType_(notifyLoadType)
{}

SimHcclThread::~SimHcclThread()
{
    SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank_);
    if (stream_ != nullptr) {
        npu.ReleaseStream(stream_);
    }

    for (auto* notify : notifys_) {
        npu.ReleaseNotify(notify);
    }
    notifys_.clear();
}

HcclResult SimHcclThread::Init()
{
    SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank_);
    if (stream_ == nullptr) {
        void* streamPtr = npu.AllocSlaveStream();
        CHK_PTR_NULL(streamPtr);
        stream_ = reinterpret_cast<SimStream*>(streamPtr);
    }

    for (uint32_t i = 0; i < notifyNum_; ++i) {
        void* notifyPtr = npu.AllocNotify();
        CHK_PTR_NULL(notifyPtr);
        notifys_.push_back(reinterpret_cast<SimNotify*>(notifyPtr));
    }

    return HCCL_SUCCESS;
}

std::string SimHcclThread::ToString() const
{
    stringstream ss;
    ss << "Thread{";
    ss << "Rank[" << to_string(curRank_) << "]" << ", ";
    ss << "Idx[" << to_string(ctxIndex_) << "]";
    ss << "}";
    return ss.str();
}

uint32_t SimHcclThread::GetNotifyIdByIndex(uint32_t notifyIndex) const
{
    if (notifyIndex >= notifys_.size()) {
        Hccl::THROW<Hccl::InvalidParamsException>(
            "[SimHcclThread::%s] notifyIndex[%u] out of bound", __func__, notifyIndex);
    }
    return notifys_[notifyIndex]->GetNotifyId();
}

}; // namespace HcclSim
