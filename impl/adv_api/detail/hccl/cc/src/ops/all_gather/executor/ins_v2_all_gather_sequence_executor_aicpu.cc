/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_v2_all_gather_sequence_executor_aicpu.h"
#include "ins_temp_all_gather_mesh_1D_Z_axis_detour.h"
#include "ins_temp_all_gather_nhr.h"
#include "alg_data_trans_wrapper.h"
#include "coll_alg_v2_exec_registry.h"
#include "alg_meta_registry.h"
#include "external_alg_rules.h"

namespace mc2_ops_hccl {

constexpr u32 SEQUENCE_EXECUTOR_LEVEL_NUM = 2;

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    (void)comm;
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    algHierarchyInfo_ = algHierarchyInfo;

    HCCL_INFO(
        "[InsV2AllGatherSequenceExecutorAicpu][InitCommInfo] myRank[%u], rankSize[%u], dataType[%u], dataTypeSize[%u]",
        myRank_, rankSize_, dataType_, dataTypeSize_);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;

    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    CHK_RET(InitCommInfo(comm, param, topoInfo, algHierarchyInfo));
    if (algHierarchyInfo.infos.size() != SEQUENCE_EXECUTOR_LEVEL_NUM || algHierarchyInfo.infos[0].empty() ||
        algHierarchyInfo.infos[0][0].empty() || algHierarchyInfo.infos[1].empty() ||
        algHierarchyInfo.infos[1][0].empty()) {
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu] invalid algHierarchyInfo, requires 2-level topology but "
            "topoLevelNums[%u], infos size[%zu]",
            topoInfo->topoLevelNums, algHierarchyInfo.infos.size());
        return HCCL_E_INTERNAL;
    }
    InsAlgTemplate1 interTempAlg(param, myRank_, algHierarchyInfo.infos[1]);
    InsAlgTemplate0 intraTempAlg(param, myRank_, algHierarchyInfo.infos[0]);

    AlgResourceRequest resReqIntra;
    CHK_RET(intraTempAlg.CalcRes(comm, param, topoInfo, resReqIntra));
    AlgResourceRequest resReqInter;
    CHK_RET(interTempAlg.CalcRes(comm, param, topoInfo, resReqInter));

    resourceRequest.slaveThreadNum = std::max(resReqIntra.slaveThreadNum, resReqInter.slaveThreadNum);
    resourceRequest.notifyNumPerThread.clear();
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    for (u32 i = 0; i < resourceRequest.slaveThreadNum; ++i) {
        if (i < resReqIntra.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqIntra.notifyNumPerThread[i]);
        }
        if (i < resReqInter.notifyNumPerThread.size()) {
            resourceRequest.notifyNumPerThread[i] =
                std::max(resourceRequest.notifyNumPerThread[i], resReqInter.notifyNumPerThread[i]);
        }
    }
    resourceRequest.notifyNumOnMainThread =
        std::max(resReqIntra.notifyNumOnMainThread, resReqInter.notifyNumOnMainThread);

    if (resReqIntra.channels.empty() || resReqInter.channels.empty()) {
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu][CalcRes] template channels is empty, intra channel level "
            "size[%zu], inter channel level size[%zu]",
            resReqIntra.channels.size(), resReqInter.channels.size());
        return HCCL_E_INTERNAL;
    }
    resourceRequest.channels = {resReqIntra.channels[0], resReqInter.channels[0]};
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllGatherSequenceExecutorAicpu][Orchestrate] Orchestrate Start");
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    dataType_ = param.DataDes.dataType;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;
    if (algHierarchyInfo_.infos.size() < SEQUENCE_EXECUTOR_LEVEL_NUM || algHierarchyInfo_.infos[0].empty() ||
        algHierarchyInfo_.infos[1].empty() || algHierarchyInfo_.infos[0][0].empty() ||
        algHierarchyInfo_.infos[1][0].empty()) {
        HCCL_ERROR("[%s] invalid algHierarchyInfo infos.", __func__);
        return HCCL_E_PARA;
    }
    rankSizeLevel0_ = algHierarchyInfo_.infos[0][0].size();
    rankSizeLevel1_ = algHierarchyInfo_.infos[1][0].size();
    rankIdxLevel0_ = myRank_ % rankSizeLevel0_;
    rankIdxLevel1_ = myRank_ / rankSizeLevel0_;
    CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));

    InsAlgTemplate1 interTempAlg(param, myRank_, algHierarchyInfo_.infos[1]);
    InsAlgTemplate0 intraTempAlg(param, myRank_, algHierarchyInfo_.infos[0]);

    HcclResult ret = OrchestrateLoop(param, resCtx, intraTempAlg, interTempAlg);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu][Orchestrate]errNo[0x%016llx] Orchestrate failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    HCCL_INFO("[InsV2AllGatherSequenceExecutorAicpu][Orchestrate] Orchestrate End");
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenInterTemplateParams(
    TemplateDataParams& interTempDataParams, const u64 processedDataCount, const u64 currDataCount,
    const u64 loop) const
{
    interTempDataParams.count = currDataCount;
    interTempDataParams.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
    interTempDataParams.buffInfo.outBuffBaseOff = 0;
    interTempDataParams.buffInfo.hcclBuffBaseOff = 0;

    interTempDataParams.sliceSize = currDataCount * dataTypeSize_;
    interTempDataParams.tailSize = interTempDataParams.sliceSize;

    interTempDataParams.inputSliceStride = 0;
    interTempDataParams.outputSliceStride = 0;
    interTempDataParams.repeatNum = 1;
    interTempDataParams.inputRepeatStride = 0;
    interTempDataParams.outputRepeatStride = 0;

    HCCL_INFO(
        "[InsV2AllGatherSequenceExecutorAicpu] loop[%llu] interTempDataParams.inputSliceStride[%llu] "
        "interTempDataParams.outputSliceStride[%llu] interTempDataParams.sliceSize[%llu] "
        "interTempDataParams.buffInfo.inBuffBaseOff[%llu] interTempDataParams.buffInfo.outBuffBaseOff[%llu] "
        "interTempDataParams.repeatNum[%llu] interTempDataParams.inputRepeatStride[%llu] "
        "interTempDataParams.outputRepeatStride[%llu]",
        loop, interTempDataParams.inputSliceStride, interTempDataParams.outputSliceStride,
        interTempDataParams.sliceSize, interTempDataParams.buffInfo.inBuffBaseOff,
        interTempDataParams.buffInfo.outBuffBaseOff, interTempDataParams.repeatNum,
        interTempDataParams.inputRepeatStride, interTempDataParams.outputRepeatStride);
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenIntraTemplateParams(
    TemplateDataParams& intraTempDataParams, const u64 processedDataCount, const u64 currDataCount,
    const u64 loop) const
{
    intraTempDataParams.count = currDataCount;
    intraTempDataParams.buffInfo.inBuffBaseOff = 0;
    intraTempDataParams.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
    intraTempDataParams.buffInfo.hcclBuffBaseOff = 0;

    intraTempDataParams.sliceSize = currDataCount * dataTypeSize_;
    intraTempDataParams.tailSize = intraTempDataParams.sliceSize;
    intraTempDataParams.inputSliceStride = 0;
    intraTempDataParams.outputSliceStride = dataSize_;

    intraTempDataParams.repeatNum = rankSizeLevel1_;
    intraTempDataParams.inputRepeatStride = currDataCount * dataTypeSize_;
    intraTempDataParams.outputRepeatStride = dataSize_ * rankSizeLevel0_;

    HCCL_INFO(
        "[InsV2AllGatherSequenceExecutorAicpu] loop[%llu] intraTempDataParams.inputSliceStride[%llu] "
        "intraTempDataParams.outputSliceStride[%llu] intraTempDataParams.sliceSize[%llu] "
        "intraTempDataParams.buffInfo.inBuffBaseOff[%llu] intraTempDataParams.buffInfo.outBuffBaseOff[%llu] "
        "intraTempDataParams.repeatNum[%llu] intraTempDataParams.inputRepeatStride[%llu] "
        "intraTempDataParams.outputRepeatStride[%llu]",
        loop, intraTempDataParams.inputSliceStride, intraTempDataParams.outputSliceStride,
        intraTempDataParams.sliceSize, intraTempDataParams.buffInfo.inBuffBaseOff,
        intraTempDataParams.buffInfo.outBuffBaseOff, intraTempDataParams.repeatNum,
        intraTempDataParams.inputRepeatStride, intraTempDataParams.outputRepeatStride);
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
template <typename InsAlgTemplate>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTempResource(
    const AlgResourceCtxSerializable& resCtx, const u32 channelLevelIdx, const InsAlgTemplate& algTemplate,
    TemplateResource& tempReousrce) const
{
    AlgResourceRequest req;
    CHK_RET(algTemplate.GetRes(req));
    if (channelLevelIdx >= remoteRankToChannelInfo_.size()) {
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu][GenTempResource] channelLevelIdx[%u] should be lower"
            "than remoteRankToChannelInfo_.size()[%u]",
            channelLevelIdx, remoteRankToChannelInfo_.size());
        return HCCL_E_INTERNAL;
    }
    tempReousrce.channels = remoteRankToChannelInfo_[channelLevelIdx];
    if (resCtx.threads.size() < static_cast<size_t>(req.slaveThreadNum) + 1) {
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu][GenTempResource] threads size[%zu] is less than required "
            "slave thread num[%u] plus main thread",
            resCtx.threads.size(), req.slaveThreadNum);
        return HCCL_E_INTERNAL;
    }
    tempReousrce.threads.assign(resCtx.threads.begin(), resCtx.threads.begin() + 1 + req.slaveThreadNum);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& intraTempAlg,
    InsAlgTemplate1& interTempAlg)
{
    HCCL_INFO("[InsV2AllGatherSequenceExecutorAicpu][OrchestrateLoop] Start");
    if (remoteRankToChannelInfo_.size() < SEQUENCE_EXECUTOR_LEVEL_NUM) {
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutorAicpu][OrchestrateLoop] remoteRankToChannelInfo_ size[%zu] is less "
            "than required level num[%u]",
            remoteRankToChannelInfo_.size(), SEQUENCE_EXECUTOR_LEVEL_NUM);
        return HCCL_E_INTERNAL;
    }

    TemplateDataParams interTempDataParams;
    interTempDataParams.buffInfo.inputPtr = param.inputPtr;
    interTempDataParams.buffInfo.outputPtr = resCtx.cclMem.addr;
    interTempDataParams.buffInfo.outBuffType = BufferType::HCCL_BUFFER;
    interTempDataParams.buffInfo.hcclBuff = resCtx.cclMem;
    interTempDataParams.buffInfo.inBuffType = BufferType::INPUT;
    interTempDataParams.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;

    interTempAlg.SetchannelsPerRank(remoteRankToChannelInfo_[1]);

    TemplateDataParams intraTempDataParams;
    intraTempDataParams.buffInfo.inputPtr = resCtx.cclMem.addr;
    intraTempDataParams.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
    intraTempDataParams.buffInfo.outputPtr = param.outputPtr;
    intraTempDataParams.buffInfo.hcclBuff = resCtx.cclMem;
    intraTempDataParams.buffInfo.outBuffType = BufferType::OUTPUT;
    intraTempDataParams.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;

    intraTempAlg.SetchannelsPerRank(remoteRankToChannelInfo_[0]);

    u32 templateScratchMultiplier = interTempAlg.CalcScratchMultiple(BufferType::INPUT, BufferType::HCCL_BUFFER);

    TemplateResource templateResourceInter;
    TemplateResource templateResourceIntra;

    CHK_RET(GenTempResource(resCtx, 1, interTempAlg, templateResourceInter));
    CHK_RET(GenTempResource(resCtx, 0, intraTempAlg, templateResourceIntra));

    if (templateScratchMultiplier == 0) {
        HCCL_ERROR("[%s] templateScratchMultiplier is 0, division by zero.", __func__);
        return HCCL_E_INTERNAL;
    }
    u64 maxCountPerLoop = interTempDataParams.buffInfo.hcclBuff.size / templateScratchMultiplier /
                          HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    if (maxCountPerLoop == 0) {
        HCCL_ERROR(
            "[%s] maxCountPerLoop is 0, dataTypeSize[%llu], hcclBuffSize[%llu], templateScratchMultiplier[%u], "
            "division by zero.",
            __func__, dataTypeSize_, static_cast<unsigned long long>(interTempDataParams.buffInfo.hcclBuff.size),
            templateScratchMultiplier);
        return HCCL_E_INTERNAL;
    }
    u64 loopTimes = dataCount_ / maxCountPerLoop + static_cast<u64>(dataCount_ % maxCountPerLoop != 0);
    u64 processedDataCount = 0;

    for (u64 loop = 0; loop < loopTimes; loop++) {
        u64 currDataCount = (loop == loopTimes - 1) ? dataCount_ - processedDataCount : maxCountPerLoop;

        GenInterTemplateParams(interTempDataParams, processedDataCount, currDataCount, loop);
        CHK_RET(SplitData(currDataCount, rankSizeLevel1_, interTempDataParams));
        CHK_RET(interTempAlg.KernelRun(param, interTempDataParams, templateResourceInter));

        GenIntraTemplateParams(intraTempDataParams, processedDataCount, currDataCount, loop);
        CHK_RET(intraTempAlg.KernelRun(param, intraTempDataParams, templateResourceIntra));

        processedDataCount += currDataCount;
    }

    HCCL_INFO("[InsV2AllGatherSequenceExecutorAicpu][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutorAicpu<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::SplitData(
    const u64 dataCount, const u64 rankSize, TemplateDataParams& tempAlgParams)
{
    u32 sliceNum = rankSize;
    tempAlgParams.allRankSliceSize.clear();
    tempAlgParams.allRankDispls.clear();
    tempAlgParams.allRankProcessedDataCount.clear();
    tempAlgParams.allRankSliceSize.reserve(sliceNum);
    tempAlgParams.allRankDispls.reserve(sliceNum);
    tempAlgParams.allRankProcessedDataCount.reserve(sliceNum);

    u64 sliceSize = dataCount * dataTypeSize_;
    for (u32 i = 0; i < sliceNum; i++) {
        tempAlgParams.allRankDispls.emplace_back(i * sliceSize);
        tempAlgParams.allRankSliceSize.emplace_back(sliceSize);
        tempAlgParams.allRankProcessedDataCount.emplace_back(dataCount);
    }
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLGATHER, AicpuAllGatherSequenceMeshConcurNHR, InsV2AllGatherSequenceExecutorAicpu,
    TopoMatchMultilevel, InsTempAllGatherMesh1D1DZAxisDetour, InsTempAllGatherNHR);
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_ALLGATHER, AicpuAllGatherSequenceMeshConcurNHR, AlgEngine::AICPU, "sequence[mesh,nhr]",
    COND_NONE, FLAG_MULTI_LEVEL, 0);

} // namespace mc2_ops_hccl
