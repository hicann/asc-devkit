/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_model.h"

namespace HcclSim {

constexpr uint32_t GRID_SIZE = 8;
constexpr uint32_t NetLayerL0 = 0;
constexpr uint32_t NetLayerL1 = 1;
constexpr uint32_t NetLayerL2 = 2;
constexpr uint32_t SERVER_CLOS_INSTID = 16;

TopoModel::TopoModel(const TopoMeta& topoMeta)
{
    uint32_t superpodId = 0;
    uint32_t serverId = 0;
    uint32_t rankId = 0;
    uint32_t devIpStart = 3232238090;
    for (auto& pod : topoMeta) {
        uint32_t rankNumInPod = 0;
        for (auto& server : pod) {
            uint32_t rankNumInServer = 0;
            InitTopoInstsMap(serverId, rankId, server);
            for (auto& phyId : server) {
                rankId2PhyId_[rankId] = phyId;
                rankId2ServerId_[rankId] = serverId;
                rankId2PodId_[rankId] = superpodId;
                serverId2RankList_[serverId].push_back(rankId);
                podId2RankList_[superpodId].push_back(rankId);
                allRankList_.push_back(rankId);

                InitEndpointMap(rankId, phyId, serverId, superpodId, devIpStart);

                rankId++;
                rankNumInPod++;
                rankNumInServer++;
            }
            podServersGroup_.push_back(rankNumInServer);
            serverId++;
        }
        podRanksGroup_.push_back(rankNumInPod);
        superpodId++;
    }

    allRankNum_.push_back(rankId);
    InitNetLayerInfo(serverId, superpodId);
    Init910BLinkMap();
    Init910CLinkMap();
    Init910DLinkMap();
    InitL1L2TopoInsts(superpodId);
    InitHostDpuInfo(serverId);
}

void TopoModel::InitHostDpuInfo(uint32_t serverNum)
{
    char* dpnEnv = getenv("ENABLE_HOSTDPU_FOR_LLT");
    if (dpnEnv == nullptr || serverNum < 2) {
        return;
    }

    if (std::string(dpnEnv) != "1") {
        return;
    }

    isDpuEnable = true;
    dpuDesc_.clear();
    EndpointDesc endpoint;
    endpoint.loc.locType = EndpointLocType::ENDPOINT_LOC_TYPE_HOST;

    dpuDesc_.push_back(endpoint);
}

void TopoModel::InitL1L2TopoInsts(uint32_t podNum)
{
    // level1和level2认为只有一个实例0
    for (uint32_t podId = 0; podId < podNum; podId++) {
        level1TopoInsts_[podId].push_back(0);
    }

    level2TopoInsts_.push_back(0);
}

void TopoModel::InitTopoInstsMap(uint32_t serverId, uint32_t rankId, const std::vector<uint32_t>& phyIds)
{
    for (auto phyId : phyIds) {
        auto rowId = phyId / GRID_SIZE;
        auto rowInstId = rowId;
        auto rowRankIds = CheckRowPeerDevice(phyIds, rankId, rowId);
        instId2RankIds_[serverId][rowInstId] = rowRankIds;
        if (rowRankIds.size() > 1) { // X轴实例1dmesh
            dev2TopoInsts_[serverId][phyId].push_back(rowInstId);
        }

        auto colId = phyId % GRID_SIZE;
        auto colInstId = colId + GRID_SIZE;
        auto colRankIds = CheckColPeerDevice(phyIds, rankId, colId);
        instId2RankIds_[serverId][colInstId] = colRankIds;
        if (colRankIds.size() > 1) { // Y轴实例1dmesh
            dev2TopoInsts_[serverId][phyId].push_back(colInstId);
        }

        // 单Rank场景
        if (rowRankIds.size() == 1 && colRankIds.size() == 1) {
            instId2RankIds_[serverId][rowInstId] = rowRankIds;
            dev2TopoInsts_[serverId][phyId].push_back(rowInstId);
        }

        // 2D场景才有Z轴实例CLOS
        if (rowRankIds.size() > 1 && colRankIds.size() > 1) {
            is2D = true;
            dev2TopoInsts_[serverId][phyId].push_back(SERVER_CLOS_INSTID);
        }
    }
}

std::vector<uint32_t> TopoModel::GetAllRanks(const std::vector<uint32_t>& phyIds, uint32_t rankId)
{
    std::vector<uint32_t> ranks;
    for (auto phyId : phyIds) {
        ranks.push_back(rankId);
        rankId++;
    }

    return ranks;
}

std::vector<uint32_t> TopoModel::CheckRowPeerDevice(
    const std::vector<uint32_t>& phyIds, uint32_t rankId, uint32_t rowId)
{
    // curPhyId也计算在内了
    std::vector<uint32_t> devices;
    for (auto phyId : phyIds) {
        if (phyId / GRID_SIZE == rowId) {
            devices.push_back(rankId);
        }
        rankId++;
    }

    return devices;
}

std::vector<uint32_t> TopoModel::CheckColPeerDevice(
    const std::vector<uint32_t>& phyIds, uint32_t rankId, uint32_t colId)
{
    std::vector<uint32_t> devices;
    for (auto phyId : phyIds) {
        if (phyId % GRID_SIZE == colId) {
            devices.push_back(rankId);
        }
        rankId++;
    }

    return devices;
}

void TopoModel::InitEndpointMap(
    uint32_t rankId, uint32_t phyId, uint32_t serverId, uint32_t superpodId, uint32_t& devIpStart)
{
    EndpointDesc endpoint;
    CommAddr addr;
    addr.type = CommAddrType::COMM_ADDR_TYPE_IP_V4;
    addr.id = htonl(devIpStart++);
    endpoint.commAddr = addr;
    endpoint.loc.device.devPhyId = phyId;
    endpoint.loc.device.superPodIdx = superpodId;
    endpoint.loc.device.serverIdx = serverId;
    endpoint.loc.locType = EndpointLocType::ENDPOINT_LOC_TYPE_DEVICE;
    rankId2Endpoint_[rankId] = endpoint;
}

void TopoModel::InitNetLayerInfo(uint32_t serverNum, uint32_t podNum)
{
    if (serverNum == 1) {
        netLayerList_ = {0};
    }

    if (serverNum > 1) {
        netLayerList_ = {0, 1};
    }

    if (podNum > 1) {
        netLayerList_ = {0, 1, 2};
    }
}

uint32_t TopoModel::GetRankSize() const { return allRankList_.size(); }

void TopoModel::GetNetLayers(uint32_t** netLayers, uint32_t* netLayerNum)
{
    *netLayerNum = netLayerList_.size();
    *netLayers = netLayerList_.data();
}

void TopoModel::GetInstSizeByNetLayer(uint32_t curRank, uint32_t netLayer, uint32_t* rankNum)
{
    auto serverId = rankId2ServerId_[curRank];
    auto podId = rankId2PodId_[curRank];
    if (netLayer == NetLayerL0) {
        *rankNum = serverId2RankList_[serverId].size();
    } else if (netLayer == NetLayerL1) {
        *rankNum = podId2RankList_[podId].size();
    } else {
        *rankNum = allRankList_.size();
    }
}

void TopoModel::GetLinks(
    DevType devType, uint32_t netLayer, uint32_t srcRank, uint32_t dstRank, CommLink** linkList, uint32_t* listSize)
{
    auto rankPair = std::make_pair(srcRank, dstRank);
    auto it = allLinkMap_.find(rankPair);
    if (it != allLinkMap_.end()) {
        auto& levelLinkMap = it->second;
        *listSize = levelLinkMap[netLayer].size();
        *linkList = levelLinkMap[netLayer].data();
        return;
    }

    // 当前link还没添加至map，构造link并添加至map
    if (devType == DevType::DEV_TYPE_910B) {
        Create910BLinks(srcRank, dstRank);
    } else if (devType == DevType::DEV_TYPE_910_93) {
        Create910CLinks(srcRank, dstRank);
#ifdef MACRO_DEV_TYPE_NEW
    } else if (devType == DevType::DEV_TYPE_950) {
#else
    } else if (devType == DevType::DEV_TYPE_910_95) {
#endif
        Create910DLinks(srcRank, dstRank);
    }

    *listSize = allLinkMap_[rankPair][netLayer].size();
    *linkList = allLinkMap_[rankPair][netLayer].data();
}

void TopoModel::GetInstSizeListByNetLayer(uint32_t netLayer, uint32_t** instSizeList, uint32_t* listSize)
{
    if (netLayer == NetLayerL0) {
        *listSize = podServersGroup_.size();
        *instSizeList = podServersGroup_.data();
    } else if (netLayer == NetLayerL1) {
        *listSize = podRanksGroup_.size();
        *instSizeList = podRanksGroup_.data();
    } else {
        *listSize = allRankNum_.size();
        *instSizeList = allRankNum_.data();
    }
}

void TopoModel::GetInstTopoTypeByNetLayer(DevType devType, uint32_t netLayer, CommTopo* topoType)
{
    if (netLayer == NetLayerL0) {
        if (devType == DevType::DEV_TYPE_910B) {
            *topoType = CommTopo::COMM_TOPO_1DMESH;
        } else if (devType == DevType::DEV_TYPE_910_93) {
            *topoType = CommTopo::COMM_TOPO_910_93;
#ifdef MACRO_DEV_TYPE_NEW
        } else if (devType == DevType::DEV_TYPE_950) {
#else
        } else if (devType == DevType::DEV_TYPE_910_95) {
#endif
            *topoType = CommTopo::COMM_TOPO_CUSTOM; // A5topo使用新API查询
        }
    } else if (netLayer == NetLayerL1) {
        *topoType = CommTopo::COMM_TOPO_CLOS;
    } else {
        *topoType = CommTopo::COMM_TOPO_CLOS;
    }
}

void TopoModel::GetInstRanksByNetLayer(uint32_t curRank, uint32_t netLayer, uint32_t** ranks, uint32_t* rankNum)
{
    auto serverId = rankId2ServerId_[curRank];
    auto podId = rankId2PodId_[curRank];
    if (netLayer == NetLayerL0) {
        *rankNum = serverId2RankList_[serverId].size();
        *ranks = serverId2RankList_[serverId].data();
    } else if (netLayer == NetLayerL1) {
        *rankNum = podId2RankList_[podId].size();
        *ranks = podId2RankList_[podId].data();
    } else {
        *rankNum = allRankList_.size();
        *ranks = allRankList_.data();
    }
}

void TopoModel::GetTopoInstsByLayer(uint32_t curRank, uint32_t netLayer, uint32_t** topoInsts, uint32_t* topoInstNum)
{
    auto phyId = rankId2PhyId_[curRank];
    auto serverId = rankId2ServerId_[curRank];
    auto podId = rankId2PodId_[curRank];
    if (netLayer == NetLayerL0) {
        *topoInsts = dev2TopoInsts_[serverId][phyId].data();
        *topoInstNum = dev2TopoInsts_[serverId][phyId].size();
    } else if (netLayer == NetLayerL1) {
        *topoInsts = level1TopoInsts_[podId].data();
        *topoInstNum = level1TopoInsts_[podId].size();
    } else if (netLayer == NetLayerL2) {
        *topoInsts = level2TopoInsts_.data();
        *topoInstNum = level2TopoInsts_.size();
    }
}

void TopoModel::GetRanksByTopoInst(
    uint32_t curRank, uint32_t netLayer, uint32_t topoInstId, uint32_t** ranks, uint32_t* rankNum)
{
    auto serverId = rankId2ServerId_[curRank];
    auto podId = rankId2PodId_[curRank];
    if (netLayer == NetLayerL0) {
        *ranks = instId2RankIds_[serverId][topoInstId].data();
        *rankNum = instId2RankIds_[serverId][topoInstId].size();
    } else if (netLayer == NetLayerL1) {
        *ranks = podId2RankList_[podId].data();
        *rankNum = podId2RankList_[podId].size();
    } else if (netLayer == NetLayerL2) {
        *ranks = allRankList_.data();
        *rankNum = allRankList_.size();
    }
}

void TopoModel::GetTopoType(uint32_t curRank, uint32_t netLayer, uint32_t topoInstId, CommTopo* topoType)
{
    if (netLayer == NetLayerL0) {
        if (topoInstId == SERVER_CLOS_INSTID) {
            *topoType = CommTopo::COMM_TOPO_CLOS;
        } else {
            *topoType = CommTopo::COMM_TOPO_1DMESH;
        }
    } else {
        *topoType = CommTopo::COMM_TOPO_CLOS;
    }
}

void TopoModel::GetEndpointNum(uint32_t curRank, uint32_t layer, uint32_t topoInstId, uint32_t* num)
{
    auto serverId = rankId2ServerId_[curRank];
    auto podId = rankId2PodId_[curRank];
    if (layer == NetLayerL0) {
        *num = serverId2RankList_[serverId].size();
    } else if (layer == NetLayerL1) {
        *num = podId2RankList_[podId].size();
    } else {
        *num = allRankList_.size();
    }
}

void TopoModel::GetEndpointDesc(
    uint32_t curRank, uint32_t layer, uint32_t topoInstId, uint32_t* descNum, EndpointDesc* endpointDesc)
{
    // 仅支持hostdpu使用，暂时仅支持layer1的出框的通信对端查询
    if (layer != NetLayerL1) {
        printf("[ERROR][GetEndpointDesc] not support for layer[%u]\n", layer);
        return;
    }

    *descNum = dpuDesc_.size();
    for (auto i = 0; i < dpuDesc_.size(); i++) {
        endpointDesc[i] = dpuDesc_[i];
    }
}

void TopoModel::Init910BLinkMap()
{
    // 910B按照A+X结构初始化
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (i == j) {
                continue;
            }
            link910b_[{i, j}] = CommProtocol::COMM_PROTOCOL_RESERVED;
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == j) {
                continue;
            }
            link910b_[{i, j}] = CommProtocol::COMM_PROTOCOL_HCCS;
        }
    }

    for (int i = 8; i < 16; i++) {
        for (int j = 8; j < 16; j++) {
            if (i == j) {
                continue;
            }
            link910b_[{i, j}] = CommProtocol::COMM_PROTOCOL_HCCS;
        }
    }

    link910b_[{0, 8}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{1, 9}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{2, 10}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{3, 11}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{4, 12}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{5, 13}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{6, 14}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{7, 15}] = CommProtocol::COMM_PROTOCOL_PCIE;

    link910b_[{8, 0}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{9, 1}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{10, 2}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{11, 3}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{12, 4}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{13, 5}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{14, 6}] = CommProtocol::COMM_PROTOCOL_PCIE;
    link910b_[{15, 7}] = CommProtocol::COMM_PROTOCOL_PCIE;
}

