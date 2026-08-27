/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "all_gather_auto_selector.h"
#include "selector_registry.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {
constexpr u64 AG_2D_SMALL_DATA_SIZE = 1024 * 1024;
constexpr u32 MAX_RANK_NUM_FOR_CONCURRENT_ALGO = 4;
constexpr u32 PCIE_BASIC_RS_MAX_DATA_SIZE = 4 * 1024 * 1024;

SelectorStatus AllGatherAutoSelector::SelectCcuMsAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start, topoInfo topoLevelNums[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    // Temporary KFC convergence: only enable single-level Mesh1D Mem2Mem before other CCU paths are verified.
    if (topoInfo->topoLevelNums > 1 || topoInfo->level0Topo != Level0Shape::MESH_1D) {
        HCCL_DEBUG("[AllGatherAutoSelector] only single level MESH_1D is supported for ccu_ms mode.");
        return SelectorStatus::NOT_MATCH;
    }
    if (IsInputOutputOverlap(opParam)) {
        HCCL_WARNING("[Algo][AllGatherAutoSelector] ccu_ms does not support inplace allgather.");
        return SelectorStatus::NOT_MATCH;
    }
    selectAlgName = "CcuAllGatherMesh1DMem2Mem";
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectMeshAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start", __func__);
    // Temporary KFC convergence: keep ccu_ms selection on Mesh1D Mem2Mem only.
    if (topoInfo->level0Topo != Level0Shape::MESH_1D) {
        HCCL_DEBUG(
            "[AllGatherAutoSelector] Level0Topo[%u] is not supported for ccu_ms mode, reset to default.",
            topoInfo->level0Topo);
        return SelectorStatus::NOT_MATCH;
    }
    if (IsInputOutputOverlap(opParam)) {
        HCCL_WARNING("[Algo][AllGatherAutoSelector] ccu_ms does not support inplace allgather.");
        return SelectorStatus::NOT_MATCH;
    }
    selectAlgName = "CcuAllGatherMesh1DMem2Mem";
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectCcuScheduleUBXAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName, const u64 dataSize) const
{
    if (topoInfo->level0PcieMix) {
        return SelectorStatus::NOT_MATCH;
    }
    bool meshNumEqualsClosNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, meshNumEqualsClosNum) != HCCL_SUCCESS,
        HCCL_DEBUG("[AllGatherAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
    if (dataSize > SMALL_COUNT_512KB && meshNumEqualsClosNum &&
        topoInfo->userRankSize <= MAX_RANK_NUM_FOR_CONCURRENT_ALGO) {
        selectAlgName = KFC_CONCURRENT_ALL_GATHER_ALG_NAME;
        return SelectorStatus::MATCH;
    }
    selectAlgName = "CcuAllGatherMesh1DMem2Mem";
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectCcuScheduleLevel0Algo(
    const TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName, const u64 dataSize) const
{
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        return SelectCcuScheduleUBXAlgo(topoInfo, selectAlgName, dataSize);
    }
    // Temporary KFC convergence: route only regular Mesh1D to the KFC Mem2Mem implementation.
    if (topoInfo->level0Topo != Level0Shape::MESH_1D ||
        topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
        HCCL_DEBUG(
            "[AllGatherAutoSelector] level0Topo[%d], level0MeshType[%d] is not supported for ccu schedule mode.",
            topoInfo->level0Topo, topoInfo->level0MeshType);
        return SelectorStatus::NOT_MATCH;
    }

    selectAlgName = "CcuAllGatherMesh1DMem2Mem";
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start", __func__);
    (void)configAlgMap;
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    if (IsInputOutputOverlap(opParam)) {
        HCCL_WARNING("[Algo][AllGatherAutoSelector] ccu schedule does not support inplace allgather.");
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums > 1) {
        // Temporary KFC convergence: multilevel NHR/parallel CCU variants are not selected until they are
        // re-registered.
        HCCL_DEBUG("[AllGatherAutoSelector] multi-level topo is not supported for ccu schedule mode.");
        return SelectorStatus::NOT_MATCH;
    } else {
        return SelectCcuScheduleLevel0Algo(topoInfo, selectAlgName, dataSize);
    }
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start, topoInfo topoLevelNums[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    HCCL_INFO(
        "[AllGatherAutoSelector][SelectAicpuAlgo] topoLevelNums=[%d], deviceNumPerModule=[%d], level0Topo=[%d]",
        topoInfo->topoLevelNums, topoInfo->deviceNumPerModule, topoInfo->level0Topo);
    if (topoInfo->topoLevelNums > 1) {
        if (topoInfo->level0PcieMix) {
            // pcie mixed + 多层topology，不支持
            return SelectorStatus::NOT_MATCH;
        }
        // Level1Nhr 已在 CalcTopoShape 中设置（GCD==1 时为 true）
        if (topoInfo->Level1Nhr) {
            selectAlgName = "InsAllGatherNHR";
            HCCL_INFO("[AllGatherAutoSelector] Level1Nhr=true, select [%s]", selectAlgName.c_str());
        } else if (topoInfo->Level0Nhr) {
            selectAlgName = "InsAllGatherNHR"; // 预留给NHRNHR
        } else if (topoInfo->netLayerDetails.localNetInsSizeOfLayer[0] == 1) {
            selectAlgName = "InsAllGatherNHR";
        } else if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
            selectAlgName = "InsAllGatherParallelMesh1DNHR";
        } else {
            HCCL_ERROR("[AllGatherAutoSelector] topo not match");
            return SelectorStatus::NOT_MATCH;
        }
    } else {
        if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
            selectAlgName = "InsAllGatherMesh1D";
        } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
            // PCIE-SW定制机型，Mesh无法链接全卡时，需要跨pcie链路，选择适配算法
            if (topoInfo->level0PcieMix) {
                if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
                    selectAlgName = "InsAllGatherMesh1D";
                } else {
                    selectAlgName = (dataSize < PCIE_BASIC_RS_MAX_DATA_SIZE) ? "InsAllGatherParallelMesh1DNHRPcie" :
                                                                               "AicpuAllGatherPipeLinePcie";
                }
                HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            // UBX机型
            bool isMeshNumEqualToClosNum = false;
            bool isClosNumMultipleOfMeshNum = false;
            CHK_PRT_RET(
                CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
                HCCL_ERROR("[AllGatherAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
            CHK_PRT_RET(
                CheckClosNumMultipleOfMeshNum(topoInfo, isClosNumMultipleOfMeshNum) != HCCL_SUCCESS,
                HCCL_ERROR("[AllGatherAutoSelector] CheckClosNumMultipleOfMeshNum failed."), SelectorStatus::NOT_MATCH);
            if (isMeshNumEqualToClosNum && (topoInfo->userRankSize <= MAX_RANK_NUM_FOR_CONCURRENT_ALGO)) {
                if (dataSize > SMALL_COUNT_512KB) {
                    selectAlgName = "InsAllGatherConcurrentMesh1DNHR";
                } else {
                    selectAlgName = "InsAllGatherMesh1D";
                }
            } else if (isClosNumMultipleOfMeshNum && dataSize > SMALL_COUNT_512KB) {
                selectAlgName = "InsAllGatherParallelMesh1DNHRUBX";
            } else {
                selectAlgName = "InsAllGatherNHR";
            }
        } else if (topoInfo->level0Topo == Level0Shape::CLOS) {
            selectAlgName = "InsAllGatherNHR";
        } else {
            HCCL_ERROR("[AllGatherAutoSelector] topo not match");
            return SelectorStatus::NOT_MATCH;
        }
    }
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectAivAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start, topoInfo topoLevelNums[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    (void)opParam;

    selectAlgName = "AivAllGatherMesh1D";
    HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllGatherAutoSelector::SelectDPUAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[AllGatherAutoSelector][%s] start, topoInfo topoLevelNums[%u]", __func__, topoInfo->topoLevelNums);
    if (topoInfo->topoLevelNums > 1) {
        if ((topoInfo->netLayerDetails.localNetInsSizeOfLayer[0] == 1) ||
            (topoInfo->level0Topo == Level0Shape::MESH_1D)) {
            selectAlgName = "InsAllGatherMeshNhrDPU";
            HCCL_DEBUG("[AllGatherAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
            return SelectorStatus::MATCH;
        }
    }
    HCCL_DEBUG("[AllGatherAutoSelector][%s] end", __func__);
    return SelectorStatus::NOT_MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLGATHER, 18, AllGatherAutoSelector);

} // namespace mc2_ops_hccl
