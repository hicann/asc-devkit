/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_1d.h"
#include "topo_match_multilevel.h"
#include "topo_match_ubx.h"

#include <algorithm>

namespace mc2_ops_hccl {
namespace {
constexpr u32 NET_LAYER0 = 0;
constexpr u32 NET_LAYER1 = 1;

std::vector<u32> MakeAllRankIds(u32 rankSize)
{
    std::vector<u32> ranks;
    ranks.reserve(rankSize);
    for (u32 rankId = 0; rankId < rankSize; ++rankId) {
        ranks.push_back(rankId);
    }
    return ranks;
}

bool AllSame(const uint32_t* values, uint32_t valueNum)
{
    if (values == nullptr || valueNum == 0) {
        return true;
    }
    return std::all_of(values + 1, values + valueNum, [first = values[0]](uint32_t value) { return value == first; });
}
} // namespace

TopoMatch1D::TopoMatch1D() {}

TopoMatch1D::~TopoMatch1D() {}

HcclResult TopoMatch1D::MatchTopo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    (void)comm;
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(topoInfo->userRankSize == 0, HCCL_ERROR("[TopoMatch1D][UT] rankSize is 0."), HCCL_E_PARA);

    algHierarchyInfo.infos.clear();
    algHierarchyInfo.infos.resize(1);
    algHierarchyInfo.infos[0].push_back(MakeAllRankIds(topoInfo->userRankSize));
    return HCCL_SUCCESS;
}

TopoMatchMultilevel::TopoMatchMultilevel() : TopoMatchBase() {}

TopoMatchMultilevel::~TopoMatchMultilevel() {}

HcclResult TopoMatchMultilevel::TopoForLayer0(
    const HcclComm comm, uint32_t& layer0Size, const uint32_t myRank, AlgHierarchyInfoForAllLevel& algHierarchyInfo,
    uint32_t gcdInstSize) const
{
    uint32_t* topoInsts = nullptr;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, NET_LAYER0, &topoInsts, &topoInstNum));

    if (topoInstNum == NET_INST_NUM_1) {
        uint32_t* ranks = nullptr;
        uint32_t rankNum = 0;
        CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, NET_LAYER0, topoInsts[0], &ranks, &rankNum));
        if (gcdInstSize > 0 && gcdInstSize < rankNum) {
            auto it = std::find(ranks, ranks + rankNum, myRank);
            CHK_PRT_RET(
                it == ranks + rankNum,
                HCCL_ERROR("[TopoMatchMultilevel][UT] myRank [%u] not found in layer0 ranks.", myRank),
                HCCL_E_INTERNAL);
            const uint32_t myIdx = static_cast<uint32_t>(it - ranks);
            const uint32_t startIdx = (myIdx / gcdInstSize) * gcdInstSize;
            const uint32_t endIdx = std::min(startIdx + gcdInstSize, rankNum);
            algHierarchyInfo.infos[0].push_back(std::vector<uint32_t>(ranks + startIdx, ranks + endIdx));
            layer0Size = gcdInstSize;
        } else {
            algHierarchyInfo.infos[0].push_back(std::vector<uint32_t>(ranks, ranks + rankNum));
            layer0Size = rankNum;
        }
    } else if (topoInstNum == 0) {
        algHierarchyInfo.infos[0].push_back({myRank});
        layer0Size = 1;
    } else {
        layer0Size = 1;
        for (uint32_t idx = 0; idx < topoInstNum; ++idx) {
            CommTopo topoType;
            CHK_RET(HcclRankGraphGetTopoType(comm, NET_LAYER0, topoInsts[idx], &topoType));
            if (topoType == CommTopo::COMM_TOPO_CLOS) {
                continue;
            }
            uint32_t* ranks = nullptr;
            uint32_t rankNum = 0;
            CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, NET_LAYER0, topoInsts[idx], &ranks, &rankNum));
            std::vector<uint32_t> rankVec(ranks, ranks + rankNum);
            layer0Size = std::max(layer0Size, static_cast<uint32_t>(rankVec.size()));
            algHierarchyInfo.infos[0].push_back(std::move(rankVec));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult TopoMatchMultilevel::TopoForLayer1(
    const HcclComm comm, uint32_t netLayer, uint32_t& layer0Size, const uint32_t myRank,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo) const
{
    uint32_t* topoInsts = nullptr;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayer, &topoInsts, &topoInstNum));
    CHK_PRT_RET(
        topoInstNum != NET_INST_NUM_1,
        HCCL_ERROR("[TopoMatchMultilevel][UT] invalid layer1 topoInstNum [%u].", topoInstNum), HCCL_E_PARA);

    uint32_t* ranks = nullptr;
    uint32_t rankNum = 0;
    CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, netLayer, topoInsts[0], &ranks, &rankNum));

    std::vector<uint32_t> layer1Ranks;
    for (uint32_t i = 0; i < rankNum; ++i) {
        const uint32_t rankId = ranks[i];
        if (rankId == myRank || (layer0Size != 0 && rankId % layer0Size == myRank % layer0Size)) {
            layer1Ranks.push_back(rankId);
        }
    }
    algHierarchyInfo.infos[1].push_back(std::move(layer1Ranks));
    return HCCL_SUCCESS;
}