void TopoModel::Init910CLinkMap()
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (i == j) {
                continue;
            }
            link910b_[{i, j}] = CommProtocol::COMM_PROTOCOL_RESERVED;
        }
    }

    link910b_[{0, 1}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{1, 0}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{2, 3}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{3, 2}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{4, 5}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{5, 4}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{6, 7}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{7, 6}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{8, 9}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{9, 8}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{10, 11}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{11, 10}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{12, 13}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{13, 12}] = CommProtocol::COMM_PROTOCOL_SIO;

    link910b_[{14, 15}] = CommProtocol::COMM_PROTOCOL_SIO;
    link910b_[{15, 14}] = CommProtocol::COMM_PROTOCOL_SIO;

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (i == j) {
                continue;
            }
            // 同封装的两个卡之间已经有了SIO链路，不需要重复赋值
            if ((i / 2) == (j / 2)) {
                continue;
            }
            link910b_[{i, j}] = CommProtocol::COMM_PROTOCOL_HCCS;
        }
    }
}

void TopoModel::Init910DLinkMap()
{
    const uint32_t GRID_SIZE = 8;
    const uint32_t TOTAL_NPUS = GRID_SIZE * GRID_SIZE;
    for (uint32_t i = 0; i < TOTAL_NPUS; ++i) {
        for (uint32_t j = 0; j < TOTAL_NPUS; ++j) {
            if (i == j) {
                continue;
            }

            // 计算行列坐标
            uint32_t rowI = i / GRID_SIZE;
            uint32_t colI = i % GRID_SIZE;
            uint32_t rowJ = j / GRID_SIZE;
            uint32_t colJ = j % GRID_SIZE;

            // 判断是否在同一行或同一列
            if (rowI == rowJ || colI == colJ) {
                link910d_[{i, j}] = CommProtocol::COMM_PROTOCOL_UBC_CTP;
            }
        }
    }
}

