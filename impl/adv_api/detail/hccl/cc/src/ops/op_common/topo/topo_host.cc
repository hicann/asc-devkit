/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <numeric>
#include <vector>
#include "topo_host.h"
#include "hccl_rank_graph.h"
#include "hcomm_primitives.h"
#include "hccl_res.h"
#include "hccl.h"
#include "adapter_acl.h"
#include "channel.h"
#include "hccl_common.h"
#include "config_log.h"
#include "topo.h"
#include "dtype_common.h"
#include "dlsym_common.h"
#include "hccl_rank_graph_dl.h"
#include "physical_level.h"

constexpr u32 FACTOR_NUM_TWO = 2;
constexpr s32 DEVICE_PER_MODULE = 8;
constexpr uint32_t NET_LAYER_NUM_TWO = 2;
constexpr uint32_t NET_LAYER_NUM_THREE = 3;
constexpr u32 DEVICE_NO_HCCS_LINK_COUNT = 2; // 设备没有与自身和通过同一SIO链路连接的companion设备的HCCS_SW链路
constexpr u32 TOPO_INST_NUM_MESH_1D_CLOS = 2; // MESH_1D_CLOS拓扑类型的实例数量

namespace mc2_ops_hccl {

HcclResult InitRankInfo(HcclComm comm, TopoInfo* topoInfo)
{
    // 提取本rank的信息
    CHK_RET(CalcMyRankInfo(comm, topoInfo));
    // 提取服务器层级的信息，比如服务器个数、每服务器卡数、服务器层拓扑是否对称
    std::unordered_map<u32, u32> pairLinkCounter;
    CHK_RET(GetPairLinkCounter(comm, topoInfo, pairLinkCounter));
    CHK_RET(SetServerModuleInfo(comm, topoInfo, pairLinkCounter));
    topoInfo->multiSuperPodDiffServerNumMode = false;
    if (topoInfo->deviceType == DevType::DEV_TYPE_910_93) {
        // 提取超节点层级的信息，比如超节点个数、每个超节点的服务器个数、
        // 超节点层拓扑是否对称
        CHK_RET(SetSuperPodInfo(comm, topoInfo));
        // 获取本服务器内的链路信息
        CHK_RET(CalcLinkInfo(topoInfo, pairLinkCounter));
    }
    HCCL_CONFIG_INFO(
        HCCL_ALG,
        "[InitRankInfo] userRank[%u] userRankSize[%u] serverIdx[%u] superPodIdx[%u] "
        "deviceType[%u] deviceNumPerModule[%u] serverNumPerSuperPod[%u] serverNum[%u] moduleNum[%u] superPodNum[%u] "
        "moduleIdx[%u] "
        "isDiffDeviceModule[%d] multiModuleDiffDeviceNumMode[%d] multiSuperPodDiffServerNumMode[%d] "
        "isHCCSSWNumEqualToTwiceSIONum[%d],",
        topoInfo->userRank, topoInfo->userRankSize, topoInfo->serverIdx, topoInfo->superPodIdx, topoInfo->deviceType,
        topoInfo->deviceNumPerModule, topoInfo->serverNumPerSuperPod, topoInfo->serverNum, topoInfo->moduleNum,
        topoInfo->superPodNum, topoInfo->moduleIdx, topoInfo->isDiffDeviceModule,
        topoInfo->multiModuleDiffDeviceNumMode, topoInfo->multiSuperPodDiffServerNumMode,
        topoInfo->isHCCSSWNumEqualToTwiceSIONum);
    return HCCL_SUCCESS;
}

HcclResult InitRankInfo(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    CHK_RET(InitRankInfo(comm, static_cast<TopoInfo*>(topoInfo)));
    CHK_RET(CalcTopoShape(comm, topoInfo));
    return HCCL_SUCCESS;
}

HcclResult CalcMyRankInfo(HcclComm comm, TopoInfo* topoInfo)
{
    CHK_RET(HcclGetRankSize(comm, &(topoInfo->userRankSize)));
    CHK_RET(HcclGetRankId(comm, &(topoInfo->userRank)));
    CHK_RET(hrtGetDeviceType(topoInfo->deviceType));
    uint32_t* netlayers = nullptr;
    uint32_t netLayersNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netlayers, &netLayersNum));

    // 获取moduleIdx
    CHK_RET(CalcGroupIdx(comm, topoInfo, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0)));
    // 获取superPodIdx
    if ((netLayersNum >= NET_LAYER_NUM_TWO) &&
        (netlayers[netLayersNum - 1] == static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L1))) {
        CHK_RET(CalcGroupIdx(comm, topoInfo, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L1)));
    } else {
        topoInfo->superPodIdx = 0;
    }
    HCCL_DEBUG(
        "[CalcMyRankInfo]userRank[%u], userRankSize[%u], deviceType[%d], netLayersNum[%u], moduleIdx[%u] and "
        "superPodIdx[%u]",
        topoInfo->userRank, topoInfo->userRankSize, topoInfo->deviceType, netLayersNum, topoInfo->moduleIdx,
        topoInfo->superPodIdx);
    return HCCL_SUCCESS;
}

HcclResult SetServerModuleInfo(HcclComm comm, TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter)
{
    topoInfo->isDiffDeviceModule = IsDiffDeviceModule(topoInfo, pairLinkCounter);
    CHK_RET(GetModuleIdx(comm, topoInfo));
    HCCL_DEBUG(
        "[SetServerModuleInfo]isDiffDeviceModule[%u], moduleIdx[%u]", topoInfo->isDiffDeviceModule,
        topoInfo->moduleIdx);
    // 910B A+X场景下RankGraph在初始化时已经通过GetModuleIdx刷新moduleIdx与serverIdx关系, 新接口应当不感知
    std::map<u32, std::vector<u32>> moduleMap;
    CHK_RET(GetModuleMap(comm, topoInfo, moduleMap));
    CHK_RET(GetDeviceNumPerModule(comm, topoInfo, moduleMap));
    topoInfo->moduleNum = moduleMap.size();

    topoInfo->multiModuleDiffDeviceNumMode = false;
    for (u32 i = 0; i < moduleMap.size(); ++i) {
        if (moduleMap[i].size() != topoInfo->deviceNumPerModule) {
            topoInfo->multiModuleDiffDeviceNumMode = true;
        }
        HCCL_INFO("module[%u] contains [%d]devices", i, moduleMap[i].size());
    }

    HCCL_RUN_INFO("different module contains different numbers of cards:[%d]", topoInfo->multiModuleDiffDeviceNumMode);
    return HCCL_SUCCESS;
}

