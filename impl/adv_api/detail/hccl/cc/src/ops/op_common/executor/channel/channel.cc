/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
#include <vector>
#include <set>
#include <hccl/hccl_types.h>
#include "hccl/base.h"
#include "alg_type.h"
#include "channel_request.h"
#include "topo.h"
#include "topo_host.h"
#include "alg_env_config.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 PORT_IDX = 5U;
constexpr u8 NON_ISOLATED_PORT = 127U;
constexpr size_t MIN_PHY_LINK_COUNT = 8U;
} // namespace

HcclResult CalcLevel0ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    (void)param;
    (void)topoInfo;
    channels.clear();
    SubCommInfo& subCommInfo = algHierarchyInfo.infos[COMM_LEVEL0];
    std::set<u32> connectRanks; // 非通信域rank

    switch (algType.algoLevel0) {
        case AlgTypeLevel0::ALG_LEVEL0_NP_SINGLE_RING:
        case AlgTypeLevel0::ALG_LEVEL0_NP_DOUBLE_RING:
            CHK_RET(CalcRingChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
        case AlgTypeLevel0::ALG_LEVEL0_NP_MESH:
        default:
            CHK_RET(CalcMeshChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
    }

    CommProtocol protocol = CommProtocol::COMM_PROTOCOL_HCCS;
    for (u32 rank : connectRanks) {
        HcclChannelDesc channelDesc;
        CHK_RET(HcclChannelDescInit(&channelDesc, 1));
        CHK_RET(GetUserRankBySubCommRank(rank, COMM_LEVEL0, algHierarchyInfo, channelDesc.remoteRank));
        channelDesc.channelProtocol = protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channels.push_back(channelDesc);
    }
    return HCCL_SUCCESS;
}

HcclResult CalcLevel1ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    (void)param;
    channels.clear();
    SubCommInfo& subCommInfo = algHierarchyInfo.infos[COMM_LEVEL1];
    std::set<u32> connectRanks; // 非通信域rank

    switch (algType.algoLevel1) {
        case AlgTypeLevel1::ALG_LEVEL1_NB:
            CHK_RET(CalcNBChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
        case AlgTypeLevel1::ALG_LEVEL1_NHR:
            CHK_RET(CalcNHRChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
        default:
            CHK_RET(CalcRingChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
    }

    // level1走rdma的几种条件：A2单机A+X开启switch；A2多机；A3开启disableHccs；A3跨超卡数不一致
    bool isA2UsedRdma = topoInfo->deviceType == DevType::DEV_TYPE_910B &&
                        (topoInfo->serverNum > 1 || (topoInfo->serverNum == 1 && topoInfo->isDiffDeviceModule &&
                                                     GetExternalInputIntraRoceSwitch() > 0));
    bool isA3UsedRdma = topoInfo->deviceType == DevType::DEV_TYPE_910_93 &&
                        ((topoInfo->superPodNum > 1 &&
                          (topoInfo->multiSuperPodDiffServerNumMode || topoInfo->multiModuleDiffDeviceNumMode)) ||
                         (topoInfo->superPodNum == 1 && topoInfo->serverNum > 1 && GetExternalInputInterHccsDisable()));
    bool isUsedRdma = isA2UsedRdma || isA3UsedRdma;

    CommProtocol protocol = isUsedRdma ? CommProtocol::COMM_PROTOCOL_ROCE : CommProtocol::COMM_PROTOCOL_HCCS;
    for (u32 rank : connectRanks) {
        HcclChannelDesc channelDesc;
        CHK_RET(HcclChannelDescInit(&channelDesc, 1));
        CHK_RET(GetUserRankBySubCommRank(rank, COMM_LEVEL1, algHierarchyInfo, channelDesc.remoteRank));
        channelDesc.channelProtocol = protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channels.push_back(channelDesc);
    }
    return HCCL_SUCCESS;
}

HcclResult CalcLevel2ChannelRequest(
    const OpParam& param, const TopoInfo* topoInfo, AlgHierarchyInfo& algHierarchyInfo, const AlgType& algType,
    std::vector<HcclChannelDesc>& channels)
{
    (void)param;
    (void)topoInfo;
    channels.clear();
    SubCommInfo& subCommInfo = algHierarchyInfo.infos[COMM_LEVEL2];
    std::set<u32> connectRanks; // 非通信域rank

    switch (algType.algoLevel2) {
        case AlgTypeLevel2::ALG_LEVEL2_NB:
            CHK_RET(CalcNBChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
        case AlgTypeLevel2::ALG_LEVEL2_NHR:
            CHK_RET(CalcNHRChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
        default:
            CHK_RET(CalcRingChannelConnect(
                subCommInfo.localRank, subCommInfo.localRankSize, INVALID_VALUE_RANKID, connectRanks));
            break;
    }

    // level2当前一定走rdma
    CommProtocol protocol = CommProtocol::COMM_PROTOCOL_ROCE;

    for (u32 rank : connectRanks) {
        HcclChannelDesc channelDesc;
        CHK_RET(HcclChannelDescInit(&channelDesc, 1));
        CHK_RET(GetUserRankBySubCommRank(rank, COMM_LEVEL2, algHierarchyInfo, channelDesc.remoteRank));
        channelDesc.channelProtocol = protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channels.push_back(channelDesc);
    }
    return HCCL_SUCCESS;
}

HcclResult GetProtocolByEngine(const OpParam& param, std::vector<CommProtocol>& protocols)
{
    protocols.clear();
    switch (param.engine) {
        case CommEngine::COMM_ENGINE_AICPU:
        case CommEngine::COMM_ENGINE_AICPU_TS:
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_CTP);
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_TP);
            protocols.push_back(CommProtocol::COMM_PROTOCOL_PCIE);
            break;
        case CommEngine::COMM_ENGINE_CCU:
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_CTP);
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_TP);
            break;
        case CommEngine::COMM_ENGINE_CPU:
            // level 1到level n-1使用UB协议，server内建联，最外层使用网卡建联
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_CTP);
            protocols.push_back(CommProtocol::COMM_PROTOCOL_UBC_TP);
            protocols.push_back(CommProtocol::COMM_PROTOCOL_ROCE);
            break;
        case CommEngine::COMM_ENGINE_CPU_TS:
            protocols.push_back(CommProtocol::COMM_PROTOCOL_ROCE);
            break;
        default:
            HCCL_WARNING(
                "[GetProtocolByEngine] Unknown engine[%d], set protocol to RESERVED", static_cast<int>(param.engine));
            break;
    }
    return HCCL_SUCCESS;
}