bool TopoModel::IsSamePod(uint32_t srcRank, uint32_t dstRank)
{
    uint32_t srcPodId = rankId2PodId_[srcRank];
    uint32_t dstPodId = rankId2PodId_[dstRank];

    return srcPodId == dstPodId;
}

bool TopoModel::IsSameServer(uint32_t srcRank, uint32_t dstRank)
{
    uint32_t srcServerId = rankId2ServerId_[srcRank];
    uint32_t dstServerId = rankId2ServerId_[dstRank];

    return srcServerId == dstServerId;
}

void TopoModel::Create910BLinks(uint32_t srcRank, uint32_t dstRank)
{
    auto rankPair = std::make_pair(srcRank, dstRank);

    CommLink link;
    link.srcEndpointDesc = rankId2Endpoint_[srcRank];
    link.dstEndpointDesc = rankId2Endpoint_[dstRank];
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_RESERVED;

    // level1 910B没有level2
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_ROCE;
    allLinkMap_[rankPair][NetLayerL1].push_back(link);

    // level0 同server才有level0链路
    if (!IsSameServer(srcRank, dstRank)) {
        return;
    }

    uint32_t srcPhyId = rankId2PhyId_[srcRank];
    uint32_t dstPhyId = rankId2PhyId_[dstRank];
    auto protocol = link910b_.find({srcPhyId, dstPhyId});
    if (protocol != link910b_.end()) {
        link.linkAttr.linkProtocol = protocol->second;
        allLinkMap_[rankPair][NetLayerL0].push_back(link);
    }
}

