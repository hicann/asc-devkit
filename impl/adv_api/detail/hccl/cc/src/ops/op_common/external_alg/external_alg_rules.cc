/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "external_alg_rules.h"

#include "auto_selector_base.h"
#include "hccl_rank_graph.h"
#include "log.h"

namespace mc2_ops_hccl {

// 零侵入复用 AutoSelectorBase 的 const 成员函数（类无状态，单实例安全）
namespace {
const AutoSelectorBase kRuleSelectorBase;

constexpr u64 RS_2P_DETOUR_DATA_SIZE = 4ULL * 1024 * 1024;                 // 4MB
constexpr u64 RS_1D_TWO_LEVEL_DATA_SIZE_THRESHOLD = 1536ULL * 1024 * 1024; // 1536MB
} // namespace

// 两层阈值取 RS 值 1536MB（AG 是 1GB；本条件仅 RS 候选消费）
bool CondZAxis(const TopoInfoWithNetLayerDetails* topo, const OpParam& opParam)
{
    if (topo == nullptr) {
        return false;
    }
    if (!kRuleSelectorBase.IsTwoLevelNetLayer(topo)) {
        return false;
    }
    const u64 dataSize = opParam.DataDes.count * DATATYPE_SIZE_TABLE[opParam.DataDes.dataType];
    if (topo->userRankSize == 2U && dataSize >= RS_2P_DETOUR_DATA_SIZE) {
        return true;
    }
    return dataSize * topo->userRankSize > RS_1D_TWO_LEVEL_DATA_SIZE_THRESHOLD;
}

// 查询前提不满足或查询失败时保守不命中，裸名候选兜底
bool CondPeerOnly(const TopoInfoWithNetLayerDetails* topo, const OpParam& opParam)
{
    if (topo == nullptr || topo->userRankSize != 2U) {
        return false;
    }
    if (opParam.hcclComm == nullptr || topo->netLayerDetails.netLayers.empty()) {
        return false;
    }
    CommLink* links = nullptr;
    uint32_t linkNum = 0;
    const uint32_t peerRank = (topo->userRank == 0U) ? 1U : 0U;
    const HcclResult ret = HcclRankGraphGetLinks(
        opParam.hcclComm, topo->netLayerDetails.netLayers[0], topo->userRank, peerRank, &links, &linkNum);
    if (ret != HCCL_SUCCESS) {
        HCCL_INFO(
            "[MC2_EXT_ALG] COND_PEER_ONLY query links failed, ret[%d], fallback to bare candidate.",
            static_cast<int>(ret));
        return false;
    }
    return linkNum == 1U;
}

// soft_reduce 优先级语义由"特化候选在前 + 裸名兜底"的排序自动成立
bool CondSpecialDt(const TopoInfoWithNetLayerDetails* topo, const OpParam& opParam)
{
    (void)topo;
    return Is64BitDataType(opParam.DataDes.dataType) || opParam.reduceType == HcclReduceOp::HCCL_REDUCE_PROD;
}

bool FlagPcieMix(const TopoInfoWithNetLayerDetails* topo)
{
    if (topo == nullptr) {
        return false;
    }
    return topo->level0Topo == Level0Shape::MESH_1D_CLOS && topo->level0PcieMix &&
           !kRuleSelectorBase.IsLayerAllConnetedWithTopo(topo, 0U, CommTopo::COMM_TOPO_1DMESH);
}

bool FlagUbx(const TopoInfoWithNetLayerDetails* topo)
{
    if (topo == nullptr) {
        return false;
    }
    return topo->level0Topo == Level0Shape::MESH_1D_CLOS && !topo->level0PcieMix;
}

bool FlagMultiLevel(const TopoInfoWithNetLayerDetails* topo)
{
    if (topo == nullptr) {
        return false;
    }
    return topo->topoLevelNums > 1U && topo->level0Topo == Level0Shape::MESH_1D;
}

// A2A 裸名专用
bool FlagFlat1D(const TopoInfoWithNetLayerDetails* topo)
{
    if (topo == nullptr) {
        return false;
    }
    return topo->level0Topo != Level0Shape::MESH_1D_CLOS;
}

// DPU 行服务 CPU 引擎请求；AICPU 行兼容 AICPU_TS
bool EngineMatch(AlgEngine candEngine, const OpParam& opParam)
{
    switch (candEngine) {
        case AlgEngine::DPU:
            return opParam.engine == CommEngine::COMM_ENGINE_CPU;
        case AlgEngine::CCU:
            return opParam.engine == CommEngine::COMM_ENGINE_CCU;
        case AlgEngine::AICPU:
        default:
            return opParam.engine == CommEngine::COMM_ENGINE_AICPU ||
                   opParam.engine == CommEngine::COMM_ENGINE_AICPU_TS;
    }
}

// 表序与枚举序钉扎。extern 表不可用于常量表达式（GCC: "not usable in a constant
// expression"，constexpr 又不允许出现在变量的非定义声明上），故绑定关系先落 constexpr
// 内表并 static_assert 钉死；extern 表逐元素按枚举下标引用内表，enum 重排/绑定错位/
// 漏项（NoHole）全部编译期暴露。
namespace {
constexpr VariantCond VARIANT_COND_BINDING[VARIANT_COND_COUNT] = {
    CondZAxis,     // COND_Z_AXIS
    CondPeerOnly,  // COND_PEER_ONLY
    CondSpecialDt, // COND_SPECIAL_DT
};
constexpr MachineFlag MACHINE_FLAG_BINDING[MACHINE_FLAG_COUNT] = {
    FlagPcieMix,    // FLAG_PCIE_MIX
    FlagUbx,        // FLAG_UBX
    FlagMultiLevel, // FLAG_MULTI_LEVEL
    FlagFlat1D,     // FLAG_FLAT_1D
};

// 枚举新增槽位而内表漏填时尾部为 nullptr，编译期拦截
constexpr bool NoHole(const VariantCond* table, int count)
{
    for (int i = 0; i < count; ++i) {
        if (table[i] == nullptr) {
            return false;
        }
    }
    return true;
}

constexpr bool NoHole(const MachineFlag* table, int count)
{
    for (int i = 0; i < count; ++i) {
        if (table[i] == nullptr) {
            return false;
        }
    }
    return true;
}
} // namespace

static_assert(VARIANT_COND_BINDING[COND_Z_AXIS] == CondZAxis, "table binding drifted");
static_assert(VARIANT_COND_BINDING[COND_PEER_ONLY] == CondPeerOnly, "table binding drifted");
static_assert(VARIANT_COND_BINDING[COND_SPECIAL_DT] == CondSpecialDt, "table binding drifted");
static_assert(MACHINE_FLAG_BINDING[FLAG_PCIE_MIX] == FlagPcieMix, "table binding drifted");
static_assert(MACHINE_FLAG_BINDING[FLAG_UBX] == FlagUbx, "table binding drifted");
static_assert(MACHINE_FLAG_BINDING[FLAG_MULTI_LEVEL] == FlagMultiLevel, "table binding drifted");
static_assert(MACHINE_FLAG_BINDING[FLAG_FLAT_1D] == FlagFlat1D, "table binding drifted");
static_assert(NoHole(VARIANT_COND_BINDING, VARIANT_COND_COUNT), "variant cond binding has holes");
static_assert(NoHole(MACHINE_FLAG_BINDING, MACHINE_FLAG_COUNT), "machine flag binding has holes");

const VariantCond VARIANT_COND_TABLE[VARIANT_COND_COUNT] = {
    VARIANT_COND_BINDING[COND_Z_AXIS],
    VARIANT_COND_BINDING[COND_PEER_ONLY],
    VARIANT_COND_BINDING[COND_SPECIAL_DT],
};

const MachineFlag MACHINE_FLAG_TABLE[MACHINE_FLAG_COUNT] = {
    MACHINE_FLAG_BINDING[FLAG_PCIE_MIX],
    MACHINE_FLAG_BINDING[FLAG_UBX],
    MACHINE_FLAG_BINDING[FLAG_MULTI_LEVEL],
    MACHINE_FLAG_BINDING[FLAG_FLAT_1D],
};

const char* VariantCondName(int id)
{
    static const char* NAMES[VARIANT_COND_COUNT] = {"COND_Z_AXIS", "COND_PEER_ONLY", "COND_SPECIAL_DT"};
    return (id >= 0 && id < VARIANT_COND_COUNT) ? NAMES[id] : "n/a";
}

const char* MachineFlagName(int id)
{
    static const char* NAMES[MACHINE_FLAG_COUNT] = {"FLAG_PCIE_MIX", "FLAG_UBX", "FLAG_MULTI_LEVEL", "FLAG_FLAT_1D"};
    return (id >= 0 && id < MACHINE_FLAG_COUNT) ? NAMES[id] : "n/a";
}
} // namespace mc2_ops_hccl
