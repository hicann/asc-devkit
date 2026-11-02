/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alltoallv_auto_selector.h"

#include "selector_registry.h"
#include "ins_temp_all_to_all_v_pairwise.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 CONCURRENT_RANK_LIMIT = 4U;
constexpr u32 A2AV_CCU_MAX_RANK_SIZE = 64U; // 与 hccl A2AV_CCU_MAX_RANK_SIZE 一致
constexpr u32 TOPO_LEVEL_NUM_3 = 3U;
// 对齐模板 CalcRes 的 CCU_MAX_RANK_SIZE=16 元数据守卫（alg_param.h）：
// kernel 虽为多 event 可支持更多 rank，模板层仍限 16，selector 前置拒绝
constexpr u32 CCU_MESH1D_MAX_RANK_SIZE = 16U;
// 大禹纯 CLOS 单层 CCU 上限：event mask 已按 16 rank 一组拆分（ccu_kernel_all_to_all_v_mesh1d.cc），
// 通道数组受 CcuKernelArgBase::channels[CCU_MAX_RANK_SIZE] 限制（CCU_MAX_RANK_SIZE=64）
constexpr u32 CCU_CLOS_MAX_RANK_SIZE = 64U;
} // namespace

SelectorStatus AlltoAllVAutoSelector::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)opParam;
    (void)configAlgMap;
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[AlltoAllVAutoSelector] topoInfo is nullptr."), SelectorStatus::NOT_MATCH);
    if (topoInfo->userRankSize <= 1U) {
        HCCL_WARNING(
            "[AlltoAllVAutoSelector] rankSize[%u] is not supported by local AICPU AllToAllV.", topoInfo->userRankSize);
        return SelectorStatus::NOT_MATCH;
    }
    // 对齐 hccl alltoallv_auto_selector.cc:110-118：跨框多级 MESH_1D 时 Pairwise 优先于 SoleMesh；
    // hccl 侧由 REGISTER_ALG_ATTRS topoCustomCheck 承担的 level0Symmetric + <=128P 约束
    // （hccl ins_v2_all_to_all_v_sole_executor.cc:613-620），devkit 无注册框架 attrs 注入链，
    // 与模板 CalcRes 共用完整约束，确保自动选择和 forced-alg 资源预检一致。
    if (InsTempAllToAllVPairwise::IsSupportedTopology(topoInfo)) {
        selectAlgName = "AicpuAllToAllVSolePairwise";
        HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s].", __func__, selectAlgName.c_str());
        return SelectorStatus::MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D || topoInfo->level0Topo == Level0Shape::CLOS) {
        selectAlgName = "AicpuAllToAllVSoleMesh";
        HCCL_INFO("[AlltoAllVAutoSelector] Algo match[%s].", selectAlgName.c_str());
        return SelectorStatus::MATCH;
    } else if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        // PCIE-SW定制机型，使用mesh1d算法
        if (topoInfo->level0PcieMix) {
            selectAlgName = "AicpuAllToAllVSoleMesh";
            HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s]", __func__, selectAlgName.c_str());
            return SelectorStatus::MATCH;
        }
        // 注：多级 MESH_1D_CLOS 对齐 hccl 走 SoleMesh 被 executor 的双实例 hierarchy 契约阻塞
        // （TopoMatch1D 为扁平结构），保持既有行为：由下方单级判断拒绝
    }
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS || topoInfo->topoLevelNums != 1U || topoInfo->level0PcieMix) {
        HCCL_WARNING(
            "[AlltoAllVAutoSelector] rankSize[%u], level0Topo[%u] is not supported by local AICPU AllToAllV.",
            topoInfo->userRankSize, static_cast<u32>(topoInfo->level0Topo));
        return SelectorStatus::NOT_MATCH;
    }
#if defined(HCCL_CANN_COMPAT_850)
    return SelectorStatus::NOT_MATCH;
#else
    bool isMeshNumEqualToClosNum = false;
    CHK_PRT_RET(
        CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
        HCCL_ERROR("[AlltoAllVAutoSelector] failed to compare Mesh/CLOS rank counts."), SelectorStatus::NOT_MATCH);
    CHK_PRT_RET(
        !isMeshNumEqualToClosNum || topoInfo->userRankSize > CONCURRENT_RANK_LIMIT,
        HCCL_WARNING("[AlltoAllVAutoSelector] topology does not satisfy Concurrent constraints."),
        SelectorStatus::NOT_MATCH);
    selectAlgName = "AicpuAllToAllVSoleMeshConcurrent";
    HCCL_INFO("[AlltoAllVAutoSelector] Algo match[%s].", selectAlgName.c_str());
    return SelectorStatus::MATCH;