HcclResult CreateChannelFromLink(
    HcclComm comm, u32 myRank, u32 rank, uint32_t netLayer, u32 idx, const CommLink& link, const std::string& funcName,
    std::vector<HcclChannelDesc>& channels)
{
    (void)comm;
    HcclChannelDesc channelDesc;
    HcclChannelDescInit(&channelDesc, 1);
    channelDesc.remoteRank = rank;
    channelDesc.localEndpoint.protocol = link.srcEndpointDesc.protocol;
    channelDesc.localEndpoint.commAddr = link.srcEndpointDesc.commAddr;
    channelDesc.localEndpoint.loc = link.srcEndpointDesc.loc;
    channelDesc.remoteEndpoint.protocol = link.dstEndpointDesc.protocol;
    channelDesc.remoteEndpoint.commAddr = link.dstEndpointDesc.commAddr;
    channelDesc.remoteEndpoint.loc = link.dstEndpointDesc.loc;
    HCCL_DEBUG(
        "%s local device phyId: %u, remote device phyId: %u.", funcName.c_str(),
        channelDesc.localEndpoint.loc.device.devPhyId, channelDesc.remoteEndpoint.loc.device.devPhyId);
    HCCL_INFO(
        "%s Add channel request between %zu and %zu, netLayerIdx %u, "
        "linkListIdx %u, protocol %zu",
        funcName.c_str(), myRank, channelDesc.remoteRank, netLayer, idx, channelDesc.remoteEndpoint.protocol);
    channelDesc.channelProtocol = link.linkAttr.linkProtocol;
    channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
    channels.push_back(channelDesc);
    return HCCL_SUCCESS;
}

HcclResult ProcessLinkForProtocol(
    HcclComm comm, const std::vector<CommProtocol>& expectedProtocols, const std::vector<CommLink>& linkList,
    u32 myRank, u32 remoteRank, uint32_t netLayer, std::vector<HcclChannelDesc>& channels, bool& protocolFound,
    const std::string& funcName)
{
    protocolFound = false;
    for (auto expectedProtocol : expectedProtocols) {
        for (u32 idx = 0; idx < linkList.size(); idx++) {
            if (linkList[idx].linkAttr.linkProtocol == expectedProtocol) {
                CHK_RET(
                    CreateChannelFromLink(comm, myRank, remoteRank, netLayer, idx, linkList[idx], funcName, channels));
                protocolFound = true;
            }
        }
        if (protocolFound) {
            break;
        }
    }
    return HCCL_SUCCESS;
}

