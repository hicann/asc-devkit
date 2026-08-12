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

SelectorStatus AlltoAllAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)opParam;
    (void)configAlgMap;
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[AlltoAllAutoSelector] topoInfo is nullptr."), SelectorStatus::NOT_MATCH);
    if (topoInfo->userRankSize <= 1U ||
        (topoInfo->level0Topo != Level0Shape::MESH_1D && topoInfo->level0Topo != Level0Shape::CLOS)) {
        HCCL_WARNING(
            "[AlltoAllAutoSelector] rankSize[%u], level0Topo[%u] is not supported by local AICPU Mesh1D.",
            topoInfo->userRankSize, static_cast<u32>(topoInfo->level0Topo));
        return SelectorStatus::NOT_MATCH;
    }

    // Host tiling has no runtime count. Select the multi-channel token to allocate a resource upper bound.
    selectAlgName = "InsAlltoAllMesh1D";
    HCCL_INFO("[AlltoAllAutoSelector] Algo match[%s].", selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLTOALL, 18, AlltoAllAutoSelector);
} // namespace mc2_ops_hccl
