/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "sim_channel_exchange_handler.h"
#include "log.h"

using namespace std;

namespace HcclSim {

SimChannelExchangeHandler& SimChannelExchangeHandler::GetInstance()
{
    static SimChannelExchangeHandler instance;
    return instance;
}

std::string SimChannelExchangeHandler::GetExchangeKey(std::shared_ptr<SimChannel> channel)
{
    return channel->GetCommId() + ":" + channel->GetTag() + ":" + to_string(channel->GetEngine()) + ":" +
           to_string(channel->GetProtocol());
}

void SimChannelExchangeHandler::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    channelMap_.clear();
}

void SimChannelExchangeHandler::PutChannel(std::shared_ptr<SimChannel> channel)
{
    string key = SimChannelExchangeHandler::GetExchangeKey(channel);
    std::lock_guard<std::mutex> lock(mutex_);
    channelMap_[key][channel->GetLocRankId()][channel->GetRmtRankId()][channel->GetChannelIdx()] = channel;
}

std::shared_ptr<SimChannel> SimChannelExchangeHandler::GetChannel(
    const std::string& key, uint32_t srcRank, uint32_t dstRank, uint32_t channelIdx)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return channelMap_[key][srcRank][dstRank][channelIdx];
}

} // namespace HcclSim
