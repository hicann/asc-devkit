/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_COMMUNICATOR_H
#define SIM_COMMUNICATOR_H

#include <vector>
#include "hccl_sim_pub.h"
#include "topo_model.h"
#include "hccl_common.h"
#include "sim_context_manager.h"
#include "sim_thread_manager.h"
#include "sim_channel.h"
#include "sim_channel_manager.h"

namespace HcclSim {

HcclResult Sim_HcclCommInitClusterInfo(const TopoMeta& topoMeta, uint32_t rank, HcclComm* comm);

class SimCommunicator {
public:
    explicit SimCommunicator() = default;
    ~SimCommunicator() = default;

public:
    HcclResult Init(const char* clusterInfo, uint32_t rank);
    HcclResult Init(const TopoMeta& topoMeta, uint32_t rank);
    uint32_t GetRankId();
    uint32_t GetRankSize();
    std::string GetIdentifier();
    HcclResult GetHcclBuffer(void** buffer, uint64_t* size);
    HcclResult ChannelCommGetHcclBuffer(ChannelHandle channel, void** buffer, uint64_t* size);
    HcclResult ChannelCommCreate(
        const std::string& commId, const std::string& tag, CommEngine engine, const HcclChannelDesc* channelDescList,
        uint32_t listNum, ChannelHandle* channelList);

private:
    HcclResult GetFileRealPath(const char* rankTable, std::string& realFilePath);
    HcclResult ParseRankTable(const char* clusterInfo);

    HcclResult GetDefaultCommConfig(HcclCommConfig& commConfig, const std::string& commName) const;
    HcclResult SetIndependentOpConfig(const HcclCommConfig& commConfig);

public:
    std::unique_ptr<TopoModel> topoModel_;
    std::unique_ptr<SimContextMgr> contextManager_{nullptr};
    std::unique_ptr<SimThreadMgr> independentOpThreadMgr_{nullptr};
    std::unique_ptr<SimChannelMgr> channelMgr_{nullptr};

private:
    // config内容
    int32_t commEngine_ = -1;
    uint32_t threadNum_ = 0;
    uint32_t notifyNumPerThread_ = 0;
    uint64_t cclBufferSize_;
    std::string commId_;

    uint32_t curRank_{0};
    uint32_t rankSize_{0};
    std::string identifier_{""};
}; // SimCommunicator

}; // namespace HcclSim

#endif // SIM_COMMUNICATOR_H
