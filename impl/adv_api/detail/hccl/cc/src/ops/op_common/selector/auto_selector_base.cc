/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "auto_selector_base.h"
#include "selector_registry.h"

namespace mc2_ops_hccl {

SelectorStatus AutoSelectorBase::Select(
    OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName) const
{
    HCCL_INFO(
        "[asc][AlgoSelect][AutoSelectorBase::Select] start, opType[%d], opExecuteConfig[%d], engine[%d], "
        "isMc2[%d], topoInfo[%p].",
        opParam.opType, opParam.opExecuteConfig, opParam.engine, opParam.isMc2, topoInfo);
    std::map<HcclCMDType, std::vector<HcclAlgoType>> configAlgMap = GetExternalInputHcclAlgoConfigAllType();
    SelectorStatus ret = SelectorStatus::NOT_MATCH;
    bool hostDPUOnly = false;
    if ((CheckHostDPUOnly(topoInfo, opParam, hostDPUOnly) == HCCL_SUCCESS) && hostDPUOnly) {
        opParam.opExecuteConfig = OpExecuteConfig::HOSTCPU;
        opParam.engine = CommEngine::COMM_ENGINE_CPU;
        HCCL_INFO(
            "[asc][AlgoSelect][AutoSelectorBase::Select] switch to host DPU, opType[%d], "
            "opExecuteConfig[%d], engine[%d].",
            opParam.opType, opParam.opExecuteConfig, opParam.engine);
        return SelectDPUAlgo(topoInfo, opParam, configAlgMap, selectAlgName);
    }
    if (opParam.opExecuteConfig == OpExecuteConfig::CCU_MS) {
        HCCL_DEBUG("[asc][AlgoSelect][AutoSelectorBase::Select] try CCU_MS, opType[%d].", opParam.opType);
        ret = SelectCcuMsAlgo(topoInfo, opParam, configAlgMap, selectAlgName);
        if (ret == SelectorStatus::NOT_MATCH) {
            opParam.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
            HCCL_INFO(
                "[asc][AlgoSelect][AutoSelectorBase::Select] CCU_MS not match, fallback to CCU_SCHED, "
                "opType[%d].",
                opParam.opType);
        } else {
            HCCL_INFO(
                "[asc][AlgoSelect][AutoSelectorBase::Select] CCU_MS matched, opType[%d], algName[%s].", opParam.opType,
                selectAlgName.c_str());
            return ret;
        }
    }
    if (opParam.opExecuteConfig == OpExecuteConfig::CCU_SCHED) {
        HCCL_DEBUG("[asc][AlgoSelect][AutoSelectorBase::Select] try CCU_SCHED, opType[%d].", opParam.opType);
        ret = SelectCcuScheduleAlgo(topoInfo, opParam, configAlgMap, selectAlgName);
        if (ret == SelectorStatus::NOT_MATCH) {
            opParam.opExecuteConfig = OpExecuteConfig::CCU_FAIL;
            HCCL_INFO(
                "[asc][AlgoSelect][AutoSelectorBase::Select] CCU_SCHED not match, fallback to CCU_FAIL, "
                "opType[%d].",
                opParam.opType);
        } else {
            HCCL_INFO(
                "[asc][AlgoSelect][AutoSelectorBase::Select] CCU_SCHED matched, opType[%d], algName[%s].",
                opParam.opType, selectAlgName.c_str());
            return ret;
        }
    }
    if (ProcessAivConfig(opParam, topoInfo, configAlgMap, selectAlgName, ret)) {
        HCCL_INFO(
            "[asc][AlgoSelect][AutoSelectorBase::Select] AIV config handled, opType[%d], algName[%s], "
            "opExecuteConfig[%d], status[%d].",
            opParam.opType, selectAlgName.c_str(), opParam.opExecuteConfig, static_cast<int>(ret));
        return ret;
    }
    if (IsStarsState(opParam.opExecuteConfig)) {
        HCCL_DEBUG(
            "[asc][AlgoSelect][AutoSelectorBase::Select] try AICPU algo, opType[%d], opExecuteConfig[%d].",
            opParam.opType, opParam.opExecuteConfig);
        ret = SelectAicpuAlgo(topoInfo, opParam, configAlgMap, selectAlgName);
        if (ret == SelectorStatus::MATCH) {
            opParam.opExecuteConfig = OpExecuteConfig::AICPU_TS;
            HCCL_INFO(
                "[asc][AlgoSelect][AutoSelectorBase::Select] AICPU algo matched, opType[%d], algName[%s], "
                "opExecuteConfig[%d].",
                opParam.opType, selectAlgName.c_str(), opParam.opExecuteConfig);
        }
    }
    HCCL_INFO(
        "[asc][AlgoSelect][AutoSelectorBase::Select] end, opType[%d], status[%d], algName[%s], "
        "opExecuteConfig[%d], engine[%d].",
        opParam.opType, static_cast<int>(ret), selectAlgName.c_str(), opParam.opExecuteConfig, opParam.engine);
    return ret;
}

bool AutoSelectorBase::IsStarsState(const OpExecuteConfig& opExecuteConfig) const
{
    return (
        opExecuteConfig == OpExecuteConfig::AICPU_TS || opExecuteConfig == OpExecuteConfig::HOSTCPU_TS ||
        opExecuteConfig == OpExecuteConfig::CCU_FAIL);
}

bool AutoSelectorBase::IsDefaultAlg(const HcclAlgoType algoType) const
{
    return (algoType == HcclAlgoType::HCCL_ALGO_TYPE_DEFAULT) || (algoType == HcclAlgoType::HCCL_ALGO_TYPE_NA);
}

bool AutoSelectorBase::IsSmallData(const u64 dataSize) const { return dataSize < SMALL_COUNT_512KB; }

bool AutoSelectorBase::IsLargeData(const u64 dataSize) const { return dataSize >= LARGE_COUNT_1024KB; }

SelectorStatus AutoSelectorBase::SelectCcuMsAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)topoInfo;
    (void)configAlgMap;
    (void)selectAlgName;
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AutoSelectorBase::SelectCcuScheduleAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)topoInfo;
    (void)configAlgMap;
    (void)selectAlgName;
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AutoSelectorBase::SelectAicpuAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)topoInfo;
    (void)configAlgMap;
    (void)selectAlgName;
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AutoSelectorBase::SelectAivAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)topoInfo;
    (void)configAlgMap;
    (void)selectAlgName;
    return SelectorStatus::NOT_MATCH;
}

