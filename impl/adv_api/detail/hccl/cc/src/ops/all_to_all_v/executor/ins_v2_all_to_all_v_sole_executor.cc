/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_all_to_all_v_sole_executor.h"

#include <algorithm>
#include <limits>

#include "ins_temp_all_to_all_v_mesh_1D.h"
#include "ins_temp_ubx_all_to_all_v_mesh_1D.h"
#include "topo_match_1d.h"
#include "topo_match_ubx_1d.h"
#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
#include "ccu_temp_all_to_all_v_mesh_1D.h"
#endif
#ifndef AICPU_COMPILE
#include "ccu_temp_all_to_all_mesh_1D.h"
#endif

namespace mc2_ops_hccl {
namespace {
bool IsSupportedMesh1DTopo(Level0Shape topo)
{
    return topo == Level0Shape::MESH_1D || topo == Level0Shape::CLOS || topo == Level0Shape::MESH_1D_CLOS;
}

HcclResult BuildResourceHierarchyInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails& topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, std::vector<std::vector<u32>>& templateHierarchyInfo)
{
    CHK_PRT_RET(
        algHierarchyInfo.infos.empty() || algHierarchyInfo.infos[0].empty(),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] hierarchy info is empty."), HCCL_E_PARA);
    if (topoInfo.level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo.level0PcieMix &&
        param.engine != CommEngine::COMM_ENGINE_AIV) {
        CHK_PRT_RET(
            algHierarchyInfo.infos[0].size() != 2U,
            HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] MESH_1D_CLOS requires two level-0 instances."), HCCL_E_PARA);
        if (topoInfo.topoLevelNums == 1U || param.engine == CommEngine::COMM_ENGINE_CCU) {
            templateHierarchyInfo = {algHierarchyInfo.infos[0][1]};
        } else {
            CHK_PRT_RET(
                algHierarchyInfo.infos.size() <= 1U || algHierarchyInfo.infos[1].empty() ||
                    algHierarchyInfo.infos[0][1].size() >= algHierarchyInfo.infos[1][0].size(),
                HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] invalid MESH_1D_CLOS hierarchy sizes."), HCCL_E_PARA);
            templateHierarchyInfo = {algHierarchyInfo.infos[0][1], algHierarchyInfo.infos[1][0]};
        }
    } else {
        templateHierarchyInfo = algHierarchyInfo.infos[0];
    }
    return HCCL_SUCCESS;
}

HcclResult BuildExecutionHierarchyInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails& topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, std::vector<std::vector<u32>>& templateHierarchyInfo)
{
    CHK_PRT_RET(
        algHierarchyInfo.infos.empty() || algHierarchyInfo.infos[0].empty(),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] execution hierarchy info is empty."), HCCL_E_PARA);
    if (topoInfo.level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo.level0PcieMix &&
        param.engine != CommEngine::COMM_ENGINE_AIV && algHierarchyInfo.infos.size() > 1U) {
        CHK_PRT_RET(
            algHierarchyInfo.infos[0].size() != 2U,
            HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] MESH_1D_CLOS requires two level-0 instances."), HCCL_E_PARA);
        if (topoInfo.topoLevelNums == 1U) {
            templateHierarchyInfo = {algHierarchyInfo.infos[0][1]};
        } else {
            CHK_PRT_RET(
                algHierarchyInfo.infos[1].empty(),
                HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] execution hierarchy level is empty."), HCCL_E_PARA);
            templateHierarchyInfo = algHierarchyInfo.infos[1];
        }
    } else {
        templateHierarchyInfo = algHierarchyInfo.infos[0];
    }
    return HCCL_SUCCESS;
}

bool AddOverflows(u64 lhs, u64 rhs) { return lhs > std::numeric_limits<u64>::max() - rhs; }

bool MultiplyOverflows(u64 lhs, u64 rhs) { return lhs != 0U && rhs > std::numeric_limits<u64>::max() / lhs; }

