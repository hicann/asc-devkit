/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_CHANNEL_EXCHANGE_HANDLER_H
#define SIM_CHANNEL_EXCHANGE_HANDLER_H

#include <memory>
#include <mutex>
#include <map>
#include <unordered_map>
#include "hccl_sim_pub.h"
#include "sim_channel.h"

namespace HcclSim {

class SimChannelExchangeHandler {
public:
    static SimChannelExchangeHandler& GetInstance();
    static std::string GetExchangeKey(std::shared_ptr<SimChannel> channel);

    void Clear();
    void PutChannel(std::shared_ptr<SimChannel> channel);
    std::shared_ptr<SimChannel> GetChannel(
        const std::string& key, uint32_t srcRank, uint32_t dstRank, uint32_t channelIdx);

private:
    SimChannelExchangeHandler() = default;
    ~SimChannelExchangeHandler() = default;

private:
    std::mutex mutex_;
    // <Exchange Key, Src rank, Dst rank, ChannelIdx, SimChannel>
    std::unordered_map<
        std::string, std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, std::shared_ptr<SimChannel>>>>>
        channelMap_;
};

} // namespace HcclSim
#endif