/* 超节点数目以及超节点间server数解析 */
HcclResult SetSuperPodInfo(HcclComm comm, TopoInfo* topoInfo)
{
    topoInfo->multiSuperPodDiffServerNumMode = false;

    uint32_t level0RankListNum = 0;
    uint32_t level1RankListNum = 0;
    uint32_t* level0SizeList = nullptr;
    uint32_t* level1SizeList = nullptr; // 每个超节点里的rankSize {8, 8}
    std::vector<uint32_t> superPodToServerNum;
    uint32_t* netlayers = nullptr;
    uint32_t netLayersNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netlayers, &netLayersNum));
    if (netLayersNum == NET_LAYER_NUM_THREE) {
        CHK_RET(HcclRankGraphGetInstSizeListByLayer(
            comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &level0SizeList, &level0RankListNum));
        for (uint32_t i = 0; i < level0RankListNum; i++) {
            HCCL_DEBUG(
                "[SetSuperPodInfo]netLayer[%u] level0RankListNum[%u] level0SizeList[%u]=[%u]",
                static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), level0RankListNum, i, level0SizeList[i]);
        }
        CHK_RET(HcclRankGraphGetInstSizeListByLayer(
            comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L1), &level1SizeList, &level1RankListNum));
        for (uint32_t i = 0; i < level1RankListNum; i++) {
            HCCL_DEBUG(
                "[SetSuperPodInfo]netLayer[%u] level1RankListNum[%u] level1SizeList[%u]=[%u]",
                static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L1), level1RankListNum, i, level1SizeList[i]);
        }
        topoInfo->superPodNum = level1RankListNum;
        // 根据level0SizeList, level2SizeList, 重新构造superPodToServerNum
        std::vector<uint32_t> level0SizeListVector(level0SizeList, level0SizeList + level0RankListNum);
        std::vector<uint32_t> level1SizeListVector(level1SizeList, level1SizeList + level1RankListNum);
        CHK_RET(CalculateServersPerSuperPod(level0SizeListVector, level1SizeListVector, superPodToServerNum));
        for (uint32_t i = 0; i < superPodToServerNum.size(); i++) {
            HCCL_DEBUG("[SetSuperPodInfo]superpod[%u]: severNum[%u]", i, superPodToServerNum[i]);
        }
        topoInfo->serverNumPerSuperPod = superPodToServerNum[topoInfo->superPodIdx];
        HCCL_DEBUG(
            "level0RankListNum[%u], level1RankListNum[%u], set superPodNum[%u], serverNumPerSuperPod[%u]",
            level0RankListNum, level1RankListNum, topoInfo->superPodNum, topoInfo->serverNumPerSuperPod);
    } else if (netLayersNum == NET_LAYER_NUM_TWO) {
        CHK_RET(HcclRankGraphGetInstSizeListByLayer(
            comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &level0SizeList, &level0RankListNum));
        for (uint32_t i = 0; i < level0RankListNum; i++) {
            HCCL_DEBUG(
                "[SetSuperPodInfo]netLayer[%u] level0RankListNum[%u] level0SizeList[%u]=[%u]",
                static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), level0RankListNum, i, level0SizeList[i]);
        }
        topoInfo->superPodNum = 1;
        topoInfo->serverNumPerSuperPod = level0RankListNum;
        HCCL_DEBUG(
            "[SetSuperPodInfo]level0RankListNum[%u], set superPodNum[%u], serverNumPerSuperPod[%u]", level0RankListNum,
            topoInfo->superPodNum, topoInfo->serverNumPerSuperPod);
        return HCCL_SUCCESS;
    } else {
        topoInfo->superPodNum = 1;
        topoInfo->serverNumPerSuperPod = 1;
        HCCL_DEBUG(
            "[SetSuperPodInfo]level0RankListNum[%u], set superPodNum[%u], serverNumPerSuperPod[%u]", level0RankListNum,
            topoInfo->superPodNum, topoInfo->serverNumPerSuperPod);
        return HCCL_SUCCESS;
    }
    // 根据superPodToServerNum判断多个超节点内的sever数是否一致
    for (size_t i = 1; i < superPodToServerNum.size(); ++i) {
        if (superPodToServerNum[i] != superPodToServerNum[0]) {
            topoInfo->multiSuperPodDiffServerNumMode = true;
        }
    }
    HCCL_RUN_INFO(
        "[Set][SuperPodInfo]different surperPod contains different numbers of servers:[%d]",
        topoInfo->multiSuperPodDiffServerNumMode);

    // 跨超Server数非对称场景走NHR-HCF算法，该不存在server数不一致场景
    if (!topoInfo->multiModuleDiffDeviceNumMode && topoInfo->multiSuperPodDiffServerNumMode) {
        topoInfo->serverNumPerSuperPod = CalGCD(superPodToServerNum);
        topoInfo->multiSuperPodDiffServerNumMode = false;
        topoInfo->superPodNum = topoInfo->serverNum / topoInfo->serverNumPerSuperPod;
        HCCL_RUN_INFO(
            "[SuperPodInfo] gcdServerNumPerSuperPod[%u] original superPodNum[%u] "
            "converted superPodNum[%u]",
            topoInfo->serverNumPerSuperPod, level1RankListNum, topoInfo->superPodNum);
    }

    return HCCL_SUCCESS;
}

/* 用于标识集群中是否存在A2 A+X形态 */
bool IsDiffDeviceModule(const TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter)
{
    bool isDiffMeshAggregation = false;
    if (topoInfo->deviceType != DevType::DEV_TYPE_910B || topoInfo->userRankSize == 0) {
        HCCL_INFO(
            "[IsDiffDeviceModule] deviceType[%d], topoInfo->userRankSize[%u]", topoInfo->deviceType,
            topoInfo->userRankSize);
        return false;
    }
    // 统计除HCCS外的所有非HCCS通信协议链路的总数
    // 如果总计数大于零，则表示拓扑中存在不同的设备模块（将isDiffMeshAggregation标志设置为true）
    u32 count = 0;
    u32 excludedKey = static_cast<u32>(CommProtocol::COMM_PROTOCOL_HCCS);
    for (const auto& pair : pairLinkCounter) {
        if (pair.first != excludedKey) {
            count += pair.second;
            HCCL_INFO("[IsDiffDeviceModule] Found key[%u]-value[%u] pair", pair.first, pair.second);
        }
    }
    if (count != 0) {
        isDiffMeshAggregation = true;
    }
    return isDiffMeshAggregation;
}

HcclResult CalcLinkInfo(TopoInfo* topoInfo, const std::unordered_map<u32, u32>& pairLinkCounter)
{
    // 解析得到各类算法需要的信息
    u32 hccsSWNum = 0;
    auto it = pairLinkCounter.find(static_cast<u32>(CommProtocol::COMM_PROTOCOL_HCCS));
    if (it != pairLinkCounter.end()) {
        hccsSWNum = it->second;
    }

    u32 sioNum = 0;
    it = pairLinkCounter.find(static_cast<u32>(CommProtocol::COMM_PROTOCOL_SIO));
    if (it != pairLinkCounter.end()) {
        sioNum = it->second;
    }
    HCCL_DEBUG(
        "[CalcLinkInfo] hccsSWNum[%u], sioNum[%u], deviceNumPerModule[%u]", hccsSWNum, sioNum,
        topoInfo->deviceNumPerModule);
    if (hccsSWNum == 0 || sioNum == 0) {
        topoInfo->isHCCSSWNumEqualToTwiceSIONum = false;
    } else {
        topoInfo->isHCCSSWNumEqualToTwiceSIONum =
            (hccsSWNum == (topoInfo->deviceNumPerModule - DEVICE_NO_HCCS_LINK_COUNT) * topoInfo->deviceNumPerModule) &&
            (sioNum == topoInfo->deviceNumPerModule);
    }
    return HCCL_SUCCESS;
}

