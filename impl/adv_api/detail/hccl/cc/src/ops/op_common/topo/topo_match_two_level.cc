/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "topo_match_two_level.h"
#include <algorithm>
#include "log.h"

namespace mc2_ops_hccl {

namespace {
// 计算内层维度 d0：LOCAL 取 localRanks.size()；GLOBAL 对称取 localRanks.size()，非对称 GCD 打平
HcclResult CalcLevel0Dim(
    const PhysicalLevelInfo& level0, u32 myRank, u32& d0, bool& asymmetric, u32& gcd, const AlgAttrs& algAttrs)
{
    if (level0.view == PhysicalLevelView::LOCAL) {
        d0 = static_cast<u32>(level0.localRanks.size());
        return HcclResult::HCCL_SUCCESS;
    }
    if (level0.instSizeListByLayer.empty()) {
        HCCL_INFO("[TopoMatchTwoLevel] netLayer [ref = %u] instSizeListByLayer is empty.", level0.ref.netLayer);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    if (IsInstListSymmetric(level0.instSizeListByLayer)) {
        d0 = static_cast<u32>(level0.localRanks.size());
        asymmetric = false;
        return HcclResult::HCCL_SUCCESS;
    }
    // GLOBAL 非对称：对 instSizeListByLayer 取 GCD 打平为对称子组
    asymmetric = true;
    gcd = CalcGcd(level0.instSizeListByLayer);
    HCCL_INFO("[TopoMatchTwoLevel] Rank [%u], asymmetric level0, instList GCD[%u], d0=gcd.", myRank, gcd);
    if (gcd == 1 && algAttrs.engine != OpExecuteConfig::HOSTCPU) {
        HCCL_INFO("[TopoMatchTwoLevel] Rank [%u], asymmetric GCD=1, not support.", myRank);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    d0 = gcd;
    return HcclResult::HCCL_SUCCESS;
}

// 构造含 myRank 的内层组；非对称时按 gcd 从 localRanks 切子组
std::vector<u32> BuildLevel0Group(const PhysicalLevelInfo& level0, u32 myRank, bool asymmetric, u32 gcd)
{
    if (!asymmetric) {
        return level0.localRanks;
    }
    const auto& ranks = level0.localRanks;
    auto it = std::find(ranks.begin(), ranks.end(), myRank);
    if (it == ranks.end()) {
        return {};
    }
    u32 myIdx = static_cast<u32>(it - ranks.begin());
    u32 startIdx = (myIdx / gcd) * gcd;
    u32 endIdx = std::min(startIdx + gcd, static_cast<u32>(ranks.size()));
    return std::vector<u32>(ranks.begin() + startIdx, ranks.begin() + endIdx);
}
} // namespace

TopoMatchTwoLevel::TopoMatchTwoLevel() {}
TopoMatchTwoLevel::~TopoMatchTwoLevel() {}

HcclResult TopoMatchTwoLevel::MatchTopo(
    TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo, const AlgAttrs& algAttrs)
{
    const auto& physicalLevels = topoInfo->physicalLevels;
    u32 myRank = topoInfo->userRank;
    u32 userRankSize = topoInfo->userRankSize;
    if (physicalLevels.empty() || userRankSize == 0 || algAttrs.algoTypes.size() != ALGO_LEVEL_NUM_TWO) {
        HCCL_WARNING(
            "[TopoMatchTwoLevel] Rank [%u], invalid input. "
            "physicalLevels.size[%zu], userRankSize[%u], algoTypes.size[%zu].",
            myRank, physicalLevels.size(), userRankSize, algAttrs.algoTypes.size());
        return HcclResult::HCCL_E_INTERNAL;
    }

    HcclResult ret = HCCL_SUCCESS;
    // 引擎过滤 + 锚点匹配 + 分段 + 最高层校验
    std::vector<u32> effIdx;
    std::vector<u32> pIndices;
    ret = ResolveMapping(physicalLevels, algAttrs, userRankSize, effIdx, pIndices);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_INFO("[TopoMatchTwoLevel] ResolveMapping failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    u32 phys0 = effIdx[pIndices[0]];

    // GCD 校验 p_0（TwoLevel 非对称打平），外层 d1 = userRankSize / d0
    u32 d0 = 0;
    bool asymmetric = false;
    u32 gcd = 0;
    ret = CalcLevel0Dim(physicalLevels[phys0], myRank, d0, asymmetric, gcd, algAttrs);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_INFO("[TopoMatchTwoLevel] CalcLevel0Dim failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    if (d0 == 0 || userRankSize % d0 != 0 || (d0 == 1 && algAttrs.engine != OpExecuteConfig::HOSTCPU)) {
        HCCL_INFO("[TopoMatchTwoLevel] Rank [%u], userRankSize[%u] not divisible by d0[%u].", myRank, userRankSize, d0);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    u32 d1 = userRankSize / d0;
    if (d1 == 1) {
        HCCL_INFO("[TopoMatchTwoLevel] Rank [%u], d0=%u, d1=1, not support two level.", myRank, d0);
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }

    // 构造 infos
    std::vector<u32> group0 = BuildLevel0Group(physicalLevels[phys0], myRank, asymmetric, gcd);
    std::vector<u32> group1 = BuildRepresentativeGroup(d0, d1, myRank % d0);
    ret = ValidateGroup(group0, d0, myRank, "level0");
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_WARNING("[TopoMatchTwoLevel] ValidateGroup level0 failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    ret = ValidateGroup(group1, d1, myRank, "level1");
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_WARNING("[TopoMatchTwoLevel] ValidateGroup level1 failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    algHierarchyInfo.infos.resize(ALGO_LEVEL_NUM_TWO);
    algHierarchyInfo.infos[0].resize(1);
    algHierarchyInfo.infos[1].resize(1);
    algHierarchyInfo.infos[0][0] = std::move(group0);
    algHierarchyInfo.infos[1][0] = std::move(group1);

    // 填充 physicalIdxForAlgoLevels（二级：MeshConcur 双层，普通单层）
    ret = FillPhysicalIdxForAlgoLevels(
        physicalLevels, effIdx, pIndices, algAttrs.algoTypes, algHierarchyInfo.physicalIdxForAlgoLevels);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS, HCCL_INFO("[TopoMatchTwoLevel] FillPhysicalIdxForAlgoLevels failed: hcclRet -> %d", ret),
        HcclResult::HCCL_E_NOT_SUPPORT);
    HCCL_INFO(
        "[TopoMatchTwoLevel] Rank [%u], d0[%u] d1[%u] asym[%d], physicalIdxForAlgoLevels: [%s].", myRank, d0, d1,
        static_cast<int32_t>(asymmetric),
        FormatPhysicalIdxForAlgoLevels(algHierarchyInfo.physicalIdxForAlgoLevels).c_str());
    return HcclResult::HCCL_SUCCESS;
}

// devkit 适配（2026-09-22）：V1 签名兼容入口，内置固化 AlgAttrs 后转发 V2 实现。
// 依据：InsReduceScatterParallelExecutor 家族全部注册为 Mesh1D + NHR 双模板两级形态，
// AICPU_TS 引擎（GCD=1 / d0=1 的 HOSTCPU 豁免分支不触发，语义与 hccl AICPU 注册一致）。
HcclResult TopoMatchTwoLevel::MatchTopo(
    const void* comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    (void)comm;
    AlgAttrs algAttrs;
    algAttrs.engine = OpExecuteConfig::AICPU_TS;
    algAttrs.algoTypes = {AlgoType::MESH, AlgoType::NHR};
    return MatchTopo(topoInfo, algHierarchyInfo, algAttrs);
}

} // namespace mc2_ops_hccl