HcclResult CalcChannelRequestMesh1D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
#ifndef AICPU_COMPILE
    (void)param;
    channels.clear();
    auto it = std::find(subcommInfo[COMM_LEVEL0].begin(), subcommInfo[COMM_LEVEL0].end(), topoInfo->userRank);
    CHK_PRT_RET(
        (it == subcommInfo[COMM_LEVEL0].end()),
        HCCL_ERROR("[CollAlgFactory] [channel] Rank [%d] is not in commInfo.", topoInfo->userRank),
        HcclResult::HCCL_E_PARA);

    u32 myRank = topoInfo->userRank;
    std::vector<CommProtocol> expectedProtocols;
    CHK_RET(GetProtocolByEngine(param, expectedProtocols));

    for (u32 rank : subcommInfo[COMM_LEVEL0]) {
        if (rank == topoInfo->userRank) {
            continue;
        }
        size_t channelCountBefore = channels.size();
        uint32_t* netLayers;
        uint32_t netLayerNum;
        CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
        std::vector<uint32_t> netLayersVector(netLayers, netLayers + netLayerNum);

        for (auto netLayer : netLayersVector) {
            CommLink* linkList = nullptr;
            u32 listSize;
            CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, rank, &linkList, &listSize));

            if (listSize == 0) {
                continue;
            }

            std::vector<CommLink> links(linkList, linkList + listSize);
            bool protocolFound = false;
            CHK_RET(ProcessLinkForProtocol(
                comm, expectedProtocols, links, myRank, rank, netLayer, channels, protocolFound,
                std::string("[CalcChannelRequestMesh1D]")));

            if (channels.size() > channelCountBefore) {
                break;
            }
        }

        CHK_PRT_RET(
            channels.size() == channelCountBefore,
            HCCL_ERROR(
                "[CalcChannelRequestMesh1D] Failed to create channel between myRank=%u and rank=%u, there is no link.",
                myRank, rank),
            HcclResult::HCCL_E_INTERNAL);
    }
#endif
    return HCCL_SUCCESS;
}

static HcclResult CheckNetLayerExists(HcclComm comm, u32 netLayer, const std::string& tag, bool linkRequired)
{
#ifndef AICPU_COMPILE
    uint32_t* netLayers = nullptr;
    uint32_t netLayerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
    std::vector<uint32_t> netLayersVector(netLayers, netLayers + netLayerNum);
    bool netLayerValid = false;
    for (auto layer : netLayersVector) {
        if (layer == netLayer) {
            netLayerValid = true;
            break;
        }
    }
    if (!netLayerValid) {
        CHK_PRT_RET(
            linkRequired, HCCL_ERROR("[%s] netLayer[%u] does not exist in rankGraph.", tag.c_str(), netLayer),
            HcclResult::HCCL_E_INTERNAL);
        HCCL_WARNING("[%s] netLayer[%u] does not exist in rankGraph, skip.", tag.c_str(), netLayer);
        return HCCL_SUCCESS;
    }
#else
    (void)comm;
    (void)netLayer;
    (void)tag;
    (void)linkRequired;
#endif
    return HCCL_SUCCESS;
}

static HcclResult CalcChannelRequestMesh1DByLevel(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels, u32 netLayer,
    const std::string& tag, bool linkRequired)
{
#ifndef AICPU_COMPILE
    channels.clear();

    CHK_RET(CheckNetLayerExists(comm, netLayer, tag, linkRequired));

    auto it = std::find(subcommInfo[COMM_LEVEL0].begin(), subcommInfo[COMM_LEVEL0].end(), topoInfo->userRank);
    CHK_PRT_RET(
        (it == subcommInfo[COMM_LEVEL0].end()),
        HCCL_ERROR("[%s] Rank [%u] is not in commInfo.", tag.c_str(), topoInfo->userRank), HcclResult::HCCL_E_PARA);

    u32 myRank = topoInfo->userRank;
    std::vector<CommProtocol> expectedProtocols;
    CHK_RET(GetProtocolByEngine(param, expectedProtocols));

    for (u32 rank : subcommInfo[COMM_LEVEL0]) {
        if (rank == topoInfo->userRank) {
            continue;
        }
        size_t channelCountBefore = channels.size();

        CommLink* linkList = nullptr;
        u32 listSize = 0;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, rank, &linkList, &listSize));

        if (listSize == 0) {
            if (linkRequired) {
                HCCL_ERROR("[%s] No intra-frame link between myRank=%u and rank=%u.", tag.c_str(), myRank, rank);
                return HcclResult::HCCL_E_INTERNAL;
            }
            HCCL_WARNING("[%s] No inter-frame link between myRank=%u and rank=%u.", tag.c_str(), myRank, rank);
            continue;
        }

        std::vector<CommLink> links(linkList, linkList + listSize);
        bool protocolFound = false;
        CHK_RET(ProcessLinkForProtocol(
            comm, expectedProtocols, links, myRank, rank, netLayer, channels, protocolFound, tag));

        if (channels.size() == channelCountBefore) {
            if (linkRequired) {
                HCCL_ERROR(
                    "[%s] No matching protocol intra-frame link between myRank=%u and rank=%u.", tag.c_str(), myRank,
                    rank);
                return HcclResult::HCCL_E_INTERNAL;
            }
            HCCL_WARNING(
                "[%s] No matching protocol inter-frame link between myRank=%u and rank=%u.", tag.c_str(), myRank, rank);
        }
    }
