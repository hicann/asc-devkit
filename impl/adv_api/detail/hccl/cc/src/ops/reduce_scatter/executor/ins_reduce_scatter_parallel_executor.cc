/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_reduce_scatter_parallel_executor.h"
#include <cmath>
#include "ins_temp_reduce_scatter_mesh_1D.h"
#include "ins_temp_reduce_scatter_nhr.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

// 并行执行器中两个 template 算法所需的 notify 数量
constexpr u32 TEMPLATE_NOTIFY_NUM = 2;
// 并行执行器中两个 template 算法的主 thread 数量
constexpr u32 TEMPLATE_MAIN_THREAD_NUM = 2;
constexpr u64 PARALLEL_DATA_ALIGN_SIZE = 16 * 1024;

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InsReduceScatterParallelExecutor()
    : InsCollAlgBase()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::~InsReduceScatterParallelExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    // 构建template
    std::vector<std::vector<u32>> temp0HierarchyInfo;
    std::vector<std::vector<u32>> temp1HierarchyInfo;
    constexpr u32 minTopoLevels = 2;
    if (algHierarchyInfo.infos.empty() || algHierarchyInfo.infos.size() < minTopoLevels) {
        HCCL_ERROR("[%s] algHierarchyInfo.infos is invalid (empty or size < %u).", __func__, minTopoLevels);
        return HCCL_E_PARA;
    }
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        if (algHierarchyInfo.infos[0].size() < 2) {
            HCCL_ERROR(
                "[%s] algHierarchyInfo.infos[0] size[%zu] is less than 2.", __func__, algHierarchyInfo.infos[0].size());
            return HCCL_E_PARA;
        }
        temp0HierarchyInfo = {algHierarchyInfo.infos[0][0]};
        std::vector<u32> closRanks;
        u32 meshSize = algHierarchyInfo.infos[0][0].size();
        for (auto rank : algHierarchyInfo.infos[0][1]) {
            if (rank % meshSize == topoInfo->userRank % meshSize) {
                closRanks.push_back(rank);
            }
        }
        temp1HierarchyInfo = {closRanks};
    } else {
        temp0HierarchyInfo = algHierarchyInfo.infos[0];
        temp1HierarchyInfo = algHierarchyInfo.infos[1];
    }
    InsAlgTemplate0 intraTempAlg(param, topoInfo->userRank, temp0HierarchyInfo);
    InsAlgTemplate1 interTempAlg(param, topoInfo->userRank, temp1HierarchyInfo);

    // 调用计算资源的函数
    AlgResourceRequest intraTempRequest;
    AlgResourceRequest interTempRequest;
    CHK_RET(intraTempAlg.CalcRes(comm, param, topoInfo, intraTempRequest));
    CHK_RET(interTempAlg.CalcRes(comm, param, topoInfo, interTempRequest));
    // 申请一条控制thread作为主thread，该thread仅用于两个template之间同步
    resourceRequest.notifyNumOnMainThread = TEMPLATE_NOTIFY_NUM;
    // 由于主thread被单独作为控制thread，因此总的slaveThread需要额外加上两个template的主thread
    resourceRequest.slaveThreadNum =
        intraTempRequest.slaveThreadNum + interTempRequest.slaveThreadNum + TEMPLATE_MAIN_THREAD_NUM;
    // 第一个template的zhuthread需要的notify数量，+1是因为需要和控制thread做同步
    resourceRequest.notifyNumPerThread.emplace_back(intraTempRequest.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), intraTempRequest.notifyNumPerThread.begin(),
        intraTempRequest.notifyNumPerThread.end());
    // 这一条是interTemplate的主thread，需要+1是为了和控制thread进行同步
    resourceRequest.notifyNumPerThread.emplace_back(interTempRequest.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), interTempRequest.notifyNumPerThread.begin(),
        interTempRequest.notifyNumPerThread.end());
    CHK_PRT_RET(
        intraTempRequest.channels.empty(),
        HCCL_ERROR("[InsReduceScatterParallelExecutor][CalcRes] intra template channels is empty."), HCCL_E_INTERNAL);
    CHK_PRT_RET(
        interTempRequest.channels.empty(),
        HCCL_ERROR("[InsReduceScatterParallelExecutor][CalcRes] inter template channels is empty."), HCCL_E_INTERNAL);
    resourceRequest.channels.emplace_back(intraTempRequest.channels[0]);
    resourceRequest.channels.emplace_back(interTempRequest.channels[0]);

    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

