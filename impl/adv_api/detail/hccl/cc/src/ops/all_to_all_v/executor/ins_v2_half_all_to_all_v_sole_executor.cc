/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_half_all_to_all_v_sole_executor.h"
#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
#include "ccu_temp_kfc_half_alltoallv_mesh_1D.h"
#endif
#include "topo_match_1d.h"
#include "alg_meta_registry.h"
#include "external_alg_rules.h"

namespace mc2_ops_hccl {
namespace {
bool IsValidHalfA2AVDataType(HcclDataType dataType)
{
    return static_cast<u32>(dataType) < static_cast<u32>(HCCL_DATA_TYPE_RESERVED);
}
} // namespace

template <typename AlgTopoMatch, typename InsAlgTemplate>
InsV2HalfAlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::InsV2HalfAlltoAllVSoleExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2HalfAlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2HalfAlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        algHierarchyInfo.infos.empty() || algHierarchyInfo.infos[0].empty(),
        HCCL_ERROR("[InsV2HalfAlltoAllVSoleExecutor][CalcRes] hierarchy info is empty."), HCCL_E_PARA);

    std::shared_ptr<InsAlgTemplate> algTemplate =
        std::make_shared<InsAlgTemplate>(param, topoInfo->userRank, algHierarchyInfo.infos[0]);
    CHK_RET(algTemplate->CalcRes(comm, param, topoInfo, resourceRequest));
    myRank_ = topoInfo->userRank;
    HCCL_DEBUG(
        "[InsV2HalfAlltoAllVSoleExecutor][CalcRes] myRank[%u], slaveThreadNum[%u], channels[%u]", myRank_,
        resourceRequest.slaveThreadNum, resourceRequest.channels.size());
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2HalfAlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO(
        "[InsV2HalfAlltoAllVSoleExecutor][Orchestrate] Start, varMemSize[%llu], opType[%u], algName[%s].",
        static_cast<unsigned long long>(param.varMemSize), static_cast<u32>(param.opType), param.algName);

    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;
    CHK_PRT_RET(
        rankSize_ == 0U || myRank_ >= rankSize_,
        HCCL_ERROR("[InsV2HalfAlltoAllVSoleExecutor] invalid rank[%u]/rankSize[%u].", myRank_, rankSize_), HCCL_E_PARA);
    CHK_PRT_RET(
        resCtx.algHierarchyInfo.infos.empty() || resCtx.algHierarchyInfo.infos[0].empty(),
        HCCL_ERROR("[InsV2HalfAlltoAllVSoleExecutor] hierarchy info is empty."), HCCL_E_PARA);
    // MC2 变长区必须按 4 段 * rankSize * u64 布局，kernel 依赖该约定读取分片信息
    CHK_PRT_RET(
        param.varMemSize != ALL_TO_ALL_V_VECTOR_NUM * static_cast<u64>(rankSize_) * sizeof(u64),
        HCCL_ERROR(
            "[InsV2HalfAlltoAllVSoleExecutor] invalid varMemSize[%llu] for rankSize[%u].",
            static_cast<unsigned long long>(param.varMemSize), rankSize_),
        HCCL_E_PARA);

    dataType_ = param.all2AllVDataDes.sendType;
    CHK_PRT_RET(
        !IsValidHalfA2AVDataType(dataType_) || param.all2AllVDataDes.sendType != param.all2AllVDataDes.recvType,
        HCCL_ERROR("[InsV2HalfAlltoAllVSoleExecutor] invalid or mismatched send/recv datatype."), HCCL_E_PARA);
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(
        dataTypeSize_ == 0U, HCCL_ERROR("[InsV2HalfAlltoAllVSoleExecutor] datatype size is zero."), HCCL_E_PARA);

    threads_ = resCtx.threads;
    if (param.engine != CommEngine::COMM_ENGINE_AIV && param.engine != CommEngine::COMM_ENGINE_CCU) {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
    }
    return OrchestrateLoop(param, resCtx);
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2HalfAlltoAllVSoleExecutor<AlgTopoMatch, InsAlgTemplate>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    TemplateResource templateAlgRes;
    if (param.engine == COMM_ENGINE_CCU) {
        templateAlgRes.ccuKernels = resCtx.ccuKernels;
    }
    if (param.engine != CommEngine::COMM_ENGINE_AIV && !remoteRankToChannelInfo_.empty()) {
        templateAlgRes.channels = remoteRankToChannelInfo_[0];
    }
    templateAlgRes.threads = resCtx.threads;
    templateAlgRes.aivCommInfoPtr = resCtx.aivCommInfoPtr;

    TemplateDataParams tempAlgParams;
    tempAlgParams.buffInfo.inputPtr = param.inputPtr;
    tempAlgParams.buffInfo.outputPtr = param.outputPtr;
    tempAlgParams.buffInfo.inputSize = param.inputSize;
    tempAlgParams.buffInfo.outputSize = param.outputSize;
    tempAlgParams.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParams.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParams.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParams.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParams.buffInfo.inBuffBaseOff = 0U;
    tempAlgParams.buffInfo.outBuffBaseOff = 0U;
    tempAlgParams.buffInfo.hcclBuffBaseOff = 0U;
    tempAlgParams.dataType = dataType_;
    tempAlgParams.repeatNum = 1U;
    tempAlgParams.inputRepeatStride = 0U;
    tempAlgParams.outputRepeatStride = 0U;
    tempAlgParams.enableRemoteMemAccess = param.opMode == OpMode::OFFLOAD;

    // 分片大小运行时才确定，host 侧无法切 loop。以每 rank 均分容量作为上界：
    // outputSliceStride 供 kernel 算 recvOffset，sliceSize 供本地 GroupCopy 定 goSize 上界。
    const u64 perRankCapacity = param.inputSize / rankSize_ / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
    CHK_PRT_RET(
        perRankCapacity == 0U,
        HCCL_ERROR(
            "[InsV2HalfAlltoAllVSoleExecutor] perRankCapacity is zero, inputSize[%llu], rankSize[%u].",
            static_cast<unsigned long long>(param.inputSize), rankSize_),
        HCCL_E_PARA);
    tempAlgParams.inputSliceStride = perRankCapacity;
    tempAlgParams.outputSliceStride = perRankCapacity;
    tempAlgParams.sliceSize = perRankCapacity;
    tempAlgParams.tailSize = 0U;
    tempAlgParams.count = perRankCapacity / dataTypeSize_;

    InsAlgTemplate algTemplate(param, resCtx.topoInfo.userRank, resCtx.algHierarchyInfo.infos[0]);
    HCCL_DEBUG(
        "[InsV2HalfAlltoAllVSoleExecutor][OrchestrateLoop] myRank[%u], perRankCapacity[%llu], ccuKernels[%lu]", myRank_,
        static_cast<unsigned long long>(perRankCapacity), templateAlgRes.ccuKernels.size());
    CHK_RET(algTemplate.KernelRun(param, tempAlgParams, templateAlgRes));
    HCCL_INFO("[InsV2HalfAlltoAllVSoleExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU

REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_HALF_ALLTOALLV, CcuHalfAll2AllVMesh1D, InsV2HalfAlltoAllVSoleExecutor, TopoMatch1D,
    CcuTempKfcHalfAlltoAllVMesh1D);
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_HALF_ALLTOALLV, CcuHalfAll2AllVMesh1D, AlgEngine::CCU, "sole[mesh]", COND_NONE, FLAG_NONE, 0);

#endif
} // namespace mc2_ops_hccl