bool IsValidDataType(HcclDataType dataType)
{
    return static_cast<u32>(dataType) < static_cast<u32>(HCCL_DATA_TYPE_RESERVED);
}
} // namespace

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        !IsSupportedMesh1DTopo(topoInfo->level0Topo),
        HCCL_ERROR(
            "[InsV2AlltoAllVSoleExecutor][CalcAlgHierarchyInfo] unsupported level0Topo[%u].",
            static_cast<u32>(topoInfo->level0Topo)),
        HCCL_E_NOT_SUPPORT);
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        !IsSupportedMesh1DTopo(topoInfo->level0Topo),
        HCCL_ERROR(
            "[InsV2AlltoAllVSoleExecutor][CalcRes] unsupported level0Topo[%u].",
            static_cast<u32>(topoInfo->level0Topo)),
        HCCL_E_NOT_SUPPORT);
    std::vector<std::vector<u32>> templateHierarchyInfo;
    CHK_RET(BuildResourceHierarchyInfo(param, *topoInfo, algHierarchyInfo, templateHierarchyInfo));
    InsAlgTemplate algTemplate(param, topoInfo->userRank, templateHierarchyInfo);
    return algTemplate.CalcRes(comm, param, topoInfo, resourceRequest);
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::ValidateAndInit(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    CHK_PRT_RET(
        !IsSupportedMesh1DTopo(resCtx.topoInfo.level0Topo),
        HCCL_ERROR(
            "[InsV2AlltoAllVSoleExecutor] unsupported level0Topo[%u].", static_cast<u32>(resCtx.topoInfo.level0Topo)),
        HCCL_E_NOT_SUPPORT);
    rankSize_ = resCtx.topoInfo.userRankSize;
    myRank_ = resCtx.topoInfo.userRank;
    CHK_PRT_RET(
        rankSize_ == 0U || myRank_ >= rankSize_,
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] invalid rank[%u]/rankSize[%u].", myRank_, rankSize_), HCCL_E_PARA);
    CHK_PRT_RET(
        !IsValidDataType(param.all2AllVDataDes.sendType) || !IsValidDataType(param.all2AllVDataDes.recvType),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] invalid send/recv datatype."), HCCL_E_PARA);
    CHK_PRT_RET(
        param.all2AllVDataDes.sendType != param.all2AllVDataDes.recvType,
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] send/recv datatype must match."), HCCL_E_PARA);
    dataType_ = param.all2AllVDataDes.sendType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(dataTypeSize_ == 0U, HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] datatype size is zero."), HCCL_E_PARA);
    CHK_PRT_RET(
        param.varMemSize != ALL_TO_ALL_V_VECTOR_NUM * static_cast<u64>(rankSize_) * sizeof(u64),
        HCCL_ERROR(
            "[InsV2AlltoAllVSoleExecutor] invalid varMemSize[%llu] for rankSize[%u].",
            static_cast<unsigned long long>(param.varMemSize), rankSize_),
        HCCL_E_PARA);
    CHK_PTR_NULL(param.all2AllVDataDes.sendCounts);
    CHK_PTR_NULL(param.all2AllVDataDes.recvCounts);
    CHK_PTR_NULL(param.all2AllVDataDes.sdispls);
    CHK_PTR_NULL(param.all2AllVDataDes.rdispls);
    CHK_PRT_RET(
        resCtx.algHierarchyInfo.infos.empty() || resCtx.algHierarchyInfo.infos[0].empty(),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] hierarchy info is empty."), HCCL_E_PARA);

    remoteRankToChannelInfo_.clear();
    if (resCtx.topoInfo.level0Topo == Level0Shape::MESH_1D_CLOS && !resCtx.topoInfo.level0PcieMix &&
        param.engine != CommEngine::COMM_ENGINE_AIV && param.engine != CommEngine::COMM_ENGINE_CCU) {
        CHK_PRT_RET(
            resCtx.channels.size() != 1U,
            HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] MESH_1D_CLOS requires one channel group."), HCCL_E_PARA);
        remoteRankToChannelInfo_.resize(1U);
        for (const auto& channel : resCtx.channels[0]) {
            remoteRankToChannelInfo_[0][channel.remoteRank].push_back(channel);
        }
    } else {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
    }
    CHK_PRT_RET(
        rankSize_ > 1U && (remoteRankToChannelInfo_.empty() || remoteRankToChannelInfo_[0].empty()),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] restored channel map is empty."), HCCL_E_INTERNAL);
    maxTmpMemSize_ = resCtx.cclMem.size;
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    CHK_RET(ValidateAndInit(param, resCtx));
    CHK_RET(OrchestrateLoop(param, resCtx));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    TemplateResource templateResource{};
    if (!remoteRankToChannelInfo_.empty()) {
        templateResource.channels = remoteRankToChannelInfo_[0];
    }
    templateResource.threads = resCtx.threads;
    templateResource.aivCommInfoPtr = resCtx.aivCommInfoPtr;
    templateResource.npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
    templateResource.dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;

    TemplateDataParams tempAlgParams;
    tempAlgParams.buffInfo.inputPtr = param.inputPtr;
    tempAlgParams.buffInfo.outputPtr = param.outputPtr;
    tempAlgParams.buffInfo.inputSize = param.inputSize;
    tempAlgParams.buffInfo.outputSize = param.outputSize;
    tempAlgParams.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParams.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParams.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParams.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;

    const auto* sourceSendCounts = static_cast<const u64*>(param.all2AllVDataDes.sendCounts);
    const auto* sourceRecvCounts = static_cast<const u64*>(param.all2AllVDataDes.recvCounts);
    const auto* sourceSendDispls = static_cast<const u64*>(param.all2AllVDataDes.sdispls);
    const auto* sourceRecvDispls = static_cast<const u64*>(param.all2AllVDataDes.rdispls);
    std::vector<u64> sendCounts(sourceSendCounts, sourceSendCounts + rankSize_);
    std::vector<u64> recvCounts(sourceRecvCounts, sourceRecvCounts + rankSize_);
    std::vector<u64> sendDispls(sourceSendDispls, sourceSendDispls + rankSize_);
    std::vector<u64> recvDispls(sourceRecvDispls, sourceRecvDispls + rankSize_);

    std::vector<std::vector<u32>> templateHierarchyInfo;
    CHK_RET(BuildExecutionHierarchyInfo(param, resCtx.topoInfo, resCtx.algHierarchyInfo, templateHierarchyInfo));
    InsAlgTemplate algTemplate(param, resCtx.topoInfo.userRank, templateHierarchyInfo);
    const u64 scratchMultiplier =
        algTemplate.CalcScratchMultiple(tempAlgParams.buffInfo.inBuffType, tempAlgParams.buffInfo.outBuffType);
    u64 maxDataSizePerLoop = UB_MAX_DATA_SIZE;
    if (scratchMultiplier != 0U) {
        const u64 scratchBound = maxTmpMemSize_ / scratchMultiplier / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
        maxDataSizePerLoop = std::min(maxDataSizePerLoop, scratchBound);
    }
    const u64 maxDataCountPerLoop = maxDataSizePerLoop / dataTypeSize_;
    CHK_PRT_RET(
        maxDataCountPerLoop == 0U,
        HCCL_ERROR(
            "[InsV2AlltoAllVSoleExecutor] maxDataCountPerLoop is zero, scratch[%llu], multiplier[%llu].",
            static_cast<unsigned long long>(maxTmpMemSize_), static_cast<unsigned long long>(scratchMultiplier)),
        HCCL_E_INTERNAL);

    u64 maxSendOrRecvDataCount = 0U;
    for (u32 rank = 0U; rank < rankSize_; ++rank) {
        maxSendOrRecvDataCount = std::max(maxSendOrRecvDataCount, sendCounts[rank]);
        maxSendOrRecvDataCount = std::max(maxSendOrRecvDataCount, recvCounts[rank]);
    }
    if (maxSendOrRecvDataCount == 0U) {
        return HCCL_SUCCESS;
    }
    const u64 loopTimes = maxSendOrRecvDataCount / maxDataCountPerLoop +
                          static_cast<u64>(maxSendOrRecvDataCount % maxDataCountPerLoop != 0U);
    CHK_PRT_RET(
        MultiplyOverflows(maxDataCountPerLoop, dataTypeSize_) ||
            MultiplyOverflows(maxSendOrRecvDataCount, dataTypeSize_),
        HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] slice stride overflows."), HCCL_E_PARA);
    tempAlgParams.inputSliceStride = maxDataCountPerLoop * dataTypeSize_;
    tempAlgParams.outputSliceStride = maxSendOrRecvDataCount * dataTypeSize_;

    u64 processedDataCount = 0U;
    for (u64 loop = 0U; loop < loopTimes; ++loop) {
        const u64 currDataCount =
            loop + 1U == loopTimes ? maxSendOrRecvDataCount - processedDataCount : maxDataCountPerLoop;
        CHK_PRT_RET(
            MultiplyOverflows(processedDataCount, dataTypeSize_) || MultiplyOverflows(currDataCount, dataTypeSize_),
            HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] loop byte offset overflows."), HCCL_E_PARA);
        tempAlgParams.count = currDataCount;
        tempAlgParams.dataType = dataType_;
        tempAlgParams.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParams.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParams.buffInfo.hcclBuffBaseOff = 0U;
        tempAlgParams.processedDataCount = processedDataCount;
        tempAlgParams.sliceSize = currDataCount * dataTypeSize_;
        tempAlgParams.tailSize = tempAlgParams.sliceSize;
        tempAlgParams.repeatNum = 1U;
        tempAlgParams.inputRepeatStride = 0U;
        tempAlgParams.outputRepeatStride = 0U;
        tempAlgParams.sendCounts.assign(rankSize_, 0U);
        tempAlgParams.recvCounts.assign(rankSize_, 0U);
        tempAlgParams.sdispls.assign(rankSize_, 0U);
        tempAlgParams.rdispls.assign(rankSize_, 0U);

        for (u32 rank = 0U; rank < rankSize_; ++rank) {
            if (sendCounts[rank] > processedDataCount) {
                CHK_PRT_RET(
                    AddOverflows(sendDispls[rank], processedDataCount),
                    HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] send displacement overflows."), HCCL_E_PARA);
                tempAlgParams.sendCounts[rank] = std::min(currDataCount, sendCounts[rank] - processedDataCount);
                tempAlgParams.sdispls[rank] = sendDispls[rank] + processedDataCount;
            } else {
                CHK_PRT_RET(
                    AddOverflows(sendDispls[rank], sendCounts[rank]),
                    HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] final send displacement overflows."), HCCL_E_PARA);
                tempAlgParams.sdispls[rank] = sendDispls[rank] + sendCounts[rank];
            }
            if (recvCounts[rank] > processedDataCount) {
                CHK_PRT_RET(
                    AddOverflows(recvDispls[rank], processedDataCount),
                    HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] receive displacement overflows."), HCCL_E_PARA);
                tempAlgParams.recvCounts[rank] = std::min(currDataCount, recvCounts[rank] - processedDataCount);
                tempAlgParams.rdispls[rank] = recvDispls[rank] + processedDataCount;
            } else {
                CHK_PRT_RET(
                    AddOverflows(recvDispls[rank], recvCounts[rank]),
                    HCCL_ERROR("[InsV2AlltoAllVSoleExecutor] final receive displacement overflows."), HCCL_E_PARA);
                tempAlgParams.rdispls[rank] = recvDispls[rank] + recvCounts[rank];
            }
        }

        CHK_RET(algTemplate.KernelRun(param, tempAlgParams, templateResource));
        processedDataCount += currDataCount;
    }
    return HCCL_SUCCESS;
}

REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALLV, AicpuAllToAllVSoleMesh, InsV2AlltoAllVSoleExecutor, TopoMatch1D,
    InsTempAlltoAllVMesh1D);
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALL, AicpuAllToAllSoleMeshSingleChannel, InsV2AlltoAllVSoleExecutor, TopoMatch1D,
    InsTempAlltoAllVMesh1D);
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALL, AicpuAllToAllSoleMesh, InsV2AlltoAllVSoleExecutor, TopoMatch1D,
    InsTempAlltoAllVMesh1D);
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALL, AicpuAllToAllSoleMeshUBX, InsV2AlltoAllVSoleExecutor, TopoMatchUBX1d,
    InsTempUBXAllToAllVMesh1D);

#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALLV, CcuSchedAllToAllVSoleMesh, InsV2AlltoAllVSoleExecutor, TopoMatch1D,
    CcuTempAlltoAllVMesh1D);
#endif
#ifndef AICPU_COMPILE
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLTOALL, CcuSchedAllToAllSoleMesh, InsV2AlltoAllVSoleExecutor, TopoMatch1D,
    CcuTempAlltoAllMesh1D);
#endif

} // namespace mc2_ops_hccl