void TopoModel::Create910CLinks(uint32_t srcRank, uint32_t dstRank)
{
    auto rankPair = std::make_pair(srcRank, dstRank);

    CommLink link;
    link.srcEndpointDesc = rankId2Endpoint_[srcRank];
    link.dstEndpointDesc = rankId2Endpoint_[dstRank];
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_RESERVED;

    // level2
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_ROCE;
    allLinkMap_[rankPair][NetLayerL2].push_back(link);

    // level1 同pod才有level1链路
    if (IsSamePod(srcRank, dstRank)) {
        allLinkMap_[rankPair][NetLayerL1].push_back(link);
    }

    // level0 同server才有level0链路
    if (!IsSameServer(srcRank, dstRank)) {
        return;
    }

    uint32_t srcPhyId = rankId2PhyId_[srcRank];
    uint32_t dstPhyId = rankId2PhyId_[dstRank];
    auto protocol = link910c_.find({srcPhyId, dstPhyId});
    if (protocol != link910c_.end()) {
        link.linkAttr.linkProtocol = protocol->second;
        allLinkMap_[rankPair][NetLayerL0].push_back(link);
    }
}

void TopoModel::Create910DLinks(uint32_t srcRank, uint32_t dstRank)
{
    auto rankPair = std::make_pair(srcRank, dstRank);

    CommLink link;
    link.srcEndpointDesc = rankId2Endpoint_[srcRank];
    link.dstEndpointDesc = rankId2Endpoint_[dstRank];
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_RESERVED;

    // level2
    link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_ROCE; // 待确定协议类型
    allLinkMap_[rankPair][NetLayerL2].push_back(link);

    // level1 同pod才有level1链路
    if (IsSamePod(srcRank, dstRank)) {
        // HostDPU场景出框的连接需要对端描述为Host
        if (!IsSameServer(srcRank, dstRank) && isDpuEnable) {
            link.dstEndpointDesc.loc.locType = EndpointLocType::ENDPOINT_LOC_TYPE_HOST;
        }
        link.linkAttr.linkProtocol =
            isDpuEnable ? CommProtocol::COMM_PROTOCOL_ROCE : CommProtocol::COMM_PROTOCOL_UBC_CTP;
        allLinkMap_[rankPair][NetLayerL1].push_back(link);
    }

    // level0 同server才有level0链路
    if (!IsSameServer(srcRank, dstRank)) {
        return;
    }

    uint32_t srcPhyId = rankId2PhyId_[srcRank];
    uint32_t dstPhyId = rankId2PhyId_[dstRank];
    auto protocol = link910d_.find({srcPhyId, dstPhyId});
    if (protocol != link910d_.end()) {
        link.linkAttr.linkProtocol = protocol->second;
        allLinkMap_[rankPair][NetLayerL0].push_back(link);
    }

    if (is2D) {
        // 910D的level0有两条link(横向或纵向的1DMESH+出框的CLOS)
        link.linkAttr.linkProtocol = CommProtocol::COMM_PROTOCOL_UBC_CTP;
        allLinkMap_[rankPair][NetLayerL0].push_back(link);
    }
}
}; // namespace HcclSim
