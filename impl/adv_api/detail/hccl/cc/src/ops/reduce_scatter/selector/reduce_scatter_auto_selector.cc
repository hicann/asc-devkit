/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "reduce_scatter_auto_selector.h"
#include "selector_registry.h"

namespace mc2_ops_hccl {
constexpr u32 MAX_RANK_NUM_FOR_CONCURRENT_ALGO = 4;
constexpr u64 RS_AICPU_1D_MAX_DATA_SIZE = 16 * 1024 * 1024;
constexpr u64 RS_AICPU_1D_MIN_DATA_SIZE = 4 * 1024 * 1024;
constexpr u64 RS_AICPU_1D_TWO_LEVER_DATA_SIZE_THRESHOLD = 1536 * 1024 * 1024;
constexpr u64 RS_AICPU_SEQUENCE_SIZE_THRESHOLD = 1 * 1024 * 1024 * 1024;

SelectorStatus ReduceScatterAutoSelector::SelectCcuMsAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    if (topoInfo->topoLevelNums > 1) {
        HCCL_WARNING("[ReduceScatterAutoSelector] layerNum > 1 is not supported yet for ccu_ms mode.");
        return SelectorStatus::NOT_MATCH;
    }
    // MS 模式不支持 int8
    CHK_PRT_RET(
        opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT8,
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] dataType[%d] is not supported yet for ccu_ms mode.", opParam.DataDes.dataType),
        SelectorStatus::NOT_MATCH);

    // MS 模式不支持 PROD
    CHK_PRT_RET(
        opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD,
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] ReduceOp[%d] is not supported yet for ccu_ms mode.", opParam.reduceType),
        SelectorStatus::NOT_MATCH);

    if (Is64BitDataType(opParam.DataDes.dataType)) {
        HCCL_WARNING("[ReduceScatterAutoSelector] ccu_ms mode not support INT64, UINT64, FP64.");
        return SelectorStatus::NOT_MATCH;
    }

    SelectorStatus ret = SelectMeshAlgoCcums(topoInfo, opParam, selectAlgName);
    if (ret == SelectorStatus::MATCH) {
        HCCL_INFO("[ReduceScatterAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    }
    return ret;
}

SelectorStatus ReduceScatterAutoSelector::SelectMeshAlgoCcums(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
        if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_REGULAR) {
            selectAlgName = "CcuReduceScatterMesh2Die";
        } else if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
            HCCL_INFO("[%s] TWO_DIE_NOT_REGULAR not match", __func__);
            return SelectorStatus::NOT_MATCH;
        } else {
            selectAlgName = "CcuReduceScatterMesh1D";
        }
    } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        // UBX机型
        bool isMeshNumEqualToClosNum = false;
        bool isClosNumMultipleOfMeshNum = false;
        CHK_PRT_RET(
            CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
            HCCL_ERROR("[ReduceScatterAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
        CHK_PRT_RET(
            CheckClosNumMultipleOfMeshNum(topoInfo, isClosNumMultipleOfMeshNum) != HCCL_SUCCESS,
            HCCL_ERROR("[ReduceScatterAutoSelector] CheckClosNumMultipleOfMeshNum failed."), SelectorStatus::NOT_MATCH);
        if (isMeshNumEqualToClosNum && topoInfo->userRankSize <= MAX_RANK_NUM_FOR_CONCURRENT_ALGO) {
            // 4P mesh
            if (IsSmallData(dataSize)) {
                // 小数据量，用1d mesh算法
                selectAlgName = "CcuReduceScatterMesh1D";
            } else {
                // 大数据量，用mesh+clos并行算法
                selectAlgName = "CcuReduceScatterConcurrentMeshNHRMs";
            }
        } else if (isClosNumMultipleOfMeshNum && !IsSmallData(dataSize)) {
            HCCL_WARNING("[%s] MESH_1D_CLOS not match.", __func__);
            return SelectorStatus::NOT_MATCH;
        } else {
            selectAlgName = "CcuReduceScatterMesh1D";
        }
    } else {
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] level0Topo[%d] is not supported yet for ccu_ms mode.", topoInfo->level0Topo);
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_INFO("[ReduceScatterAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    // KFC ReduceScatter currently supports only single-level, single-die Mesh1D Mem2Mem.
    CHK_PRT_RET(
        opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD,
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] ReduceOp[%d] is not supported yet for ccu schedule mode.", opParam.reduceType),
        SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT8,
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] dataType[%d] is not supported by KFC ReduceScatter.",
            opParam.DataDes.dataType),
        SelectorStatus::NOT_MATCH);
    if (Is64BitDataType(opParam.DataDes.dataType)) {
        HCCL_WARNING("[ReduceScatterAutoSelector] KFC ReduceScatter does not support INT64, UINT64 or FP64.");
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums != 1 || topoInfo->level0Topo != Level0Shape::MESH_1D ||
        topoInfo->level0MeshType != Level0MeshType::SINGLE_DIE) {
        HCCL_WARNING(
            "[ReduceScatterAutoSelector] unsupported KFC topology, levelNum[%u], level0Topo[%d], meshType[%d].",
            topoInfo->topoLevelNums, topoInfo->level0Topo, topoInfo->level0MeshType);
        return SelectorStatus::NOT_MATCH;
    }
    return SelectMeshAlgoCcuSchedule(topoInfo, opParam, selectAlgName);
}

