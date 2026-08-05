/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "topo_match_multilevel.h"
#include "op_common.h"

namespace mc2_ops_hccl {
TopoMatchMultilevel::TopoMatchMultilevel() : TopoMatchBase() {}

TopoMatchMultilevel::~TopoMatchMultilevel() {}

HcclResult TopoMatchMultilevel::TopoForLayer0(
    const HcclComm comm, uint32_t& layer0Size, const uint32_t myRank, AlgHierarchyInfoForAllLevel& algHierarchyInfo,
    uint32_t gcdInstSize) const
{
#ifndef AICPU_COMPILE
    uint32_t* topoInsts;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, 0, &topoInsts, &topoInstNum));

    if (topoInstNum == NET_INST_NUM_1) {
        // mesh1d
        HCCL_INFO("[CollAlgFactory] [TopoMatchMultilevel] layer0 topoInstNum [%d], Mesh 1D.", topoInstNum);
        uint32_t* ranks;
        uint32_t rankNum = 0;
        CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, 0, topoInsts[0], &ranks, &rankNum));
        HCCL_DEBUG(
            "[CollAlgFactory] [TopoMatchMultilevel] Rank [%d], all [%u] ranks in this pod: [%s]", myRank, rankNum,
            PrintCArray<uint32_t>(ranks, rankNum).c_str());
        if (gcdInstSize > 0 && gcdInstSize < rankNum) {
            // Asymmetric: split this pod into GCD-sized subgroups
            // ranks guaranteed ascending by HcclRankGraphGetRanksByTopoInst (backed by std::set)
            auto it = std::find(ranks, ranks + rankNum, myRank);
            CHK_PRT_RET(
                it == ranks + rankNum,
                HCCL_ERROR("[TopoMatchMultilevel] [TopoForLayer0] myRank [%u] not found in ranks array", myRank),
                HcclResult::HCCL_E_INTERNAL);

            uint32_t myIdx = static_cast<uint32_t>(it - ranks);
            uint32_t groupId = myIdx / gcdInstSize;
            uint32_t startIdx = groupId * gcdInstSize;
            uint32_t endIdx = std::min(startIdx + gcdInstSize, rankNum);
            std::vector<uint32_t> rankVecLayer0(ranks + startIdx, ranks + endIdx);
            HCCL_DEBUG(
                "[TopoMatchMultilevel] [TopoForLayer0] Rank [%d], GCD subgroup: [%s]", myRank,
                PrintCArray<uint32_t>(rankVecLayer0.data(), static_cast<u32>(rankVecLayer0.size())).c_str());
            algHierarchyInfo.infos[0].push_back({rankVecLayer0});
            layer0Size = gcdInstSize;
        } else {
            // Symmetric: original logic (whole pod as one group)
            std::vector<uint32_t> rankVecLayer0(ranks, ranks + rankNum);
            algHierarchyInfo.infos[0].push_back({rankVecLayer0});
            layer0Size = rankVecLayer0.size();
        }
    } else if (topoInstNum == 0) {
        algHierarchyInfo.infos[0].push_back({{myRank}});
        layer0Size = 1;
    } else if (topoInstNum >= NET_INST_NUM_2) {
        // mesh2d
        HCCL_INFO("[CollAlgFactory] [TopoMatchMultilevel] layer0 topoInstNum [%d], Mesh 1D.", topoInstNum);
        std::vector<uint32_t> ranks_x;
        std::vector<uint32_t> ranks_y;

        for (uint32_t idx = 0; idx < topoInstNum; idx++) {
            CommTopo topoType;
            CHK_RET(HcclRankGraphGetTopoType(comm, 0, topoInsts[idx], &topoType));
            if (topoType == CommTopo::COMM_TOPO_CLOS)
                continue;

            uint32_t* ranks;
            uint32_t rankNum;
            CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, 0, topoInsts[idx], &ranks, &rankNum));

            std::sort(ranks, ranks + rankNum);
            if (ranks[1] - ranks[0] == 1) {
                ranks_x.assign(ranks, ranks + rankNum);
            } else {
                ranks_y.assign(ranks, ranks + rankNum);
            }
        }
        algHierarchyInfo.infos[0].push_back(ranks_x);
        algHierarchyInfo.infos[0].push_back(ranks_y);
        layer0Size = ranks_x.size() * ranks_y.size();
    }
