/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alltoall_auto_selector.h"

#include "selector_registry.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 CONCURRENT_RANK_LIMIT = 4U;
constexpr u64 CONCURRENT_COUNT_LIMIT = 512U;
} // namespace

SelectorStatus AlltoAllAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)configAlgMap;
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[AlltoAllAutoSelector] topoInfo is nullptr."), SelectorStatus::NOT_MATCH);
    if (topoInfo->userRankSize <= 1U) {
        HCCL_WARNING(
            "[AlltoAllAutoSelector] rankSize[%u] is not supported by local AICPU AllToAll.", topoInfo->userRankSize);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D || topoInfo->level0Topo == Level0Shape::CLOS) {
        // Host tiling has no runtime count. Select the multi-channel token to allocate a resource upper bound.
        selectAlgName = "AicpuAlltoAllSoleMesh";
        HCCL_INFO("[AlltoAllAutoSelector] Algo match[%s].", selectAlgName.c_str());
        return SelectorStatus::MATCH;
    }
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS || topoInfo->topoLevelNums != 1U || topoInfo->level0PcieMix) {
        HCCL_WARNING(
            "[AlltoAllAutoSelector] rankSize[%u], level0Topo[%u] is not supported by local AICPU AllToAll.",
            topoInfo->userRankSize, static_cast<u32>(topoInfo->level0Topo));
        return SelectorStatus::NOT_MATCH;
    }
#if defined(HCCL_CANN_COMPAT_850)
    return SelectorStatus::NOT_MATCH;
#else
    bool isMeshNumEqualToClosNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_ERROR("[AlltoAllAutoSelector] failed to compare Mesh/CLOS rank counts."), SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        !isMeshNumEqualToClosNum || topoInfo->userRankSize > CONCURRENT_RANK_LIMIT,
        HCCL_WARNING("[AlltoAllAutoSelector] topology does not satisfy Concurrent constraints."),
        SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        opParam.all2AllVDataDes.sendCounts == nullptr, HCCL_WARNING("[AlltoAllAutoSelector] sendCounts is nullptr."),
        SelectorStatus::NOT_MATCH);
    const auto* sendCounts = static_cast<const u64*>(opParam.all2AllVDataDes.sendCounts);
    CHK_PRT_RET(
        sendCounts[0] <= CONCURRENT_COUNT_LIMIT,
        HCCL_WARNING(
            "[AlltoAllAutoSelector] sendCount[%llu] is too small for Concurrent.",
            static_cast<unsigned long long>(sendCounts[0])),
        SelectorStatus::NOT_MATCH);
    selectAlgName = "AicpuAllToAllSoleMeshConcurrent";
    HCCL_INFO("[AlltoAllAutoSelector] Algo match[%s].", selectAlgName.c_str());
    return SelectorStatus::MATCH;
#endif
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLTOALL, 18, AlltoAllAutoSelector);
} // namespace mc2_ops_hccl