SelectorStatus ReduceScatterAutoSelector::SelectMeshAlgoCcuSchedule(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D && topoInfo->level0MeshType == Level0MeshType::SINGLE_DIE) {
        double ratio; // 以8卡为基线确定ratio，用来表示不同卡数对下发的影响系数
        if (topoInfo->userRankSize == 0) {
            HCCL_WARNING("[ReduceScatterAutoSelector]the selector is not set topoInfo->userRankSize]");
            ratio = 1;
        } else {
            ratio = DEFAULT_RANK_SIZE / topoInfo->userRankSize;
        }
        if (dataSize * ratio >= RS_M2M_1D_MAX_DATA_SIZE) {
            HCCL_DEBUG(
                "[ReduceScatterAutoSelector] dataSize[%lu] * ratio[%f] >= MAX_DATA_SIZE[%lu].", dataSize, ratio,
                RS_M2M_1D_MAX_DATA_SIZE);
            return SelectorStatus::NOT_MATCH;
        }
        selectAlgName = "CcuKfcReduceScatterMesh1DMem2Mem";
    } else {
        HCCL_DEBUG("[ReduceScatterAutoSelector] only single-die MESH_1D matches the KFC path.");
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    (void)configAlgMap;
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    if (topoInfo->topoLevelNums > 1) {
        if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
            selectAlgName = "InsReduceScatterAicpuReduceNHR";
        } else if (topoInfo->Level1Nhr) {
            selectAlgName = "InsReduceScatterNHR";
            HCCL_INFO("[ReduceScatterAutoSelector] Level1Nhr=true, select [%s]", selectAlgName.c_str());
        } else if (topoInfo->Level0Nhr) {
            selectAlgName = "InsReduceScatterNHR"; // InsReduceScatterParallelNHRNHR备用
        } else if (
            topoInfo->netLayerDetails.localNetInsSizeOfLayer.at(0) > 1 &&
            topoInfo->level0Topo == Level0Shape::MESH_1D) {
            if (dataSize > RS_AICPU_1D_MIN_DATA_SIZE) {
                selectAlgName = (dataSize * topoInfo->userRankSize > RS_AICPU_SEQUENCE_SIZE_THRESHOLD) ?
                                    "InsReduceScatterSequenceMesh1DNhr" :
                                    "InsReduceScatterParallelMesh1DNHR";
            } else {
                selectAlgName = "InsReduceScatterNHR";
            }
        } else if (
            topoInfo->netLayerDetails.localNetInsSizeOfLayer.at(0) == 1 || topoInfo->level0Topo == Level0Shape::CLOS) {
            selectAlgName = "InsReduceScatterNHR"; // InsReduceScatterParallelNHRNHR备用
        } else {
            HCCL_ERROR(
                "[ReduceScatterAutoSelector] topo not match, level0Topo [%d], deviceNumPerModule [%d]",
                topoInfo->level0Topo, topoInfo->netLayerDetails.localNetInsSizeOfLayer.at(0));
            return SelectorStatus::NOT_MATCH;
        }
    } else {
        return SelectMeshAlgoAicpu(topoInfo, opParam, selectAlgName);
    }

    return SelectorStatus::MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectMeshAlgoAicpu(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;
    double ratio;
    if (topoInfo->userRankSize == 0) {
        HCCL_WARNING("[ReduceScatterAutoSelector]the selector is not set userRankSize]");
        ratio = 1;
    } else {
        ratio = (DEFAULT_RANK_SIZE / topoInfo->userRankSize) * (DEFAULT_RANK_SIZE / topoInfo->userRankSize);
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
        if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
            selectAlgName = "InsReduceScatterMesh1D";
        } else {
            if (IsTwoLevelNetLayer(topoInfo)) {
                if (dataSize * topoInfo->userRankSize > RS_AICPU_1D_TWO_LEVER_DATA_SIZE_THRESHOLD) {
                    selectAlgName = "InsReduceScatterMesh1DZAxisDetour";
                } else if (dataSize * ratio > RS_AICPU_1D_MAX_DATA_SIZE) {
                    selectAlgName = "InsReduceScatterMesh1DMeshChunk";
                } else {
                    selectAlgName = "InsReduceScatterMesh1D";
                }
            } else {
                if (dataSize * ratio > RS_AICPU_1D_MAX_DATA_SIZE) {
                    selectAlgName = "InsReduceScatterMesh1DMeshChunk";
                } else {
                    selectAlgName = "InsReduceScatterMesh1D";
                }
            }
        }
    } else if (topoInfo->level0Topo == Level0Shape::CLOS) {
        if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
            selectAlgName = "InsReduceScatterAicpuReduceNHR";
        } else {
            selectAlgName = "InsReduceScatterNHR";
        }
    } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        bool isClosNumMultipleOfMeshNum = false;
        CHK_PRT_RET(
            CheckClosNumMultipleOfMeshNum(topoInfo, isClosNumMultipleOfMeshNum) != HCCL_SUCCESS,
            HCCL_ERROR("[ReduceScatterAutoSelector] CheckClosNumMultipleOfMeshNum failed."), SelectorStatus::NOT_MATCH);
        return SelectMeshAlgoAicpuForMesh1DClos(
            topoInfo, opParam, dataSize, ratio, isClosNumMultipleOfMeshNum, selectAlgName);
    } else {
        HCCL_WARNING("[ReduceScatterAutoSelector] topo not match");
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_DEBUG("[%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectAivAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    // aiv 模式不支持 PROD
    CHK_PRT_RET(
        opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD,
        HCCL_WARNING("[ReduceScatterAutoSelector] ReduceOp[%d] is not supported yet for aiv mode.", opParam.reduceType),
        SelectorStatus::NOT_MATCH);

    if (Is64BitDataType(opParam.DataDes.dataType)) {
        HCCL_WARNING("[ReduceScatterAutoSelector] aiv mode not support INT64, UINT64, FP64.");
        return SelectorStatus::NOT_MATCH;
    }

    selectAlgName = "AivReduceScatterMesh1D";
    HCCL_DEBUG("[ReduceScatterAutoSelector][%s] end, selectAlgName[%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectDPUAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    HCCL_INFO(
        "topoInfo->topoLevelNums is %u, topoInfo->level0Topo is %u", topoInfo->topoLevelNums, topoInfo->level0Topo);
    (void)configAlgMap;
    if (topoInfo->topoLevelNums > 1) {
        selectAlgName = "InsReduceScatterSequenceMeshMeshDPU";
        HCCL_INFO("Using algo InsReduceScatterSequenceMeshMeshDPU");
        return SelectorStatus::MATCH;
    }

    return SelectorStatus::NOT_MATCH;
}

SelectorStatus ReduceScatterAutoSelector::SelectMeshAlgoAicpuForMesh1DClos(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, u64 dataSize, double ratio,
    bool isClosNumMultipleOfMeshNum, std::string& selectAlgName) const
{
    if (topoInfo->level0PcieMix) {
        // PCIE mixed topology uses the parallel AICPU executor in this scope.
        if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
            selectAlgName = "InsReduceScatterMesh1D";
        } else if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
            selectAlgName = "InsReduceScatterAicpuReduceNHR";
        } else {
            selectAlgName = "InsReduceScatterParallelMesh1DNHRPcie";
        }
    } else if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
        // MESH_1D 即可链接所有卡， 使用 MESH_1D 算法
        if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
            selectAlgName = "InsReduceScatterMesh1D";
        } else if (!IsSmallData(dataSize)) {
            selectAlgName = "InsReduceScatterMesh1D";
        } else {
            if (dataSize * ratio > RS_AICPU_1D_MAX_DATA_SIZE) {
                selectAlgName = "InsReduceScatterMesh1DMeshChunk";
            } else {
                selectAlgName = "InsReduceScatterMesh1D";
            }
        }
    } else if (Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD) {
        selectAlgName = "InsReduceScatterAicpuReduceNHR";
    } else if (isClosNumMultipleOfMeshNum && !IsSmallData(dataSize)) {
        selectAlgName = "InsReduceScatterParallelMesh1DNHRUBX";
    } else {
        selectAlgName = "InsReduceScatterNHR";
    }
    HCCL_DEBUG("[%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, 18, ReduceScatterAutoSelector);
} // namespace mc2_ops_hccl