HcclResult CalcGroupIdx(HcclComm comm, TopoInfo* topoInfo, uint32_t netLayer)
{
    uint32_t rankListNum;
    uint32_t* rankSizeList;
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, netLayer, &rankSizeList, &rankListNum));
    for (uint32_t i = 0; i < rankListNum; i++) {
        HCCL_DEBUG(
            "[CalcGroupIdx]netLayer[%u] rankListNum[%u] rankSizeList[%u]=[%u]", netLayer, rankListNum, i,
            rankSizeList[i]);
    }
    uint32_t currentGroup = 0;
    uint32_t cumulativeRank = 0;

    for (uint32_t i = 0; i < rankListNum; ++i) {
        cumulativeRank += rankSizeList[i];
        if (topoInfo->userRank < cumulativeRank) {
            currentGroup = i;
            break;
        }
    }
    HCCL_DEBUG("[CalcGroupIdx]currentGroup[%u]", currentGroup);
    if (netLayer == static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0)) {
        topoInfo->serverIdx = currentGroup;
        topoInfo->serverNum = rankListNum;
        HCCL_DEBUG(
            "[CalcGroupIdx]netLayer[%u] currentGroup[%u] serverIdx[%u] serverNum[%u]", netLayer, currentGroup,
            topoInfo->serverIdx, topoInfo->serverNum);
    } else if (
        (netLayer == static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L1)) ||
        (netLayer == static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L3))) {
        topoInfo->superPodIdx = currentGroup;
        HCCL_DEBUG(
            "[CalcGroupIdx]netLayer[%u] currentGroup[%u] superPodIdx[%u]", netLayer, currentGroup,
            topoInfo->superPodIdx);
    } else {
        HCCL_ERROR("[CalcGroupIdx]netLayer[%u] is not supported", netLayer);
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult GetPairLinkCounter(HcclComm comm, TopoInfo* topoInfo, std::unordered_map<u32, u32>& pairLinkCounter)
{
    // 需要当前sever里的pairLinkCounter
    pairLinkCounter[static_cast<u32>(CommProtocol::COMM_PROTOCOL_HCCS)] = 0;
    pairLinkCounter[static_cast<u32>(CommProtocol::COMM_PROTOCOL_ROCE)] = 0;
    pairLinkCounter[static_cast<u32>(CommProtocol::COMM_PROTOCOL_PCIE)] = 0;
    pairLinkCounter[static_cast<u32>(CommProtocol::COMM_PROTOCOL_SIO)] = 0;

    // 首先获取当前rank所在服务器的信息，确定服务器的起始和结束rank
    uint32_t currentServerStartRank = GetCurrentServerStartRank(comm, topoInfo);
    uint32_t currentServerEndRank = GetCurrentServerEndRank(comm, topoInfo);

    for (u32 srcRank = currentServerStartRank; srcRank < currentServerEndRank; ++srcRank) {
        for (u32 dstRank = currentServerStartRank; dstRank < currentServerEndRank; ++dstRank) {
            if (srcRank == dstRank) {
                continue;
            }
            CommLink* linkList = nullptr; // 必须初始化为nullptr
            uint32_t listSize = 0;
            HCCL_DEBUG("[GetPairLinkCounter]Getting links between srcRank[%u] and dstRank[%u]", srcRank, dstRank);
            CHK_RET(HcclRankGraphGetLinks(
                comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), srcRank, dstRank, &linkList, &listSize));
            // 如果listSize为0，表示这两个rank之间没有直接link，直接进入下一轮循环
            if (listSize == 0) {
                HCCL_DEBUG("[GetPairLinkCounter]No links found between srcRank[%u] and dstRank[%u]", srcRank, dstRank);
                continue;
            }
            // =======================================================
            // 关键部分：遍历获取到的 linkList
            // =======================================================
            for (uint32_t i = 0; i < listSize; ++i) {
                CommLink& currentLink = linkList[i]; // 获取当前循环到的链路对象

                // 双向兼容处理：先处理版本号1的字段
                if (currentLink.header.version >= 1) {
                    // --- 在这里处理 currentLink ---
                    HCCL_DEBUG(
                        "Link[%u] found between srcRank[%u] and dstRank[%u]:"
                        "LinkType: %u, srcEndpointDesc: %u, dstEndpointDesc: %u",
                        i, srcRank, dstRank, currentLink.linkAttr.linkProtocol, currentLink.srcEndpointDesc,
                        currentLink.dstEndpointDesc);
                    // 可以将链路类型统计起来
                    // 原始代码中的 pairLinkCounter 应该在这里使用
                    pairLinkCounter[static_cast<u32>(currentLink.linkAttr.linkProtocol)]++;
                }
                // 未来处理版本2及之后
            }
        }
    }
    for (auto it : pairLinkCounter) {
        HCCL_DEBUG("[GetPairLinkCounter] pair link counter information linkType[%u], size[%u]", it.first, it.second);
    }
    return HCCL_SUCCESS;
}

// 获取当前服务器的startRank
uint32_t GetCurrentServerStartRank(HcclComm comm, const TopoInfo* topoInfo)
{
    uint32_t rankListNum = 0;
    uint32_t* rankSizeList = nullptr;

    // 获取L0层级（服务器级别）的实例大小列表
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(
        comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &rankSizeList, &rankListNum));

    // 确定当前rank属于哪个服务器
    uint32_t currentServerStartRank = 0;
    for (u32 i = 0; i < topoInfo->serverIdx; ++i) {
        currentServerStartRank += rankSizeList[i];
    }
    return currentServerStartRank;
}

// 获取当前服务器的EndRank
uint32_t GetCurrentServerEndRank(HcclComm comm, const TopoInfo* topoInfo)
{
    uint32_t rankListNum = 0;
    uint32_t* rankSizeList = nullptr;

    // 获取L0层级（服务器级别）的实例大小列表
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(
        comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &rankSizeList, &rankListNum));

    // 确定当前rank属于哪个服务器
    uint32_t currentServerStartRank = 0;
    for (u32 i = 0; i < topoInfo->serverIdx; ++i) {
        currentServerStartRank += rankSizeList[i];
    }
    uint32_t currentServerCount = rankSizeList[topoInfo->serverIdx];
    uint32_t currentServerEndRank = currentServerStartRank + currentServerCount;
    return currentServerEndRank;
}

HcclResult GetDeviceNumPerModule(HcclComm comm, TopoInfo* topoInfo, std::map<u32, std::vector<u32>>& moduleMap)
{
    if (topoInfo->deviceType == DevType::DEV_TYPE_910B && topoInfo->isDiffDeviceModule) {
        // 根据生成好的moduleMap计算当前rank所在module的设备数
        uint32_t srcRank = topoInfo->userRank;
        uint32_t moduleIdx = 0;
        HcclResult ret = GetModuleIdxByRank(comm, srcRank, topoInfo, moduleIdx);
        if (ret != HCCL_SUCCESS) {
            HCCL_ERROR("[GetDeviceNumPerModule]Failed to get moduleIdx for rank[%u]", srcRank);
            return ret;
        }

        // 从moduleMap中获取当前rank所在module的设备数量
        auto it = moduleMap.find(moduleIdx);
        if (it != moduleMap.end()) {
            topoInfo->deviceNumPerModule = static_cast<u32>(it->second.size());
        } else {
            HCCL_ERROR("[GetDeviceNumPerModule]ModuleIdx[%u] not found in moduleMap", moduleIdx);
            return HCCL_E_PARA;
        }
    } else {
        uint32_t rankNum = 0;
        CHK_RET(HcclRankGraphGetRankSizeByLayer(comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &rankNum));
        topoInfo->deviceNumPerModule = rankNum;
    }
    return HCCL_SUCCESS;
}

