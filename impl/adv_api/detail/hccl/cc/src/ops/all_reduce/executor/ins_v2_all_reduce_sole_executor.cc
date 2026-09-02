/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_v2_all_reduce_sole_executor.h"
#include "ins_temp_all_reduce_mesh_1D_two_shot.h"
#include "ins_temp_all_reduce_mesh_1D_one_shot.h"
#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
#include "ccu_temp_kfc_all_reduce_mesh_1D_mem2mem.h"
#endif
#include "ins_temp_all_reduce_mesh_1D_two_shot_mesh_chunk.h"

namespace mc2_ops_hccl {

template <typename AlgTopoMatch, typename InsAlgTemplate>
InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::InsV2AllReduceSoleExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    // 使用topo match计算AlgHierarchyInfoForAllLevel
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    // 构建template
    std::shared_ptr<InsAlgTemplate> algTemplate =
        std::make_shared<InsAlgTemplate>(param, topoInfo->userRank, algHierarchyInfo.infos[0]);
    // 调用计算资源的函数
    CHK_RET(algTemplate->CalcRes(comm, param, topoInfo, resourceRequest));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllReduceSoleExecutor][Orchestrate] Orchestrate Start.");
    // maxTmpMemSize_设定为cclIn的大小，op中将申请的HcclBuff全给了cclIn
    maxTmpMemSize_ = resCtx.cclMem.size;
    // 给channels_和threads_赋值
    threads_ = resCtx.threads;
    if (param.engine != CommEngine::COMM_ENGINE_AIV && param.engine != CommEngine::COMM_ENGINE_CCU) {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
    }
    dataCount_ = param.DataDes.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    if (dataCount_ > UINT64_MAX / dataTypeSize_) {
        HCCL_ERROR(
            "[InsV2AllReduceSoleExecutor][Orchestrate] dataCount[%llu] * dataTypeSize_[%llu] is greater than "
            "UINT64_MAX",
            dataCount_, dataTypeSize_);
        return HCCL_E_INTERNAL;
    }
    dataSize_ = dataCount_ * dataTypeSize_;
    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2AllReduceSoleExecutor][Orchestrate]errNo[0x%016llx] AllReduce executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllReduceSoleExecutor][OrchestrateLoop] Start");
    // 准备资源
    TemplateResource templateAlgRes;
    if (param.engine == COMM_ENGINE_CCU) {
        templateAlgRes.ccuKernels = resCtx.ccuKernels;
    }
    if (param.engine != CommEngine::COMM_ENGINE_AIV && remoteRankToChannelInfo_.size() > 0) {
        templateAlgRes.channels = remoteRankToChannelInfo_[0];
    }
    templateAlgRes.threads = resCtx.threads;
    templateAlgRes.aivCommInfoPtr = resCtx.aivCommInfoPtr;
    // 准备数据
    TemplateDataParams tempAlgParams;
    tempAlgParams.buffInfo.inputPtr = param.inputPtr;
    tempAlgParams.buffInfo.outputPtr = param.outputPtr;
    tempAlgParams.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParams.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParams.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParams.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParams.buffInfo.inputSize = param.inputSize;
    tempAlgParams.buffInfo.outputSize = param.outputSize;
    tempAlgParams.enableRemoteMemAccess = param.opMode == OpMode::OFFLOAD;
    // 不需要重复；repeat用于处理rank存在多块不连续数据块的情况（all-reduce不涉及）
    tempAlgParams.repeatNum = 1;
    tempAlgParams.inputRepeatStride = 0;
    tempAlgParams.outputRepeatStride = 0;

    // 构建template
    std::shared_ptr<InsAlgTemplate> algTemplate =
        std::make_shared<InsAlgTemplate>(param, resCtx.topoInfo.userRank, resCtx.algHierarchyInfo.infos[0]);
    u32 templateScratchMultiplier =
        algTemplate->CalcScratchMultiple(tempAlgParams.buffInfo.inBuffType, tempAlgParams.buffInfo.outBuffType);

    // 计算最小传输大小
    u64 maxDataSizePerLoop = 0;
    maxTmpMemSize_ = tempAlgParams.buffInfo.hcclBuff.size;
    u64 transportBoundDataSize = (param.engine == CommEngine::COMM_ENGINE_AICPU_TS) ? maxTmpMemSize_ : UB_MAX_DATA_SIZE;
    HCCL_INFO("[InsV2AllReduceSoleExecutor]maxTmpMemSize_ [%u]", maxTmpMemSize_);
    if (templateScratchMultiplier != 0) {
        u64 scratchBoundDataSize =
            maxTmpMemSize_ / templateScratchMultiplier / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
        maxDataSizePerLoop = std::min(transportBoundDataSize, scratchBoundDataSize);
    } else {
        maxDataSizePerLoop = transportBoundDataSize;
    }
    // 单次循环处理的数据量大小
    CHK_PRT_RET(
        dataTypeSize_ == 0, HCCL_ERROR("[InsV2AllReduceSoleExecutor][OrchestrateOpbase] dataTypeSize_ is 0"),
        HCCL_E_INTERNAL);
    u64 maxDataCountPerLoop = maxDataSizePerLoop / dataTypeSize_;
    HCCL_INFO(
        "[InsV2AllReduceSoleExecutor][OrchestrateOpbase] maxDataCountPerLoop[%llu], maxDataSizePerLoop[%llu], "
        "transportBoundDataSize[%llu], templateScratchMultiplier[%llu]",
        maxDataCountPerLoop, maxDataSizePerLoop, transportBoundDataSize, templateScratchMultiplier);
    CHK_PRT_RET(
        maxDataCountPerLoop == 0,
        HCCL_ERROR("[InsV2AllReduceSoleExecutor][OrchestrateOpbase] maxDataCountPerLoop is 0"), HCCL_E_INTERNAL);
    // 计算loopTimes
    u64 loopTimes = dataCount_ / maxDataCountPerLoop + static_cast<u64>(dataCount_ % maxDataCountPerLoop != 0);
    // count已经处理的数据
    u64 processedDataCount = 0;
    for (u64 loop = 0; loop < loopTimes; loop++) {
        // dataCount_实际总数据量 和 maxDataCountPerLoop 一次搬运数据量之间不一定是整除关系，需要对尾块进行处理
        u64 currDataCount = (loop == loopTimes - 1) ? dataCount_ - processedDataCount : maxDataCountPerLoop;
        tempAlgParams.count = currDataCount;
        tempAlgParams.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParams.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
        tempAlgParams.buffInfo.hcclBuffBaseOff = 0;

        tempAlgParams.sliceSize = currDataCount * dataTypeSize_;
        tempAlgParams.tailSize = tempAlgParams.sliceSize;
        tempAlgParams.inputSliceStride = 0;
        tempAlgParams.outputSliceStride = 0;
        HCCL_INFO(
            "[InsV2AllReduceSoleExecutor] loop [%u] tempAlgParams.inputSliceStride [%u],"
            "tempAlgParams.outputSliceStride [%u] tempAlgParams.sliceSize [%u]",
            loop, tempAlgParams.inputSliceStride, tempAlgParams.outputSliceStride, tempAlgParams.sliceSize);
        HCCL_INFO(
            "[InsV2AllReduceSoleExecutor] loop [%u] tempAlgParams.buffInfo.inBuffBaseOff [%u],"
            "tempAlgParams.outBuffBaseOff [%u]",
            loop, tempAlgParams.buffInfo.inBuffBaseOff, tempAlgParams.buffInfo.outBuffBaseOff);

        CHK_RET(algTemplate->KernelRun(param, tempAlgParams, templateAlgRes));
        processedDataCount += currDataCount;
    }
