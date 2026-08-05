/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_ubx_1d.h"

namespace mc2_ops_hccl {
TopoMatchUBX1d::TopoMatchUBX1d() : TopoMatchUBX() {}

TopoMatchUBX1d::~TopoMatchUBX1d() {}

HcclResult TopoMatchUBX1d::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
#ifndef AICPU_COMPILE
    constexpr uint32_t EXPECTED_TOPO_LEVEL_NUM_2 = 2;
    CHK_PRT_RET(
        topoInfo->topoLevelNums == 0 || topoInfo->topoLevelNums > EXPECTED_TOPO_LEVEL_NUM_2,
        HCCL_ERROR("[CalcTopoLevelNums] topoLevelNum[%u] is invalid.", topoInfo->topoLevelNums), HCCL_E_INTERNAL);
    uint32_t myRank;
    CHK_RET(HcclGetRankId(comm, &myRank));
#ifdef MACRO_DEV_TYPE_NEW
    CHK_PRT_RET(
        topoInfo->deviceType != DevType::DEV_TYPE_950,
#else
    CHK_PRT_RET(
        topoInfo->deviceType != DevType::DEV_TYPE_910_95,
#endif
        HCCL_ERROR("[CollAlgFactory] [TopoMatchUBX] Rank [%d], deviceType not supported yet.", myRank),
        HcclResult::HCCL_E_PARA);
    // 1.获取并校验通信层数
    uint32_t* netLayers;
    uint32_t layerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &layerNum));

    HCCL_DEBUG(
        "[CollAlgFactory] [TopoMatchUBX] Rank [%d], netLayers[%u][%s]", myRank, layerNum,
        PrintCArray<uint32_t>(netLayers, layerNum).c_str());

    // 2. 获取每个pod上rank数量以及pod数量
    uint32_t* instSizeList;
    uint32_t listSize = 0;
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, 0, &instSizeList, &listSize));
    HCCL_INFO(
        "[CollAlgFactory] [TopoMatchUBX] Rank [%d], [%u] pods ,ranksize on each pod :[%s]", myRank, listSize,
        PrintCArray<uint32_t>(instSizeList, listSize).c_str());
    // 3. 计算layer0的topo
    algHierarchyInfo.infos.resize(COMM_LAYER_SIZE_2);
    uint32_t layer0Size = 0;
    CHK_RET(TopoMatchUBX::TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo));
    // 4. 计算layer1的topo
    if (layerNum >= COMM_LAYER_SIZE_2) {
        CHK_RET(TopoForLayer1(comm, layer0Size, myRank, algHierarchyInfo));
    }
#endif
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchUBX1d::TopoForLayer1(
    const HcclComm comm, uint32_t layer0Size, const uint32_t myRank,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo) const
{
    HCCL_DEBUG("[TopoMatchUBX1d::MeshTopoForLayer1] layer0Size [%d]", layer0Size);
#ifndef AICPU_COMPILE
    // 1. 查出layer 1的所有ranks
    uint32_t* topoInsts;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, 1, &topoInsts, &topoInstNum));
    CHK_PRT_RET(
        (topoInstNum != NET_INST_NUM_1),
        HCCL_ERROR("[TopoMatchUBX1d::MeshTopoForLayer1] layer1 topoInstNum [%d], Invalid topo.", topoInstNum),
        HcclResult::HCCL_E_PARA);
    uint32_t* ranks;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, 1, topoInsts[0], &ranks, &rankNum));
    HCCL_DEBUG("[TopoMatchUBX1d::MeshTopoForLayer1] Rank [%d], all [%u] ranks in layer1", myRank, rankNum);
    // 2. 取出每张卡，作为layer1的ranks
    std::vector<uint32_t> rankVecLayer1;
    for (uint32_t i = 0; i < rankNum; i++) {
        uint32_t rankId = ranks[i];
        if (myRank == rankId) {
            rankVecLayer1.push_back(rankId);
            continue;
        }

        CommLink* links;
        uint32_t linkNum = 0;
        HcclRankGraphGetLinks(comm, 1, myRank, rankId, &links, &linkNum);
        if (linkNum == 0) {
            continue;
        }
        rankVecLayer1.push_back(rankId);
    }
    algHierarchyInfo.infos[1].push_back({rankVecLayer1});
#endif
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