HcclResult GetModuleMap(HcclComm comm, TopoInfo* topoInfo, std::map<u32, std::vector<u32>>& moduleMap)
{
    // 遍历每一个rank使用GetModuleIdxByRank获取每一个rank的moduleIdx
    for (u32 rank = 0; rank < topoInfo->userRankSize; ++rank) {
        u32 moduleIdx = 0;
        HcclResult ret = GetModuleIdxByRank(comm, rank, topoInfo, moduleIdx);
        if (ret != HCCL_SUCCESS) {
            HCCL_ERROR("[GetModuleMap]Failed to get moduleIdx for rank[%u]", rank);
            return ret;
        }
        moduleMap[moduleIdx].push_back(rank);
    }

    // 打印moduleMap
    HCCL_DEBUG("[GetModuleMap] ModuleMap:");
    for (const auto& pair : moduleMap) {
        std::string ranksStr = "{";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            if (i > 0) {
                ranksStr += ", ";
            }
            ranksStr += std::to_string(pair.second[i]);
        }
        ranksStr += "}";
        HCCL_DEBUG("[GetModuleMap] ModuleIdx[%u]: %s", pair.first, ranksStr.c_str());
    }

    return HCCL_SUCCESS;
}

HcclResult GetModuleIdx(HcclComm comm, TopoInfo* topoInfo)
{
    uint32_t moduleIdx = 0;
    HcclResult ret = GetModuleIdxByRank(comm, topoInfo->userRank, topoInfo, moduleIdx);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("[GetModuleIdx]Failed to get moduleIdx for rank[%u]", topoInfo->userRank);
        return ret;
    }
    topoInfo->moduleIdx = moduleIdx;
    return HCCL_SUCCESS;
}

HcclResult GetModuleIdxByRank(HcclComm comm, uint32_t rank, const TopoInfo* topoInfo, uint32_t& moduleIdx)
{
    uint32_t rankServerIdx = 0;
    uint32_t accumulatedRanks = 0;
    uint32_t rankListNum = 0;
    uint32_t* rankSizeList = nullptr;

    CHK_RET(HcclRankGraphGetInstSizeListByLayer(
        comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), &rankSizeList, &rankListNum));

    for (u32 i = 0; i < rankListNum; ++i) {
        if (rank < accumulatedRanks + rankSizeList[i]) {
            rankServerIdx = i;
            break;
        }
        accumulatedRanks += rankSizeList[i];
    }
    if (topoInfo->deviceType == DevType::DEV_TYPE_910B && topoInfo->isDiffDeviceModule) {
        // 计算给定rank所在server的起始rank
        // 这里需要根据给定的rank确定其对应的server索引

        uint32_t dstRank = accumulatedRanks; // 目标server的起始rank
        uint32_t srcRank = rank;             // 源rank
        CommLink* linkList = nullptr;        // 必须初始化为nullptr
        uint32_t listSize = 0;
        uint32_t rankModuleIdx = 1;
        if (srcRank != dstRank) {
            HCCL_DEBUG("[GetModuleIdxByRank]Getting links between srcRank[%u] and dstRank[%u]", srcRank, dstRank);
            CHK_RET(HcclRankGraphGetLinks(
                comm, static_cast<uint32_t>(HcclNetLayer::HCCL_NetLayer_L0), srcRank, dstRank, &linkList, &listSize));
            for (uint32_t i = 0; i < listSize; ++i) {
                CommLink& currentLink = linkList[i]; // 获取当前循环到的链路对象

                HCCL_DEBUG("  Link[%u] found between srcRank[%u] and dstRank[%u]:", i, srcRank, dstRank);
                HCCL_DEBUG("    LinkType: %u", currentLink.linkAttr.linkProtocol);

                if (currentLink.linkAttr.linkProtocol == CommProtocol::COMM_PROTOCOL_HCCS) {
                    rankModuleIdx = 0;
                    break;
                }
            }
        } else {
            rankModuleIdx = 0;
        }
        moduleIdx = rankServerIdx * FACTOR_NUM_TWO + rankModuleIdx;
    } else {
        // 对于非 910B 或者非不同设备模块的情况，moduleIdx 等于 serverIdx
        // 需要确定给定rank对应的serverIdx
        moduleIdx = rankServerIdx;
    }
    return HCCL_SUCCESS;
}

HcclResult CalculateServersPerSuperPod(
    const std::vector<uint32_t>& l0Sizes, const std::vector<uint32_t>& l1Sizes,
    std::vector<uint32_t>& serversPerSuperPod)
{
    if (l0Sizes.empty() || l1Sizes.empty()) {
        HCCL_ERROR("[CalculateServersPerSuperPod]l0Sizes.size[%u], l1Sizes.size[%u]", l0Sizes.size(), l1Sizes.size());
        return HCCL_E_PARA;
    }

    // L0层(服务器)的总rank数应该等于L1层(超节点)的总rank数
    uint32_t totalL0Ranks = 0;
    for (uint32_t size : l0Sizes) {
        totalL0Ranks += size;
    }

    uint32_t totalL1Ranks = 0;
    for (uint32_t size : l1Sizes) {
        totalL1Ranks += size;
    }

    if (totalL0Ranks != totalL1Ranks) {
        // 这表明拓扑数据不一致
        HCCL_ERROR("[CalculateServersPerSuperPod]totalL0Ranks[%u] != totalL1Ranks[%u]", totalL0Ranks, totalL1Ranks);
        return HCCL_E_PARA;
    }

    // 通过将L0组映射到L1组来计算每个超节点的服务器数，使用累计和方法
    uint32_t l0Index = 0;
    uint32_t cumulativeL0Ranks = 0; // 累计已处理的L0 ranks数量

    for (uint32_t i = 0; i < l1Sizes.size(); ++i) {
        uint32_t targetCumulative = cumulativeL0Ranks + l1Sizes[i]; // 当前L1组所需的累计rank数
        uint32_t serversInCurrentSuperPod = 0;

        // 遍历L0组，直到累计的ranks数达到目标
        while (cumulativeL0Ranks < targetCumulative && l0Index < l0Sizes.size()) {
            // 如果当前L0组的ranks数不超过目标，直接添加整个L0组
            if (cumulativeL0Ranks + l0Sizes[l0Index] <= targetCumulative) {
                cumulativeL0Ranks += l0Sizes[l0Index];
                serversInCurrentSuperPod++; // 使用了一个完整的L0组（一个服务器）
                l0Index++;
            } else {
                HCCL_WARNING(
                    "[CalculateServersPerSuperPod]cumulativeL0Ranks:[%u] + l0Sizes[%u]:[%u] > targetCumulative:[%u], "
                    "which is equal to cumulativeL0Ranks:[%u] + l1Sizes[%u]:[%u]",
                    cumulativeL0Ranks, l0Index, l0Sizes[l0Index], targetCumulative, cumulativeL0Ranks, i, l1Sizes[i]);
                // 当前L0组的部分ranks被用于当前L1组，仍计为使用了一个服务器
                // 这种情况下我们只使用了当前L0组的一部分来达到目标
                cumulativeL0Ranks = targetCumulative;
                serversInCurrentSuperPod++; // 计数增加
                l0Index++;                  // 移动到下一个L0组
                break;                      // 已达到目标
            }
        }
        serversPerSuperPod.push_back(serversInCurrentSuperPod);
    }
    return HCCL_SUCCESS;
}