#else
    (void)comm;
    (void)param;
    (void)topoInfo;
    (void)subcommInfo;
    (void)channels;
    (void)netLayer;
    (void)tag;
    (void)linkRequired;
#endif
    return HCCL_SUCCESS;
}

HcclResult CalcChannelRequestMesh1DLevel0(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1DByLevel(
        comm, param, topoInfo, subcommInfo, channels, 0, "CalcChannelRequestMesh1DLevel0", true);
}

HcclResult CalcChannelRequestMesh1DLevel1(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
    return CalcChannelRequestMesh1DByLevel(
        comm, param, topoInfo, subcommInfo, channels, 1, "CalcChannelRequestMesh1DLevel1", false);
}

HcclResult CalcChannelRequestMesh2D(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
#ifndef AICPU_COMPILE
    channels.clear();
    u32 myRank = topoInfo->userRank; // 全局rankId

    std::set<u32> connectRanks;
    if (subcommInfo.size() == 2) { // 2D Mesh
        CHK_RET(CalcMesh2DChannelConnect(myRank, subcommInfo, connectRanks));
    }
    CommProtocol protocol = CommProtocol::COMM_PROTOCOL_UBC_CTP;

    for (u32 rank : connectRanks) {
        HcclChannelDesc channelDesc;
        HcclChannelDescInit(&channelDesc, 1);
        channelDesc.remoteRank = rank;
        CommLink* linkList = nullptr;
        u32 listSize;
        CHK_RET(HcclRankGraphGetLinks(comm, 0, myRank, channelDesc.remoteRank, &linkList, &listSize));
        bool protocolExists = false;
        for (u32 idx = 0; idx < listSize; idx++) {
            CommLink link = linkList[idx];
            if (link.linkAttr.linkProtocol == protocol) {
                channelDesc.localEndpoint.protocol = link.srcEndpointDesc.protocol;
                channelDesc.localEndpoint.commAddr = link.srcEndpointDesc.commAddr;
                channelDesc.localEndpoint.loc = link.srcEndpointDesc.loc;
                channelDesc.remoteEndpoint.protocol = link.dstEndpointDesc.protocol;
                channelDesc.remoteEndpoint.commAddr = link.dstEndpointDesc.commAddr;
                channelDesc.remoteEndpoint.loc = link.dstEndpointDesc.loc;
                protocolExists = true;
                HCCL_INFO(
                    "[%s]Add channel request between %zu and %zu with protocol %zu type %u", __func__, myRank,
                    channelDesc.remoteRank, link.dstEndpointDesc.protocol, link.srcEndpointDesc.commAddr.type);
                break;
            }
        }
        CHK_PRT_RET(
            !protocolExists,
            HCCL_ERROR(
                "[%s] protocol[%u] not exists between %zu and %zu", __func__, protocol, myRank, channelDesc.remoteRank),
            HCCL_E_NOT_FOUND);
        channelDesc.channelProtocol = protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channels.push_back(channelDesc);
    }
#endif
    return HCCL_SUCCESS;
}

HcclResult ProcessLinkForProtocolNhr(
    HcclComm comm, const std::vector<CommProtocol>& expectedProtocols, const std::vector<CommLink>& linkList,
    u32 myRank, u32 remoteRank, uint32_t netLayer, std::vector<HcclChannelDesc>& channels, bool& protocolFound)
{
    return ProcessLinkForProtocol(
        comm, expectedProtocols, linkList, myRank, remoteRank, netLayer, channels, protocolFound,
        std::string("[CalcLevel1ChannelRequestNhr]"));
}

