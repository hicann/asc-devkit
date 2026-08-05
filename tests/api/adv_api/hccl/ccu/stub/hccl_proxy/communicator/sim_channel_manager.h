/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_CHANNEL_MANAGER_H
#define SIM_CHANNEL_MANAGER_H

#include <memory>
#include <unordered_map>
#include "hccl_sim_pub.h"
#include "sim_channel.h"

namespace HcclSim {

class SimChannelMgr {
public:
    static std::string GetChannelKey(std::shared_ptr<SimChannel> channel);

    SimChannelMgr(std::string commId, uint32_t curRank) : commId_(commId), curRank_(curRank) {};
    ~SimChannelMgr() = default;

    HcclResult ChannelCommCreate(
        const std::string& commId, const std::string& tag, CommEngine engine, const HcclChannelDesc* channelDescList,
        uint32_t listNum, ChannelHandle* channelList);

private:
    std::string commId_;
    uint32_t curRank_;

    std::unordered_map<std::string, uint32_t> channelIdxMap_;
};

}; // namespace HcclSim
#endif // SIM_CHANNEL_MANAGER_H
