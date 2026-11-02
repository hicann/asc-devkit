/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_one_level.h"
#include <algorithm>
#include "log.h"

namespace mc2_ops_hccl {

TopoMatchOneLevel::TopoMatchOneLevel() {}

TopoMatchOneLevel::~TopoMatchOneLevel() {}

namespace {
// 从 effIdx 中找 localRanks==userRankSize 的最低有效层；hostdpu 额外要求 locType==HOST
// MeshConcur 算法仅接受 COMM_TOPO_1DMESH 层
u32 PickFullLocalRanksLayer(
    const std::vector<PhysicalLevelInfo>& physicalLevels, const std::vector<u32>& effIdx, u32 userRankSize,
    bool requireHost, const std::vector<AlgoType>& algoTypes)
{
    bool isMeshConcur = IsMeshConcurAlgo(algoTypes[0]);
    for (u32 i = 0; i < static_cast<u32>(effIdx.size()); i++) {
        u32 idx = effIdx[i];
        if (physicalLevels[idx].localRanks.size() != userRankSize) {
            HCCL_INFO(
                "[PickFullLocalRanksLayer] skip effIdx[%u] physical idx[%u]: localRanks.size[%zu] != "
                "userRankSize[%u].",
                i, idx, physicalLevels[idx].localRanks.size(), userRankSize);
            continue;
        }
        if (requireHost && physicalLevels[idx].locType != EndpointLocType::ENDPOINT_LOC_TYPE_HOST) {
            HCCL_INFO(
                "[PickFullLocalRanksLayer] skip effIdx[%u] physical idx[%u]: requireHost but locType[%d] != HOST.", i,
                idx, static_cast<int32_t>(physicalLevels[idx].locType));
            continue;
        }
        if (isMeshConcur && physicalLevels[idx].topoType != CommTopo::COMM_TOPO_1DMESH) {
            HCCL_INFO(
                "[PickFullLocalRanksLayer] skip effIdx[%u] physical idx[%u]: MeshConcur but topoType[%d] != "
                "1DMESH.",
                i, idx, static_cast<int32_t>(physicalLevels[idx].topoType));
            continue;
        }
        return i;
    }
    return INVALID_UINT;
}
} // namespace

HcclResult TopoMatchOneLevel::MatchTopo(
    TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo, const AlgAttrs& algAttrs)
{
    const auto& physicalLevels = topoInfo->physicalLevels;
    if (physicalLevels.empty() || topoInfo->userRankSize == 0 || algAttrs.algoTypes.size() != 1) {
        HCCL_WARNING(
            "[TopoMatchOneLevel] Rank [%u], invalid input. "
            "physicalLevels.size[%zu], userRankSize[%u], algoTypes.size[%zu].",
            topoInfo->userRank, physicalLevels.size(), topoInfo->userRankSize, algAttrs.algoTypes.size());
        return HcclResult::HCCL_E_INTERNAL;
    }

    std::vector<u32> effIdx = CollectEffectiveIndices(physicalLevels, algAttrs.engine);
    if (effIdx.empty()) {
        HCCL_INFO("[TopoMatchOneLevel] Rank [%u], no valid layer after engine filter.", topoInfo->userRank);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    bool requireHost = (algAttrs.engine == OpExecuteConfig::HOSTCPU);
    u32 picked =
        PickFullLocalRanksLayer(physicalLevels, effIdx, topoInfo->userRankSize, requireHost, algAttrs.algoTypes);
    if (picked == INVALID_UINT) {
        HCCL_INFO(
            "[TopoMatchOneLevel] Rank [%u], no layer with localRanks == userRankSize[%u] (requireHost[%d]).",
            topoInfo->userRank, topoInfo->userRankSize, static_cast<int32_t>(requireHost));
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    // hccl 侧另有 HD 算法的 2 的整数次幂 rank 校验；devkit 的 AlgoType 无 HD 取值，
    // 该分支不可达，作为接口适配不迁移（见迁移方案"接口适配清单"）

    algHierarchyInfo.infos.resize(1);
    algHierarchyInfo.infos[0].resize(1);
    algHierarchyInfo.infos[0][0] = physicalLevels[effIdx[picked]].localRanks;

    HcclResult ret = FillPhysicalIdxForAlgoLevels(
        physicalLevels, effIdx, {picked}, algAttrs.algoTypes, algHierarchyInfo.physicalIdxForAlgoLevels);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_INFO("[TopoMatchOneLevel] FillPhysicalIdxForAlgoLevels failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    HCCL_INFO(
        "[TopoMatchOneLevel] Rank [%u], userRankSize [%u], physicalIdxForAlgoLevels: [%s].", topoInfo->userRank,
        topoInfo->userRankSize, FormatPhysicalIdxForAlgoLevels(algHierarchyInfo.physicalIdxForAlgoLevels).c_str());
    return HcclResult::HCCL_SUCCESS;
}

HcclResult TopoMatchOneLevel::MatchTopo(
    const HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    (void)comm;
    AlgAttrs algAttrs;
    algAttrs.engine = OpExecuteConfig::AICPU_TS;
    algAttrs.algoTypes = {AlgoType::PAIRWISE};
    return MatchTopo(topoInfo, algHierarchyInfo, algAttrs);
}

} // namespace mc2_ops_hccl