HcclResult CalcChannelRequestNhr(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels)
{
#ifndef AICPU_COMPILE
    (void)param;
    channels.clear();
    std::set<u32> connectRanks;
    u32 myRank = topoInfo->userRank;
    auto it = std::find(subcommInfo[0].begin(), subcommInfo[0].end(), myRank);
    CHK_PRT_RET(
        (it == subcommInfo[0].end()), HCCL_ERROR("[CollAlgFactory] [channel] Rank [%d] is not in commInfo.", myRank),
        HcclResult::HCCL_E_PARA);

    u32 localRank = std::distance(subcommInfo[0].begin(), it);
    u32 localRankSize = subcommInfo[0].size();
    CHK_RET(CalcNHRChannelConnect(localRank, localRankSize, INVALID_VALUE_RANKID, connectRanks));

    // 根据engine获取期望的协议类型列表
    std::vector<CommProtocol> expectedProtocols;
    CHK_RET(GetProtocolByEngine(param, expectedProtocols));

    for (u32 rankIdx : connectRanks) {
        size_t channelCountBefore = channels.size();
        uint32_t* netLayers;
        uint32_t netLayerNum;
        CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
        std::vector<uint32_t> netLayersVector(netLayers, netLayers + netLayerNum);

        for (auto netLayer : netLayersVector) {
            CommLink* linkList = nullptr;
            u32 listSize;
            CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, subcommInfo[0][rankIdx], &linkList, &listSize));

            if (listSize == 0) {
                continue;
            }

            std::vector<CommLink> links(linkList, linkList + listSize);
            bool protocolFound = false;
            CHK_RET(ProcessLinkForProtocolNhr(
                comm, expectedProtocols, links, myRank, subcommInfo[0][rankIdx], netLayer, channels, protocolFound));

            if (channels.size() > channelCountBefore) {
                break;
            }
        }

        CHK_PRT_RET(
            channels.size() == channelCountBefore,
            HCCL_ERROR(
                "[CalcChannelRequestNhr] Failed to create channel between myRank=%u and rank=%u, there is no link.",
                myRank, subcommInfo[0][rankIdx]),
            HcclResult::HCCL_E_INTERNAL);
    }
#endif
    return HCCL_SUCCESS;
}

static bool IsEndPointEqual(const EndpointDesc& endPoint0, const EndpointDesc& endPoint1)
{
    HCCL_INFO(
        "endPoint0:phyId[%u], protocol[%u], addr.type[%u], addr.id[%u]", endPoint0.loc.device.devPhyId,
        endPoint0.protocol, endPoint0.commAddr.type, endPoint0.commAddr.id);
    HCCL_INFO(
        "endPoint1:phyId[%u], protocol[%u], addr.type[%u], addr.id[%u]", endPoint1.loc.device.devPhyId,
        endPoint1.protocol, endPoint1.commAddr.type, endPoint1.commAddr.id);
    if (endPoint0.protocol == CommProtocol::COMM_PROTOCOL_PCIE) {
        return (endPoint0.protocol == endPoint1.protocol) && (endPoint0.commAddr.type == endPoint1.commAddr.type);
    } else {
        return (endPoint0.protocol == endPoint1.protocol) && (endPoint0.commAddr.type == endPoint1.commAddr.type) &&
               (memcmp(endPoint0.commAddr.eid, endPoint1.commAddr.eid, sizeof(endPoint0.commAddr.eid)) == 0);
    }
}

HcclResult GetTopoTypeByLink(HcclComm comm, uint32_t netLayer, const CommLink& link, CommTopo& topoType)
{
#ifndef AICPU_COMPILE
    CHK_PTR_NULL(comm);
    uint32_t* topoInstList = nullptr;
    uint32_t listSize;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayer, &topoInstList, &listSize)); // 获取当前rank的所有TopoInst
    HCCL_INFO("[%s][%u] listSize = %u", __func__, __LINE__, listSize);

    for (uint32_t topoInstIdx = 0; topoInstIdx < listSize; topoInstIdx++) { // 遍历topoInst
        uint32_t topoInstId = topoInstList[topoInstIdx];
        uint32_t endPointNum;
        CHK_RET(HcclRankGraphGetEndpointNum(comm, netLayer, topoInstId, &endPointNum));
        EndpointDesc* endPointDescs = (EndpointDesc*)malloc(endPointNum * sizeof(EndpointDesc));
        if (endPointDescs == nullptr) {
            HCCL_ERROR("Malloc endPointDescs failed!");
            return HCCL_E_PARA;
        }
        HcclResult ret = HCCL_SUCCESS;
        ret = HcclRankGraphGetEndpointDesc(comm, netLayer, topoInstId, &endPointNum, endPointDescs);
        if (ret != HCCL_SUCCESS) {
            free(endPointDescs);
            return ret;
        }

        ret = HcclRankGraphGetTopoType(comm, netLayer, topoInstId, &topoType);
        if (ret != HCCL_SUCCESS) {
            free(endPointDescs);
            return ret;
        }
        HCCL_DEBUG("[%s]topoInstId=%u, endPointNum=%u, topoType=%u", __func__, topoInstId, endPointNum, topoType);
        for (uint32_t endPointIdx = 0; endPointIdx < endPointNum; endPointIdx++) {
            const EndpointDesc& endPoint = endPointDescs[endPointIdx];
            if (IsEndPointEqual(link.srcEndpointDesc, endPoint) ==
                true) { // 当前TopoInst和link的endPoint相同，说明link属于当前TopoInst
                free(endPointDescs);
                return HCCL_SUCCESS;
            }
        }
        HCCL_WARNING("[%s]No Endpoint matches on TopoInst[%u].", __func__, topoInstId);
        free(endPointDescs);
    }
    HCCL_ERROR("[%s]Cannot get TopoType by Link.", __func__);
    return HCCL_E_INTERNAL;