#endif
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchMultilevel::TopoForLayer1(
    const HcclComm comm, uint32_t netLayer, uint32_t& layer0Size, const uint32_t myRank,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo) const
{
    HCCL_DEBUG("[TopoMatchMultilevel::MeshNHRTopoForLayer1] layer0Size [%d]", layer0Size);
#ifndef AICPU_COMPILE
    // 1. 查出layer 1的所有ranks
    uint32_t* topoInsts;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayer, &topoInsts, &topoInstNum));
    CHK_PRT_RET(
        (topoInstNum != NET_INST_NUM_1),
        HCCL_ERROR("[TopoMatchMultilevel::MeshNHRTopoForLayer1] layer1 topoInstNum [%d], Invalid topo.", topoInstNum),
        HcclResult::HCCL_E_PARA);

    uint32_t* ranks;
    uint32_t rankNum;
    CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, netLayer, topoInsts[0], &ranks, &rankNum));
    HCCL_DEBUG("[TopoMatchMultilevel::MeshNHRTopoForLayer1] Rank [%d], all [%u] ranks in layer1", myRank, rankNum);

    // 2. 取出同序号卡，作为layer1的ranks
    std::vector<uint32_t> rankVecLayer1WithSameIdx;
    for (uint32_t i = 0; i < rankNum; i++) {
        uint32_t rankId = ranks[i];
        if (myRank == rankId) {
            rankVecLayer1WithSameIdx.push_back(rankId);
            continue;
        }
        if (rankId % layer0Size != myRank % layer0Size) {
            continue;
        }
        CommLink* links;
        uint32_t linkNum = 0;
        HcclRankGraphGetLinks(comm, netLayer, myRank, rankId, &links, &linkNum);
        if (linkNum == 0) {
            continue;
        }
        rankVecLayer1WithSameIdx.push_back(rankId);
    }
    algHierarchyInfo.infos[1].push_back({rankVecLayer1WithSameIdx});
#endif
    return HcclResult::HCCL_SUCCESS;
}

bool TopoMatchMultilevel::CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const
{
#ifndef AICPU_COMPILE
    uint32_t firstSize = instSizeList[0];
    for (uint32_t i = 1; i < listSize; i++) {
        if (firstSize != instSizeList[i]) {
            return false;
        }
    }
#endif
    return true;
}

uint32_t TopoMatchMultilevel::GcdTwo(uint32_t a, uint32_t b) const
{
    while (b != 0) {
        a %= b;
        std::swap(a, b);
    }
    return a;
}

uint32_t TopoMatchMultilevel::GcdOfInstSizeList(const uint32_t* instSizeList, uint32_t listSize) const
{
    uint32_t result = instSizeList[0];
    for (uint32_t i = 1; i < listSize; i++) {
        result = GcdTwo(result, instSizeList[i]);
        if (result == 1) {
            return 1;
        }
    }
    return result;
}

HcclResult TopoMatchMultilevel::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
#ifndef AICPU_COMPILE
    CHK_PRT_RET(
        topoInfo->topoLevelNums == 0 || topoInfo->topoLevelNums > COMM_LAYER_SIZE_2,
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
        HCCL_ERROR("[CollAlgFactory] [TopoMatchMultilevel] Rank [%d], deviceType not supported yet.", myRank),
        HcclResult::HCCL_E_PARA);
    // 1.获取并校验通信层数
    uint32_t* netLayers;
    uint32_t layerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &layerNum));

    HCCL_DEBUG(
        "[CollAlgFactory] [TopoMatchMultilevel] Rank [%d], netLayers[%u][%s]", myRank, layerNum,
        PrintCArray<uint32_t>(netLayers, layerNum).c_str());

    // 2. 获取每个pod上rank数量以及pod数量
    uint32_t* instSizeList;
    uint32_t listSize = 0;
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, 0, &instSizeList, &listSize));
    HCCL_INFO(
        "[CollAlgFactory] [TopoMatchMultilevel] Rank [%d], [%u] pods ,ranksize on each pod :[%s]", myRank, listSize,
        PrintCArray<uint32_t>(instSizeList, listSize).c_str());
    bool isSymmetric = CheckVecElementAllSame(instSizeList, listSize);

    // 非对称仅支持 Mesh1D，提前校验 topoInstNum
    if (!isSymmetric) {
        uint32_t* topoInsts;
        uint32_t topoInstNum = 0;
        CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, 0, &topoInsts, &topoInstNum));
        CHK_PRT_RET(
            topoInstNum != NET_INST_NUM_1,
            HCCL_ERROR(
                "[TopoMatchMultilevel][MatchTopo] Asymmetric mode only supports Mesh1D, "
                "but topoInstNum [%u]",
                topoInstNum),
            HcclResult::HCCL_E_NOT_SUPPORT);
    }

    // 3. 计算layer0的topo
    algHierarchyInfo.infos.resize(COMM_LAYER_SIZE_2);
    uint32_t layer0Size = 0;
    if (!isSymmetric) {
        uint32_t gcdInstSize = GcdOfInstSizeList(instSizeList, listSize);
        HCCL_INFO("[TopoMatchMultilevel][MatchTopo] Asymmetric mode, gcdInstSize [%u]", gcdInstSize);
        CHK_RET(TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo, gcdInstSize));
    } else {
        CHK_RET(TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo));
    }

    // 4. 计算layer1的topo
    uint32_t netLayer = 1;
    bool hostDPUOnly = false;
    if ((CheckHostDPUOnly(comm, topoInfo, hostDPUOnly) == HcclResult::HCCL_SUCCESS) && hostDPUOnly) {
        // host dpu场景使用最高层的链路
        netLayer = topoInfo->netLayerDetails.netLayers[topoInfo->netLayerDetails.netLayerNum - 1];
    }
    CHK_RET(TopoForLayer1(comm, netLayer, layer0Size, myRank, algHierarchyInfo));
#endif
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