// HOST 侧算法入口，将对应的 instruction 添加到指令队列中
// 传入的insQue为一条主流
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTemplateAlgParamsIntra0(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
    const u64 dataCountPerLoopAixs0, std::vector<u64>& scratchOffVec, TemplateDataParams& tempAlgParamsIntra0) const
{
    tempAlgParamsIntra0.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsIntra0.buffInfo.inputSize = param.inputSize;
    tempAlgParamsIntra0.buffInfo.outputPtr = resCtx.cclMem.addr;
    tempAlgParamsIntra0.buffInfo.outputSize = resCtx.cclMem.size;
    tempAlgParamsIntra0.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParamsIntra0.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParamsIntra0.buffInfo.outBuffType = BufferType::HCCL_BUFFER; // 第一步最后的数据存储在scratch buffer上
    tempAlgParamsIntra0.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsIntra0.buffInfo.inBuffBaseOff = dataOffset;
    tempAlgParamsIntra0.buffInfo.outBuffBaseOff =
        scratchOffVec[0] + rankIdxLevel0_ * dataCountPerLoopAixs0 * dataTypeSize_;
    tempAlgParamsIntra0.buffInfo.hcclBuffBaseOff = scratchOffVec[0];
    tempAlgParamsIntra0.sliceSize = dataCountPerLoopAixs0 * dataTypeSize_;
    tempAlgParamsIntra0.tailSize = tempAlgParamsIntra0.sliceSize;
    tempAlgParamsIntra0.count = dataCountPerLoopAixs0;

    tempAlgParamsIntra0.inputSliceStride = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_;
    tempAlgParamsIntra0.outputSliceStride = 0;
    tempAlgParamsIntra0.repeatNum = rankSizeLevel1_;
    tempAlgParamsIntra0.inputRepeatStride =
        (strideCount_ == 0) ? dataSize_ * rankSizeLevel0_ : strideCount_ * dataTypeSize_ * rankSizeLevel0_;
    tempAlgParamsIntra0.outputRepeatStride = dataCountPerLoopAixs0 * dataTypeSize_ * rankSizeLevel0_;
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTemplateAlgParamsInter0(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
    const u64 dataCountPerLoopAixs0, std::vector<u64>& scratchOffVec, TemplateDataParams& tempAlgParamsInter0) const
{
    tempAlgParamsInter0.buffInfo.inputPtr = resCtx.cclMem.addr;
    tempAlgParamsInter0.buffInfo.inputSize = resCtx.cclMem.size;
    tempAlgParamsInter0.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsInter0.buffInfo.outputSize = param.outputSize;
    tempAlgParamsInter0.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParamsInter0.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter0.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParamsInter0.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter0.buffInfo.inBuffBaseOff =
        scratchOffVec[0] + rankIdxLevel0_ * dataCountPerLoopAixs0 * dataTypeSize_;
    tempAlgParamsInter0.buffInfo.outBuffBaseOff = dataOffset;
    tempAlgParamsInter0.buffInfo.hcclBuffBaseOff = scratchOffVec[2];
    tempAlgParamsInter0.sliceSize = dataCountPerLoopAixs0 * dataTypeSize_;
    tempAlgParamsInter0.tailSize = tempAlgParamsInter0.sliceSize;
    tempAlgParamsInter0.count = dataCountPerLoopAixs0;

    tempAlgParamsInter0.inputSliceStride = dataCountPerLoopAixs0 * dataTypeSize_ * rankSizeLevel0_;
    tempAlgParamsInter0.outputSliceStride = 0;
    tempAlgParamsInter0.repeatNum = 1;
    tempAlgParamsInter0.inputRepeatStride = 0;
    tempAlgParamsInter0.outputRepeatStride = 0;
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTemplateAlgParamsInter1(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
    const u64 dataCountPerLoopAixs1, std::vector<u64>& scratchOffVec, TemplateDataParams& tempAlgParamsInter1) const
{
    tempAlgParamsInter1.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsInter1.buffInfo.inputSize = param.inputSize;
    tempAlgParamsInter1.buffInfo.outputPtr = resCtx.cclMem.addr;
    tempAlgParamsInter1.buffInfo.outputSize = resCtx.cclMem.size;
    tempAlgParamsInter1.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParamsInter1.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParamsInter1.buffInfo.outBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter1.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsInter1.buffInfo.inBuffBaseOff = dataOffset;
    tempAlgParamsInter1.buffInfo.outBuffBaseOff = scratchOffVec[3];
    tempAlgParamsInter1.buffInfo.hcclBuffBaseOff = scratchOffVec[3];
    tempAlgParamsInter1.sliceSize = dataCountPerLoopAixs1 * dataTypeSize_;
    tempAlgParamsInter1.tailSize = tempAlgParamsInter1.sliceSize;
    tempAlgParamsInter1.count = dataCountPerLoopAixs1;

    tempAlgParamsInter1.inputSliceStride =
        (strideCount_ == 0) ? dataSize_ * rankSizeLevel0_ : strideCount_ * dataTypeSize_ * rankSizeLevel0_;
    tempAlgParamsInter1.outputSliceStride = dataCountPerLoopAixs1 * dataTypeSize_;
    tempAlgParamsInter1.repeatNum = rankSizeLevel0_;
    tempAlgParamsInter1.inputRepeatStride = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_;
    tempAlgParamsInter1.outputRepeatStride = dataCountPerLoopAixs1 * dataTypeSize_ * rankSizeLevel1_;
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTemplateAlgParamsIntra1(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
    const u64 dataCountPerLoopAixs1, std::vector<u64>& scratchOffVec, TemplateDataParams& tempAlgParamsIntra1) const
{
    tempAlgParamsIntra1.buffInfo.inputPtr = resCtx.cclMem.addr;
    tempAlgParamsIntra1.buffInfo.inputSize = resCtx.cclMem.size;
    tempAlgParamsIntra1.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsIntra1.buffInfo.outputSize = param.outputSize;
    tempAlgParamsIntra1.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParamsIntra1.buffInfo.inBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsIntra1.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParamsIntra1.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsIntra1.buffInfo.inBuffBaseOff =
        scratchOffVec[3] + rankIdxLevel1_ * dataCountPerLoopAixs1 * dataTypeSize_;
    tempAlgParamsIntra1.buffInfo.outBuffBaseOff = dataOffset;
    tempAlgParamsIntra1.buffInfo.hcclBuffBaseOff = scratchOffVec[1];
    tempAlgParamsIntra1.sliceSize = dataCountPerLoopAixs1 * dataTypeSize_;
    tempAlgParamsIntra1.tailSize = tempAlgParamsIntra1.sliceSize;
    tempAlgParamsIntra1.count = dataCountPerLoopAixs1;

    tempAlgParamsIntra1.inputSliceStride = dataCountPerLoopAixs1 * dataTypeSize_ * rankSizeLevel1_;
    tempAlgParamsIntra1.outputSliceStride = 0;
    tempAlgParamsIntra1.repeatNum = 1;
    tempAlgParamsIntra1.inputRepeatStride = 0;
    tempAlgParamsIntra1.outputRepeatStride = 0;
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GetParallelDataSplit(
    std::vector<float>& splitDataSize) const
{
    double splitData = 0.5;
    splitDataSize.push_back(splitData);
    splitDataSize.push_back(splitData);
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::PrepareResForTemplate(
    const InsAlgTemplate0& tempAlgIntra, const InsAlgTemplate1& tempAlgInter)
{
    u64 intraThreadsNum = tempAlgIntra.GetThreadNum();
    u64 interThreadsNum = tempAlgInter.GetThreadNum();
    if (threads_.size() < intraThreadsNum + interThreadsNum + 1) {
        HCCL_ERROR(
            "[InsReduceScatterParallelExecutor][PrepareResForTemplate] threads size is %zu, but intraThreadsNum is "
            "%llu, "
            "interThreadsNum is %llu",
            threads_.size(), intraThreadsNum, interThreadsNum);
        return HCCL_E_PARA;
    }
    intraThreads_.assign(threads_.begin() + 1, threads_.begin() + 1 + intraThreadsNum);
    interThreads_.assign(threads_.begin() + 1 + intraThreadsNum, threads_.end());
    // 用于两个算法同步
    controlThread_ = threads_.at(0);
    templateMainThreads_.push_back(intraThreads_.at(0));
    templateMainThreads_.push_back(interThreads_.at(0));
    // 获取两个template各自的主thread上有多少notify
    AlgResourceRequest intraTempRequest;
    AlgResourceRequest interTempRequest;
    CHK_RET(tempAlgIntra.GetRes(intraTempRequest));
    CHK_RET(tempAlgInter.GetRes(interTempRequest));
    notifyIdxControlToTemplates_.push_back(intraTempRequest.notifyNumOnMainThread);
    notifyIdxControlToTemplates_.push_back(interTempRequest.notifyNumOnMainThread);
    notifyIdxTemplatesToControl_.push_back(0);
    notifyIdxTemplatesToControl_.push_back(1);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsReduceScatterParallelExecutor][Orchestrate] Orchestrate Start");

    // cclBuffer的大小
    maxTmpMemSize_ = resCtx.cclMem.size;
    // 给channels_和threads_赋值
    threads_ = resCtx.threads;
    if (param.engine != CommEngine::COMM_ENGINE_AIV) {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
        intraChannelMap_ = remoteRankToChannelInfo_[0];
        interChannelMap_ = remoteRankToChannelInfo_[1];
    }
    dataCount_ = param.DataDes.count;
    strideCount_ = param.DataDes.strideCount;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;

    std::vector<std::vector<u32>> temp0HierarchyInfo;
    std::vector<std::vector<u32>> temp1HierarchyInfo;
    constexpr u32 minTopoLevels = 2;
    if (resCtx.algHierarchyInfo.infos.empty() || resCtx.algHierarchyInfo.infos.size() < minTopoLevels) {
        HCCL_ERROR("[%s] algHierarchyInfo.infos is invalid (empty or size < %u).", __func__, minTopoLevels);
        return HCCL_E_PARA;
    }
    if (resCtx.topoInfo.level0Topo == Level0Shape::MESH_1D_CLOS && !resCtx.topoInfo.level0PcieMix) {
        if (resCtx.algHierarchyInfo.infos[0].size() < 2) {
            HCCL_ERROR(
                "[%s] algHierarchyInfo.infos[0] size[%zu] is less than 2.", __func__,
                resCtx.algHierarchyInfo.infos[0].size());
            return HCCL_E_PARA;
        }
        temp0HierarchyInfo = {resCtx.algHierarchyInfo.infos[0][0]};
        std::vector<u32> closRanks;
        u32 meshSize = resCtx.algHierarchyInfo.infos[0][0].size();
        for (auto rank : resCtx.algHierarchyInfo.infos[0][1]) {
            if (rank % meshSize == resCtx.topoInfo.userRank % meshSize) {
                closRanks.push_back(rank);
            }
        }
        temp1HierarchyInfo = {closRanks};
    } else {
        temp0HierarchyInfo = resCtx.algHierarchyInfo.infos[0];
        temp1HierarchyInfo = resCtx.algHierarchyInfo.infos[1];
    }

    rankSizeLevel0_ = GetRankSize(temp0HierarchyInfo);
    rankSizeLevel1_ = GetRankSize(temp1HierarchyInfo);
    myRank_ = resCtx.topoInfo.userRank;
    if (rankSizeLevel0_ == 0) {
        HCCL_ERROR("[%s] rankSizeLevel0_ is 0.", __func__);
        return HCCL_E_PARA;
    }
    rankIdxLevel0_ = myRank_ % rankSizeLevel0_;
    rankIdxLevel1_ = myRank_ / rankSizeLevel0_;
    // 实例化算法模板类
    // 构建template
    InsAlgTemplate0 intraTempAlg(param, resCtx.topoInfo.userRank, temp0HierarchyInfo);
    InsAlgTemplate1 interTempAlg(param, resCtx.topoInfo.userRank, temp1HierarchyInfo);
    if (param.engine == CommEngine::COMM_ENGINE_AICPU_TS) {
        CHK_RET(interTempAlg.SetchannelsPerRank(interChannelMap_));
    }
    // 将计算资源分配个每个算法
    CHK_RET(PrepareResForTemplate(intraTempAlg, interTempAlg));
    // 算法展开
    HcclResult ret = OrchestrateLoop(param, resCtx, intraTempAlg, interTempAlg);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsReduceScatterParallelExecutor][Orchestrate]errNo[0x%016llx] Reduce Scatter executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& tempAlgIntra,
    InsAlgTemplate1& tempAlgInter)
{
    HCCL_INFO("[InsReduceScatterParallelExecutor][OrchestrateLoop] Start");
    HCCL_INFO("[InsReduceScatterParallelExecutor] AlgTemplate inter server is [%s]", tempAlgIntra.Describe().c_str());
    HCCL_INFO("[InsReduceScatterParallelExecutor] AlgTemplate intra server is [%s]", tempAlgInter.Describe().c_str());
    std::vector<float> dataSplitSize;
    GetParallelDataSplit(dataSplitSize);
    u64 alignedSize = PARALLEL_DATA_ALIGN_SIZE;
    BufferType inBuffType = BufferType::INPUT;
    BufferType outBuffType = BufferType::OUTPUT;
    u32 intraScatchteMultipleStage0 = tempAlgIntra.CalcScratchMultiple(inBuffType, outBuffType);
    u32 interScatchteMultipleStage0 = tempAlgInter.CalcScratchMultiple(inBuffType, outBuffType);
    u32 intraScatchteMultipleStage1 = tempAlgIntra.CalcScratchMultiple(outBuffType, outBuffType);
    u32 interScatchteMultipleStage1 = tempAlgInter.CalcScratchMultiple(outBuffType, outBuffType);
    if (interScatchteMultipleStage0 == 0 || interScatchteMultipleStage1 == 0) {
        interScatchteMultipleStage0 = rankSizeLevel1_;
        interScatchteMultipleStage1 = rankSizeLevel1_;
    }
    u32 scratchMultipleIntra0 =
        static_cast<u32>(std::ceil(dataSplitSize[0] * intraScatchteMultipleStage0 * rankSizeLevel1_));
    u32 scratchMultipleIntra1 = static_cast<u32>(std::ceil(dataSplitSize[1] * intraScatchteMultipleStage1));
    u32 scratchMultipleInter1 =
        static_cast<u32>(std::ceil(dataSplitSize[1] * interScatchteMultipleStage0 * rankSizeLevel0_));
    u32 scratchMultipleInter0 = static_cast<u32>(std::ceil(dataSplitSize[0] * interScatchteMultipleStage1));
    u32 totalScratchMultiple =
        scratchMultipleIntra0 + scratchMultipleIntra1 + scratchMultipleInter0 + scratchMultipleInter1;
    u64 scratchMemBlockSize = maxTmpMemSize_;
    if (totalScratchMultiple > 0) {
        scratchMemBlockSize = (maxTmpMemSize_ / alignedSize / totalScratchMultiple) * alignedSize;
    }
    u64 intra0ScratchOffset = 0;
    u64 intra1ScratchOffset = intra0ScratchOffset + scratchMultipleIntra0 * scratchMemBlockSize;
    u64 inter0ScratchOffset = intra1ScratchOffset + scratchMultipleIntra1 * scratchMemBlockSize;
    u64 inter1ScratchOffset = inter0ScratchOffset + scratchMultipleInter0 * scratchMemBlockSize;
    std::vector<u64> scratchOffVec = {
        intra0ScratchOffset, intra1ScratchOffset, inter0ScratchOffset, inter1ScratchOffset};

    u64 maxCountPerLoop =
        std::min(static_cast<u64>(scratchMemBlockSize), static_cast<u64>(UB_MAX_DATA_SIZE)) / dataTypeSize_;
    CHK_PRT_RET(
        maxCountPerLoop == 0, HCCL_ERROR("[InsReduceScatterParallelExecutor][OrchestrateLoop] maxCountPerLoop is 0"),
        HcclResult::HCCL_E_INTERNAL);

    u64 alignSize = AICPU_ALIGN_SIZE;
    u32 loopTimes = dataCount_ / maxCountPerLoop + ((dataCount_ % maxCountPerLoop == 0) ? 0 : 1);

    TemplateDataParams tempAlgParamsIntra0;
    TemplateDataParams tempAlgParamsInter0;
    TemplateDataParams tempAlgParamsInter1;
    TemplateDataParams tempAlgParamsIntra1;

    TemplateResource templateAlgResIntra;
    TemplateResource templateAlgResInter;
    templateAlgResIntra.channels = intraChannelMap_;
    templateAlgResInter.channels = interChannelMap_;
    templateAlgResIntra.threads = intraThreads_;
    templateAlgResInter.threads = interThreads_;
    u64 processedCount = 0;
    u32 loopIndex = 0;
    while (processedCount < dataCount_) {
        u64 remainingCount = dataCount_ - processedCount;
        u32 remainingLoopTimes = (loopIndex < loopTimes) ? (loopTimes - loopIndex) : 1;
        u64 currCount = (remainingCount + remainingLoopTimes - 1) / remainingLoopTimes;
        currCount = std::min(currCount, maxCountPerLoop);
        u64 dataCountPerLoopAixs0 = static_cast<u64>(dataSplitSize[0] * currCount);
        u64 dataCountPerLoopAixs1 = currCount - dataCountPerLoopAixs0;
        if (remainingLoopTimes > 1) {
            u64 alignedCountPart0 = dataCountPerLoopAixs0;
            u64 alignedCountPart1 = dataCountPerLoopAixs1;
            alignedCountPart0 = alignedCountPart0 * dataTypeSize_ / alignSize * alignSize / dataTypeSize_;
            alignedCountPart1 = alignedCountPart1 * dataTypeSize_ / alignSize * alignSize / dataTypeSize_;
            if (alignedCountPart0 + alignedCountPart1 > 0) {
                dataCountPerLoopAixs0 = alignedCountPart0;
                dataCountPerLoopAixs1 = alignedCountPart1;
            }
        }
        CHK_PRT_RET(
            dataCountPerLoopAixs0 + dataCountPerLoopAixs1 == 0,
            HCCL_ERROR("[InsReduceScatterParallelExecutor][OrchestrateLoop] currCount is 0"),
            HcclResult::HCCL_E_INTERNAL);

        u64 dataOffset0 = processedCount * dataTypeSize_;
        u64 dataOffset1 = dataOffset0 + dataCountPerLoopAixs0 * dataTypeSize_;

        // 第一步开始前同步
        CHK_RET(PreSyncInterThreads(controlThread_, templateMainThreads_, notifyIdxControlToTemplates_));
        // 数据0的server内的mesh算法
        GenTemplateAlgParamsIntra0(
            param, resCtx, dataOffset0, dataCountPerLoopAixs0, scratchOffVec, tempAlgParamsIntra0);
        // 把每个template需要的queue传进去，比如stars的mesh要传多条queue
        CHK_RET(tempAlgIntra.KernelRun(param, tempAlgParamsIntra0, templateAlgResIntra));
        // 数据1的server间的nhr算法
        GenTemplateAlgParamsInter1(
            param, resCtx, dataOffset1, dataCountPerLoopAixs1, scratchOffVec, tempAlgParamsInter1);
        CHK_RET(tempAlgInter.KernelRun(param, tempAlgParamsInter1, templateAlgResInter));
        // 第一步做完后回到主流做尾同步
        CHK_RET(PostSyncInterThreads(controlThread_, templateMainThreads_, notifyIdxTemplatesToControl_));

        // 第二步开始前同步
        CHK_RET(PreSyncInterThreads(controlThread_, templateMainThreads_, notifyIdxControlToTemplates_));
        // 数据0的server间的nhr算法
        GenTemplateAlgParamsInter0(
            param, resCtx, dataOffset0, dataCountPerLoopAixs0, scratchOffVec, tempAlgParamsInter0);
        CHK_RET(tempAlgInter.KernelRun(param, tempAlgParamsInter0, templateAlgResInter));
        // 数据1的server内的mesh算法
        GenTemplateAlgParamsIntra1(
            param, resCtx, dataOffset1, dataCountPerLoopAixs1, scratchOffVec, tempAlgParamsIntra1);
        CHK_RET(tempAlgIntra.KernelRun(param, tempAlgParamsIntra1, templateAlgResIntra));
        // 尾同步
        CHK_RET(PostSyncInterThreads(controlThread_, templateMainThreads_, notifyIdxTemplatesToControl_));

        processedCount += dataCountPerLoopAixs0 + dataCountPerLoopAixs1;
        loopIndex++;
    }

    HCCL_INFO("[InsReduceScatterParallelExecutor][OrchestrateLoop] End.");
    return HcclResult::HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
uint64_t InsReduceScatterParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GetRankSize(
    const std::vector<std::vector<u32>>& subCommRanks) const
{
    uint64_t count = 1;
    for (const auto& i : subCommRanks) {
        count *= i.size();
    }
    return count;
}

// 算法注册
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, InsReduceScatterParallelMesh1DNHR, InsReduceScatterParallelExecutor,
    TopoMatchMultilevel, InsTempReduceScatterMesh1D, InsTempReduceScatterNHR);
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, AicpuReduceScatterParallelMeshNHRUBX, InsReduceScatterParallelExecutor,
    TopoMatchUBX, InsTempReduceScatterMesh1D, InsTempReduceScatterNHR);
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, InsReduceScatterParallelMesh1DNHRPcie, InsReduceScatterParallelExecutor,
    TopoMatchPcieMix, InsTempReduceScatterMesh1D, InsTempReduceScatterNHR);
} // namespace mc2_ops_hccl