#else
    return HCCL_SUCCESS;
#endif
}

static bool IsPortEqual(const EndpointDesc& endPoint0, const EndpointDesc& endPoint1, bool isIsolation)
{
    const u8 localPort = endPoint0.commAddr.eid[PORT_IDX];
    const u8 remotePort = endPoint1.commAddr.eid[PORT_IDX];
    if (localPort != remotePort) {
        return false;
    }
    return isIsolation ? localPort != NON_ISOLATED_PORT : localPort == NON_ISOLATED_PORT;
}

/*
 *   获取link对应的channel。对于2个rank之间，存在多条link的场景，会优先获取指定TopoType的1条channel。
 *   如果多条link都没有指定的TopoType，则返回第一条link对应的channel。
 */
HcclResult ProcessLinksForChannel(
    HcclComm comm, u32 myRank, u32 rank, std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
#ifndef AICPU_COMPILE
    uint32_t* netLayers;
    uint32_t netLayerNum;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
    std::vector<uint32_t> netLayersVector(netLayers, netLayers + netLayerNum);
    for (auto netLayer : netLayersVector) {
        CommLink* linkList = nullptr;
        u32 listSize;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, rank, &linkList, &listSize));
        HCCL_INFO("[CalcChannelRequestWithPriorTopo] netLayer=%u, linkListSize=%u", netLayer, listSize);

        if (listSize == 0) {
            HCCL_WARNING(
                "[CalcChannelRequestWithPriorTopo]There is no link between rank[%u] and rank[%u].", myRank, rank);
            break;
        }

        uint32_t priorityLink = 0;
        CommTopo topoType;
        for (u32 idx = 0; idx < listSize; idx++) {
            CHK_RET(GetTopoTypeByLink(comm, netLayer, linkList[idx], topoType));
            if (topoType == priorityTopo) {
                priorityLink = idx;
                HCCL_INFO(
                    "[CalcChannelRequestWithPriorTopo] Found link[%u] with priority topotype[%u].", idx, topoType);
                break;
            }
        }
        HcclChannelDesc channelDesc;
        HcclChannelDescInit(&channelDesc, 1);
        channelDesc.remoteRank = rank;
        CommLink link = linkList[priorityLink];
        channelDesc.localEndpoint.protocol = link.srcEndpointDesc.protocol;
        channelDesc.localEndpoint.commAddr = link.srcEndpointDesc.commAddr;
        channelDesc.localEndpoint.loc = link.srcEndpointDesc.loc;
        channelDesc.remoteEndpoint.protocol = link.dstEndpointDesc.protocol;
        channelDesc.remoteEndpoint.commAddr = link.dstEndpointDesc.commAddr;
        channelDesc.remoteEndpoint.loc = link.dstEndpointDesc.loc;
        CHK_RET(GetTopoTypeByLink(comm, netLayer, linkList[priorityLink], topoType));
        HCCL_INFO(
            "[CalcChannelRequestWithPriorTopo]Add channel request between %u and %u with protocol %u "
            "and topoType %u. And Priority topoType is %u.",
            myRank, channelDesc.remoteRank, channelDesc.remoteEndpoint.protocol, topoType, priorityTopo);
        channelDesc.channelProtocol = link.srcEndpointDesc.protocol;
        channelDesc.notifyNum = NORMAL_NOTIFY_NUM;
        channels.push_back(channelDesc);
        if (listSize > 0) {
            break;
        }
    }

#endif
    return HCCL_SUCCESS;
}

