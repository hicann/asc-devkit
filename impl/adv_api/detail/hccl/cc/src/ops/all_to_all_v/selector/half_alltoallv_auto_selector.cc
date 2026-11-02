/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "half_alltoallv_auto_selector.h"

#include "selector_registry.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 TOPO_LEVEL_NUM_3 = 3U;
// PostSync 源端保序方案上限，超过该 rank 数通道保序无法保证
constexpr u32 HALF_A2AV_MAX_RANK_SIZE = 8U;
constexpr char HALF_A2AV_ALG_NAME[] = "CcuHalfAll2AllVMesh1D";
} // namespace

SelectorStatus HalfAlltoAllVAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)opParam;
    (void)configAlgMap;
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[HalfAlltoAllVAutoSelector] topoInfo is nullptr."), SelectorStatus::NOT_MATCH);
    if (topoInfo->level2UbRtp) {
        HCCL_INFO(
            "[HalfAlltoAllVAutoSelector][%s] ccu schedule is not supported with level2UbRtp, reset to default.",
            __func__);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums >= TOPO_LEVEL_NUM_3) {
        HCCL_INFO(
            "[HalfAlltoAllVAutoSelector][%s] ccu schedule is not supported when topoLevelNums >= 3(levelNum[%u]), "
            "reset to default.",
            __func__, topoInfo->topoLevelNums);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0Topo != Level0Shape::MESH_1D && topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        HCCL_INFO(
            "[HalfAlltoAllVAutoSelector][%s] level0Topo[%u] is not 1D mesh, reset to default.", __func__,
            static_cast<u32>(topoInfo->level0Topo));
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->userRankSize > HALF_A2AV_MAX_RANK_SIZE) {
        HCCL_WARNING(
            "[HalfAlltoAllVAutoSelector][%s] rankSize[%u] exceeds source-ordering limit[%u], reset to default.",
            __func__, topoInfo->userRankSize, HALF_A2AV_MAX_RANK_SIZE);
        return SelectorStatus::NOT_MATCH;
    }
    selectAlgName = HALF_A2AV_ALG_NAME;
    HCCL_INFO("[HalfAlltoAllVAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_HALF_ALLTOALLV, 18, HalfAlltoAllVAutoSelector);
} // namespace mc2_ops_hccl