HcclResult CalcLevel0TopoShape(const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    static_cast<void>(comm);
    u32 netLayer = 0;
    CHK_PRT_RET(
        topoInfo->topoInstDetailsOfLayer.size() <= netLayer,
        HCCL_ERROR("[BaseSelector][CalcLevel0TopoShape] topoInstNumOfLayer size[%u] <= netLayer[%u]", netLayer),
        HCCL_E_INTERNAL);
    TopoInstDetails& level0TopoInstDetails = topoInfo->topoInstDetailsOfLayer[netLayer];
    CHK_PRT_RET(
        topoInfo->netLayerDetails.localNetInsSizeOfLayer.size() <= netLayer,
        HCCL_ERROR("[BaseSelector][CalcLevel0TopoShape] localNetInsSizeOfLayer size[%u] <= netLayer[%u]", netLayer),
        HCCL_E_INTERNAL);
    u32 level0LocalRankSize = topoInfo->netLayerDetails.localNetInsSizeOfLayer[netLayer];

    auto& topoInstNum = level0TopoInstDetails.topoInstNum;
    auto& rankNumForTopoType = level0TopoInstDetails.rankNumForTopoType;

    if (topoInstNum == 1 && rankNumForTopoType[CommTopo::COMM_TOPO_1DMESH].size() == 1) {
        // MESH_1D 拓扑校验
        CHK_PRT_RET(
            rankNumForTopoType[CommTopo::COMM_TOPO_1DMESH][0] != level0LocalRankSize,
            HCCL_ERROR(
                "[BaseSelector][CalcLevel0TopoShape] MESH_1D rankSize[%u] is not equal to level0LocalRankSize[%u]",
                rankNumForTopoType[CommTopo::COMM_TOPO_1DMESH][0], level0LocalRankSize),
            HCCL_E_INTERNAL);
        topoInfo->level0Topo = Level0Shape::MESH_1D;
        return HCCL_SUCCESS;
    } else if (topoInstNum == 1 && rankNumForTopoType[CommTopo::COMM_TOPO_CLOS].size() == 1) {
        // CLOS 拓扑校验
        CHK_PRT_RET(
            rankNumForTopoType[CommTopo::COMM_TOPO_CLOS][0] != level0LocalRankSize,
            HCCL_ERROR(
                "[BaseSelector][CalcLevel0TopoShape] CLOS rankSize[%u] is not equal to level0LocalRankSize[%u]",
                rankNumForTopoType[CommTopo::COMM_TOPO_CLOS][0], level0LocalRankSize),
            HCCL_E_INTERNAL);
        topoInfo->level0Topo = Level0Shape::CLOS;
        return HCCL_SUCCESS;
    } else if (
        topoInstNum == TOPO_INST_NUM_MESH_1D_CLOS && rankNumForTopoType[CommTopo::COMM_TOPO_CLOS].size() == 1 &&
        rankNumForTopoType[CommTopo::COMM_TOPO_1DMESH].size() == 1) {
        if (rankNumForTopoType[CommTopo::COMM_TOPO_CLOS].at(0) > BIG_CLOS_RANGE) {
            topoInfo->level0BigClosRange = true;
        }
        // MESH_1D_CLOS 拓扑校验
        CHK_PRT_RET(
            rankNumForTopoType[CommTopo::COMM_TOPO_CLOS][0] != level0LocalRankSize,
            HCCL_ERROR(
                "[BaseSelector][CalcLevel0TopoShape] CLOS rankSize[%u] is not equal to level0LocalRankSize[%u]",
                rankNumForTopoType[CommTopo::COMM_TOPO_CLOS][0], level0LocalRankSize),
            HCCL_E_INTERNAL);
        topoInfo->level0Topo = Level0Shape::MESH_1D_CLOS;
        return HCCL_SUCCESS;
    }
    topoInfo->level0Topo = Level0Shape::CLOS; // A2场景不匹配默认为Clos
    HCCL_WARNING("Unknown topo for level 0, topoInstNum[%u], default topo:%d", topoInstNum, topoInfo->level0Topo);
    return HCCL_SUCCESS;
}

// 计算 Level1 NHR 标记：当 Level0 GCD 为 1 时，Mesh 无意义，需要退化为单级 NHR
static HcclResult CalcLevel1Nhr(const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    if (topoInfo->topoLevelNums > 1 && !topoInfo->netLayerDetails.instSizeListOfLayer[0].empty()) {
        const auto& instSizeList = topoInfo->netLayerDetails.instSizeListOfLayer[0];
        u32 gcd = instSizeList[0];
        for (size_t i = 1; i < instSizeList.size(); ++i) {
            u32 a = gcd, b = instSizeList[i];
            while (b != 0) {
                a %= b;
                std::swap(a, b);
            }
            gcd = a;
            if (gcd == 1) {
                break; // 早期终止
            }
        }
        if (gcd == 1) {
            topoInfo->Level1Nhr = true;
            HCCL_INFO("[TopoHost][CalcLevel1Nhr] Level0 GCD is 1, set Level1Nhr to true");
        }
    }
    return HCCL_SUCCESS;
}

static HcclResult CalcLevel1Hd(TopoInfoWithNetLayerDetails* topoInfo)
{
    if (topoInfo->topoLevelNums <= 1 || topoInfo->netLayerDetails.netLayers.size() <= 1) {
        return HCCL_SUCCESS;
    }
    const u32 level1Idx = topoInfo->netLayerDetails.netLayers[1];
    CHK_PRT_RET(
        topoInfo->topoInstDetailsOfLayer.size() <= level1Idx,
        HCCL_WARNING(
            "[TopoHost][CalcLevel1Hd] topoInstDetailsOfLayer size[%zu] <= level1Idx[%u]",
            topoInfo->topoInstDetailsOfLayer.size(), level1Idx),
        HCCL_SUCCESS);

    const auto& rankNumForTopoType = topoInfo->topoInstDetailsOfLayer[level1Idx].rankNumForTopoType;
    auto closIter = rankNumForTopoType.find(CommTopo::COMM_TOPO_CLOS);
    if (closIter == rankNumForTopoType.end() || closIter->second.size() != 1) {
        return HCCL_SUCCESS;
    }

    const u32 closRankSize = closIter->second[0];
    topoInfo->Level1Hd = closRankSize != 0 && (closRankSize & (closRankSize - 1)) == 0;
    HCCL_INFO(
        "[TopoHost][CalcLevel1Hd] level1Idx[%u], closRankSize[%u], Level1Hd[%d]", level1Idx, closRankSize,
        topoInfo->Level1Hd);
    return HCCL_SUCCESS;
}

/**
 * 查询本卡是否为POD机型。恒返回HCCL_SUCCESS: 该字段是纯附加信息, 取不到时停在false,
 * 现有字段与旧执行路径完全不受影响。
 */
