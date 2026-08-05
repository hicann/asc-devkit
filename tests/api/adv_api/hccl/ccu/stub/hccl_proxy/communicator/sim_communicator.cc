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
#include "sim_world.h"

using namespace std;

namespace HcclSim {

HcclResult Sim_HcclCommInitClusterInfo(const TopoMeta& topoMeta, uint32_t rank, HcclComm* comm)
{
    SimCommunicator* communicator = new SimCommunicator();
    CHK_RET(communicator->Init(topoMeta, rank));
    *comm = static_cast<HcclComm>(communicator);
    return HCCL_SUCCESS;
}

HcclResult SimCommunicator::Init(const char* clusterInfo, uint32_t rank)
{
    HCCL_ERROR("[SimCommunicator::%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult SimCommunicator::Init(const TopoMeta& topoMeta, uint32_t rank)
{
    curRank_ = rank;
    // 构造topo模型
    topoModel_ = make_unique<TopoModel>(topoMeta);
    HCCL_DEBUG("[SimCommunicator::%s] rankSize[%u], ", __func__, topoModel_->GetRankSize());

    // 获取默认commConfig
    HcclCommConfig comConfig;
    CHK_PRT(GetDefaultCommConfig(comConfig, "hccl_world_group"));
    identifier_ = comConfig.hcclCommName;
    CHK_PRT(SetIndependentOpConfig(comConfig));

    // manager初始化
    contextManager_ = std::make_unique<SimContextMgr>();
    channelMgr_ = std::make_unique<SimChannelMgr>(commId_, curRank_);

    return HCCL_SUCCESS;
}

HcclResult SimCommunicator::SetIndependentOpConfig(const HcclCommConfig& commConfig)
{
    commId_ = commConfig.hcclCommName;
    HCCL_INFO(
        "[%s] commEngine[%d], threadNum[%u], notifyNumPerThread[%u], commId[%s]", __func__, commEngine_, threadNum_,
        notifyNumPerThread_, commId_.c_str());
    if (!independentOpThreadMgr_) {
        independentOpThreadMgr_ = std::make_unique<SimThreadMgr>(commId_, curRank_);
    }
    return HCCL_SUCCESS;
}

HcclResult SimCommunicator::GetDefaultCommConfig(HcclCommConfig& commConfig, const std::string& commName) const
{
    commConfig.hcclBufferSize = 1024;    // GetExternalInputCCLBuffSize();
    commConfig.hcclDeterministic = 1024; // GetExternalInputHcclDeterministicV2();
    auto ret = strncpy_s(commConfig.hcclCommName, ROOTINFO_INDENTIFIER_MAX_LENGTH, commName.c_str(), commName.size());
    if (ret != EOK) {
        HCCL_ERROR("[%s] str copy fail. return %d", __func__, ret);
        return HCCL_E_INTERNAL;
    }
    commConfig.hcclOpExpansionMode = 0;
    commConfig.hcclRdmaTrafficClass = HCCL_COMM_TRAFFIC_CLASS_CONFIG_NOT_SET;
    commConfig.hcclRdmaServiceLevel = HCCL_COMM_SERVICE_LEVEL_CONFIG_NOT_SET;
    commConfig.hcclWorldRankID = 0;
    commConfig.hcclJobID = 0;
    return HCCL_SUCCESS;
}

uint32_t SimCommunicator::GetRankId() { return curRank_; }

uint32_t SimCommunicator::GetRankSize()
{
    if (topoModel_ == nullptr) {
        return 0;
    }
    return topoModel_->GetRankSize();
}

std::string SimCommunicator::GetIdentifier() { return identifier_; }

HcclResult SimCommunicator::GetHcclBuffer(void** buffer, uint64_t* size)
{
    CHK_PTR_NULL(buffer);
    CHK_PTR_NULL(size);
    SimNpu& npu = SimWorld::Global()->GetSimNpuByRankId(curRank_);
    MemBlock memBlock = npu.GetMemBlock(BufferType::CCL);
    *buffer = reinterpret_cast<void*>(memBlock.startAddr);
    *size = memBlock.size;
    return HCCL_SUCCESS;
}

HcclResult SimCommunicator::ChannelCommCreate(
    const std::string& commId, const std::string& tag, CommEngine engine, const HcclChannelDesc* channelDescList,
    uint32_t listNum, ChannelHandle* channelList)
{
    return channelMgr_->ChannelCommCreate(commId, tag, engine, channelDescList, listNum, channelList);
}

HcclResult SimCommunicator::ChannelCommGetHcclBuffer(ChannelHandle channel, void** buffer, uint64_t* size)
{
    CHK_PTR_NULL(buffer);
    CHK_PTR_NULL(size);
    auto* transport = reinterpret_cast<SimChannel*>(channel);
    CHK_PTR_NULL(transport);
    MemBlock memBlock = transport->GetRmtMem();
    *buffer = reinterpret_cast<void*>(memBlock.startAddr);
    *size = memBlock.size;
    return HCCL_SUCCESS;
}

}; // namespace HcclSim