SelectorStatus AutoSelectorBase::SelectDPUAlgo(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const
{
    (void)topoInfo;
    (void)configAlgMap;
    (void)selectAlgName;
    return SelectorStatus::NOT_MATCH;
}

// 判断通过最高一个level的网络全部没有device的可达链路，并且有host的可达链路
HcclResult AutoSelectorBase::CheckHostDPUOnly(
    const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, bool& hostDPUOnly) const
{
    hostDPUOnly = false;
    CHK_PTR_NULL(topoInfo);
    HCCL_INFO("Start CheckHostDPUOnly");
    // 只有一个server，不使用DPU
    if (topoInfo->serverNum == 1) {
        HCCL_INFO("Not using hostdpu because serverNum is 1");
        return HCCL_SUCCESS;
    }

    uint32_t* netLayers = nullptr;
    uint32_t netLayerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(opParam.hcclComm, &netLayers, &netLayerNum));
    if ((netLayers == nullptr) || (netLayerNum == 0)) {
        HCCL_WARNING("HcclRankGraphGetLayers fail");
        return HCCL_E_INTERNAL;
    }

    bool hostDPU = false;
    for (uint32_t layerIdx = 0; layerIdx < netLayerNum; layerIdx++) {
        uint32_t netLayer = netLayers[layerIdx];
        // 只校验最后一个level
        if (netLayer < (topoInfo->topoLevelNums - 1)) {
            HCCL_INFO("Skip checking layer[%u], topoLevelNums is [%u]", netLayer, topoInfo->topoLevelNums);
            continue;
        }
        uint32_t* topoInsts = nullptr;
        uint32_t topoInsNum = 0;
        CHK_RET(HcclRankGraphGetTopoInstsByLayer(opParam.hcclComm, netLayer, &topoInsts, &topoInsNum));
        if ((topoInsts == nullptr) || (topoInsNum == 0)) {
            HCCL_WARNING("HcclRankGraphGetTopoInstsByLayer fail, netLayer[%u]", netLayer);
            return HCCL_E_INTERNAL;
        }
        for (uint32_t topoInsIdx = 0; topoInsIdx < topoInsNum; topoInsIdx++) {
            uint32_t topoInstId = topoInsts[topoInsIdx];
            HCCL_INFO("Start checking topoInstId[%u]", topoInstId);
            CommTopo topoType;
            CHK_RET(HcclRankGraphGetTopoType(opParam.hcclComm, netLayer, topoInstId, &topoType));
            if (topoType != COMM_TOPO_CLOS) {
                HCCL_INFO("Not using hostdpu because topo type is not COMM_TOPO_CLOS");
                continue;
            }
            uint32_t* ranks = nullptr;
            uint32_t rankNum = 0;
            CHK_RET(HcclRankGraphGetRanksByTopoInst(opParam.hcclComm, netLayer, topoInstId, &ranks, &rankNum));
            // 校验当前rank与其他所有rank连通
            if (rankNum != topoInfo->userRankSize) {
                HCCL_INFO("Not using hostdpu because current rank is not fully connected to all other ranks");
                continue;
            }
            uint32_t endPointNums = 0;
            CHK_RET(HcclRankGraphGetEndpointNum(opParam.hcclComm, netLayer, topoInstId, &endPointNums));
            EndpointDesc endPointDescs[endPointNums];
            CHK_RET(HcclRankGraphGetEndpointDesc(opParam.hcclComm, netLayer, topoInstId, &endPointNums, endPointDescs));
            for (uint32_t endPointIdx = 0; endPointIdx < endPointNums; endPointIdx++) {
                EndpointDesc endPointDesc = endPointDescs[endPointIdx];
                if (endPointDesc.loc.locType == ENDPOINT_LOC_TYPE_DEVICE) {
                    HCCL_INFO(
                        "Not using hostdpu because there is links on device in netLayer[%u] in endPointIdx[%u]",
                        netLayer, endPointIdx);
                    return HCCL_SUCCESS;
                } else if (endPointDesc.loc.locType == ENDPOINT_LOC_TYPE_HOST) {
                    HCCL_INFO("Found a host endPoint in netLayer[%u] endPointIdx[%u]", netLayer, endPointIdx);
                    hostDPU = true;
                }
            }
        }
    }
    if (hostDPU) {
        HCCL_INFO("Using host dpu trans.");
        hostDPUOnly = true;
    }
    return HCCL_SUCCESS;
}

