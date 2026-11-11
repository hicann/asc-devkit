/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "all_reduce_auto_selector.h"
#include "selector_registry.h"

namespace mc2_ops_hccl {

constexpr u64 AR_M2M_1D_MAX_DATA_SIZE = 8 * 1024 * 1024;
namespace {
constexpr u64 AR_AICPU_1D_SMALL_DATA_SIZE = 8 * 1024 * 1024;
constexpr u64 AR_AICPU_1D_MAX_DATA_SIZE = 32 * 1024 * 1024;
constexpr u64 AR_AICPU_1D_CROSS_SMALL_DATA_SIZE = 32 * 1024 * 1024;
constexpr u64 AR_AICPU_1D_64DATATYPE_DATA_SIZE = 8 * 1024 * 1024;
constexpr u64 AR_AICPU_SEQUENCE_DATA_SIZE = 4ULL * 1024 * 1024 * 1024;
constexpr u64 AR_2P_DETOUR_DATA_SIZE = 8 * 1024 * 1024;
constexpr u64 AR_PCIE_PARALLEL_MAX_DATA_SIZE = 32ULL * 1024ULL * 1024ULL;
// 与 hccl all_reduce_auto_selector.cc 常量取值一致（CcuSchedAllReduce* 选择链路）。
constexpr u32 TOPO_LEVEL_NUM_3 = 3;
constexpr u32 MAX_RANK_NUM_FOR_CONCURRENT_ALGO = 4;
constexpr u32 AR_CCU_MAX_RANK_SIZE = 64;
constexpr u64 AR_MORE_64P_SEQ_MAX_DATA_SIZE = 32 * 1024 * 1024;
constexpr u64 RS_MAX_DATA_SIZE = 16 * 1024 * 1024;
constexpr u64 AR_FLATTEN_MAX_DATA_SIZE = 512 * 1024;
constexpr u64 AR_CCU_SEQ_MAX_DATA_SIZE = 64 * 1024 * 1024;
constexpr u64 AR_CCU_CLOS_1D_SMALL_DATA_SIZE = 8 * 1024 * 1024;
constexpr u64 OMNI_UBX_AR_SCHED_DATA_SIZE = 64 * 1024 * 1024;
constexpr u64 CCU_PARALLEL_MAX_DATA_SIZE = 64 * 1024 * 1024;
constexpr u64 OMNI_PCIE_AR_DATA_SIZE = 32 * 1024 * 1024;
constexpr u64 OMNI_UBX_AR_LARGE_SIZE = 128 * 1024 * 1024;
constexpr u64 OMNI_UBX_AR_16P_LARGE_SIZE = 256 * 1024 * 1024;
constexpr u32 OMNI_UBX_AR_RANK_SIZE = 16;
constexpr u32 DEVICE_NUM_PER_MODULE_8 = 8;
constexpr u32 TOPO_LEVEL_NUM_2 = 2;
constexpr u32 MIN_STRICT_RANK_NUM_ORDER_PRESERVED = 2;

// hccl AutoSelectorBase 的同名 helper（asc-devkit base 未同步，按文件内联等价实现）
bool Is8BitDataType(const HcclDataType dataType) { return dataType == HcclDataType::HCCL_DATA_TYPE_INT8; }

// hccl 口径：dataSize <= CCU_PARALLEL_MAX_DATA_SIZE 即小数据（rankSize 仅用于告警）
bool IsSmallDataCCU(const u64 dataSize, const u64 rankSize)
{
    if (rankSize == 0) {
        HCCL_WARNING("the selector is not set RankSize");
    }
    return dataSize <= CCU_PARALLEL_MAX_DATA_SIZE;
}
} // namespace

SelectorStatus AllReduceAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)configAlgMap;
    if (topoInfo == nullptr) {
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_DEBUG("[AllReduceAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    u64 dataSize = opParam.DataDes.count * perDataSize;

    // 当前调用路径（MC2 tiling 无 count 字段）不下发 DataDes.count，dataSize 恒为 0，无法按真实
    // 数据量选路。约定 dataSize==0 时统一按小数据量算法选路：0 小于全部阈值，本函数及
    // SelectMeshAlgoAicpu/SelectMeshAlgoAicpuUBX 中所有依赖 dataSize 的分支均命中小数据侧
    // （MESH_1D/PCIe 全连通/UBX 4P 选 SoleMeshOneShot，多层 MESH_1D 选 SoleNHR，
    // PCIe 非全连通选 ParallelMeshNHRPcie，UBX 矩形场景选 SoleNHR），
    // TwoShot/ChunkTwoShot/PipeLinePcie 等大数据量算法不会被选中。
    if (dataSize == 0) {
        HCCL_WARNING(
            "[AllReduceAutoSelector][%s] dataSize is 0 (DataDes.count not delivered), treat as small data, all "
            "dataSize-based branches select small-data algorithms.",
            __func__);
    }

    bool isDataTypeOrReduceTypeSpecial = opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT64 ||
                                         opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
                                         opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_FP64 ||
                                         opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD;

    if (topoInfo->topoLevelNums > 1) {
        bool level0AndLevel1Symetric = topoInfo->level0Symmetric && topoInfo->level1Symmetric;
        if (isDataTypeOrReduceTypeSpecial) {
            selectAlgName = "AicpuAllReduceSoleNHRAicpuReduce";
        } else if (
            level0AndLevel1Symetric && topoInfo->deviceNumPerModule == DEVICE_NUM_PER_MODULE_8 &&
            topoInfo->topLevelUboe) {
            // hccl：topoLevelNums==2 选 AicpuAllReducePipeLineMeshNHR，否则选 AicpuAllReducePipeLineMeshNHRNHR。
            // devkit 均未迁移这两个算法，保留选路逻辑，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algos[AicpuAllReducePipeLineMeshNHR/AicpuAllReducePipeLineMeshNHRNHR]"
                " are not migrated to devkit, levelNum[%u].",
                topoInfo->topoLevelNums);
            return SelectorStatus::NOT_MATCH;
        } else if (level0AndLevel1Symetric && topoInfo->topoLevelNums == TOPO_LEVEL_NUM_3 && topoInfo->topLevelUboe) {
            selectAlgName = "AicpuAllReduceParallelMeshNHR";
        } else if (topoInfo->Level1Nhr) {
            // Level1Nhr 已在 CalcTopoShape 中设置（GCD==1 时为 true）
            selectAlgName = "AicpuAllReduceSoleNHR";
            HCCL_INFO("[AllReduceAutoSelector] Level1Nhr=true, select [%s]", selectAlgName.c_str());
        } else if (topoInfo->netLayerDetails.localNetInsSizeOfLayer[0] == 1) {
            selectAlgName = "AicpuAllReduceSoleNHR";
        } else if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
            if (topoInfo->topoLevelNums == TOPO_LEVEL_NUM_3) {
                if (level0AndLevel1Symetric) {
                    // hccl：AicpuAllReduceSequenceMeshConcurNHRNHR；devkit 未迁移，返回 NOT_MATCH。
                    HCCL_WARNING(
                        "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSequenceMeshConcurNHRNHR] is not migrated to "
                        "devkit.");
                    return SelectorStatus::NOT_MATCH;
                }
                selectAlgName = "AicpuAllReduceSoleNHR";
            } else if (topoInfo->topoLevelNums == TOPO_LEVEL_NUM_2) {
                if (dataSize > AR_AICPU_1D_CROSS_SMALL_DATA_SIZE) {
                    if (dataSize > AR_AICPU_SEQUENCE_DATA_SIZE) {
                        // hccl：AicpuAllReduceSequenceMeshConcurNHR；devkit 未迁移，返回 NOT_MATCH。
                        HCCL_WARNING(
                            "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSequenceMeshConcurNHR] is not migrated to "
                            "devkit, dataSize[%llu].",
                            dataSize);
                        return SelectorStatus::NOT_MATCH;
                    }
                    selectAlgName = "AicpuAllReduceParallelMeshNHR";
                } else {
                    selectAlgName = "AicpuAllReduceSoleNHR";
                }
            } else {
                selectAlgName = "AicpuAllReduceSoleNHR";
            }
        } else if (topoInfo->level0Topo == Level0Shape::CLOS) {
            // hccl：AicpuAllReduceSoleNHRMultiLink；devkit 未迁移，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSoleNHRMultiLink] is not migrated to devkit.");
            return SelectorStatus::NOT_MATCH;
        } else {
            return SelectorStatus::NOT_MATCH;
        }
    } else {
        return SelectMeshAlgoAicpu(topoInfo, opParam, selectAlgName);
    }

    HCCL_DEBUG("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllReduceAutoSelector::SelectMeshAlgoAicpuUBX(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, const u64 dataSize, std::string& selectAlgName,
    bool isDataTypeOrReduceTypeSpecial) const
{
    // UBX机型
    bool isMeshNumEqualToClosNum = false;
    bool isClosNumMultipleOfMeshNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_ERROR("[Algo][AllReduceAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        CheckClosNumMultipleOfMeshNum(topoInfo, isClosNumMultipleOfMeshNum) != HCCL_SUCCESS,
        HCCL_ERROR("[Algo][AllReduceAutoSelector] CheckClosNumMultipleOfMeshNum failed."), SelectorStatus::NOT_MATCH);
    if (isMeshNumEqualToClosNum && topoInfo->userRankSize <= MAX_RANK_NUM_FOR_CONCURRENT_ALGO) {
        if (isDataTypeOrReduceTypeSpecial) {
            selectAlgName = dataSize <= AR_AICPU_1D_64DATATYPE_DATA_SIZE ? "AicpuAllReduceSoleMeshOneShot" :
                                                                           "AicpuAllReduceSoleMeshTwoShot";
        } else if (dataSize <= AR_AICPU_1D_SMALL_DATA_SIZE) {
            selectAlgName = "AicpuAllReduceSoleMeshOneShot";
        } else {
            // 大数据量，用mesh+clos并行算法
            // hccl：AicpuAllReduceConcurMeshTwoShotNHR；devkit 未迁移，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algo[AicpuAllReduceConcurMeshTwoShotNHR] is not migrated to devkit, "
                "dataSize[%llu].",
                dataSize);
            return SelectorStatus::NOT_MATCH;
        }
    } else if (isDataTypeOrReduceTypeSpecial) {
        selectAlgName = "AicpuAllReduceSoleNHRAicpuReduce";
    } else if (isClosNumMultipleOfMeshNum && IsLargeData(dataSize)) {
        if (opParam.supportSymmetricMemory &&
            ((dataSize >= OMNI_UBX_AR_LARGE_SIZE && topoInfo->userRankSize < OMNI_UBX_AR_RANK_SIZE) ||
             dataSize >= OMNI_UBX_AR_16P_LARGE_SIZE)) {
            // hccl：AicpuAllReducePipeLineMeshNHR；devkit 未迁移，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algo[AicpuAllReducePipeLineMeshNHR] is not migrated to devkit, "
                "dataSize[%llu].",
                dataSize);
            return SelectorStatus::NOT_MATCH;
        }
        // 矩形场景大数据量，用Parallel并行算法
        // hccl：AicpuAllReduceParallelMeshNHRMultiJetty；devkit 未迁移，返回 NOT_MATCH。
        HCCL_WARNING(
            "[AllReduceAutoSelector] hccl algo[AicpuAllReduceParallelMeshNHRMultiJetty] is not migrated to devkit, "
            "dataSize[%llu].",
            dataSize);
        return SelectorStatus::NOT_MATCH;
    } else {
        // 其他场景，用1d NHR算法
        selectAlgName = "AicpuAllReduceSoleNHR";
    }

    HCCL_DEBUG("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

SelectorStatus AllReduceAutoSelector::SelectMeshAlgoAicpu(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName) const
{
    u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    // dataSize==0 时统一按小数据量算法选路（0 小于全部阈值，各分支命中小数据侧），约定见
    // SelectAicpuAlgo 入口说明
    u64 dataSize = opParam.DataDes.count * perDataSize;

    bool isDataTypeOrReduceTypeSpecial = opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT64 ||
                                         opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
                                         opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_FP64 ||
                                         opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD;

    double ratio;
    if (topoInfo->userRankSize == 0) {
        HCCL_WARNING("[AllReduceAutoSelector] the selector userRankSize not set");
        ratio = 1;
    } else {
        ratio = DEFAULT_RANK_SIZE / topoInfo->userRankSize / topoInfo->userRankSize;
    }
    // devkit 的 IsTwoLevelNetLayer 无 opParam 入参（无 hostDPUOnly 前置检查），其余判定与 hccl 一致
    bool isTwoLevelFlag = IsTwoLevelNetLayer(topoInfo);
    bool overSequenceDataThreshold = dataSize > AR_AICPU_SEQUENCE_DATA_SIZE;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
        if (isDataTypeOrReduceTypeSpecial) {
            selectAlgName = dataSize <= AR_AICPU_1D_64DATATYPE_DATA_SIZE ? "AicpuAllReduceSoleMeshOneShot" :
                                                                           "AicpuAllReduceSoleMeshTwoShot";
        } else if (isTwoLevelFlag && topoInfo->userRankSize == 2 && dataSize >= AR_2P_DETOUR_DATA_SIZE) {
            // 两p条件下生效
            // hccl：AicpuAllReduceSoleMeshConcur；devkit 未迁移，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSoleMeshConcur] is not migrated to devkit, "
                "dataSize[%llu].",
                dataSize);
            return SelectorStatus::NOT_MATCH;
        } else if (dataSize <= AR_AICPU_1D_SMALL_DATA_SIZE) {
            selectAlgName = "AicpuAllReduceSoleMeshOneShot";
        } else if (dataSize * ratio > AR_AICPU_1D_MAX_DATA_SIZE) {
            if (isTwoLevelFlag && overSequenceDataThreshold) {
                // hccl：AicpuAllReduceSoleMeshConcur；devkit 未迁移，返回 NOT_MATCH。
                HCCL_WARNING(
                    "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSoleMeshConcur] is not migrated to devkit, "
                    "dataSize[%llu].",
                    dataSize);
                return SelectorStatus::NOT_MATCH;
            }
            selectAlgName = "AicpuAllReduceSoleMeshChunkTwoShot";
        } else {
            selectAlgName = "AicpuAllReduceSoleMeshTwoShot";
        }
    } else if (topoInfo->level0Topo == Level0Shape::CLOS) {
        if (isDataTypeOrReduceTypeSpecial) {
            selectAlgName = "AicpuAllReduceSoleNHRAicpuReduce";
        } else {
            // hccl：AicpuAllReduceSoleNHRMultiLink；devkit 未迁移，返回 NOT_MATCH。
            HCCL_WARNING(
                "[AllReduceAutoSelector] hccl algo[AicpuAllReduceSoleNHRMultiLink] is not migrated to devkit.");
            return SelectorStatus::NOT_MATCH;
        }
    } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        if (topoInfo->level0PcieMix) {
            if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
                if (isDataTypeOrReduceTypeSpecial) {
                    selectAlgName = dataSize <= AR_AICPU_1D_64DATATYPE_DATA_SIZE ? "AicpuAllReduceSoleMeshOneShot" :
                                                                                   "AicpuAllReduceSoleMeshTwoShot";
                } else if (dataSize <= AR_AICPU_1D_SMALL_DATA_SIZE) {
                    selectAlgName = "AicpuAllReduceSoleMeshOneShot";
                } else if (dataSize * ratio > AR_AICPU_1D_MAX_DATA_SIZE) {
                    selectAlgName = "AicpuAllReduceSoleMeshChunkTwoShot";
                } else {
                    selectAlgName = "AicpuAllReduceSoleMeshTwoShot";
                }
            } else {
                if (isDataTypeOrReduceTypeSpecial) {
                    // hccl：AicpuAllReduceSequenceMeshNHRAicpuReduce；devkit 未迁移，返回 NOT_MATCH。
                    HCCL_WARNING("[AllReduceAutoSelector] hccl algo[AicpuAllReduceSequenceMeshNHRAicpuReduce] is not "
                                 "migrated to devkit.");
                    return SelectorStatus::NOT_MATCH;
                }
                // hccl：< 32M 选 AicpuAllReduceParallelMeshNHR，>= 32M 选 AicpuAllReducePipeLineMeshNHR。
                // devkit 的 Parallel/PipeLine executor 注册在 TopoMatchPcieMix 上，对应 PCIe 机型实现为
                // AicpuAllreduceParallelMeshNHRPcie / AicpuAllReducePipeLinePcie。
                selectAlgName = (dataSize < OMNI_PCIE_AR_DATA_SIZE) ? "AicpuAllreduceParallelMeshNHRPcie" :
                                                                      "AicpuAllReducePipeLinePcie";
            }
        } else {
            return SelectMeshAlgoAicpuUBX(topoInfo, opParam, dataSize, selectAlgName, isDataTypeOrReduceTypeSpecial);
        }
    } else {
        HCCL_ERROR("[AllReduceAutoSelector] topo not match");
        return SelectorStatus::NOT_MATCH;
    }

    HCCL_DEBUG("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

// 对齐 hccl AllReduceAutoSelector::SelectCcuScheduleAlgo 分支框架：已注册算法（当前
// CcuSchedAllReduceSoleMesh / CcuSchedAllReduceSoleNHR）MATCH；未注册算法设名后 NOT_MATCH（由框架回退
// CCU_FAIL/AICPU）。
// hccl 侧 IsNeedStrictModeForOrderPreserved（保序模式回退 AICPU）asc-devkit 无对应机制，未移植。
SelectorStatus AllReduceAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)configAlgMap;
    const u32 ccuSize = AR_CCU_MAX_RANK_SIZE;
    HCCL_DEBUG("[AllReduceAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);

    if (topoInfo->level2UbRtp) {
        HCCL_INFO(
            "[AllReduceAutoSelector][%s] ccu schedule is not supported with level2UbRtp, reset to default.", __func__);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums >= TOPO_LEVEL_NUM_3) {
        HCCL_INFO(
            "[AllReduceAutoSelector][%s] ccu schedule is not supported when topoLevelNums >= 3(levelNum[%u]), reset "
            "to default.",
            __func__, topoInfo->topoLevelNums);
        return SelectorStatus::NOT_MATCH;
    }
    // ccu 模式不支持 PROD
    CHK_PRT_RET(
        opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD,
        HCCL_DEBUG(
            "[AllReduceAutoSelector] ReduceOp[%d] is not supported yet for ccu schedule mode.", opParam.reduceType),
        SelectorStatus::NOT_MATCH);
    if (Is64BitDataType(opParam.DataDes.dataType)) {
        HCCL_DEBUG("[AllReduceAutoSelector] ccu_schedule mode does not support INT64, UINT64, FP64.");
        return SelectorStatus::NOT_MATCH;
    }
    const u64 perDataSize = DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    const u64 dataSize = opParam.DataDes.count * perDataSize;

    if (topoInfo->topoLevelNums > 1) {
        if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
            CHK_PRT_RET(
                IsInputOutputOverlap(opParam) == true,
                HCCL_WARNING("[Algo][AllReduceAutoSelector] ccu_sched does not support in-place allreduce."),
                SelectorStatus::NOT_MATCH);
            // Level1Nhr 已在拓扑形状计算中设置（GCD==1 时为 true）
            if (topoInfo->Level1Nhr) {
                // 对齐 hccl：Level1Nhr 选 SoleNHR（asc-devkit 已注册）
                selectAlgName = "CcuSchedAllReduceSoleNHR";
                HCCL_INFO("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            // 与基类 IsTwoLevelNetLayer/IsLayerAllConnetedWithTopo 判空契约一致：层信息缺失时跳过单实例分支
            if (!topoInfo->netLayerDetails.localNetInsSizeOfLayer.empty() &&
                topoInfo->netLayerDetails.localNetInsSizeOfLayer[0] == 1) {
                // 对齐 hccl：机内单实例选 SoleNHR（asc-devkit 已注册）
                selectAlgName = "CcuSchedAllReduceSoleNHR";
                HCCL_INFO("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            if (topoInfo->is2DieFullMesh) {
                HCCL_DEBUG("[AllReduceAutoSelector] 2DieFullMesh is not supported yet for ccu schedule mode.");
                return SelectorStatus::NOT_MATCH;
            }
            if (dataSize <= AR_MORE_64P_SEQ_MAX_DATA_SIZE && topoInfo->userRankSize > ccuSize) {
                // 对齐 hccl：>64P 且 ≤32MB 选 SequenceMeshMesh（asc-devkit 未注册）
                selectAlgName = "CcuSchedAllReduceSequenceMeshMesh";
                HCCL_WARNING(
                    "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__,
                    selectAlgName.c_str());
                return SelectorStatus::NOT_MATCH;
            }
            if (dataSize <= RS_MAX_DATA_SIZE && topoInfo->userRankSize >= ccuSize &&
                !Is8BitDataType(opParam.DataDes.dataType)) {
                // 对齐 hccl：≥64P 且 ≤16MB 选 SequenceMeshMesh（asc-devkit 未注册）
                selectAlgName = "CcuSchedAllReduceSequenceMeshMesh";
                HCCL_WARNING(
                    "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__,
                    selectAlgName.c_str());
                return SelectorStatus::NOT_MATCH;
            }
            // hccl 原条件含冗余的 !IsInputOutputOverlap（本分支入口已拦截 in-place），此处去除恒真条件
            if (dataSize <= AR_FLATTEN_MAX_DATA_SIZE && topoInfo->userRankSize <= ccuSize &&
                !Is8BitDataType(opParam.DataDes.dataType)) {
                // 对齐 hccl：≤64P 且 ≤512KB 选 SoleMesh（asc-devkit 已注册）
                selectAlgName = "CcuSchedAllReduceSoleMesh";
                HCCL_INFO("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            if (dataSize <= AR_CCU_SEQ_MAX_DATA_SIZE && topoInfo->userRankSize < ccuSize &&
                !Is8BitDataType(opParam.DataDes.dataType)) {
                // 对齐 hccl：<64P 且 ≤64MB 选 SequenceMeshMesh（asc-devkit 未注册）
                selectAlgName = "CcuSchedAllReduceSequenceMeshMesh";
                HCCL_WARNING(
                    "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__,
                    selectAlgName.c_str());
                return SelectorStatus::NOT_MATCH;
            }
            if (IsSmallDataCCU(dataSize, topoInfo->userRankSize)) { // 64M以下跑ccu
                // 性能优化改用MS做reduce后不支持int8
                CHK_PRT_RET(
                    opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT8,
                    HCCL_DEBUG(
                        "[AllReduceAutoSelector] dataType[%d] is not supported yet for ccu schedule mode with ms "
                        "reduce.",
                        opParam.DataDes.dataType),
                    SelectorStatus::NOT_MATCH);
                // 对齐 hccl：选 ParallelMeshNHR（asc-devkit 未注册）
                selectAlgName = "CcuSchedAllReduceParallelMeshNHR";
                HCCL_WARNING(
                    "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__,
                    selectAlgName.c_str());
                return SelectorStatus::NOT_MATCH;
            }
            return SelectorStatus::NOT_MATCH; // 64M以上切为aicpu
        }
        if (topoInfo->level0Topo == Level0Shape::CLOS && (!IsInputOutputOverlap(opParam))) {
            if (dataSize < AR_CCU_CLOS_1D_SMALL_DATA_SIZE) {
                // 对齐 hccl：两层 CLOS 小数据选 SoleNHR（asc-devkit 已注册）
                selectAlgName = "CcuSchedAllReduceSoleNHR";
                HCCL_INFO("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            return SelectorStatus::NOT_MATCH;
        }
        HCCL_DEBUG(
            "[AllReduceAutoSelector] level0Topo[%d] is not supported yet for ccu schedule mode.", topoInfo->level0Topo);
        return SelectorStatus::NOT_MATCH;
    }
    return SelectCcuScheduleLevel0Algo(topoInfo, opParam, selectAlgName, dataSize);
}

// 对齐 hccl AllReduceAutoSelector::SelectCcuScheduleLevel0Algo：ccu_sched 单层选算分发。
SelectorStatus AllReduceAutoSelector::SelectCcuScheduleLevel0Algo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName,
    const u64 dataSize) const
{
    // ccu 模式不支持 in-place 场景
    CHK_PRT_RET(
        IsInputOutputOverlap(opParam) == true,
        HCCL_WARNING("[Algo][AllReduceAutoSelector] ccu_sched does not support in-place allreduce."),
        SelectorStatus::NOT_MATCH);
    if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
        return SelectCcuScheduleLevel0AlgoMesh1D(topoInfo, opParam, selectAlgName, dataSize);
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        if (topoInfo->level0PcieMix) {
            if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
                return SelectCcuScheduleLevel0AlgoMesh1D(topoInfo, opParam, selectAlgName, dataSize);
            }
            // PCIE-SW定制机型，Mesh无法链接全卡时，需要跨pcie链路，不支持ccu模式
            HCCL_WARNING("[AllReduceAutoSelector] pcie mixed topo is not supported yet for ccu schedule mode.");
            return SelectorStatus::NOT_MATCH;
        }
        // 性能优化改用MS做reduce后不支持int8
        CHK_PRT_RET(
            opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT8,
            HCCL_DEBUG(
                "[AllReduceAutoSelector] dataType[%d] is not supported yet for ccu schedule mode with ms reduce.",
                opParam.DataDes.dataType),
            SelectorStatus::NOT_MATCH);
        return SelectCcuScheduleLevel0UBXAlgo(topoInfo, selectAlgName, dataSize);
    }
    if (topoInfo->level0Topo == Level0Shape::CLOS) {
        if (topoInfo->level0PcieMix) {
            // PCIE-SW定制机型，Mesh无法链接全卡时，需要跨pcie链路，不支持ccu模式
            HCCL_WARNING("[AllReduceAutoSelector] pcie mixed topo is not supported yet for ccu schedule mode.");
            return SelectorStatus::NOT_MATCH;
        }
        if (dataSize < AR_CCU_CLOS_1D_SMALL_DATA_SIZE) {
            // 对齐 hccl：单层 CLOS 小数据选 SoleNHR（asc-devkit 已注册）
            selectAlgName = "CcuSchedAllReduceSoleNHR";
            HCCL_INFO("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
            return SelectorStatus::MATCH;
        }
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_DEBUG(
        "[AllReduceAutoSelector] level0Topo[%d] is not supported yet for ccu schedule mode.", topoInfo->level0Topo);
    return SelectorStatus::NOT_MATCH;
}

// 对齐 hccl AllReduceAutoSelector::SelectCcuScheduleLevel0AlgoMesh1D：单层 Mesh1D（含 pcieMix 全连通）。
SelectorStatus AllReduceAutoSelector::SelectCcuScheduleLevel0AlgoMesh1D(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, std::string& selectAlgName,
    const u64 dataSize) const
{
    // 性能优化改用MS做reduce后不支持int8
    CHK_PRT_RET(
        opParam.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT8,
        HCCL_DEBUG(
            "[AllReduceAutoSelector] dataType[%d] is not supported yet for ccu schedule mode with ms reduce.",
            opParam.DataDes.dataType),
        SelectorStatus::NOT_MATCH);
    double ratio;
    if (topoInfo->userRankSize == 0) {
        HCCL_DEBUG("[AllReduceAutoSelector] the selector userRankSize not set");
        ratio = 1;
    } else {
        ratio = DEFAULT_RANK_SIZE / topoInfo->userRankSize / topoInfo->userRankSize;
    }
    if (dataSize * ratio > AR_M2M_1D_MAX_DATA_SIZE) {
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_REGULAR) {
        // 对齐 hccl：小数据 SoleMesh2Die / 大数据 SequenceMesh2Die（asc-devkit 均未注册）
        selectAlgName = IsSmallData(dataSize) ? "CcuSchedAllReduceSoleMesh2Die" : "CcuSchedAllReduceSequenceMesh2Die";
        HCCL_WARNING(
            "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
        HCCL_DEBUG("[AllReduceAutoSelector][%s] TWO_DIE_NOT_REGULAR not match", __func__);
        return SelectorStatus::NOT_MATCH;
    }
    selectAlgName = "CcuSchedAllReduceSoleMesh";
    HCCL_DEBUG("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
    return SelectorStatus::MATCH;
}

// 对齐 hccl AllReduceAutoSelector::SelectCcuScheduleLevel0UBXAlgo（单层 MESH_1D_CLOS 非 pcieMix）。
SelectorStatus AllReduceAutoSelector::SelectCcuScheduleLevel0UBXAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName, const u64 dataSize) const
{
    bool isMeshNumEqualToClosNum = false;
    bool isClosNumMultipleOfMeshNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_DEBUG("[AllReduceAutoSelector][SelectCcuScheduleLevel0UBXAlgo] CheckMeshNumEqualToClosNum failed."),
        SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        CheckClosNumMultipleOfMeshNum(topoInfo, isClosNumMultipleOfMeshNum) != HCCL_SUCCESS,
        HCCL_DEBUG("[AllReduceAutoSelector][SelectCcuScheduleLevel0UBXAlgo] CheckClosNumMultipleOfMeshNum failed."),
        SelectorStatus::NOT_MATCH);
    if (isMeshNumEqualToClosNum && topoInfo->userRankSize <= MAX_RANK_NUM_FOR_CONCURRENT_ALGO) {
        // 4P mesh
        if (IsSmallData(dataSize)) {
            // 小数据量，用1d mesh算法（asc-devkit 已注册）
            selectAlgName = "CcuSchedAllReduceSoleMesh";
            HCCL_DEBUG("[AllReduceAutoSelector][%s] Algo match [%s]", __func__, selectAlgName.c_str());
            return SelectorStatus::MATCH;
        }
        // 大数据量，用mesh+clos并行算法（asc-devkit 未注册）
        selectAlgName = "CcuSchedAllReduceConcurMeshNHRMultiLink";
        HCCL_WARNING(
            "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
        return SelectorStatus::NOT_MATCH;
    }
    if (isClosNumMultipleOfMeshNum && !IsSmallData(dataSize)) {
        // 矩形场景大数据量，用Parallel并行算法（asc-devkit 未注册）
        selectAlgName = (dataSize < OMNI_UBX_AR_SCHED_DATA_SIZE) ? "CcuSchedAllReduceParallelMeshNHRMultiJetty" :
                                                                   "CcuSchedAllReducePipeLineMeshNHR";
        HCCL_WARNING(
            "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
        return SelectorStatus::NOT_MATCH;
    }
    // 其他场景，用1d NHR算法（asc-devkit 未注册）
    selectAlgName = "CcuSchedAllReduceSoleNHRMultiLink";
    HCCL_WARNING(
        "[AllReduceAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
    return SelectorStatus::NOT_MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLREDUCE, 18, AllReduceAutoSelector);

} // namespace mc2_ops_hccl