HcclResult CalcDeviceFormFactor(TopoInfoWithNetLayerDetails* topoInfo)
{
    CHK_PTR_NULL(topoInfo);
    topoInfo->isPod = false;
#ifndef AICPU_COMPILE
#if !HCCL_SUPPORT_DEV_FORM_FACTOR
    // 老CANN的acl_rt.h没有ACL_DEV_ATTR_DEVICE_FORM_FACTOR, 无从查起, 一律按非POD建模。
    // 与成功路径同为INFO同前缀: 现场grep一次就能分清走的是哪一条
    HCCL_INFO("[Topo][CalcDeviceFormFactor] acl has no device form factor attr, isPod stays false");
#else
    // aclrtGetDeviceInfo的deviceId形参就是userDevId(aclrtGetDevice的返回值), ACL内部自己做user->logic映射。
    // 这里不能再调aclrtGetLogicDevIdByUserDevId转一道: 配了ASCEND_RT_VISIBLE_DEVICES时会二次映射, 读到另一张卡
    s32 userDevId = 0;
    aclError aclRet = aclrtGetDevice(&userDevId);
    if (aclRet != ACL_SUCCESS) {
        HCCL_WARNING("[Topo][CalcDeviceFormFactor] get current device failed, ret[%d]. isPod stays false.", aclRet);
        return HCCL_SUCCESS;
    }

    s64 val = 0;
    // quiet: 老驱动不支持该infoType时会稳定失败, 按ERROR打会在正常的老环境上持续刷错误日志
    HcclResult ret = hcalrtGetDeviceInfo(static_cast<u32>(userDevId), ACL_DEV_ATTR_DEVICE_FORM_FACTOR, val, true);
    if (ret != HCCL_SUCCESS) {
        HCCL_WARNING(
            "[Topo][CalcDeviceFormFactor] get device form factor failed, ret[%d], userDevId[%d]. "
            "isPod stays false.",
            ret, userDevId);
        return HCCL_SUCCESS;
    }

    // 必须是严格相等的正向判断: ACL将来新增形态时, 未识别的取值必须落到false一侧
    topoInfo->isPod = (val == ACL_DEVICE_FORM_FACTOR_POD);
    // 原始取值和设备号都打出来: 现场据此分辨"确实不是POD"还是"取到了个没见过的形态"
    HCCL_INFO(
        "[Topo][CalcDeviceFormFactor] userDevId[%d] formFactor[%ld] isPod[%d]", userDevId, val,
        static_cast<s32>(topoInfo->isPod));
#endif // HCCL_SUPPORT_DEV_FORM_FACTOR
#endif // AICPU_COMPILE
    return HCCL_SUCCESS;
}

HcclResult CalcTopoShape(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    CHK_RET(ExtractNetLayerDetails(comm, topoInfo));
    CHK_RET(CalcLevel1Nhr(comm, topoInfo));
    CHK_RET(ExtractTopoDetails(comm, topoInfo));
    CHK_RET(CalcLevel1Hd(topoInfo));
    CHK_RET(CalcLevel0TopoShape(comm, topoInfo));
    CHK_RET(Is2DieFullMesh(comm, topoInfo));
    CHK_RET(IsLevel0PcieMix(comm, topoInfo));
    CHK_RET(CalcLevel0MeshType(comm, topoInfo));
    // 与comm无关, 只查本卡; 恒返回HCCL_SUCCESS, 取不到时停在false
    CHK_RET(CalcDeviceFormFactor(topoInfo));
    // 放在最后: 现有字段的提取与派生逻辑完全不受影响, 且可复用已提取的netLayerDetails。
    // BuildPhysicalLevels恒返回HCCL_SUCCESS, 内部失败一律降级为空视图
    CHK_RET(BuildPhysicalLevels(comm, topoInfo));
    return HCCL_SUCCESS;
}