#ifndef AICPU_COMPILE
    if (loopTimes == 1 && param.engine == CommEngine::COMM_ENGINE_CCU && param.opMode != OpMode::OFFLOAD) {
        CHK_RET(FastLaunchSaveCtx(param, templateAlgRes, resCtx.notifyNumOnMainThread));
    }
#endif

    HCCL_INFO("[InsV2AllReduceSoleExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

#ifndef AICPU_COMPILE
template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::FastLaunchSaveCtx(
    const OpParam& param, const TemplateResource& templateAlgRes, u32 notifyNumOnMainThread) const
{
    HCCL_INFO("[InsV2AllReduceSoleExecutor] loopTimes==1, save fast launch ctx.");
    u32 threadNum = 1;
    u32 ccuKernelNum = templateAlgRes.submitInfos.size();
    if (ccuKernelNum < 1) {
        HCCL_INFO("[InsV2AllReduceSoleExecutor] ccu kernel num is 0, no need to save.");
        return HCCL_SUCCESS;
    }
    HCCL_INFO(
        "[InsV2AllReduceSoleExecutor][HcclEngineCtxCreate] threadNum[%llu], ccuKernelNum[%llu]", threadNum,
        ccuKernelNum);

    u64 size = CcuFastLaunchCtx::GetCtxSize(threadNum, ccuKernelNum);
    // 申请ctx
    void* ctxPtr = nullptr;
    HCCL_INFO("[InsV2AllReduceSoleExecutor][HcclEngineCtxCreate] Tag[%s], size[%llu]", param.fastLaunchTag, size);
    CHK_RET(HcclEngineCtxCreate(param.hcclComm, param.fastLaunchTag, CommEngine::COMM_ENGINE_CCU, size, &ctxPtr));

    CcuFastLaunchCtx* ccuFastLaunchCtx = reinterpret_cast<CcuFastLaunchCtx*>(ctxPtr);
    // 1 算法名
    CHK_SAFETY_FUNC_RET(strcpy_s(ccuFastLaunchCtx->algName, sizeof(ccuFastLaunchCtx->algName), param.algName));
    HCCL_INFO("[InsV2AllReduceSoleExecutor][FastLaunchSaveCtx] algName[%s]", ccuFastLaunchCtx->algName);

    // 2 thread
    ccuFastLaunchCtx->threadNum = threadNum;
    ccuFastLaunchCtx->notifyNumOnMainThread = notifyNumOnMainThread;
    ThreadHandle* threads = ccuFastLaunchCtx->GetThreadHandlePtr();
    threads[0] = templateAlgRes.threads[0];

    // 3 ccu kernel handle, taskArg入参
    ccuFastLaunchCtx->ccuKernelNum[0] = ccuKernelNum;
    CcuKernelSubmitInfo* kernelSubmitInfos = ccuFastLaunchCtx->GetCcuKernelSubmitInfoPtr();
    for (int i = 0; i < ccuKernelNum; i++) {
        kernelSubmitInfos[i] = templateAlgRes.submitInfos[i];
    }
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsV2AllReduceSoleExecutor<AlgTopoMatch, InsAlgTemplate>::FastLaunch(
    const OpParam& param, const CcuFastLaunchCtx* fastLaunchCtx)
{
    HCCL_INFO("[InsV2AllReduceSoleExecutor][FastLaunch] Start.");
    TemplateFastLaunchCtx tempFastLaunchCtx;
    // 1 取thread
    ThreadHandle* threads = fastLaunchCtx->GetThreadHandlePtr();
    tempFastLaunchCtx.threads.assign(threads, threads + fastLaunchCtx->threadNum);
    HCCL_INFO("[InsV2AllReduceSoleExecutor][FastLaunch] threadNum[%llu]", fastLaunchCtx->threadNum);

    // 2 取arg
    CcuKernelSubmitInfo* ccuKernelSubmitInfos = fastLaunchCtx->GetCcuKernelSubmitInfoPtr();
    tempFastLaunchCtx.ccuKernelSubmitInfos.assign(
        ccuKernelSubmitInfos, ccuKernelSubmitInfos + fastLaunchCtx->ccuKernelNum[0]);
    HCCL_INFO("[InsV2AllReduceSoleExecutor][FastLaunch] ccuKernelNum[%llu]", fastLaunchCtx->ccuKernelNum[0]);
    tempFastLaunchCtx.buffInfo.inputPtr = param.inputPtr;
    tempFastLaunchCtx.buffInfo.outputPtr = param.outputPtr;
    tempFastLaunchCtx.buffInfo.hcclBuff = param.hcclBuff;

    // 3 调template
    std::unique_ptr<InsAlgTemplate> algTemplate = std::make_unique<InsAlgTemplate>();
    CHK_RET(algTemplate->FastLaunch(param, tempFastLaunchCtx));
    HCCL_INFO("[InsV2AllReduceSoleExecutor][FastLaunch] End.");
    return HCCL_SUCCESS;
}
#endif

REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllReduceSoleMeshTwoShot, InsV2AllReduceSoleExecutor, TopoMatch1D,
    InsTempAllReduceMesh1DTwoShot);

REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllReduceSoleMeshOneShot, InsV2AllReduceSoleExecutor, TopoMatch1D,
    InsTempAllReduceMesh1DOneShot);

#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLREDUCE, CcuSchedAllReduceSoleMesh, InsV2AllReduceSoleExecutor, TopoMatch1D,
    CcuTempKfcAllReduceMesh1DMem2Mem);
#endif

REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllReduceSoleMeshChunkTwoShot, InsV2AllReduceSoleExecutor, TopoMatch1D,
    InsTempAllReduceMesh1DTwoShotMeshChunk);

} // namespace mc2_ops_hccl
