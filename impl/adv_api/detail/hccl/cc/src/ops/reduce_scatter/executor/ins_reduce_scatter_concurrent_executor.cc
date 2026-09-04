/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cmath>
#include "alg_data_trans_wrapper.h"
#include "ins_reduce_scatter_concurrent_executor.h"
#include "ins_temp_reduce_scatter_nhr.h"
#include "ins_temp_reduce_scatter_mesh_1D.h"
#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
#include "ccu_temp_reduce_scatter_mesh_1D_mem2mem.h"
#endif
namespace mc2_ops_hccl {
namespace {
constexpr u32 MESH_BW_AICPU = 21;
constexpr u32 CLOS_BW_AICPU = 20;
} // namespace

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InsReduceScatterConcurrentExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    // 初始化一些基本成员变量
    InitCommInfo(param, topoInfo, algHierarchyInfo);
    constexpr u32 minMembers = 2;

    // 拆一下algHierarchyInfo
    if (algHierarchyInfo.infos.size() == 0 || algHierarchyInfo.infos[0].size() < minMembers) {
        HCCL_ERROR(
            "[InsReduceScatterConcurrentExecutor] algHierarchyInfo has no members, Please check the algHierarchyInfo!");
        return HCCL_E_PARA;
    }
    std::vector<std::vector<u32>> temp0HierarchyInfo = {algHierarchyInfo.infos[0][0]};
    std::vector<std::vector<u32>> temp1HierarchyInfo = {algHierarchyInfo.infos[0][1]};

    std::shared_ptr<InsAlgTemplate0> tempAlg0 = std::make_shared<InsAlgTemplate0>(param, myRank_, temp0HierarchyInfo);

    std::shared_ptr<InsAlgTemplate1> tempAlg1 = std::make_shared<InsAlgTemplate1>(param, myRank_, temp1HierarchyInfo);

    AlgResourceRequest temp0ResReq;
    AlgResourceRequest temp1ResReq;

    CHK_RET(tempAlg0->CalcRes(comm, param, topoInfo, temp0ResReq));
    CHK_RET(tempAlg1->CalcRes(comm, param, topoInfo, temp1ResReq));

