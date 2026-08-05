/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <chrono>
#include <thread>
#include "sim_channel_manager.h"
#include "log.h"
#include "sim_channel_exchange_handler.h"

using namespace std;

namespace HcclSim {

constexpr uint32_t WAIT_CHANNEL_READY_TIMEOUT_MS = 20 * 1000;

std::string SimChannelMgr::GetChannelKey(std::shared_ptr<SimChannel> channel)
{
    return channel->GetTag() + ":" + to_string(channel->GetEngine()) + ":" + to_string(channel->GetRmtRankId()) + ":" +
           to_string(channel->GetProtocol());
}

HcclResult SimChannelMgr::ChannelCommCreate(
    const std::string& commId, const std::string& tag, CommEngine engine, const HcclChannelDesc* channelDescList,
    uint32_t listNum, ChannelHandle* channelList)
{
    vector<shared_ptr<SimChannel>> tmpChannels{};

    for (uint32_t i = 0; i < listNum; ++i) {
        string channelKey = tag + ":" + to_string(engine) + ":" + to_string(channelDescList[i].remoteRank) + ":" +
                            to_string(channelDescList[i].channelProtocol);
        auto channelIdx = channelIdxMap_[channelKey]++;
        // channel不再支持复用
        shared_ptr<SimChannel> channel = make_shared<SimChannel>(
            commId, tag, engine, channelDescList[i].channelProtocol, curRank_, channelDescList[i].remoteRank,
            channelDescList[i].notifyNum, channelIdx);
        CHK_RET(channel->Init());
        tmpChannels.push_back(channel);
    }

    // 模拟建链
    auto timeout = std::chrono::milliseconds(WAIT_CHANNEL_READY_TIMEOUT_MS);
    auto startTime = std::chrono::steady_clock::now();
    for (auto& channel : tmpChannels) {
        SimChannelExchangeHandler::GetInstance().PutChannel(channel);
    }
    while (true) {
        uint32_t finCount = 0;
        for (auto& channel : tmpChannels) {
            if (channel->IsReady()) {
                // 已完成资源交换，计数
                finCount++;
                continue;
            }
            // 尝试交换资源
            string exchangeKey = SimChannelExchangeHandler::GetExchangeKey(channel);
            auto reverseChannel = SimChannelExchangeHandler::GetInstance().GetChannel(
                exchangeKey, channel->GetRmtRankId(), channel->GetLocRankId(), channel->GetChannelIdx());
            if (reverseChannel != nullptr) {
                CHK_RET(channel->ResExchange(reverseChannel));
            }
        }

        if (finCount == tmpChannels.size()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if ((std::chrono::steady_clock::now() - startTime) >= timeout) {
            HCCL_ERROR("[SimChannelMgr::%s] wait channel ready timeout", __func__);
            return HCCL_E_TIMEOUT;
        }
    }

    for (int i = 0; i < tmpChannels.size(); i++) {
        channelList[i] = reinterpret_cast<ChannelHandle>(tmpChannels[i].get());
    }
    return HCCL_SUCCESS;
}

} // namespace HcclSim
