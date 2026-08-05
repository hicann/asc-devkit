/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_CHANNEL_H
#define SIM_CHANNEL_H

#include <memory>
#include "hccl_res.h"
#include "hccl_sim_pub.h"
#include "sim_notify.h"
#include "sim_task.h"

namespace HcclSim {

class SimChannel {
public:
    SimChannel(
        const std::string& commId, const std::string& tag, CommEngine engine, CommProtocol protocol, uint32_t locRankId,
        uint32_t rmtRankId, uint32_t notifyNum, uint32_t channelIdx);
    ~SimChannel();

    HcclResult Init();

    std::string ToString() const;

    const std::string& GetCommId() const { return commId_; };
    const std::string& GetTag() const { return tag_; };
    CommEngine GetEngine() const { return engine_; };
    CommProtocol GetProtocol() const { return protocol_; };
    LinkProtoStub GetLinkType() const;
    uint32_t GetLocRankId() const { return locRankId_; };
    uint32_t GetRmtRankId() const { return rmtRankId_; };
    MemBlock GetLocMem() const { return locMem_; };
    MemBlock GetRmtMem() const { return rmtMem_; };
    uint32_t GetLocNotifyIdByIndex(uint32_t notifyIdx) const;
    uint32_t GetRmtNotifyIdByIndex(uint32_t notifyIdx) const;
    const std::vector<SimNotify*>& GetLocNotifys() const { return locNotifys_; };
    const std::vector<SimNotify*>& GetRmtNotifys() const { return rmtNotifys_; };
    uint32_t GetChannelIdx() const { return channelIdx_; };

    bool IsReady() const { return isReady_; };
    HcclResult ResExchange(std::shared_ptr<SimChannel> reverseChannel);

private:
    std::string commId_{""};
    std::string tag_{""};
    CommEngine engine_{CommEngine::COMM_ENGINE_RESERVED};
    CommProtocol protocol_{CommProtocol::COMM_PROTOCOL_RESERVED};

    uint32_t locRankId_;
    uint32_t rmtRankId_;
    MemBlock locMem_; // local CCL Buffer
    MemBlock rmtMem_; // remote CCL Buffer
    uint32_t notifyNum_{0};
    std::vector<SimNotify*> locNotifys_;
    std::vector<SimNotify*> rmtNotifys_;

    bool isReady_{false};
    uint32_t channelIdx_{0};
};

} // namespace HcclSim
#endif // SIM_MEM_LAYOUT_H
