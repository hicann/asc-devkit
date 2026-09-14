/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_concurrent_v2.h"

namespace mc2_ops_hccl {

// 照 hccl TopoMatchConcurrentV2::MatchTopo 移植：两组同 rank（mesh 组 + clos 组并发），
// 均为全通信域 [0, userRankSize)，mesh 流与 NHR 流覆盖同一组卡、仅物理链路不同
// （mesh 与 clos 同在 layer0，两类拓扑实例并存，见 topoInstDetailsOfLayer[0].rankNumForTopoType）。
// 准入与 selector 的 CheckMeshNumEqualToClosNum 同款：mesh 实例 rank 数 == clos 实例 rank 数
// （即通信域 = 一个 mesh 实例 = 一个 clos 实例，"meshNum == closNum"）；
// 跨多 mesh 的域（如 8P = 2*4P mesh + 1 clos）因 mesh[0] != clos[0] 被拒绝，对齐 hccl selector 行为。
HcclResult TopoMatchConcurrentV2::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
#ifndef AICPU_COMPILE
    (void)comm;
    const u32 myRank = topoInfo->userRank;
    const u32 rankSize = topoInfo->userRankSize;
    CHK_PRT_RET(
        rankSize < 2U, HCCL_ERROR("[TopoMatchConcurrentV2] Rank [%u], rankSize[%u] < 2 is invalid.", myRank, rankSize),
        HcclResult::HCCL_E_PARA);

    const auto& topoInstDetails = topoInfo->topoInstDetailsOfLayer;
    if (topoInstDetails.empty()) {
        HCCL_INFO("[TopoMatchConcurrentV2] Rank [%u], topoInstDetailsOfLayer is empty, not support.", myRank);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    const auto& rankNumMap = topoInstDetails[0].rankNumForTopoType;
    auto closItr = rankNumMap.find(CommTopo::COMM_TOPO_CLOS);
    auto meshItr = rankNumMap.find(CommTopo::COMM_TOPO_1DMESH);
    if (closItr == rankNumMap.end() || closItr->second.empty() || meshItr == rankNumMap.end() ||
        meshItr->second.empty()) {
        HCCL_INFO(
            "[TopoMatchConcurrentV2] Rank [%u], layer0 lacks MESH/CLOS topo instance, not support for concurrent "
            "algo.",
            myRank);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    if (closItr->second[0] != meshItr->second[0]) {
        HCCL_INFO(
            "[TopoMatchConcurrentV2] Rank [%u], mesh inst size[%u] != clos inst size[%u], comm domain must satisfy "
            "meshNum == closNum for concurrent algo.",
            myRank, meshItr->second[0], closItr->second[0]);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    // 两组同 rank（mesh 组 + clos 组并发），不依赖 physicalLevels 内容
    std::vector<u32> rankIds;
    rankIds.reserve(rankSize);
    for (u32 rankId = 0; rankId < rankSize; ++rankId) {
        rankIds.push_back(rankId);
    }
    algHierarchyInfo.infos.resize(1);
    algHierarchyInfo.infos[0].resize(2U);
    algHierarchyInfo.infos[0][0] = rankIds; // temp0: mesh 流
    algHierarchyInfo.infos[0][1] = rankIds; // temp1: NHR 流（同一组 rank，走 clos 链路）
    HCCL_INFO(
        "[TopoMatchConcurrentV2] Rank [%u], rankSize[%u], mesh group == clos group == full comm domain.", myRank,
        rankSize);
#else
    (void)comm;
    (void)topoInfo;
    (void)algHierarchyInfo;
#endif
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