bool AutoSelectorBase::IsLayerAllConnetedWithTopo(
    const TopoInfoWithNetLayerDetails* topoInfo, const u32 netLayer, const CommTopo topoType) const
{
    CHK_PRT_RET(
        topoInfo->netLayerDetails.localNetInsSizeOfLayer.size() <= netLayer,
        HCCL_WARNING(
            "[BaseSelector][IsLayerAllConnetedWithTopo] localNetInsSizeOfLayer size[%u] <= netLayer[%u]",
            topoInfo->netLayerDetails.localNetInsSizeOfLayer.size(), netLayer),
        false);
    u32 localRankSize = topoInfo->netLayerDetails.localNetInsSizeOfLayer[netLayer];

    CHK_PRT_RET(
        topoInfo->topoInstDetailsOfLayer.size() <= netLayer,
        HCCL_WARNING(
            "[BaseSelector][IsLayerAllConnetedWithTopo] topoInstDetailsOfLayer size[%u] <= netLayer[%u]",
            topoInfo->topoInstDetailsOfLayer.size(), netLayer),
        false);

    auto rankNumForTopoTypeItr = topoInfo->topoInstDetailsOfLayer[netLayer].rankNumForTopoType.find(topoType);
    if (rankNumForTopoTypeItr == topoInfo->topoInstDetailsOfLayer[netLayer].rankNumForTopoType.end()) {
        return false;
    }

    for (auto topoRankNum : rankNumForTopoTypeItr->second) {
        if (topoRankNum == localRankSize) {
            return true;
        }
    }
    return false;
}