    // 合并两个resourceRequest
    resourceRequest.slaveThreadNum =
        temp0ResReq.slaveThreadNum + temp1ResReq.slaveThreadNum + 1; // 将mesh作为主流，其他都是从流
    resourceRequest.notifyNumOnMainThread = temp0ResReq.notifyNumOnMainThread + 1;
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), temp0ResReq.notifyNumPerThread.begin(),
        temp0ResReq.notifyNumPerThread.end());
    resourceRequest.notifyNumPerThread.emplace_back(
        temp1ResReq.notifyNumOnMainThread + 1); // 把nhr里面需要的notifyNum数取出并+1
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), temp1ResReq.notifyNumPerThread.begin(),
        temp1ResReq.notifyNumPerThread.end());
    // 分别获取两种拓扑的链路，这里约束temp0为mesh拓扑，走mesh算法；temp1为clos拓扑，走nhr算法
    std::vector<HcclChannelDesc> channelDescs0;
    std::vector<HcclChannelDesc> channelDescsTemp0;
    CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
        comm, param, topoInfo, temp0HierarchyInfo, channelDescsTemp0, CommTopo::COMM_TOPO_1DMESH));
    for (auto channel : channelDescsTemp0) {
        if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
            channelDescs0.push_back(channel);
        }
    }

    CHK_PRT_RET(
        channelDescs0.empty(), HCCL_ERROR("[%s] channelDescs0.size()[%zu] is zero.", __func__, channelDescs0.size()),
        HcclResult::HCCL_E_INTERNAL);

    std::vector<HcclChannelDesc> channelDescs1;
    std::vector<HcclChannelDesc> channelDescsTemp1;

    CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, temp1HierarchyInfo, channelDescsTemp1));

    for (auto channel : channelDescsTemp1) {
        if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
            channelDescs1.push_back(channel);
        }
    }

    CHK_PRT_RET(
        channelDescs1.empty(), HCCL_ERROR("[%s] channelDescs1.size()[%zu] is zero.", __func__, channelDescs1.size()),
        HcclResult::HCCL_E_INTERNAL);

    // 两者数量应相等
    CHK_PRT_RET(
        channelDescs0.size() != channelDescs1.size(),
        HCCL_ERROR(
            "[%s] channelDescs0.size()[%zu] is not equal to channelDescs1.size()[%zu]", __func__, channelDescs0.size(),
            channelDescs1.size()),
        HcclResult::HCCL_E_INTERNAL);

    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        resourceRequest.ccuKernelNum.insert(
            resourceRequest.ccuKernelNum.end(), temp0ResReq.ccuKernelNum.begin(), temp0ResReq.ccuKernelNum.end());
        resourceRequest.ccuKernelNum.insert(
            resourceRequest.ccuKernelNum.end(), temp1ResReq.ccuKernelNum.begin(), temp1ResReq.ccuKernelNum.end());
        // 将两个合并
        resourceRequest.ccuKernelInfos.insert(
            resourceRequest.ccuKernelInfos.end(), temp0ResReq.ccuKernelInfos.begin(), temp0ResReq.ccuKernelInfos.end());
        resourceRequest.ccuKernelInfos.insert(
            resourceRequest.ccuKernelInfos.end(), temp1ResReq.ccuKernelInfos.begin(), temp1ResReq.ccuKernelInfos.end());
    } else if (param.engine == CommEngine::COMM_ENGINE_AICPU || param.engine == CommEngine::COMM_ENGINE_AICPU_TS) {
        resourceRequest.channels.resize(1);
        resourceRequest.channels[0].insert(
            resourceRequest.channels[0].end(), channelDescs0.begin(), channelDescs0.end());
        resourceRequest.channels[0].insert(
            resourceRequest.channels[0].end(), channelDescs1.begin(), channelDescs1.end());
    } else {
        HCCL_ERROR("[InsReduceScatterConcurrentExecutor][CalcRes] the communication engine is not supported currently"
                   ", please check");
        return HCCL_E_PARA;
    }
    HCCL_INFO("[InsReduceScatterConcurrentExecutor::CalRes] CalRes success!");
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsReduceScatterConcurrentExecutor][Orchestrate] Orchestrate Start");
    // 参数填充
    CHK_RET(InitExectorInfo(param, resCtx));
    HcclResult ret = OrchestrateLoop(param, resCtx); // 算法展开
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsReduceScatterConcurrentExecutor][Orchestrate]errNo[0x%016llx] Reduce scatter executor kernel run "
            "failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    constexpr u32 minMembers = 2;
    if (algHierarchyInfo_.infos.empty() || algHierarchyInfo_.infos[0].size() < minMembers) {
        HCCL_ERROR("[%s] algHierarchyInfo_.infos[0] is invalid (empty or size < %u).", __func__, minMembers);
        return HCCL_E_PARA;
    }
    HCCL_INFO("[InsReduceScatterConcurrentExecutor][OrchestrateLoop] Start");
    std::vector<std::vector<u32>> temp0HierarchyInfo{algHierarchyInfo_.infos[0][0]};
    std::vector<std::vector<u32>> temp1HierarchyInfo{algHierarchyInfo_.infos[0][1]};
    std::shared_ptr<InsAlgTemplate0> tempAlg0 =
        std::make_shared<InsAlgTemplate0>(param, myRank_, temp0HierarchyInfo); // same as calres
    std::shared_ptr<InsAlgTemplate1> tempAlg1 = std::make_shared<InsAlgTemplate1>(param, myRank_, temp1HierarchyInfo);
    // 准备资源
    // mesh的流向nhr的流发一个信号，并等nhr流收到
    PrepareThreadFromTemplate(tempAlg0, tempAlg1); // 计算不同的流
    TemplateResource templateAlgResforTemp0;
    templateAlgResforTemp0.threads = temp0Threads_; // 这里用重新算出的thream计算
    TemplateResource templateAlgResforTemp1;
    templateAlgResforTemp1.threads = temp1Threads_;
    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        // CCU模式处理逻辑
        templateAlgResforTemp0.ccuKernels.push_back(resCtx.ccuKernels[0]);
        templateAlgResforTemp1.ccuKernels.push_back(resCtx.ccuKernels[1]);
    } else if (param.engine == CommEngine::COMM_ENGINE_AIV) {
        // AIV模式
        templateAlgResforTemp0.aivCommInfoPtr = resCtx.aivCommInfoPtr;
        templateAlgResforTemp1.aivCommInfoPtr = resCtx.aivCommInfoPtr;
    } else if (param.engine == CommEngine::COMM_ENGINE_AICPU || param.engine == CommEngine::COMM_ENGINE_AICPU_TS) {
        // AICPU模式 从channel中取出分给两个template的channel
        const auto& channels = resCtx.channels[0];
        const size_t channelCount = channels.size();
        for (u32 i = 0; i < channelCount; ++i) {
            const auto& channel = channels[i];
            auto& targetChannels =
                (i < channelCount / 2) ? templateAlgResforTemp0.channels : templateAlgResforTemp1.channels;
            targetChannels[channel.remoteRank].push_back(channel);
        }
        CHK_RET(tempAlg0->SetchannelsPerRank(templateAlgResforTemp0.channels));
        CHK_RET(tempAlg1->SetchannelsPerRank(templateAlgResforTemp1.channels));
    }
    // 准备数据
    TemplateDataParams tempAlgParamsforTemp0;
    tempAlgParamsforTemp0.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsforTemp0.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsforTemp0.buffInfo.inputSize = param.inputSize;
    tempAlgParamsforTemp0.buffInfo.outputSize = param.outputSize;
    tempAlgParamsforTemp0.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParamsforTemp0.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParamsforTemp0.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsforTemp0.repeatNum = 1; // 不重复

    TemplateDataParams tempAlgParamsforTemp1;
    tempAlgParamsforTemp1.buffInfo.inputPtr = param.inputPtr;
    tempAlgParamsforTemp1.buffInfo.outputPtr = param.outputPtr;
    tempAlgParamsforTemp1.buffInfo.inputSize = param.inputSize;
    tempAlgParamsforTemp1.buffInfo.outputSize = param.outputSize;
    tempAlgParamsforTemp1.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParamsforTemp1.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParamsforTemp1.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
    tempAlgParamsforTemp1.repeatNum = 1; // 不重复

    u32 templateScratchMultiplier0 = tempAlg0->CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT);
    u32 templateScratchMultiplier1 = tempAlg1->CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT);
    u64 portNum0 = rankSize_ - 1;
    u64 portNum = 4;
    if (param.opExecuteConfig == OpExecuteConfig::AICPU_TS) {
        portNum0 = MESH_BW_AICPU;
        portNum = CLOS_BW_AICPU;
    }
    const u64 sliceAlignCount = HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    // 划分cclbuffer
    void* cclMemAddr = resCtx.cclMem.addr;
    const u64 cclMemSize = resCtx.cclMem.size;
    const auto cclMemType = resCtx.cclMem.type;
    HcclMem cclMem0 = {cclMemType, cclMemAddr, cclMemSize};
    HcclMem cclMem1 = {cclMemType, cclMemAddr, cclMemSize};

    // ccl buffer 按数据比例和ScratchMultiple比例划分给两个template用
    if (templateScratchMultiplier0 > 0 || templateScratchMultiplier1 > 0) {
        const u64 bufferRatioTerm0 = portNum0 * templateScratchMultiplier0;
        const u64 bufferRatioTerm1 = portNum * templateScratchMultiplier1;
        const double bufferRatio0 = static_cast<double>(bufferRatioTerm0) / (bufferRatioTerm0 + bufferRatioTerm1);
        cclMem0.size = static_cast<u64>(cclMemSize * bufferRatio0);
        cclMem0.size = cclMem0.size / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
        cclMem1.addr = static_cast<void*>(static_cast<s8*>(cclMemAddr) + cclMem0.size);
        cclMem1.size = cclMemSize - cclMem0.size;
    }

    u64 maxCountPerLoopforTemp0 = static_cast<u64>(UB_MAX_DATA_SIZE) / dataTypeSize_;
    u64 maxCountPerLoopforTemp1 = static_cast<u64>(UB_MAX_DATA_SIZE) / dataTypeSize_;

    if (templateScratchMultiplier0 > 0) {
        u64 scratchMemBlockSizeforTemp0 =
            cclMem0.size / templateScratchMultiplier0 / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
        maxCountPerLoopforTemp0 =
            static_cast<u64>(std::min(scratchMemBlockSizeforTemp0, static_cast<u64>(UB_MAX_DATA_SIZE)) / dataTypeSize_);
    }
    if (templateScratchMultiplier1 > 0) {
        u64 scratchMemBlockSizeforTemp1 =
            cclMem1.size / templateScratchMultiplier1 / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
        maxCountPerLoopforTemp1 =
            static_cast<u64>(std::min(scratchMemBlockSizeforTemp1, static_cast<u64>(UB_MAX_DATA_SIZE)) / dataTypeSize_);
    }

    u64 dataCountforTemp0 = dataCount_ * portNum0 / (portNum0 + portNum) / sliceAlignCount * sliceAlignCount; // 128对齐
    u64 dataCountforTemp1 = dataCount_ - dataCountforTemp0;
    CHK_PRT_RET(
        maxCountPerLoopforTemp0 == 0 || maxCountPerLoopforTemp1 == 0,
        HCCL_ERROR(
            "[%s] maxCountPerLoopforTemp is 0, maxCount0[%llu], maxCount1[%llu], dataTypeSize_[%llu].", __func__,
            maxCountPerLoopforTemp0, maxCountPerLoopforTemp1, dataTypeSize_),
        HcclResult::HCCL_E_INTERNAL);
    u32 loopTimesforTemp0 = (dataCountforTemp0 + maxCountPerLoopforTemp0 - 1) / maxCountPerLoopforTemp0;
    u32 loopTimesforTemp1 = (dataCountforTemp1 + maxCountPerLoopforTemp1 - 1) / maxCountPerLoopforTemp1;

    HCCL_INFO(
        "[%s]portNum0[%u], portNum1[%u], dataCount[%llu], maxCountPerLoopforTemp0[%llu], "
        "maxCountPerLoopforTemp1[%llu], dataCountforTemp0[%llu], dataCountforTemp1[%llu]",
        __func__, portNum0, portNum, dataCount_, maxCountPerLoopforTemp0, maxCountPerLoopforTemp1, dataCountforTemp0,
        dataCountforTemp1);

    // 同步资源信息
    std::vector<ThreadHandle> subThreads;
    subThreads.emplace_back(temp1ThreadMain_);
    std::vector<u32> notifyIdxMainToSub = {static_cast<u32>(temp1Threads_.size() - 1)};
    CHK_RET(PreSyncInterThreads(temp0ThreadMain_, subThreads, notifyIdxMainToSub)); // 前同步
    // 再次填充buffer信息
    tempAlgParamsforTemp0.buffInfo.hcclBuff = cclMem0;
    tempAlgParamsforTemp0.buffInfo.hcclBuffBaseOff = 0;
    tempAlgParamsforTemp0.buffInfo.hcclBuffSize = cclMem0.size;
    tempAlgParamsforTemp0.buffInfo.inBuffBaseOff = 0;
    tempAlgParamsforTemp0.buffInfo.outBuffBaseOff = 0;
    tempAlgParamsforTemp0.inputRepeatStride = 0;
    tempAlgParamsforTemp0.outputRepeatStride = 0;
    tempAlgParamsforTemp1.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParamsforTemp1.buffInfo.hcclBuffBaseOff = cclMem0.size;
    tempAlgParamsforTemp1.buffInfo.hcclBuffSize = cclMem1.size;
    tempAlgParamsforTemp1.buffInfo.inBuffBaseOff = dataCountforTemp0 * dataTypeSize_;
    tempAlgParamsforTemp1.buffInfo.outBuffBaseOff = dataCountforTemp0 * dataTypeSize_;
    tempAlgParamsforTemp1.inputRepeatStride = 0;
    tempAlgParamsforTemp1.outputRepeatStride = 0;
    // 循环处理每个template
    for (u32 loopIndex = 0; loopIndex < loopTimesforTemp0 || loopIndex < loopTimesforTemp1; loopIndex++) {
        HCCL_INFO(
            "[%s]loopIndex[%u], loopTimesforTemp0[%u], loopTimesforTemp1[%u]", __func__, loopIndex, loopTimesforTemp0,
            loopTimesforTemp1);
        if (loopIndex < loopTimesforTemp0) {
            u64 currCount = (loopIndex == loopTimesforTemp0 - 1) ?
                                (dataCountforTemp0 - loopIndex * maxCountPerLoopforTemp0) :
                                maxCountPerLoopforTemp0;
            u64 dataOffsetforMesh = loopIndex * maxCountPerLoopforTemp0 * dataTypeSize_;
            GenTempAlgParams(dataOffsetforMesh, currCount, maxCountPerLoopforTemp0, tempAlgParamsforTemp0);
            tempAlgParamsforTemp0.outputSliceStride = 0;
            CHK_RET(tempAlg0->KernelRun(param, tempAlgParamsforTemp0, templateAlgResforTemp0));
        }

        if (loopIndex < loopTimesforTemp1) {
            u64 currCount = (loopIndex == loopTimesforTemp1 - 1) ?
                                (dataCountforTemp1 - loopIndex * maxCountPerLoopforTemp1) :
                                maxCountPerLoopforTemp1;
            u64 dataOffsetforNhr =
                dataCountforTemp0 * dataTypeSize_ + loopIndex * maxCountPerLoopforTemp1 * dataTypeSize_;
            GenTempAlgParams(dataOffsetforNhr, currCount, maxCountPerLoopforTemp1, tempAlgParamsforTemp1);
            tempAlgParamsforTemp1.outputSliceStride = 0;
            CHK_RET(tempAlg1->KernelRun(param, tempAlgParamsforTemp1, templateAlgResforTemp1));
        }
    }
    // postSync
    std::vector<u32> notifyIdxMSubToMain = {static_cast<u32>(temp0Threads_.size() - 1)};
    CHK_RET(PostSyncInterThreads(temp0ThreadMain_, subThreads, notifyIdxMSubToMain));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;       // 全局的
    rankSize_ = topoInfo->userRankSize; // 全局的
    devType_ = topoInfo->deviceType;
    reduceOp_ = param.reduceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count; // recvCount
    strideCount_ = param.DataDes.strideCount;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];

    algHierarchyInfo_ = algHierarchyInfo;
    HCCL_INFO(
        "[InsReduceScatterConcurrentExecutor][InitCommInfo] myRank [%u], rankSize [%u], devType [%u], redOp [%u], "
        "dataType [%u] dataTypeSize [%u]",
        myRank_, rankSize_, devType_, reduceOp_, dataType_, dataTypeSize_);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitExectorInfo(
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
    maxTmpMemSize_ = resCtx.cclMem.size;
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;

    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTempAlgParams(
    const u64 dataOffset, const u64 dataCountforTemp, const u64 maxCountPerLoop,
    TemplateDataParams& tempAlgParams) const
{
    (void)maxCountPerLoop;
    tempAlgParams.count = dataCountforTemp;
    tempAlgParams.buffInfo.inBuffBaseOff = dataOffset;
    tempAlgParams.buffInfo.outBuffBaseOff = dataOffset;
    tempAlgParams.sliceSize = dataCountforTemp * dataTypeSize_;
    tempAlgParams.tailSize = tempAlgParams.sliceSize;
    tempAlgParams.inputSliceStride = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_; // 输出长度
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult
InsReduceScatterConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::PrepareThreadFromTemplate(
    std::shared_ptr<InsAlgTemplate0>& tempAlg0, std::shared_ptr<InsAlgTemplate1>& tempAlg1)
{
    (void)tempAlg1;
    // 流的数量
    u64 meshThreadsNum = tempAlg0->GetThreadNum();                             // check流数
    temp0Threads_.assign(threads_.begin(), threads_.begin() + meshThreadsNum); // 从0开始前meshThreadNum是mesh的流
    temp1Threads_.assign(threads_.begin() + meshThreadsNum, threads_.end());   // 后面几个是nhr的流
    temp0ThreadMain_ = temp0Threads_.at(0);
    temp1ThreadMain_ = temp1Threads_.at(0);
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_REDUCE_SCATTER, AicpuReduceScatterConcurMeshNHR, InsReduceScatterConcurrentExecutor,
    TopoMatchUBX, InsTempReduceScatterMesh1D, InsTempReduceScatterNHR);
} // namespace mc2_ops_hccl
