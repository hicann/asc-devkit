/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "channel.h"
#include <algorithm>
#include <cstring>

namespace mc2_ops_hccl {
namespace {
constexpr u32 MULTI_JETTY_CHANNEL_NUM = 4U;
constexpr char MULTI_JETTY_ALGORITHM_NAME[] = "InsAllGatherParallelMesh1DNHRMultiJetty";

CommProtocol SelectLocalProtocol(const OpParam& param)
{
    if (param.engine == CommEngine::COMM_ENGINE_CPU || param.engine == CommEngine::COMM_ENGINE_CPU_TS) {
        return CommProtocol::COMM_PROTOCOL_ROCE;
    }
    return CommProtocol::COMM_PROTOCOL_UBC_CTP;
}

HcclResult AddChannelRequest(
    const OpParam& param, u32 remoteRank, std::vector<HcclChannelDesc>& channels, u32 channelRepeatNum = 1)
{
    const CommProtocol protocol = SelectLocalProtocol(param);
    for (u32 idx = 0; idx < channelRepeatNum; ++idx) {
        HcclChannelDesc channelDesc;
        CHK_RET(HcclChannelDescInit(&channelDesc, 1));
        channelDesc.remoteRank = remoteRank;
        channelDesc.channelProtocol = protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channelDesc.localEndpoint.protocol = protocol;
        channelDesc.remoteEndpoint.protocol = protocol;
        channelDesc.localEndpoint.loc.locType = EndpointLocType::ENDPOINT_LOC_TYPE_DEVICE;
        channelDesc.remoteEndpoint.loc.locType = EndpointLocType::ENDPOINT_LOC_TYPE_DEVICE;
        channels.emplace_back(channelDesc);
    }
    return HCCL_SUCCESS;
}

HcclResult BuildFullyConnectedRequests(
    const OpParam& param, u32 myRank, const std::vector<u32>& subCommRanks, std::vector<HcclChannelDesc>& channels)
{
    channels.clear();
    for (u32 rank : subCommRanks) {
        if (rank == myRank) {
            continue;
        }
        CHK_RET(AddChannelRequest(param, rank, channels));
    }
    return HCCL_SUCCESS;
}
} // namespace

HcclResult GetProtocolByEngine(const OpParam& param, std::vector<CommProtocol>& protocols)
{
    protocols.clear();
    protocols.emplace_back(SelectLocalProtocol(param));
    return HCCL_SUCCESS;
}

HcclResult CreateChannelRequestByRankId(
    HcclComm comm, const OpParam& param, u32 myRank, u32 remoteRank, std::vector<HcclChannelDesc>& channels,
    u32 channelRepeatNum)
{
    (void)comm;
    (void)myRank;
    return AddChannelRequest(param, remoteRank, channels, channelRepeatNum);
}

HcclResult CalcChannelRequestMesh1D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    (void)comm;
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(subcommInfo.empty(), HCCL_ERROR("[CalcChannelRequestMesh1D] empty subcommInfo"), HCCL_E_PARA);
    return BuildFullyConnectedRequests(param, topoInfo->userRank, subcommInfo[COMM_LEVEL0], channels);
}

HcclResult CalcChannelRequestMesh1DLevel0(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1D(comm, param, topoInfo, subcommInfo, channels);
}

HcclResult CalcChannelRequestMesh1DLevel1(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1D(comm, param, topoInfo, subcommInfo, channels);
}

HcclResult CalcChannelRequestNhr(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1D(comm, param, topoInfo, subcommInfo, channels);
}