HcclResult ProcessLinksForChannelMutiJetty(
    HcclComm comm, CommProtocol expectedProtocol, const std::vector<CommLink>& linkList, u32 myRank, u32 remoteRank,
    uint32_t netLayer, std::vector<HcclChannelDesc>& channels, bool expectMesh, bool isIsolation)
{
#ifndef AICPU_COMPILE
    if (linkList.size() < MIN_PHY_LINK_COUNT) {
        isIsolation = false;
    }
    std::vector<HcclChannelDesc> selectedChannels;
    for (u32 idx = 0; idx < linkList.size(); ++idx) {
        const CommLink& link = linkList[idx];
        if (link.linkAttr.linkProtocol != expectedProtocol) {
            continue;
        }
        CommTopo topoType = CommTopo::COMM_TOPO_RESERVED;
        CHK_RET(GetTopoTypeByLink(comm, netLayer, link, topoType));
        if (topoType == CommTopo::COMM_TOPO_CLOS &&
            IsPortEqual(link.srcEndpointDesc, link.dstEndpointDesc, isIsolation)) {
            CHK_RET(CreateChannelFromLink(
                comm, myRank, remoteRank, netLayer, idx, link, "ProcessLinksForChannelMutiJetty", selectedChannels));
        } else if (topoType == CommTopo::COMM_TOPO_1DMESH && expectMesh) {
            selectedChannels.clear();
            CHK_RET(CreateChannelFromLink(
                comm, myRank, remoteRank, netLayer, idx, link, "ProcessLinksForChannelMutiJetty", selectedChannels));
            break;
        }
    }
    channels.insert(channels.end(), selectedChannels.begin(), selectedChannels.end());
#else
    (void)comm;
    (void)expectedProtocol;
    (void)linkList;
    (void)myRank;
    (void)remoteRank;
    (void)netLayer;
    (void)channels;
    (void)expectMesh;
    (void)isIsolation;
#endif
    return HCCL_SUCCESS;
}

#ifndef AICPU_COMPILE
static HcclResult CalcChannelRequestMeshClosMultiJettyByRank(
    HcclComm comm, CommProtocol expectedProtocol, u32 localRank, u32 remoteRank, std::vector<HcclChannelDesc>& channels,
    bool isIsolation, bool expectMesh)
{
    const size_t channelCountBefore = channels.size();
    uint32_t* netLayers = nullptr;
    uint32_t netLayerNum = 0U;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
    CHK_PRT_RET(
        netLayers == nullptr || netLayerNum == 0U,
        HCCL_ERROR("[CalcChannelRequestMeshClosMultiJetty] no network layer."), HCCL_E_INTERNAL);
    for (uint32_t netLayerIdx = 0U; netLayerIdx < netLayerNum; ++netLayerIdx) {
        const uint32_t netLayer = netLayers[netLayerIdx];
        CommLink* linkList = nullptr;
        u32 listSize = 0U;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, localRank, remoteRank, &linkList, &listSize));
        if (linkList == nullptr || listSize == 0U) {
            continue;
        }
        const std::vector<CommLink> links(linkList, linkList + listSize);
        CHK_RET(ProcessLinksForChannelMutiJetty(
            comm, expectedProtocol, links, localRank, remoteRank, netLayer, channels, expectMesh, isIsolation));
        if (channels.size() > channelCountBefore) {
            break;
        }
    }
    CHK_PRT_RET(
        channels.size() == channelCountBefore,
        HCCL_ERROR(
            "[CalcChannelRequestMeshClosMultiJetty] no channel between local rank[%u] and remote rank[%u].", localRank,
            remoteRank),
        HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}
#endif

HcclResult CalcChannelRequestMeshClosMultiJetty(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const std::vector<std::vector<u32>>& subcommInfo, std::vector<HcclChannelDesc>& channels, bool isIsolation,
    bool expectMesh)
{
#ifndef AICPU_COMPILE
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        subcommInfo.empty(), HCCL_ERROR("[CalcChannelRequestMeshClosMultiJetty] subcommInfo is empty."), HCCL_E_PARA);
    const auto localRankIt =
        std::find(subcommInfo[COMM_LEVEL0].begin(), subcommInfo[COMM_LEVEL0].end(), topoInfo->userRank);
    CHK_PRT_RET(
        localRankIt == subcommInfo[COMM_LEVEL0].end(),
        HCCL_ERROR("[CalcChannelRequestMeshClosMultiJetty] local rank[%u] is absent.", topoInfo->userRank),
        HCCL_E_PARA);
    CHK_PRT_RET(
        param.engine != CommEngine::COMM_ENGINE_AICPU && param.engine != CommEngine::COMM_ENGINE_AICPU_TS,
        HCCL_ERROR("[CalcChannelRequestMeshClosMultiJetty] unsupported engine[%u].", static_cast<u32>(param.engine)),
        HCCL_E_NOT_SUPPORT);

    channels.clear();
    constexpr CommProtocol EXPECTED_PROTOCOL = CommProtocol::COMM_PROTOCOL_UBC_CTP;
    for (u32 remoteRank : subcommInfo[COMM_LEVEL0]) {
        if (remoteRank == topoInfo->userRank) {
            continue;
        }
        CHK_RET(CalcChannelRequestMeshClosMultiJettyByRank(
            comm, EXPECTED_PROTOCOL, topoInfo->userRank, remoteRank, channels, isIsolation, expectMesh));
    }