#endif
}

// 对齐 hccl 分支框架：已注册算法 MATCH，未注册算法设名后 NOT_MATCH（报错由框架统一处理）。
SelectorStatus AlltoAllVAutoSelector::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)configAlgMap;
    HCCL_DEBUG("[AlltoAllVAutoSelector][%s] start, topoInfo levelNum[%u]", __func__, topoInfo->topoLevelNums);
    if (topoInfo->level2UbRtp) {
        HCCL_INFO("[AlltoAllVAutoSelector][%s] ccu schedule is not supported with level2UbRtp.", __func__);
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->topoLevelNums >= TOPO_LEVEL_NUM_3) {
        HCCL_INFO(
            "[AlltoAllVAutoSelector][%s] ccu schedule is not supported when topoLevelNums >= 3(levelNum[%u]).",
            __func__, topoInfo->topoLevelNums);
        return SelectorStatus::NOT_MATCH;
    }
    // ccu_schedule 模式不支持原地算子（与 alltoall/all_gather 等同框架算子的选择器契约一致；
    // hccl 的 alltoallv 恰好缺失此检查，此处不照抄其缺口）
    CHK_PRT_RET(
        IsInputOutputOverlap(opParam) == true,
        HCCL_WARNING("[Algo][AlltoAllVAutoSelector] ccu schedule does not support inplace alltoallv."),
        SelectorStatus::NOT_MATCH);
    if (topoInfo->topoLevelNums > 1U) {
        if (topoInfo->level0Topo != Level0Shape::CLOS) {
            // 非 pcieMix 的 MESH_1D_CLOS：SoleMesh（TopoMatch1D 扁平 hierarchy）被 executor 的
            // MESH_1D_CLOS 双实例契约阻塞（BuildResourceHierarchyInfo 要求 infos[0].size()==2，
            // 资源阶段必报错），selector 前置拒绝；pcieMix 场景走扁平分支不受影响
            if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
                HCCL_WARNING(
                    "[AlltoAllVAutoSelector][%s] multi-level non-pcieMix MESH_1D_CLOS is blocked by executor "
                    "hierarchy contract.",
                    __func__);
                return SelectorStatus::NOT_MATCH;
            }
            // hccl 侧 int8 判断借 union 重叠读 all2AllDataDes.sendType，此处直接用 all2AllVDataDes 语义等价
            if (opParam.all2AllVDataDes.sendType == HcclDataType::HCCL_DATA_TYPE_INT8) {
                HCCL_WARNING("[Algo][AlltoAllVAutoSelector] int8 is not supported yet for ccu_schedule mode.");
                return SelectorStatus::NOT_MATCH;
            }
            if (topoInfo->userRankSize > A2AV_CCU_MAX_RANK_SIZE) {
                HCCL_WARNING("[Algo][AlltoAllVAutoSelector] rankSize > 64 is not supported yet for ccu_schedule mode.");
                return SelectorStatus::NOT_MATCH;
            }
            // 与单级路径契约一致：2Die 形态与 pcie 混插未全连通的 level0 不选 SoleMesh
            // （hccl 多级分支缺此检查属其既有缺口，不照抄）
            if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_REGULAR ||
                topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
                HCCL_WARNING(
                    "[AlltoAllVAutoSelector][%s] 2-die level0 mesh is not supported by SoleMesh in multi-level topo.",
                    __func__);
                return SelectorStatus::NOT_MATCH;
            }
            if (topoInfo->level0PcieMix && !IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
                HCCL_WARNING(
                    "[AlltoAllVAutoSelector] pcie mixed topo is not fully connected with 1dmesh in multi-level topo.");
                return SelectorStatus::NOT_MATCH;
            }
            if (topoInfo->userRankSize <= CONCURRENT_RANK_LIMIT) {
                selectAlgName = "CcuSchedAllToAllVSoleMesh";
                HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s].", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            selectAlgName = "CcuSchedAllToAllVSoleMeshMultiLink";
            HCCL_WARNING(
                "[AlltoAllVAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        }
        HCCL_WARNING("[Algo][AlltoAllVAutoSelector] clos is not supported for ccu_schedule mode.");
        return SelectorStatus::NOT_MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D) {
        if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_REGULAR) {
            selectAlgName = "CcuSchedAllToAllVSoleMesh2Die";
            HCCL_WARNING(
                "[AlltoAllVAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        }
        if (topoInfo->level0MeshType == Level0MeshType::TWO_DIE_NOT_REGULAR) {
            HCCL_DEBUG("[AlltoAllVAutoSelector][%s] TWO_DIE_NOT_REGULAR not match", __func__);
            return SelectorStatus::NOT_MATCH;
        }
        // 模板 CalcRes 的 CCU_MAX_RANK_SIZE=16 元数据守卫（alg_param.h）：
        // 超限会在资源阶段报错且 CCU 路径无回退，selector 前置拒绝
        if (topoInfo->userRankSize > CCU_MESH1D_MAX_RANK_SIZE) {
            HCCL_WARNING(
                "[AlltoAllVAutoSelector][%s] rankSize[%u] exceeds CCU_MESH1D_MAX_RANK_SIZE[%u].", __func__,
                topoInfo->userRankSize, CCU_MESH1D_MAX_RANK_SIZE);
            return SelectorStatus::NOT_MATCH;
        }
        selectAlgName = "CcuSchedAllToAllVSoleMesh";
        HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s].", __func__, selectAlgName.c_str());
        return SelectorStatus::MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::CLOS && topoInfo->userRankSize <= CCU_CLOS_MAX_RANK_SIZE) {
        // 大禹纯 CLOS 单层：CCU SoleMesh（DIE0-only 建链，模板内做 die 过滤）
        selectAlgName = "CcuSchedAllToAllVSoleMesh";
        HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s].", __func__, selectAlgName.c_str());
        return SelectorStatus::MATCH;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS) {
        // PCIE-SW定制机型，Mesh无法链接全卡时需跨pcie链路，仅全连通1DMESH支持ccu
        if (topoInfo->level0PcieMix) {
            if (IsLayerAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH)) {
                // 与单级 MESH_1D 分支命中同一算法/模板，rank 上限契约一致：
                // 模板 CalcRes 的 CCU_MAX_RANK_SIZE=16 守卫同样生效，前置拒绝
                if (topoInfo->userRankSize > CCU_MESH1D_MAX_RANK_SIZE) {
                    HCCL_WARNING(
                        "[AlltoAllVAutoSelector][%s] rankSize[%u] exceeds CCU_MESH1D_MAX_RANK_SIZE[%u].", __func__,
                        topoInfo->userRankSize, CCU_MESH1D_MAX_RANK_SIZE);
                    return SelectorStatus::NOT_MATCH;
                }
                selectAlgName = "CcuSchedAllToAllVSoleMesh";
                HCCL_INFO("[AlltoAllVAutoSelector][%s] Algo match[%s].", __func__, selectAlgName.c_str());
                return SelectorStatus::MATCH;
            }
            HCCL_WARNING("[AlltoAllVAutoSelector] pcie mixed topo is not supported yet for ccu schedule mode.");
            return SelectorStatus::NOT_MATCH;
        }
        bool isMeshNumEqualToClosNum = false;
        CHK_PRT_RET(
            CheckMeshNumEqualToClosNum(topoInfo, isMeshNumEqualToClosNum) != HCCL_SUCCESS,
            HCCL_DEBUG("[AlltoAllVAutoSelector] CheckMeshNumEqualToClosNum failed."), SelectorStatus::NOT_MATCH);
        if (isMeshNumEqualToClosNum && topoInfo->userRankSize <= CONCURRENT_RANK_LIMIT) {
            selectAlgName = "CcuSchedAllToAllVSoleMeshConcurrent";
            HCCL_WARNING(
                "[AlltoAllVAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
            return SelectorStatus::NOT_MATCH;
        }
        selectAlgName = "CcuSchedAllToAllVSoleMeshMultiJetty";
        HCCL_WARNING(
            "[AlltoAllVAutoSelector][%s] algo[%s] not registered, not supported.", __func__, selectAlgName.c_str());
        return SelectorStatus::NOT_MATCH;
    }
    HCCL_DEBUG("[AlltoAllVAutoSelector] algo is not supported yet for ccu_schedule mode.");
    return SelectorStatus::NOT_MATCH;
}

REGISTER_SELECTOR_BY_OPTYPE(HcclCMDType::HCCL_CMD_ALLTOALLV, 18, AlltoAllVAutoSelector);
} // namespace mc2_ops_hccl