HcclResult ExtractNetLayerDetails(const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    CHK_PRT_RET(comm == nullptr, HCCL_ERROR("[Topo][ExtractNetLayerDetails] comm is null"), HCCL_E_PTR);

    auto& topoLevelNum = topoInfo->topoLevelNums;
    auto& netLayerNum = topoInfo->netLayerDetails.netLayerNum;
    auto& netLayers = topoInfo->netLayerDetails.netLayers;
    auto& netInstNumOfLayer = topoInfo->netLayerDetails.netInstNumOfLayer;
    auto& instSizeListOfLayer = topoInfo->netLayerDetails.instSizeListOfLayer;
    auto& localNetInsSizeOfLayer = topoInfo->netLayerDetails.localNetInsSizeOfLayer;

    uint32_t* netlayersTemp = nullptr;
    CHK_RET(HcclRankGraphGetLayers(comm, &netlayersTemp, &netLayerNum));
    for (uint32_t netLayerIdx = 0; netLayerIdx < netLayerNum; netLayerIdx++) {
        netLayers.push_back(netlayersTemp[netLayerIdx]);
    }
    // 取最高层级+1适配ranktable配置netLayers={0,3}的情况
    uint32_t actualLayerNum = netLayers[netLayerNum - 1] + 1;
    netInstNumOfLayer.resize(actualLayerNum);   // 每层网络中有几个网络实例
    instSizeListOfLayer.resize(actualLayerNum); // 每层网络中的各个网络实例的大小
    localNetInsSizeOfLayer.resize(actualLayerNum);

    HcclResult ret;
    // 获取并校验每一层的网路实例大小
    for (auto layerIdx : netLayers) {
        std::vector<u32>& currLayerInstSizeList = instSizeListOfLayer[layerIdx];
        u32& currLayerNetInstNum = netInstNumOfLayer[layerIdx];
        uint32_t* instSizeListSingleLevel = nullptr;
        CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, layerIdx, &instSizeListSingleLevel, &currLayerNetInstNum));
        for (uint32_t index = 0; index < currLayerNetInstNum; index++) {
            currLayerInstSizeList.push_back(instSizeListSingleLevel[index]);
        }
        u32 currLayerRankSize = std::accumulate(currLayerInstSizeList.begin(), currLayerInstSizeList.end(), 0);
        HCCL_INFO("[BaseSelector][ExtractNetLayerDetails] Net layer[%u] instNum[%u]", layerIdx, currLayerNetInstNum);
        CHK_PRT_RET(
            currLayerRankSize != topoInfo->userRankSize,
            HCCL_ERROR(
                "[BaseSelector][ExtractNetLayerDetails] NetLayer[%u], totalRankSize[%u] is not equal to comm "
                "rankSize[%u]",
                layerIdx, currLayerRankSize, topoInfo->userRankSize),
            HCCL_E_PARA);
        uint32_t rankNum = 0;
        uint32_t* ranks;
        CHK_RET(HcclRankGraphGetRanksByLayer(comm, layerIdx, &ranks, &rankNum));
        localNetInsSizeOfLayer[layerIdx] = rankNum;
    }

    topoLevelNum = 0;
    // 获取最小的能覆盖所有卡的 layer，topoLevelNum表示实际层级数量
    for (auto layerIdx : netLayers) {
        topoLevelNum++;
        if (netInstNumOfLayer[layerIdx] == 1) {
            // 当本层只有一个网络实例时, 认为已覆盖所有卡
            break;
        }
    }

    HCCL_INFO(
        "[BaseSelector][ExtractNetLayerDetails] topoLevelNum[%u], netLayerNum[%u], netLayers.size[%u]", topoLevelNum,
        netLayerNum, netLayers.size());

    CHK_PRT_RET(
        topoLevelNum == 0,
        HCCL_ERROR(
            "[BaseSelector][ExtractNetLayerDetails] topoLevelNum[%u] is invalid, netLayerNum[%u]", topoLevelNum,
            netLayerNum),
        HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult ExtractTopoDetails(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    HcclResult ret;
    CHK_PRT_RET(comm == nullptr, HCCL_ERROR("[Topo][ExtractNetLayerDetails] comm is null"), HCCL_E_PTR);
    u32 netLayerNum = topoInfo->netLayerDetails.netLayerNum;
    auto& netLayers = topoInfo->netLayerDetails.netLayers;
    uint32_t actualLayerNum = netLayers[netLayerNum - 1] + 1;

    // 初始化每一层的 TopoInstDetails
    topoInfo->topoInstDetailsOfLayer.resize(actualLayerNum);
    topoInfo->topoInstDetailsOfLayerSize = actualLayerNum;
    for (auto netLayerIdx : netLayers) {
        auto& currentNetLayerTopoTopoDetail = topoInfo->topoInstDetailsOfLayer[netLayerIdx];
        auto& currentLayerTopoSize = currentNetLayerTopoTopoDetail.sizeOfTopo;
        auto& currentLayerTopoType = currentNetLayerTopoTopoDetail.typeOfTopo;
        auto& currentLayerTopoRanks = currentNetLayerTopoTopoDetail.ranksInTopo;
        auto& currentLayerTopo2SizeMap = currentNetLayerTopoTopoDetail.rankNumForTopoType;
        auto& topoInstNum = currentNetLayerTopoTopoDetail.topoInstNum;

        std::vector<u32> topoInsts;
        uint32_t* topoInstsTemp = nullptr;
        HcclRankGraphGetTopoInstsByLayer(comm, netLayerIdx, &topoInstsTemp, &topoInstNum);
        for (uint32_t topoInstIdx = 0; topoInstIdx < topoInstNum; topoInstIdx++) {
            topoInsts.push_back(topoInstsTemp[topoInstIdx]);
        }
        HCCL_INFO("[BaseSelector][ExtractTopoDetails] netLayerIdx[%u], topoInstNum[%u]", netLayerIdx, topoInstNum);
        // 初始化当前层的拓扑信息
        currentLayerTopoSize.resize(topoInstNum);
        currentLayerTopoType.resize(topoInstNum);
        currentLayerTopoRanks.resize(topoInstNum);
        currentLayerTopo2SizeMap.clear();

        // 填充当前层的拓扑信息
        for (u32 topoInstIdx = 0; topoInstIdx < topoInstNum; topoInstIdx++) {
            u32& topoInstId = topoInsts[topoInstIdx];
            u32& topoSize = currentLayerTopoSize[topoInstIdx];
            CommTopo& topoType = currentLayerTopoType[topoInstIdx];
            std::vector<u32>& ranks = currentLayerTopoRanks[topoInstIdx];

            // 获取拓扑实例的类型
            ret = HcclRankGraphGetTopoType(comm, netLayerIdx, topoInstId, &topoType);
            CHK_PRT_RET(
                ret != HCCL_SUCCESS,
                HCCL_ERROR(
                    "[BaseSelector][ExtractTopoDetails] GetTopoType failed, netLayerIdx[%u], topoInstId[%u]",
                    netLayerIdx, topoInstId),
                ret);

            // 获取拓扑实例中包含的rank
            uint32_t* ranksTemp;
            uint32_t rankNum;
            HcclRankGraphGetRanksByTopoInst(comm, netLayerIdx, topoInstId, &ranksTemp, &rankNum);
            for (uint32_t rankIdx = 0; rankIdx < rankNum; rankIdx++) {
                ranks.push_back(ranksTemp[rankIdx]);
            }
            CHK_PRT_RET(
                ret != HCCL_SUCCESS,
                HCCL_ERROR(
                    "[BaseSelector][ExtractTopoDetails] GetRanksByTopoInst failed, netLayerIdx[%u], topoInstId[%u]",
                    netLayerIdx, topoInstId),
                ret);

            // 将topoInstId按照topoType进行归类
            currentLayerTopo2SizeMap[topoType].push_back(rankNum);

            HCCL_INFO(
                "[BaseSelector][ExtractTopoDetails] netLayerIdx[%u], topoInstIdx[%u] type is[%u], topoInstId is[%u], "
                "topoSize is[%u]",
                netLayerIdx, topoInstIdx, topoType, topoInstId, rankNum);
        }
    }
    return HCCL_SUCCESS;
}

HcclResult Is2DieFullMesh(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    topoInfo->is2DieFullMesh = false;
    if (topoInfo->level0Topo != Level0Shape::MESH_1D) {
        return HCCL_SUCCESS;
    }
    uint32_t myRank = topoInfo->userRank;
    u32 netLayer = 0; // 0 级拓扑
    uint32_t* ranks = nullptr;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByLayer(comm, netLayer, &ranks, &rankNum));
    if (rankNum <= 2) { // 小于2张卡的话，肯定不是2die全互连
        return HCCL_SUCCESS;
    }
    // 遍历所有对端，校验是否和所有卡有全连链路，并判断链路中本端端口所所对应的 CCU die 是否一致;
    u32 dieNum = 2; // 一共2个die
    std::vector<u32> dieLinkCounter(dieNum, 0);
    for (uint32_t rankIdx = 0; rankIdx < rankNum; rankIdx++) {
        if (ranks[rankIdx] == myRank) {
            continue;
        }
        CommLink* links = nullptr;
        uint32_t linkNum;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, ranks[rankIdx], &links, &linkNum));
        CHK_PTR_NULL(links);
        CHK_PRT_RET(
            linkNum == 0,
            HCCL_INFO(
                "[Topo][Is2DieFullMesh], Can not find path from Local[%u] to Rmt[%u], in netLayer %u. "
                "Topo is not mesh",
                myRank, ranks[rankIdx], netLayer),
            HCCL_E_INTERNAL);
        EndpointDesc srcEndPointDesc = links[0].srcEndpointDesc;
        EndpointAttrDieId dieId;
        uint32_t infoLen = sizeof(EndpointAttrDieId);
        CHK_RET(HcclRankGraphGetEndpointInfo(
            comm, myRank, &srcEndPointDesc, EndpointAttr::ENDPOINT_ATTR_DIE_ID, infoLen, &dieId));
        CHK_PRT_RET(
            dieId >= dieNum,
            HCCL_ERROR(
                "[Topo][Is2DieFullMesh], Link from Local[%u] to Rmt[%u] die id[%u] is out of range[%u].", myRank,
                ranks[rankIdx], dieId, dieNum),
            HCCL_E_INTERNAL);
        dieLinkCounter[dieId]++;
        HCCL_INFO(
            "[Topo][Is2DieFullMesh], Link from Local[%u] to Rmt[%u] use die[%u], current counter[%u]", myRank,
            ranks[rankIdx], dieId, dieLinkCounter[dieId]);
    }
    for (u32 i = 0; i < dieNum; i++) {
        if (dieLinkCounter[i] == 0) {
            return HCCL_SUCCESS;
        }
    }
    topoInfo->is2DieFullMesh = true;
    return HCCL_SUCCESS;
}