HcclResult CalcChannelRequestNhrMultiJetty(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels, bool isIsolation)
{
    (void)isIsolation;
    if (std::strcmp(param.algName, MULTI_JETTY_ALGORITHM_NAME) != 0) {
        return CalcChannelRequestNhr(comm, param, topoInfo, subcommInfo, channels);
    }

    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(subcommInfo.empty(), HCCL_ERROR("[CalcChannelRequestNhrMultiJetty] empty subcommInfo"), HCCL_E_PARA);
    channels.clear();
    for (u32 rank : subcommInfo[COMM_LEVEL0]) {
        if (rank != topoInfo->userRank) {
            CHK_RET(AddChannelRequest(param, rank, channels, MULTI_JETTY_CHANNEL_NUM));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult CalcChannelRequestMesh2D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1D(comm, param, topoInfo, subcommInfo, channels);
}

HcclResult CalcChannelRequestMesh1DWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
    (void)comm;
    (void)priorityTopo;
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        subcommInfo.empty(), HCCL_ERROR("[CalcChannelRequestMesh1DWithPriorityTopo] empty subcommInfo"), HCCL_E_PARA);
    return BuildFullyConnectedRequests(param, topoInfo->userRank, subcommInfo[COMM_LEVEL0], channels);
}

HcclResult CalcChannelRequestNHRWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
    return CalcChannelRequestMesh1DWithPriorityTopo(comm, param, topoInfo, subcommInfo, channels, priorityTopo);
}

HcclResult ProcessLinksForChannelMutiJetty(
    HcclComm comm, CommProtocol expectedProtocol, const std::vector<CommLink>& linkList, u32 myRank, u32 remoteRank,
    uint32_t netLayer, std::vector<HcclChannelDesc>& channels, bool expectMesh, bool isIsolation)
{
    (void)comm;
    (void)expectedProtocol;
    (void)linkList;
    (void)myRank;
    (void)netLayer;
    (void)expectMesh;
    (void)isIsolation;
    OpParam param{};
    param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    return AddChannelRequest(param, remoteRank, channels, MULTI_JETTY_CHANNEL_NUM);
}

HcclResult CalcChannelRequestMeshClosMultiJetty(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels, bool isIsolation,
    bool expectMesh)
{
    (void)comm;
    (void)isIsolation;
    (void)expectMesh;
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        subcommInfo.empty(), HCCL_ERROR("[CalcChannelRequestMeshClosMultiJetty][UT] empty subcommInfo"), HCCL_E_PARA);
    channels.clear();
    for (u32 rank : subcommInfo[COMM_LEVEL0]) {
        if (rank != topoInfo->userRank) {
            CHK_RET(AddChannelRequest(param, rank, channels, MULTI_JETTY_CHANNEL_NUM));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult GetTopoTypeByLink(HcclComm comm, uint32_t netLayer, const CommLink& link, CommTopo& topoType)
{
    (void)comm;
    (void)netLayer;
    (void)link;
    topoType = CommTopo::COMM_TOPO_1DMESH;
    return HCCL_SUCCESS;
}

HcclResult ProcessLinksForChannel(
    HcclComm comm, u32 myRank, u32 rank, std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
    (void)comm;
    (void)priorityTopo;
    OpParam param;
    param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    return CreateChannelRequestByRankId(comm, param, myRank, rank, channels);
}

HcclResult CalcLevel0ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    (void)algType;
    CHK_PTR_NULL(topoInfo);
    channels.clear();
    for (u32 rank = 0; rank < algHierarchyInfo.infos[COMM_LEVEL0].localRankSize; ++rank) {
        if (rank == topoInfo->userRank) {
            continue;
        }
        CHK_RET(AddChannelRequest(param, rank, channels));
    }
    return HCCL_SUCCESS;
}

HcclResult CalcLevel1ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    (void)algHierarchyInfo;
    (void)algType;
    channels.clear();
    return CreateChannelRequestByRankId(
        nullptr, param, topoInfo == nullptr ? 0 : topoInfo->userRank, topoInfo == nullptr ? 0 : topoInfo->userRank,
        channels);
}

HcclResult CalcLevel2ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    return CalcLevel1ChannelRequest(param, topoInfo, algHierarchyInfo, algType, channels);
}
} // namespace mc2_ops_hccl