HcclResult AutoSelectorBase::CheckMeshNumEqualToClosNum(
    const TopoInfoWithNetLayerDetails* topoInfo, bool& isEqual) const
{
    const auto& topoInstDetails = topoInfo->topoInstDetailsOfLayer;

    // 检查topoInstDetails是否为空
    CHK_PRT_RET(
        topoInstDetails.empty(),
        HCCL_ERROR("[BaseSelector][CheckMeshNumEqualToClosNum] topoInstDetailsOfLayer0 size is zero."),
        HCCL_E_INTERNAL);

    const auto& rankNumMap = topoInstDetails[0].rankNumForTopoType;
    auto closItr = rankNumMap.find(COMM_TOPO_CLOS);
    auto meshItr = rankNumMap.find(COMM_TOPO_1DMESH);
    CHK_PRT_RET(
        closItr == rankNumMap.end() || closItr->second.empty() || meshItr == rankNumMap.end() ||
            meshItr->second.empty(),
        HCCL_ERROR("[BaseSelector][CheckMeshNumEqualToClosNum] topoInstDetailsOfLayer0 size is zero."),
        HCCL_E_INTERNAL);

    // 获取CLOS和1DMESH拓扑的rank数量并比较是否相等
    isEqual = (closItr->second[0] == meshItr->second[0]);
    return HCCL_SUCCESS;
}

HcclResult AutoSelectorBase::CheckClosNumMultipleOfMeshNum(
    const TopoInfoWithNetLayerDetails* topoInfo, bool& isMultiple) const
{
    const auto& topoInstDetails = topoInfo->topoInstDetailsOfLayer;
    // 检查topoInstDetails是否为空
    CHK_PRT_RET(
        topoInstDetails.empty(),
        HCCL_ERROR("[BaseSelector][CheckClosNumMultipleOfMeshNum] topoInstDetailsOfLayer0 size is zero."),
        HCCL_E_INTERNAL);

    const auto& rankNumMap = topoInstDetails[0].rankNumForTopoType;
    auto closItr = rankNumMap.find(COMM_TOPO_CLOS);
    auto meshItr = rankNumMap.find(COMM_TOPO_1DMESH);
    CHK_PRT_RET(
        closItr == rankNumMap.end() || closItr->second.empty() || meshItr == rankNumMap.end() ||
            meshItr->second.empty(),
        HCCL_ERROR("[BaseSelector][CheckClosNumMultipleOfMeshNum] topoInstDetailsOfLayer0 size is zero."),
        HCCL_E_INTERNAL);

    // 获取CLOS和1DMESH拓扑的rank数量
    const auto closRankNums = closItr->second[0];
    const auto meshRankNums = meshItr->second[0];

    // 检查CLOS数量是否大于1DMESH数量且是1DMESH数量的倍数
    isMultiple = (meshRankNums > 1) && (closRankNums > meshRankNums) && (closRankNums % meshRankNums == 0);
    return HCCL_SUCCESS;
}

