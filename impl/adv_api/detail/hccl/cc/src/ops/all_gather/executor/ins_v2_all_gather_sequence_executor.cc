/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_all_gather_sequence_executor.h"
#include "topo_match_multilevel.h"
#include "ins_temp_all_gather_mesh_1D.h"
#include "ins_temp_all_gather_nhr_dpu.h"
#include "coll_alg_v2_exec_registry.h"
#include "alg_meta_registry.h"
#include "external_alg_rules.h"

namespace mc2_ops_hccl {
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
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
        "[InsV2AllGatherSequenceExecutor][InitCommInfo] myRank[%u], rankSize[%u], dataType[%u], dataTypeSize[%u]",
        myRank_, rankSize_, dataType_, dataTypeSize_);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;

    // 使用topo match计算AlgHierarchyInfoForAllLevel
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    // 初始化一些基本成员变量
    InitCommInfo(comm, param, topoInfo, algHierarchyInfo);

    InsAlgTemplate0 intraTempAlg(param, myRank_, algHierarchyInfo.infos[0]);
    InsAlgTemplate1 interTempAlg(param, myRank_, algHierarchyInfo.infos[1]);

    AlgResourceRequest resReqIntra;
    intraTempAlg.CalcRes(comm, param, topoInfo, resReqIntra);
    AlgResourceRequest resReqInter;
    interTempAlg.CalcRes(comm, param, topoInfo, resReqInter);

    // 分级算法，slaveThread和对应notify可以复用
    resourceRequest.slaveThreadNum = std::max(resReqIntra.slaveThreadNum, resReqInter.slaveThreadNum);
    resourceRequest.notifyNumPerThread = resReqIntra.notifyNumPerThread; // dpu目前没有notify
    resourceRequest.notifyNumOnMainThread =
        std::max(resReqIntra.notifyNumOnMainThread, resReqInter.notifyNumOnMainThread);

    resourceRequest.channels = {resReqIntra.channels[0], resReqInter.channels[0]};
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllGatherSequenceExecutor][Orchestrate] Orchestrate Start");
    // 参数填充
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    strideCount_ = param.DataDes.strideCount;
    HCCL_DEBUG("[InsV2AllGatherSequenceExecutor][Orchestrate] strideCount[%lu]", strideCount_);
    dataType_ = param.DataDes.dataType;
    reduceOp_ = param.reduceType;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;
    rankSizeLevel0_ = algHierarchyInfo_.infos[0][0].size();
    rankSizeLevel1_ = algHierarchyInfo_.infos[1][0].size();
    CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));

    // 算法展开
    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2AllGatherSequenceExecutor][Orchestrate]errNo[0x%016llx] Orchestrate failed", HCCL_ERROR_CODE(ret)),
        ret);
    HCCL_INFO("[InsV2AllGatherSequenceExecutor][Orchestrate] Orchestrate End");
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherSequenceExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllGatherSequenceExecutor][OrchestrateLoop] Start");

    // 框间template
    TemplateDataParams interTempDataParams;
    interTempDataParams.buffInfo.inputPtr = param.inputPtr;
    interTempDataParams.buffInfo.outputPtr = param.outputPtr;
    interTempDataParams.buffInfo.hcclBuff = resCtx.cclMem;

    // 构建框间template
    InsAlgTemplate1 interTempAlg(param, myRank_, algHierarchyInfo_.infos[1]);

    // 框内template
    TemplateDataParams intraTempDataParams;
    intraTempDataParams.buffInfo.inputPtr = param.outputPtr;
    intraTempDataParams.buffInfo.outputPtr = param.outputPtr;
    intraTempDataParams.buffInfo.hcclBuff = resCtx.cclMem;

    // 构建框内template
    InsAlgTemplate0 intraTempAlg(param, myRank_, algHierarchyInfo_.infos[0]);

    u32 intraTemplateScratchMultiplier = intraTempAlg.CalcScratchMultiple(BufferType::OUTPUT, BufferType::OUTPUT);
    u32 interTemplateScratchMultiplier = interTempAlg.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT);
    u32 templateScratchMultiplier =
        std::max(interTemplateScratchMultiplier, intraTemplateScratchMultiplier * rankSizeLevel1_);

    // 构造框间template资源
    TemplateResource templateResourceInter;
    templateResourceInter.channels = remoteRankToChannelInfo_[1];
    templateResourceInter.threads = resCtx.threads;
    templateResourceInter.npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
    templateResourceInter.dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;
    // 构造框内template资源
    TemplateResource templateResourceIntra;
    templateResourceIntra.channels = remoteRankToChannelInfo_[0];
    templateResourceIntra.threads = resCtx.threads;
    templateResourceIntra.npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
    templateResourceIntra.dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;

    u64 maxCountPerLoop = interTempDataParams.buffInfo.hcclBuff.size / templateScratchMultiplier /
                          HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    // 计算loopTimes
    u64 loopTimes = dataCount_ / maxCountPerLoop + static_cast<u64>(dataCount_ % maxCountPerLoop != 0);
    u64 processedDataCount = 0;
    const u64 rankStrideBytes = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_;
    for (u64 loop = 0; loop < loopTimes; loop++) {
        u64 currDataCount = (loop == loopTimes - 1) ? dataCount_ - processedDataCount : maxCountPerLoop;

        // 框间的数据偏移和搬运计算
        interTempDataParams.count = currDataCount;
        interTempDataParams.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        u64 rankIdxInLevel0 = myRank_ % rankSizeLevel0_;
        interTempDataParams.buffInfo.outBuffBaseOff =
            rankIdxInLevel0 * rankStrideBytes + processedDataCount * dataTypeSize_;
        interTempDataParams.buffInfo.hcclBuffBaseOff = 0;

        interTempDataParams.sliceSize = currDataCount * dataTypeSize_;
        interTempDataParams.tailSize = interTempDataParams.sliceSize;
        // 这里的stride当成传统意义上的stride间隔
        interTempDataParams.inputSliceStride = 0;
        interTempDataParams.outputSliceStride = rankStrideBytes * rankSizeLevel0_;

        interTempDataParams.repeatNum = 1;
        interTempDataParams.inputRepeatStride = 0;
        interTempDataParams.outputRepeatStride = 0;

        HCCL_INFO(
            "[InsV2AllGatherSequenceExecutor] loop[%llu] interTempDataParams.inputSliceStride[%llu] "
            "interTempDataParams.outputSliceStride[%llu] interTempDataParams.sliceSize[%llu] "
            "interTempDataParams.buffInfo.inBuffBaseOff[%llu] interTempDataParams.buffInfo.outBuffBaseOff[%llu] "
            "interTempDataParams.repeatNum[%llu] interTempDataParams.inputRepeatStride[%llu] "
            "interTempDataParams.outputRepeatStride[%llu]",
            loop, interTempDataParams.inputSliceStride, interTempDataParams.outputSliceStride,
            interTempDataParams.sliceSize, interTempDataParams.buffInfo.inBuffBaseOff,
            interTempDataParams.buffInfo.outBuffBaseOff, interTempDataParams.repeatNum,
            interTempDataParams.inputRepeatStride, interTempDataParams.outputRepeatStride);

        CHK_RET(interTempAlg.KernelRun(param, interTempDataParams, templateResourceInter));

        // 框内的数据偏移和搬运量计算
        intraTempDataParams.count = currDataCount;
        intraTempDataParams.buffInfo.inBuffBaseOff = processedDataCount * dataTypeSize_;
        intraTempDataParams.buffInfo.outBuffBaseOff = processedDataCount * dataTypeSize_;
        intraTempDataParams.buffInfo.hcclBuffBaseOff = 0;

        intraTempDataParams.sliceSize = currDataCount * dataTypeSize_;
        intraTempDataParams.tailSize = intraTempDataParams.sliceSize;
        // 这里的stride当成传统意义上的stride间隔
        intraTempDataParams.inputSliceStride = rankStrideBytes;
        intraTempDataParams.outputSliceStride = rankStrideBytes;

        intraTempDataParams.repeatNum = rankSizeLevel1_;
        intraTempDataParams.inputRepeatStride = rankStrideBytes * rankSizeLevel0_;
        intraTempDataParams.outputRepeatStride = rankStrideBytes * rankSizeLevel0_;

        HCCL_INFO(
            "[InsV2AllGatherSequenceExecutor] loop[%llu] intraTempDataParams.inputSliceStride[%llu] "
            "intraTempDataParams.outputSliceStride[%llu] intraTempDataParams.sliceSize[%llu] "
            "intraTempDataParams.buffInfo.inBuffBaseOff[%llu] intraTempDataParams.buffInfo.outBuffBaseOff[%llu] "
            "intraTempDataParams.repeatNum[%llu] intraTempDataParams.inputRepeatStride[%llu] "
            "intraTempDataParams.outputRepeatStride[%llu]",
            loop, intraTempDataParams.inputSliceStride, intraTempDataParams.outputSliceStride,
            intraTempDataParams.sliceSize, intraTempDataParams.buffInfo.inBuffBaseOff,
            intraTempDataParams.buffInfo.outBuffBaseOff, intraTempDataParams.repeatNum,
            intraTempDataParams.inputRepeatStride, intraTempDataParams.outputRepeatStride);

        CHK_RET(intraTempAlg.KernelRun(param, intraTempDataParams, templateResourceIntra));

        processedDataCount += currDataCount;
    }
    HCCL_INFO("[InsV2AllGatherSequenceExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLGATHER, InsAllGatherMeshNhrDPU, InsV2AllGatherSequenceExecutor, TopoMatchMultilevel,
    InsTempAllGatherMesh1D, InsTempAllGatherNHRDPU);
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_ALLGATHER, InsAllGatherMeshNhrDPU, AlgEngine::DPU, "sequence[mesh,nhr]", COND_NONE, FLAG_NONE,
    0);
} // namespace mc2_ops_hccl
