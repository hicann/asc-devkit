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

constexpr uint32_t CONCURRENT_RANK_LIMIT = 4;
constexpr uint64_t CONCURRENT_COUNT_LIMIT = 512;
constexpr uint64_t BIG_DATA_SIZE_LIMIT = 512;
constexpr u32 TOPO_LEVEL_NUM_3 = 3;
constexpr uint64_t SMALL_COUNT_16M = 16 * 1024 * 1024;
constexpr uint32_t CCU_MESH1D_MAX_RANK_SIZE = 16; // uint16_t event mask 最多支持 16 rank

namespace {
bool IsDevType960() { return false; } // 960设备类型检测待实现
} // namespace

uint64_t AlltoAllAutoSelector::CalcDataSize(const OpParam& opParam, const TopoInfoWithNetLayerDetails* topoInfo) const
{
    uint32_t dataTypeSize = DATATYPE_SIZE_TABLE[opParam.all2AllDataDes.sendType];
    uint64_t* sendCountPtr = reinterpret_cast<uint64_t*>(opParam.all2AllVDataDes.sendCounts);
    if (sendCountPtr == nullptr) {
        return 0;
    }
    return *sendCountPtr * dataTypeSize * topoInfo->userRankSize;
}

SelectorStatus AlltoAllAutoSelector::SelectMultiLevelAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName) const
{
    if (topoInfo->level0Topo == Level0Shape::MESH_1D && topoInfo->userRankSize <= 64U) {
        selectAlgName = "CcuAllToAllMesh1D2Die";
        HCCL_WARNING("[AlltoAllAutoSelector][%s] algo[%s] not registered, fallback.", __func__, selectAlgName.c_str());
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::CLOS) {
        HCCL_WARNING("[AlltoAllAutoSelector] levelNum > 1 is not supported yet for clos.");
    } else {
        HCCL_WARNING("[AlltoAllAutoSelector] levelNum > 1 is not supported yet for 2d schedule mode.");
    }
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AlltoAllAutoSelector::SelectMesh1DClosAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    if (topoInfo->level0PcieMix) {
        if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
            selectAlgName = "CcuAllToAllMesh1D";
            HCCL_WARNING(
                "[AlltoAllAutoSelector][%s] algo[%s] not registered, fallback.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        }
        HCCL_WARNING("[AlltoAllAutoSelector] pcie mixed topo is not supported yet for ccu schedule mode.");
        return SelectorStatus::NOT_MATCH;
    }
    uint32_t dataTypeSize = DATATYPE_SIZE_TABLE[opParam.all2AllVDataDes.sendType];
    u64* sendCounts = reinterpret_cast<u64*>(opParam.all2AllVDataDes.sendCounts);
    CHK_PRT_RET(
        sendCounts == nullptr, HCCL_WARNING("[AlltoAllAutoSelector][%s] sendCounts is nullptr.", __func__),
        SelectorStatus::NOT_MATCH);
    // AllToAll（非 AllToAllV）场景下各 rank 的 sendCount 相同，取 sendCounts[0] 作为代表
    uint64_t dataSize = sendCounts[0] * dataTypeSize;
    bool isMeshNumEqualToClosNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_DEBUG("[AlltoAllAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
    if ((isMeshNumEqualToClosNum == true) && (topoInfo->userRankSize <= CONCURRENT_RANK_LIMIT) &&
        (dataSize > BIG_DATA_SIZE_LIMIT)) {
        selectAlgName = "CcuAllToAllMesh1DConcurrent";
    } else {
        selectAlgName = "CcuAlltoAllMesh1DMultiJetty";
    }
    HCCL_WARNING("[AlltoAllAutoSelector][%s] algo[%s] not registered, fallback.", __func__, selectAlgName.c_str());
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AlltoAllAutoSelector::SelectSingleLevelAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    if (topoInfo->level0Topo == Level0Shape::MESH_1D && topoInfo->userRankSize <= CCU_MESH1D_MAX_RANK_SIZE) {
        if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_REGULAR) {
            selectAlgName = "CcuAllToAllMesh2Die";
            HCCL_WARNING(
                "[AlltoAllAutoSelector][%s] algo[%s] not registered, fallback.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        }
        if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
            HCCL_DEBUG("[AlltoAllAutoSelector][%s] TWO_DIE_NOT_REGULAR not match", __func__);
            return SelectorStatus::NOT_MATCH;
        }
        uint64_t dataSize = CalcDataSize(opParam, topoInfo);
        if (IsDevType960() && dataSize > SMALL_COUNT_16M && IsTwoLevelNetLayer(topoInfo)) {
            selectAlgName = "CcuAllToAllSoleMeshScheConcur";
            HCCL_WARNING(
                "[AlltoAllAutoSelector][%s] algo[%s] not registered, fallback.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        } else {
            selectAlgName = "CcuAlltoAllMesh1D";
        }
        return SelectorStatus::MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        return SelectMesh1DClosAlgo(topoInfo, opParam, selectAlgName);
    }
    HCCL_DEBUG("[Algo][AlltoAllAutoSelector] algo is not supported yet for ccu_schedule mode, reset to default.");
    return SelectorStatus::NOT_MATCH;
}

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
        selectAlgName = "AicpuAllToAllSoleMesh";
        HCCL_INFO("[AlltoAllAutoSelector] Algo match[%s].", selectAlgName.c_str());
        return SelectorStatus::MATCH;
    } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        // PCIE-SW定制机型，使用mesh1d算法
        if (topoInfo->level0PcieMix) {
            selectAlgName = "AicpuAllToAllSoleMesh";
            HCCL_INFO("[AlltoAllAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
            return SelectorStatus::MATCH;
        }
    }
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS || topoInfo->level0PcieMix) {
        HCCL_WARNING(
            "[AlltoAllAutoSelector] rankSize[%u], level0Topo[%u] is not supported by local AICPU AllToAll.",
            topoInfo->userRankSize, static_cast<u32>(topoInfo->level0Topo));
        return SelectorStatus::NOT_MATCH;
    }
    bool isMeshNumEqualToClosNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_ERROR("[AlltoAllAutoSelector] failed to compare Mesh/CLOS rank counts."), SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        opParam.all2AllVDataDes.sendCounts == nullptr, HCCL_WARNING("[AlltoAllAutoSelector] sendCounts is nullptr."),
        SelectorStatus::NOT_MATCH);
    const auto* sendCounts = static_cast<const u64*>(opParam.all2AllVDataDes.sendCounts);
    const bool concurrentCondition = isMeshNumEqualToClosNum && topoInfo->userRankSize <= CONCURRENT_RANK_LIMIT &&
                                     sendCounts[0] > CONCURRENT_COUNT_LIMIT;
    selectAlgName = concurrentCondition ? "AicpuAllToAllSoleMeshConcurrent" : "AicpuAllToAllSoleMeshUBX";
    HCCL_INFO("[AlltoAllAutoSelector] Algo match[%s].", selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AlltoAllAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)configAlgMap;
    HCCL_DEBUG("[AlltoAllAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    if (topoInfo->level2Ubg) {
        HCCL_INFO(
            "[AlltoAllAutoSelector][%s] ccu schedule is not supported with level2Ubg, reset to default.", __func__);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums >= TOPO_LEVEL_NUM_3) {
        HCCL_INFO(
            "[AlltoAllAutoSelector][%s] ccu schedule is not supported when topoLevelNums >= 3(levelNum[%u]), reset to "
            "default.",
            __func__, topoInfo->topoLevelNums);
        return SelectorStatus::NOT_MATCH;
    }
    CHK_PRT_RET(
        IsInputOutputOverlap(opParam) == true,
        HCCL_WARNING("[Algo][AlltoAllAutoSelector] ccu schedule does not support inplace alltoall."),
        SelectorStatus::NOT_MATCH);

    SelectorStatus status = (topoInfo->topoLevelNums > 1) ? SelectMultiLevelAlgo(topoInfo, selectAlgName) :
                                                            SelectSingleLevelAlgo(topoInfo, opParam, selectAlgName);
    if (status == SelectorStatus::MATCH) {
        HCCL_INFO("[AlltoAllAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    }
    return status;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLTOALL, 18, AlltoAllAutoSelector);
} // namespace mc2_ops_hccl