bool TopoMatchMultilevel::CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const
{
    return AllSame(instSizeList, listSize);
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
    for (uint32_t i = 1; i < listSize; ++i) {
        result = GcdTwo(result, instSizeList[i]);
    }
    return result;
}

HcclResult TopoMatchMultilevel::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    CHK_PTR_NULL(topoInfo);
    uint32_t myRank = 0;
    CHK_RET(HcclGetRankId(comm, &myRank));

    uint32_t* instSizeList = nullptr;
    uint32_t listSize = 0;
    CHK_RET(HcclRankGraphGetInstSizeListByLayer(comm, NET_LAYER0, &instSizeList, &listSize));

    algHierarchyInfo.infos.clear();
    algHierarchyInfo.infos.resize(COMM_LAYER_SIZE_2);
    uint32_t layer0Size = 0;
    if (!CheckVecElementAllSame(instSizeList, listSize)) {
        CHK_RET(TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo, GcdOfInstSizeList(instSizeList, listSize)));
    } else {
        CHK_RET(TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo));
    }
    CHK_RET(TopoForLayer1(comm, NET_LAYER1, layer0Size, myRank, algHierarchyInfo));
    return HCCL_SUCCESS;
}

TopoMatchUBX::TopoMatchUBX() : TopoMatchBase() {}

TopoMatchUBX::~TopoMatchUBX() {}

HcclResult TopoMatchUBX::TopoForLayer0(
    const HcclComm comm, uint32_t& layer0Size, const uint32_t myRank,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo) const
{
    uint32_t* topoInsts = nullptr;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, NET_LAYER0, &topoInsts, &topoInstNum));

    if (topoInstNum == NET_INST_NUM_1) {
        uint32_t* ranks = nullptr;
        uint32_t rankNum = 0;
        CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, NET_LAYER0, topoInsts[0], &ranks, &rankNum));
        algHierarchyInfo.infos[0].push_back(std::vector<uint32_t>(ranks, ranks + rankNum));
        layer0Size = rankNum;
    } else if (topoInstNum == 0) {
        algHierarchyInfo.infos[0].push_back({myRank});
        layer0Size = 1;
    } else {
        layer0Size = 1;
        for (uint32_t idx = 0; idx < topoInstNum; ++idx) {
            uint32_t* ranks = nullptr;
            uint32_t rankNum = 0;
            CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, NET_LAYER0, topoInsts[idx], &ranks, &rankNum));
            std::vector<uint32_t> rankVec(ranks, ranks + rankNum);
            layer0Size = std::max(layer0Size, static_cast<uint32_t>(rankVec.size()));
            algHierarchyInfo.infos[0].push_back(std::move(rankVec));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult TopoMatchUBX::TopoForLayer1(
    const HcclComm comm, uint32_t layer0Size, const uint32_t myRank,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo) const
{
    uint32_t* topoInsts = nullptr;
    uint32_t topoInstNum = 0;
    CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, NET_LAYER1, &topoInsts, &topoInstNum));
    CHK_PRT_RET(
        topoInstNum != NET_INST_NUM_1, HCCL_ERROR("[TopoMatchUBX][UT] invalid layer1 topoInstNum [%u].", topoInstNum),
        HCCL_E_PARA);

    uint32_t* ranks = nullptr;
    uint32_t rankNum = 0;
    CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, NET_LAYER1, topoInsts[0], &ranks, &rankNum));
    std::vector<uint32_t> layer1Ranks;
    for (uint32_t i = 0; i < rankNum; ++i) {
        const uint32_t rankId = ranks[i];
        if (rankId == myRank || (layer0Size != 0 && rankId % layer0Size == myRank % layer0Size)) {
            layer1Ranks.push_back(rankId);
        }
    }
    algHierarchyInfo.infos[1].push_back(std::move(layer1Ranks));
    return HCCL_SUCCESS;
}

HcclResult TopoMatchUBX::CheckVecElementAllSame(const uint32_t* instSizeList, uint32_t listSize) const
{
    return AllSame(instSizeList, listSize) ? HCCL_SUCCESS : HCCL_E_PARA;
}

HcclResult TopoMatchUBX::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    CHK_PTR_NULL(topoInfo);
    uint32_t myRank = 0;
    CHK_RET(HcclGetRankId(comm, &myRank));
    algHierarchyInfo.infos.clear();
    algHierarchyInfo.infos.resize(COMM_LAYER_SIZE_2);
    uint32_t layer0Size = 0;
    CHK_RET(TopoForLayer0(comm, layer0Size, myRank, algHierarchyInfo));
    uint32_t* netLayers = nullptr;
    uint32_t layerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &layerNum));
    if (layerNum >= COMM_LAYER_SIZE_2) {
        CHK_RET(TopoForLayer1(comm, layer0Size, myRank, algHierarchyInfo));
    }
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
