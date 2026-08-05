/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_pcie_mix.h"

namespace mc2_ops_hccl {
TopoMatchPcieMix::TopoMatchPcieMix() : TopoMatchBase() {}

TopoMatchPcieMix::~TopoMatchPcieMix() {}

HcclResult TopoMatchPcieMix::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
#ifndef AICPU_COMPILE
    CHK_PRT_RET(
        topoInfo->topoLevelNums == 0,
        HCCL_ERROR("[TopoMatchPcieMix] topoLevelNum[%u] is invalid.", topoInfo->topoLevelNums), HCCL_E_INTERNAL);
    uint32_t myRank;
    CHK_RET(HcclGetRankId(comm, &myRank));

#ifdef MACRO_DEV_TYPE_NEW
    CHK_PRT_RET(
        topoInfo->deviceType != DevType::DEV_TYPE_950,
        HCCL_ERROR("[TopoMatchPcieMix] Rank [%d], deviceType not supported yet.", myRank), HcclResult::HCCL_E_PARA);
#else
    CHK_PRT_RET(
        topoInfo->deviceType != DevType::DEV_TYPE_910_95,
        HCCL_ERROR("[TopoMatchPcieMix] Rank [%d], deviceType not supported yet.", myRank), HcclResult::HCCL_E_PARA);
#endif

    // 获取通信网络层数
    uint32_t* netLayers;
    uint32_t layerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &layerNum));

    HCCL_DEBUG(
        "[TopoMatchPcieMix] Rank [%d], netLayers[%u][%s]", myRank, layerNum,
        PrintCArray<uint32_t>(netLayers, layerNum).c_str());

    // 获取layer0的topo
    uint32_t* instSizeList;
    uint32_t listSize = 0;
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, 0, &instSizeList, &listSize));
    HCCL_INFO(
        "[TopoMatchPcieMix] Rank:[%d], inst num:[%u], rank on each inst:[%s]", myRank, listSize,
        PrintCArray<uint32_t>(instSizeList, listSize).c_str());
    CHK_RET(CheckVecElementAllSame(instSizeList, listSize));

    // 计算layer0的topo
    algHierarchyInfo.infos.resize(COMM_LAYER_SIZE_2);
    CHK_RET(TopoForLayer0(comm, myRank, algHierarchyInfo));

    // 暂不支持大于1层的PCIE混合topo
    if (layerNum >= COMM_LAYER_SIZE_2) {
        HCCL_WARNING("[TopoMatchPcieMix] layerNum > 1 is not supported yet for PCIE mix topo, and is ignored");
    }
#endif
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchPcieMix::CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const
{
#ifndef AICPU_COMPILE
    uint32_t firstSize = instSizeList[0];
    for (uint32_t i = 1; i < listSize; i++) {
        if (firstSize != instSizeList[i]) {
            HCCL_ERROR(
                "[TopoMatchPcieMix] instSizeList [%u] [%u] not equal, Invalid topo.", firstSize, instSizeList[i]);
            return HcclResult::HCCL_E_PARA;
        }
    }
#endif
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchPcieMix::TopoForLayer0(
    const HcclComm comm, const uint32_t myRank, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
#ifndef AICPU_COMPILE
    uint32_t netLayer = 0;
    uint32_t* topoInsts;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayer, &topoInsts, &topoInstNum));

    std::vector<uint32_t> ranksInMeshTopo = {};
    std::vector<uint32_t> ranksInClosTopo = {};
    CommTopo topoType = CommTopo::COMM_TOPO_RESERVED;
    uint32_t topoInstId = 0;
    for (uint32_t idx = 0; idx < topoInstNum; ++idx) {
        topoInstId = topoInsts[idx];
        CHK_RET(HcclRankGraphGetTopoType(comm, netLayer, topoInstId, &topoType));
        if (topoType == CommTopo::COMM_TOPO_1DMESH) {
            CHK_RET(LoadTopoInstRanks(comm, netLayer, topoInstId, ranksInMeshTopo));
        } else if (topoType == CommTopo::COMM_TOPO_CLOS) {
            CHK_RET(LoadTopoInstRanks(comm, netLayer, topoInstId, ranksInClosTopo));
        } else {
            HCCL_ERROR(
                "[TopoMatchPcieMix] Rank[%d], topoInstId[%u], Invalid topo type[%u]", myRank, topoInstId, topoType);
            return HCCL_E_PARA;
        }
    }
    CHK_RET(DeduplicateLevelRanks(myRank, ranksInMeshTopo, ranksInClosTopo));
    HCCL_DEBUG(
        "[TopoMatchPcieMix] Rank[%d], netLayer[%u], rank num in 1DMESH topo is [%u]", myRank, netLayer,
        ranksInMeshTopo.size());
    HCCL_DEBUG(
        "[TopoMatchPcieMix] Rank[%d], netLayer[%u], rank num in CLOS topo is [%u]", myRank, netLayer,
        ranksInClosTopo.size());

    algHierarchyInfo.infos[0].push_back({ranksInMeshTopo});
    algHierarchyInfo.infos[1].push_back({ranksInClosTopo});
#endif
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchPcieMix::LoadTopoInstRanks(
    const HcclComm comm, uint32_t netLayer, uint32_t topoInstId, std::vector<uint32_t>& rankList) const
{
#ifndef AICPU_COMPILE
    uint32_t* ranks;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, netLayer, topoInstId, &ranks, &rankNum));
    for (uint32_t idx = 0; idx < rankNum; ++idx) {
        rankList.emplace_back(ranks[idx]);
    }
#endif
    return HCCL_SUCCESS;
}

HcclResult TopoMatchPcieMix::DeduplicateLevelRanks(
    const uint32_t myRank, std::vector<uint32_t>& level0Ranks, std::vector<uint32_t>& level1Ranks) const
{
    u32 level0RankSize = level0Ranks.size();
    auto level1End = std::remove_if(level1Ranks.begin(), level1Ranks.end(), [this, level0RankSize, myRank](int val) {
        return val % level0RankSize != myRank % level0RankSize;
    });
    level1Ranks.erase(level1End, level1Ranks.end());
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
