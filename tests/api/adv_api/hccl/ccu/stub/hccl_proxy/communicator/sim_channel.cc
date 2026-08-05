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
#include "log.h"
#include "sim_channel.h"
#include "sim_world.h"
#include "sim_npu.h"
#include "exception_util.h"
#include "invalid_params_exception.h"

using namespace std;

namespace HcclSim {

SimChannel::SimChannel(
    const std::string& commId, const std::string& tag, CommEngine engine, CommProtocol protocol, uint32_t locRankId,
    uint32_t rmtRankId, uint32_t notifyNum, uint32_t channelIdx)
    : commId_(commId),
      tag_(tag),
      engine_(engine),
      protocol_(protocol),
      locRankId_(locRankId),
      rmtRankId_(rmtRankId),
      notifyNum_(notifyNum),
      channelIdx_(channelIdx)
{}

SimChannel::~SimChannel()
{
    SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRankId_);
    for (auto* notify : locNotifys_) {
        npu.ReleaseNotify(notify);
    }
    locNotifys_.clear();
}

HcclResult SimChannel::Init()
{
    SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRankId_);

    locMem_ = npu.GetMemBlock(BufferType::CCL); // 参考当前业务代码，固定用CCL Buffer

    for (uint32_t i = 0; i < notifyNum_; ++i) {
        void* notifyPtr = npu.AllocNotify();
        CHK_PTR_NULL(notifyPtr);
        locNotifys_.push_back(reinterpret_cast<SimNotify*>(notifyPtr));
    }

    return HCCL_SUCCESS;
}

string SimChannel::ToString() const
{
    stringstream ss;
    ss << "Channel" << "{" << to_string(locRankId_) << "->" << to_string(rmtRankId_) << "}";
    return ss.str();
}

LinkProtoStub SimChannel::GetLinkType() const
{
    // 根据业务代码 HcclCommunicator::BuildChannelRequests()
    return (protocol_ == CommProtocol::COMM_PROTOCOL_ROCE) ? LinkProtoStub::RDMA : LinkProtoStub::SDMA;
}

uint32_t SimChannel::GetLocNotifyIdByIndex(uint32_t notifyIdx) const
{
    if (notifyIdx >= locNotifys_.size()) {
        Hccl::THROW<Hccl::InvalidParamsException>("[SimChannel::%s] notifyIndex[%u] out of bound", __func__, notifyIdx);
    }
    return locNotifys_[notifyIdx]->GetNotifyId();
}

uint32_t SimChannel::GetRmtNotifyIdByIndex(uint32_t notifyIdx) const
{
    if (notifyIdx >= rmtNotifys_.size()) {
        Hccl::THROW<Hccl::InvalidParamsException>("[SimChannel::%s] notifyIndex[%u] out of bound", __func__, notifyIdx);
    }
    return rmtNotifys_[notifyIdx]->GetNotifyId();
}

HcclResult SimChannel::ResExchange(std::shared_ptr<SimChannel> reverseChannel)
{
    CHK_PTR_NULL(reverseChannel);

    CHK_PRT_RET(
        this->GetCommId() != reverseChannel->GetCommId(), HCCL_ERROR("[SimChannel::%s] commId not match", __func__),
        HCCL_E_PARA);
    CHK_PRT_RET(
        this->GetTag() != reverseChannel->GetTag(), HCCL_ERROR("[SimChannel::%s] tag not match", __func__),
        HCCL_E_PARA);
    CHK_PRT_RET(
        this->GetEngine() != reverseChannel->GetEngine(), HCCL_ERROR("[SimChannel::%s] engine not match", __func__),
        HCCL_E_PARA);
    CHK_PRT_RET(
        this->GetProtocol() != reverseChannel->GetProtocol(),
        HCCL_ERROR("[SimChannel::%s] protocol not match", __func__), HCCL_E_PARA);

    CHK_PRT_RET(
        this->GetLocRankId() != reverseChannel->GetRmtRankId(),
        HCCL_ERROR("[SimChannel::%s] rankId not match", __func__), HCCL_E_PARA);
    CHK_PRT_RET(
        this->GetRmtRankId() != reverseChannel->GetLocRankId(),
        HCCL_ERROR("[SimChannel::%s] rankId not match", __func__), HCCL_E_PARA);

    rmtMem_ = reverseChannel->GetLocMem();

    for (SimNotify* it : reverseChannel->GetLocNotifys()) {
        rmtNotifys_.push_back(it);
    }

    isReady_ = true;
    return HCCL_SUCCESS;
}

} // namespace HcclSim