HcclResult CalcLevel0MeshType(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    if (topoInfo->level0Topo != Level0Shape::MESH_1D) {
        topoInfo->level0MeshType = Level0MeshType::NOT_MESH;
        return HCCL_SUCCESS;
    }
    uint32_t myRank = topoInfo->userRank;
    u32 netLayer = 0;
    uint32_t* ranks = nullptr;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByLayer(comm, netLayer, &ranks, &rankNum));
    if (rankNum <= 2) { // 小于2张卡的话，肯定不是2die全互连
        topoInfo->level0MeshType = Level0MeshType::SINGLE_DIE;
        return HCCL_SUCCESS;
    }

    u32 dieNum = 2; // 一共2个die
    std::vector<u32> dieLinkCounter(dieNum, 0);
    for (uint32_t rankIdx = 0; rankIdx < rankNum; rankIdx++) {
        if (myRank == ranks[rankIdx]) {
            continue;
        }
        CommLink* links = nullptr;
        uint32_t linkNum;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, ranks[rankIdx], &links, &linkNum));
        CHK_PTR_NULL(links);
        CHK_PRT_RET(
            linkNum == 0,
            HCCL_ERROR(
                "[Topo][CalcLevel0MeshType] Can not find path from Local[%u] to Rmt[%u], in netLayer %u. "
                "Topo is not mesh",
                myRank, ranks[rankIdx], netLayer),
            HCCL_E_INTERNAL);
        EndpointDesc srcEndpointDesc = links[0].srcEndpointDesc;
        EndpointAttrDieId dieId;
        uint32_t infoLen = sizeof(EndpointAttrDieId);
        CHK_RET(HcclRankGraphGetEndpointInfo(
            comm, myRank, &srcEndpointDesc, EndpointAttr::ENDPOINT_ATTR_DIE_ID, infoLen, &dieId));

        CHK_PRT_RET(
            dieId >= dieNum,
            HCCL_ERROR(
                "[Topo][CalcLevel0MeshType], Link from Local[%u] to Rmt[%u] die id[%u] is out of range[%u].", myRank,
                ranks[rankIdx], dieId, dieNum),
            HCCL_E_INTERNAL);
        dieLinkCounter[dieId]++;
    }

    for (u32 i = 0; i < dieNum; i++) {
        HCCL_INFO("[Topo][CalcLevel0MeshType] die[%u] link counter[%u].", i, dieLinkCounter[i]);
    }
    if (dieLinkCounter[0] == 0 || dieLinkCounter[1] == 0) {
        topoInfo->level0MeshType = Level0MeshType::SINGLE_DIE;
        HCCL_INFO("[Topo][CalcLevel0MeshType] one of the die have 0 links. Level 0 is 1DieFullMesh.");
        return HCCL_SUCCESS;
    }
    HCCL_INFO("[Topo][CalcLevel0MeshType] Level 0 is 2DieFullMesh.");
    if (dieLinkCounter[0] - dieLinkCounter[1] == 1 || dieLinkCounter[1] - dieLinkCounter[0] == 1) {
        topoInfo->level0MeshType = Level0MeshType::TWO_DIE_REGULAR;
        HCCL_INFO("[Topo][CalcLevel0MeshType] linkNum on 2 dies are off by 1. Level 0 is Regular.");
    } else {
        topoInfo->level0MeshType = Level0MeshType::TWO_DIE_NOT_REGULAR;
        HCCL_INFO("[Topo][CalcLevel0MeshType] linkNum on 2 dies are not off by 1. Not regular shape.");
    }
    return HCCL_SUCCESS;
}

HcclResult CalAllLevelEndpointAttrBwCoeff(
    HcclComm comm, uint32_t rankId, uint32_t levelSize, std::vector<std::vector<EndpointAttrBwCoeff>>& endpointAttrBw)
{
    (void)levelSize;
    uint32_t* netLayers = nullptr; // 网络层次list
    uint32_t netLayerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum)); // 获取layer总数和layerlist
    for (uint32_t layerIdx = 0; layerIdx < netLayerNum; layerIdx++) {
        uint32_t netLayerId = netLayers[layerIdx];
        uint32_t* topoInsts = nullptr;
        uint32_t topoInstNum = 0;
        CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayerId, &topoInsts, &topoInstNum)); // 获取topoInstId
        // 同层可以有多个topoInstId，遍历获取
        for (uint32_t topoInsIdx = 0; topoInsIdx < topoInstNum; topoInsIdx++) {
            uint32_t topoInstId = topoInsts[topoInsIdx];
            uint32_t endPointNums = 0;
            CHK_RET(HcclRankGraphGetEndpointNum(
                comm, netLayerId, topoInstId, &endPointNums)); // 获取endPointNums，计算同层有多少节点
            EndpointDesc* endPointDescs = nullptr;
            CHK_RET(HcclRankGraphGetEndpointDesc(
                comm, netLayerId, topoInstId, &endPointNums,
                endPointDescs)); // 根据Layer和topoInstId，拿到所有的Endpoint信息；返回vector(获取EndpointDesc)
            uint32_t infoLen = sizeof(EndpointAttrBwCoeff);
            EndpointAttrBwCoeff bwCoeff{};
            CHK_RET(HcclRankGraphGetEndpointInfo(
                comm, rankId, endPointDescs, ENDPOINT_ATTR_BW_COEFF, infoLen, &bwCoeff)); // 获取该维度的带宽
            endpointAttrBw.emplace_back(bwCoeff);
        }
    }
    return HCCL_SUCCESS;
}

HcclResult IsLevel0PcieMix(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    uint32_t myRank = topoInfo->userRank;
    u32 netLayer = 0;
    uint32_t* ranks = nullptr;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByLayer(comm, netLayer, &ranks, &rankNum));

    topoInfo->level0PcieMix = false;
    for (uint32_t rankIdx = 0; rankIdx < rankNum; ++rankIdx) {
        if (ranks[rankIdx] == myRank) {
            continue;
        }
        CommLink* links = nullptr;
        uint32_t linkNum;
        CHK_RET(HcclRankGraphGetLinks(comm, netLayer, myRank, ranks[rankIdx], &links, &linkNum));
        CHK_PTR_NULL(links);
        CHK_PRT_RET(
            linkNum == 0,
            HCCL_INFO(
                "[Topo][IsLevel0PcieMix] Can not find path from Local[%u] to Rmt[%u], in netLayer %u. "
                "Topo is not mesh",
                myRank, ranks[rankIdx], netLayer),
            HCCL_E_INTERNAL);

        for (u32 i = 0; i < linkNum; i++) {
            CommProtocol srcProtocol = links[i].srcEndpointDesc.protocol;
            HCCL_INFO("[IsLevel0PcieMix]link[%u] protocol[%u]", i, srcProtocol);
            if (srcProtocol == COMM_PROTOCOL_PCIE) {
                topoInfo->level0PcieMix = true;
                HCCL_INFO("[IsLevel0PcieMix] Level 0 has PCIE protocal");
                return HCCL_SUCCESS;
            }
        }
    }
    HCCL_INFO("[IsLevel0PcieMix] Level 0 has no PCIE protocol");
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