bool AutoSelectorBase::IsTwoLevelNetLayer(const TopoInfoWithNetLayerDetails* topoInfo) const
{
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_WARNING("[AutoSelectorBase][IsTwoLevelNetLayer] topoInfo is nullptr."), false);
    if (topoInfo->netLayerDetails.netLayerNum <= 1) {
        HCCL_INFO(
            "[AutoSelectorBase][IsTwoLevelNetLayer] netLayerNum[%u] <= 1, not two level net layer.",
            topoInfo->netLayerDetails.netLayerNum);
        return false;
    }

    u32 level1Idx = topoInfo->netLayerDetails.netLayers[1];
    bool hasLevel1Clos = topoInfo->topoInstDetailsOfLayer.size() > level1Idx &&
                         topoInfo->topoInstDetailsOfLayer[level1Idx].rankNumForTopoType.find(COMM_TOPO_CLOS) !=
                             topoInfo->topoInstDetailsOfLayer[level1Idx].rankNumForTopoType.end();
    if (!hasLevel1Clos) {
        HCCL_INFO(
            "[AutoSelectorBase][IsTwoLevelNetLayer] level1[%u] has no CLOS topo, not two level net layer.", level1Idx);
        return false;
    }
    if (topoInfo->netLayerDetails.localNetInsSizeOfLayer.empty() ||
        topoInfo->netLayerDetails.localNetInsSizeOfLayer[0] <= 1) {
        HCCL_INFO(
            "[AutoSelectorBase][IsTwoLevelNetLayer] level0 localNetInsSizeOfLayer[%zu] <= 1, not two level "
            "net layer.",
            topoInfo->netLayerDetails.localNetInsSizeOfLayer.size());
        return false;
    }
    HCCL_INFO(
        "[AutoSelectorBase][IsTwoLevelNetLayer] topoLevelNums[%u], netLayerNum[%u], level0Topo[MESH_1D], "
        "level1Idx[%u] has CLOS, level0LocalNetInsSize[%u], is two level net layer.",
        topoInfo->topoLevelNums, topoInfo->netLayerDetails.netLayerNum, level1Idx,
        topoInfo->netLayerDetails.localNetInsSizeOfLayer[0]);
    return true;
}

bool AutoSelectorBase::IsInputOutputOverlap(const OpParam& opParam) const
{
    CHK_PRT_RET(
        opParam.inputPtr == nullptr || opParam.outputPtr == nullptr,
        HCCL_INFO("[Algo][AutoSelectorBase][IsInputOutputOverlap] The input or output buffer is null. Not overlap."),
        false);

    u64 inputDataSize = opParam.inputSize;
    u64 outputDataSize = opParam.outputSize;

    CHK_PRT_RET(
        inputDataSize == 0 || outputDataSize == 0,
        // 不存在overlap情况
        HCCL_INFO("[Algo][AutoSelectorBase][IsInputOutputOverlap] The input or output buffer size is 0. Not overlap."),
        false);

    uintptr_t inputStart = reinterpret_cast<uintptr_t>(opParam.inputPtr);
    uintptr_t outputStart = reinterpret_cast<uintptr_t>(opParam.outputPtr);
    uintptr_t inputEnd = inputStart + inputDataSize - 1;
    uintptr_t outputEnd = outputStart + outputDataSize - 1;

    HCCL_DEBUG(
        "[Algo][AutoSelectorBase][IsInputOutputOverlap] inputStart[%llu], inputEnd[%llu], outputStart[%llu], "
        "outputEnd[%llu].",
        inputStart, inputEnd, outputStart, outputEnd);

    CHK_PRT_RET(
        inputStart <= outputEnd && outputStart <= inputEnd,
        HCCL_INFO(
            "[Algo][AutoSelectorBase][IsInputOutputOverlap] inputStart[%llu], inputEnd[%llu], outputStart[%llu], "
            "outputEnd[%llu]. Overlap detected.",
            inputStart, inputEnd, outputStart, outputEnd),
        true);

    HCCL_DEBUG("[Algo][AutoSelectorBase][IsInputOutputOverlap]No overlap between input and output memory.");
    return false;
}

bool AutoSelectorBase::ProcessAivConfig(
    OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
    const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName,
    SelectorStatus& ret) const
{
    if (opParam.opExecuteConfig != OpExecuteConfig::AIV && opParam.opExecuteConfig != OpExecuteConfig::AIV_ONLY) {
        return false;
    }

    ret = SelectAivAlgo(topoInfo, opParam, configAlgMap, selectAlgName);
    if (ret == SelectorStatus::NOT_MATCH) {
        if (opParam.opExecuteConfig == OpExecuteConfig::AIV_ONLY) {
            HCCL_ERROR("[Algo][AutoSelectorBase] Failed to select AIV algorithm while configured as AIV_ONLY.");
            return true;
        }
        opParam.opExecuteConfig = OpExecuteConfig::CCU_FAIL;
        return false;
    }

    return true;
}

} // namespace mc2_ops_hccl
