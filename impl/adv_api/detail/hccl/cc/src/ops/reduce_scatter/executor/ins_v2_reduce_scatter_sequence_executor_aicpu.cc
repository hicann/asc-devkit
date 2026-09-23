/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_v2_reduce_scatter_sequence_executor_aicpu.h"
#include "ins_temp_reduce_scatter_mesh_1D_Z_axis_detour.h"
#include "ins_temp_reduce_scatter_nhr.h"
#include "alg_data_trans_wrapper.h"
#include "alg_meta_registry.h"
#include "external_alg_rules.h"

namespace mc2_ops_hccl {

constexpr u32 SEQUENCE_EXECUTOR_LEVEL_NUM = 2;

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsV2ReduceScatterSequenceExecutorAicpu<
    AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InsV2ReduceScatterSequenceExecutorAicpu()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    reduceOp_ = param.reduceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    strideCount_ = param.DataDes.strideCount;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];

    algHierarchyInfo_ = algHierarchyInfo;
    HCCL_INFO(
        "[InsV2ReduceScatterSequenceExecutorAicpu][InitCommInfo] myRank[%u], rankSize[%u], reduceOp[%u], "
        "dataType[%u], dataTypeSize[%llu], strideCount[%llu]",
        myRank_, rankSize_, reduceOp_, dataType_, dataTypeSize_, strideCount_);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult
InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    HCCL_DEBUG("[InsV2ReduceScatterSequenceExecutorAicpu] CalcRes start");
    InitCommInfo(param, topoInfo, algHierarchyInfo);
    if (algHierarchyInfo.infos.size() != SEQUENCE_EXECUTOR_LEVEL_NUM) {
        HCCL_ERROR(
            "[InsV2ReduceScatterSequenceExecutorAicpu] algHierarchyInfo size should be %u",
            SEQUENCE_EXECUTOR_LEVEL_NUM);
        return HCCL_E_INTERNAL;
    }

    std::shared_ptr<InsAlgTemplate0> intraTempAlg =
        std::make_shared<InsAlgTemplate0>(param, myRank_, algHierarchyInfo.infos[0]);
    std::shared_ptr<InsAlgTemplate1> interTempAlg =
        std::make_shared<InsAlgTemplate1>(param, myRank_, algHierarchyInfo.infos[1]);

    AlgResourceRequest resReqIntra;
    AlgResourceRequest resReqInter;
    CHK_RET(intraTempAlg->CalcRes(comm, param, topoInfo, resReqIntra));
    CHK_RET(interTempAlg->CalcRes(comm, param, topoInfo, resReqInter));

    resourceRequest.slaveThreadNum = std::max(resReqInter.slaveThreadNum, resReqIntra.slaveThreadNum);
    resourceRequest.notifyNumPerThread.clear();
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    for (u32 i = 0; i < resourceRequest.slaveThreadNum; ++i) {
        if (i < resReqInter.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqInter.notifyNumPerThread[i]);
        }
        if (i < resReqIntra.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqIntra.notifyNumPerThread[i]);
        }
    }
    resourceRequest.notifyNumOnMainThread =
        std::max(resReqInter.notifyNumOnMainThread, resReqIntra.notifyNumOnMainThread);
    resourceRequest.channels.resize(SEQUENCE_EXECUTOR_LEVEL_NUM);
    resourceRequest.channels[0] = resReqIntra.channels[0];
    resourceRequest.channels[1] = resReqInter.channels[0];
    HCCL_INFO(
        "[InsV2ReduceScatterSequenceExecutorAicpu] slaveThreadNum[%u], notifyNumOnMainThread[%u], "
        "level0 channel size[%u], level1 channel size[%u]",
        resourceRequest.slaveThreadNum, resourceRequest.notifyNumOnMainThread, resourceRequest.channels[0].size(),
        resourceRequest.channels[1].size());
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;

    dataCount_ = param.DataDes.count;
    strideCount_ = param.DataDes.strideCount;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    dataType_ = param.DataDes.dataType;
    reduceOp_ = param.reduceType;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;

    rankIdxLevel0_ = myRank_ % algHierarchyInfo_.infos[0][0].size();
    rankIdxLevel1_ = myRank_ / algHierarchyInfo_.infos[0][0].size();

    rankSizeLevel0_ = algHierarchyInfo_.infos[0][0].size();
    rankSizeLevel1_ = algHierarchyInfo_.infos[1][0].size();
    CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));

    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2ReduceScatterSequenceExecutorAicpu][Orchestrate] errNo[0x%016llx] "
            "Reduce scatter executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenIntraTemplateParams(
    TemplateDataParams& tempAlgParamsIntra, const u64 processedDataCount, const u64 currDataCount, const u64 loop) const
{
    tempAlgParamsIntra.count = currDataCount;
    tempAlgParamsIntra.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
    tempAlgParamsIntra.buffInfo.outBuffBaseOff = 0;
    tempAlgParamsIntra.buffInfo.hcclBuffBaseOff = 0;

    tempAlgParamsIntra.sliceSize = currDataCount * dataTypeSize_;
    tempAlgParamsIntra.tailSize = tempAlgParamsIntra.sliceSize;

    const u64 inputRankStride = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_;
    tempAlgParamsIntra.inputSliceStride = inputRankStride;
    tempAlgParamsIntra.outputSliceStride = currDataCount * dataTypeSize_;
    tempAlgParamsIntra.repeatNum = rankSizeLevel1_;
    tempAlgParamsIntra.inputRepeatStride = rankSizeLevel0_ * inputRankStride;
    tempAlgParamsIntra.outputRepeatStride = rankSizeLevel0_ * currDataCount * dataTypeSize_;

    HCCL_INFO(
        "[InsV2ReduceScatterSequenceExecutorAicpu] loop[%llu], inputSliceStride[%llu], "
        "outputSliceStride[%llu], sliceSize[%llu], inBaseOff[%llu], outBaseOff[%llu], repeatNum[%llu], "
        "inputRepeatStride[%llu], outputRepeatStride[%llu]",
        loop, tempAlgParamsIntra.inputSliceStride, tempAlgParamsIntra.outputSliceStride, tempAlgParamsIntra.sliceSize,
        tempAlgParamsIntra.buffInfo.inBuffBaseOff, tempAlgParamsIntra.buffInfo.outBuffBaseOff,
        tempAlgParamsIntra.repeatNum, tempAlgParamsIntra.inputRepeatStride, tempAlgParamsIntra.outputRepeatStride);
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenInterTemplateParams(
    TemplateDataParams& tempAlgParamsInter, const u64 processedDataCount, const u64 currDataCount, const u64 loop) const
{
    tempAlgParamsInter.count = currDataCount;
    tempAlgParamsInter.buffInfo.inBuffBaseOff = rankIdxLevel0_ * currDataCount * dataTypeSize_;
    tempAlgParamsInter.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
    tempAlgParamsInter.buffInfo.hcclBuffBaseOff = rankIdxLevel0_ * currDataCount * dataTypeSize_;

    tempAlgParamsInter.sliceSize = currDataCount * dataTypeSize_;
    tempAlgParamsInter.tailSize = tempAlgParamsInter.sliceSize;

    tempAlgParamsInter.inputSliceStride = rankSizeLevel0_ * currDataCount * dataTypeSize_;
    tempAlgParamsInter.outputSliceStride = 0;
    tempAlgParamsInter.repeatNum = 1;
    tempAlgParamsInter.inputRepeatStride = 0;
    tempAlgParamsInter.outputRepeatStride = 0;

    HCCL_INFO(
        "[InsV2ReduceScatterSequenceExecutorAicpu] loop[%llu], interInputSliceStride[%llu], "
        "interOutputSliceStride[%llu], sliceSize[%llu], inBaseOff[%llu], outBaseOff[%llu], repeatNum[%llu], "
        "inputRepeatStride[%llu], outputRepeatStride[%llu]",
        loop, tempAlgParamsInter.inputSliceStride, tempAlgParamsInter.outputSliceStride, tempAlgParamsInter.sliceSize,
        tempAlgParamsInter.buffInfo.inBuffBaseOff, tempAlgParamsInter.buffInfo.outBuffBaseOff,
        tempAlgParamsInter.repeatNum, tempAlgParamsInter.inputRepeatStride, tempAlgParamsInter.outputRepeatStride);
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
template <typename InsAlgTemplate>
HcclResult InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTempResource(
    const AlgResourceCtxSerializable& resCtx, const u32 channelLevelIdx,
    const std::shared_ptr<InsAlgTemplate>& algTemplate, TemplateResource& tempReousrce) const
{
    AlgResourceRequest req;
    algTemplate->GetRes(req);
    if (channelLevelIdx >= remoteRankToChannelInfo_.size()) {
        HCCL_ERROR(
            "[InsV2ReduceScatterSequenceExecutorAicpu][GenTempResource] channelLevelIdx[%u] should be lower "
            "than remoteRankToChannelInfo_.size()[%u]",
            channelLevelIdx, remoteRankToChannelInfo_.size());
        return HCCL_E_INTERNAL;
    }
    tempReousrce.channels = remoteRankToChannelInfo_[channelLevelIdx];
    tempReousrce.threads.assign(resCtx.threads.begin(), resCtx.threads.begin() + 1 + req.slaveThreadNum);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2ReduceScatterSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    TemplateDataParams tempAlgParamsIntra;
    tempAlgParamsIntra.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParamsIntra.buffInfo.outBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsIntra.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsIntra.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsIntra.buffInfo.outputPtr = resCtx.cclMem.addr;
    tempAlgParamsIntra.buffInfo.hcclBuff = resCtx.cclMem;

    std::shared_ptr<InsAlgTemplate0> algTemplateIntra =
        std::make_shared<InsAlgTemplate0>(param, myRank_, algHierarchyInfo_.infos[0]);
    algTemplateIntra->SetchannelsPerRank(remoteRankToChannelInfo_[0]);

    TemplateDataParams tempAlgParamsInter;
    tempAlgParamsInter.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParamsInter.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter.buffInfo.inputPtr = resCtx.cclMem.addr;
    tempAlgParamsInter.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsInter.buffInfo.hcclBuff = resCtx.cclMem;

    std::shared_ptr<InsAlgTemplate1> algTemplateInter =
        std::make_shared<InsAlgTemplate1>(param, myRank_, algHierarchyInfo_.infos[1]);

    u32 templateScratchMultiplierIntra =
        algTemplateIntra->CalcScratchMultiple(BufferType::INPUT, BufferType::HCCL_BUFFER);
    u32 templateScratchMultiplierInter =
        algTemplateInter->CalcScratchMultiple(BufferType::HCCL_BUFFER, BufferType::OUTPUT);
    u32 templateScratchMultiplier = templateScratchMultiplierIntra * templateScratchMultiplierInter;

    TemplateResource templateResourceIntra;
    CHK_RET(GenTempResource(resCtx, 0, algTemplateIntra, templateResourceIntra));
    TemplateResource templateResourceInter;
    CHK_RET(GenTempResource(resCtx, 1, algTemplateInter, templateResourceInter));

    u64 maxCountPerLoop = tempAlgParamsInter.buffInfo.hcclBuff.size / templateScratchMultiplier / HCCL_MIN_SLICE_ALIGN *
                          HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    u64 loopTimes = dataCount_ / maxCountPerLoop + static_cast<u64>(dataCount_ % maxCountPerLoop != 0);
    u64 processedDataCount = 0;
    for (u64 loop = 0; loop < loopTimes; loop++) {
        u64 currDataCount = (loop == loopTimes - 1) ? dataCount_ - processedDataCount : maxCountPerLoop;

        GenIntraTemplateParams(tempAlgParamsIntra, processedDataCount, currDataCount, loop);
        CHK_RET(algTemplateIntra->KernelRun(param, tempAlgParamsIntra, templateResourceIntra));

        GenInterTemplateParams(tempAlgParamsInter, processedDataCount, currDataCount, loop);
        CHK_RET(algTemplateInter->KernelRun(param, tempAlgParamsInter, templateResourceInter));
        processedDataCount += currDataCount;
    }
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, InsReduceScatterSequenceMesh1DNhr, InsV2ReduceScatterSequenceExecutorAicpu,
    TopoMatchMultilevel, InsTempReduceScatterMesh1DZAxisDetour, InsTempReduceScatterNHR);
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, InsReduceScatterSequenceMesh1DNhr, AlgEngine::AICPU, "sequence[mesh,nhr]",
    COND_NONE, FLAG_NONE, 0);

} // namespace mc2_ops_hccl