#else
    (void)comm;
    (void)param;
    (void)topoInfo;
    (void)subcommInfo;
    (void)channels;
    (void)isIsolation;
    (void)expectMesh;
#endif
    return HCCL_SUCCESS;
}

HcclResult CalcChannelRequestMesh1DWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
#ifndef AICPU_COMPILE
    (void)param;
    channels.clear();
    auto it = std::find(subcommInfo[COMM_LEVEL0].begin(), subcommInfo[COMM_LEVEL0].end(), topoInfo->userRank);
    CHK_PRT_RET(
        (it == subcommInfo[COMM_LEVEL0].end()),
        HCCL_ERROR("[CollAlgFactory] [channel] Rank [%d] is not in commInfo.", topoInfo->userRank),
        HcclResult::HCCL_E_PARA);

    u32 myRank = topoInfo->userRank;
    for (u32 rank : subcommInfo[COMM_LEVEL0]) {
        if (rank != myRank) {
            CHK_RET(ProcessLinksForChannel(comm, myRank, rank, channels, priorityTopo));
        }
    }
    HCCL_INFO("[%s] success.", __func__);
#endif
    return HCCL_SUCCESS;
}

HcclResult CalcChannelRequestNHRWithPriorityTopo(
    HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, const std::vector<std::vector<u32>>& subcommInfo,
    std::vector<HcclChannelDesc>& channels, CommTopo priorityTopo)
{
#ifndef AICPU_COMPILE
    (void)param;
    channels.clear();
    auto it = std::find(subcommInfo[COMM_LEVEL0].begin(), subcommInfo[COMM_LEVEL0].end(), topoInfo->userRank);
    CHK_PRT_RET(
        (it == subcommInfo[COMM_LEVEL0].end()),
        HCCL_ERROR("[CollAlgFactory] [channel] Rank [%d] is not in commInfo.", topoInfo->userRank),
        HcclResult::HCCL_E_PARA);

    std::set<u32> connectRanks;
    u32 localRank = std::distance(subcommInfo[0].begin(), it);
    u32 localRankSize = subcommInfo[0].size();
    u32 myRank = topoInfo->userRank;
    CHK_RET(CalcNHRChannelConnect(localRank, localRankSize, INVALID_VALUE_RANKID, connectRanks));

    for (u32 rank : connectRanks) {
        if (rank != localRank) {
            CHK_RET(ProcessLinksForChannel(comm, myRank, subcommInfo[0][rank], channels, priorityTopo));
        }
    }
    HCCL_INFO("[%s] success.", __func__);
#endif
    return HCCL_SUCCESS;
}

HcclResult CreateChannelRequestByRankId(
    HcclComm comm, const OpParam& param, u32 myRank, u32 remoteRank, std::vector<HcclChannelDesc>& channels,
    u32 channelRepeatNum)
{
#ifndef AICPU_COMPILE
    channels.clear();
    std::vector<CommProtocol> expectedProtocols;
    CHK_RET(GetProtocolByEngine(param, expectedProtocols));

    uint32_t* netLayers;
    uint32_t netLayerNum;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
    std::vector<uint32_t> netLayersVector = std::vector<uint32_t>(netLayers, netLayers + netLayerNum);

    for (auto netLayer : netLayersVector) {
        CommLink* linkList = nullptr;
        u32 listSize;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, remoteRank, &linkList, &listSize));
        if (listSize == 0) {
            continue;
        }
        std::vector<CommLink> links(linkList, linkList + listSize);
        bool protocolFound = false;
        for (u32 i = 0; i < channelRepeatNum; i++) {
            CHK_RET(ProcessLinkForProtocol(
                comm, expectedProtocols, links, myRank, remoteRank, netLayer, channels, protocolFound,
                std::string("[CreateChannelRequestByRankId]")));
        }

        if (channels.size() > 0) {
            break;
        }
    }
    CHK_PRT_RET(
        channels.size() == 0,
        HCCL_ERROR(
            "[CreateChannelRequestByRankId] Failed to create channel between myRank=%u and rank=%u, there is no link.",
            myRank, remoteRank),
        HcclResult::HCCL_E_INTERNAL);

#endif
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
