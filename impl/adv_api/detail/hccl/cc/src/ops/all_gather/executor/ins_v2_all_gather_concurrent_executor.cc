/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_all_gather_concurrent_executor.h"
#include "math.h"
#include "alg_data_trans_wrapper.h"
#include "hccl_res.h"
#include "ccu_alg_template_base.h"

// AICPU template 头文件
#include "ins_temp_all_gather_mesh_1D.h"
#include "ins_temp_all_gather_nhr.h"
#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
#include "ccu_temp_kfc_all_gather_mesh_1D_mem2mem.h"
#include "ccu_temp_kfc_all_gather_nhr_1D_multi_jetty_mem2mem.h"
#endif

namespace mc2_ops_hccl {

constexpr u32 CLOS_BW = 10;
constexpr u32 MESH_BW = 11;
constexpr u32 CLOS_JETTY = 4;
constexpr u32 MESH_BW_AICPU = 37;
constexpr u32 CLOS_BW_AICPU = 25;

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InsV2AllGatherConcurrentExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::InitCommInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    myRank_ = topoInfo->userRank;
    rankSize_ = topoInfo->userRankSize;
    devType_ = topoInfo->deviceType;
    dataType_ = param.DataDes.dataType;
    dataCount_ = param.DataDes.count;
    dataTypeSize_ = SIZE_TABLE[param.DataDes.dataType];

    algHierarchyInfo_ = algHierarchyInfo;
    HCCL_INFO(
        "[InsV2AllGatherConcurrentExecutor][InitCommInfo] myRank [%u], rankSize [%u], devType [%u], "
        "dataType [%u] dataTypeSize [%u], dataCount_ [%u]",
        myRank_, rankSize_, devType_, dataType_, dataTypeSize_, dataCount_);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    // 使用topo match计算AlgHierarchyInfoForAllLevel
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    // 初始化一些基本成员变量
    CHK_RET(InitCommInfo(param, topoInfo, algHierarchyInfo));
    CHK_PRT_RET(
        algHierarchyInfo.infos.empty() || algHierarchyInfo.infos[0].size() < 2,
        HCCL_ERROR("[%s] algHierarchyInfo.infos[0] is invalid (empty or size < 2).", __func__), HCCL_E_PARA);
    // 拆分algHierarchyInfo
    std::vector<std::vector<u32>> temp0HierarchyInfo = {algHierarchyInfo.infos[0][0]};
    std::vector<std::vector<u32>> temp1HierarchyInfo = {algHierarchyInfo.infos[0][1]};
    // 构建template
    std::shared_ptr<InsAlgTemplate0> temp0Alg = std::make_shared<InsAlgTemplate0>(param, myRank_, temp0HierarchyInfo);
    std::shared_ptr<InsAlgTemplate1> temp1Alg = std::make_shared<InsAlgTemplate1>(param, myRank_, temp1HierarchyInfo);
    // 调用计算资源的函数
    AlgResourceRequest temp0ResReq;
    AlgResourceRequest temp1ResReq;
    CHK_RET(temp0Alg->CalcRes(comm, param, topoInfo, temp0ResReq));
    CHK_RET(temp1Alg->CalcRes(comm, param, topoInfo, temp1ResReq));

    // 两个模板并行，资源累加
    resourceRequest.slaveThreadNum = temp0ResReq.slaveThreadNum + temp1ResReq.slaveThreadNum + 1;
    resourceRequest.notifyNumOnMainThread = temp0ResReq.notifyNumOnMainThread + 1;
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), temp0ResReq.notifyNumPerThread.begin(),
        temp0ResReq.notifyNumPerThread.end()); // 每个从流所需Notify数量
    resourceRequest.notifyNumPerThread.emplace_back(temp1ResReq.notifyNumOnMainThread + 1); // 需要与主流通信
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), temp1ResReq.notifyNumPerThread.begin(),
        temp1ResReq.notifyNumPerThread.end());

    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        resourceRequest.ccuKernelNum.emplace_back(temp0ResReq.ccuKernelNum[0]);
        resourceRequest.ccuKernelNum.emplace_back(temp1ResReq.ccuKernelNum[0]);
        resourceRequest.ccuKernelInfos.insert(
            resourceRequest.ccuKernelInfos.end(), temp0ResReq.ccuKernelInfos.begin(), temp0ResReq.ccuKernelInfos.end());
        resourceRequest.ccuKernelInfos.insert(
            resourceRequest.ccuKernelInfos.end(), temp1ResReq.ccuKernelInfos.begin(), temp1ResReq.ccuKernelInfos.end());
    } else {
        std::vector<HcclChannelDesc> temp0Channels;
        std::vector<HcclChannelDesc> temp1Channels;
        CommTopo temp0PriorityTopo = COMM_TOPO_1DMESH;
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, temp0HierarchyInfo, temp0Channels, temp0PriorityTopo));
        CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, temp1HierarchyInfo, temp1Channels));
        CHK_PRT_RET(
            temp0Channels.size() != temp1Channels.size(),
            HCCL_ERROR(
                "[InsV2AllGatherConcurrentExecutor][CalcRes] temp0Channels.size()[%zu] is not equal to "
                "temp1Channels.size()[%zu]",
                temp0Channels.size(), temp1Channels.size()),
            HcclResult::HCCL_E_INTERNAL);
        resourceRequest.channels.resize(1);
        resourceRequest.channels[0].insert(
            resourceRequest.channels[0].end(), temp0Channels.begin(), temp0Channels.end());
        resourceRequest.channels[0].insert(
            resourceRequest.channels[0].end(), temp1Channels.begin(), temp1Channels.end());
    }
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GenTemplateAlgParams(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 dataCountPerLoop,
    const u64 scratchOffset, TemplateDataParams& tempAlgParams) const
{
    tempAlgParams.buffInfo.inputPtr = param.inputPtr;
    tempAlgParams.buffInfo.outputPtr = param.outputPtr;
    tempAlgParams.buffInfo.hcclBuff = resCtx.cclMem;
    tempAlgParams.buffInfo.inputSize = param.inputSize;
    tempAlgParams.buffInfo.outputSize = param.outputSize;
    tempAlgParams.buffInfo.inBuffType = BufferType::INPUT;
    tempAlgParams.buffInfo.outBuffType = BufferType::OUTPUT;
    tempAlgParams.count = dataCountPerLoop;
    tempAlgParams.sliceSize = dataCountPerLoop * dataTypeSize_;
    tempAlgParams.tailSize = dataCountPerLoop * dataTypeSize_;
    tempAlgParams.buffInfo.inBuffBaseOff = dataOffset;
    tempAlgParams.buffInfo.outBuffBaseOff = dataOffset;
    tempAlgParams.buffInfo.hcclBuffBaseOff = scratchOffset;

    tempAlgParams.inputSliceStride = 0;
    tempAlgParams.outputSliceStride = (strideCount_ == 0) ? dataSize_ : strideCount_ * dataTypeSize_;
    tempAlgParams.repeatNum = 1;
    tempAlgParams.inputRepeatStride = 0;
    tempAlgParams.outputRepeatStride = 0;

    HCCL_DEBUG(
        "[InsV2AllGatherConcurrentExecutor][GenTemplateAlgParams] rank[%d] inBuffBaseOff[%llu] "
        "outBuffBaseOff[%llu] hcclBuffBaseOff[%llu] sliceSize[%llu] inputSliceStride[%llu] outputSliceStride[%llu]",
        myRank_, tempAlgParams.buffInfo.inBuffBaseOff, tempAlgParams.buffInfo.outBuffBaseOff,
        tempAlgParams.buffInfo.hcclBuffBaseOff, tempAlgParams.sliceSize, tempAlgParams.inputSliceStride,
        tempAlgParams.outputSliceStride);
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
void InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::GetParallelDataSplit(
    const OpParam& param, std::vector<float>& splitDataSize) const
{
    u32 portNum0 = rankSize_ - 1; // mesh端口数为rank size - 1
    u32 portNum1 = CLOS_JETTY;
    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        portNum0 = MESH_BW;
        portNum1 = CLOS_BW;
    } else if (param.opExecuteConfig == OpExecuteConfig::AICPU_TS) {
        portNum0 = MESH_BW_AICPU;
        portNum1 = CLOS_BW_AICPU;
    }
    double splitData = static_cast<double>(portNum0) / (portNum0 + portNum1);
    splitDataSize.push_back(splitData);
    splitDataSize.push_back(1 - splitData);
    HCCL_INFO(
        "[InsV2AllGatherConcurrentExecutor][GenTemplate1AlgParams] portNum0[%u], portNum1[%u], splitData[%.4f], ",
        portNum0, portNum1, splitData);
    return;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::PrepareResForTemplate(
    InsAlgTemplate0& algTemplate0, InsAlgTemplate1& algTemplate1)
{
    AlgResourceRequest temp0Request;
    AlgResourceRequest temp1Request;
    CHK_RET(algTemplate0.GetRes(temp0Request)); // 算法0需要的资源
    CHK_RET(algTemplate1.GetRes(temp1Request)); // 算法1需要的资源

    auto tmp0ThreadsNum = temp0Request.slaveThreadNum + 1;
    auto tmp1ThreadsNum = temp1Request.slaveThreadNum + 1;
    auto tmp0NotifyOnMainThread = temp0Request.notifyNumOnMainThread;
    auto tmp1NotifyOnMainThread = temp1Request.notifyNumOnMainThread;

    CHK_PRT_RET(
        threads_.size() < tmp0ThreadsNum + tmp1ThreadsNum,
        HCCL_ERROR(
            "[%s] threads resource is not enough. threads.size=[%zu], tmp0ThreadsNum=[%u], tmp1ThreadsNum=[%u].",
            __func__, threads_.size(), tmp0ThreadsNum, tmp1ThreadsNum),
        HCCL_E_INTERNAL);
    tmp0Threads_.assign(threads_.begin(), threads_.begin() + tmp0ThreadsNum);
    tmp1Threads_.assign(threads_.begin() + tmp0ThreadsNum, threads_.end());
    // 用于两个算法同步
    mainThread_ = tmp0Threads_.at(0);
    templateMainThreads_.emplace_back(tmp1Threads_.at(0));
    syncNotifyOnTemplates_ = {tmp1NotifyOnMainThread}; // 算法1需要从流数
    syncNotifyOnMain_ = {tmp0NotifyOnMainThread};      // 算法0需要从流数

    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsV2AllGatherConcurrentExecutor][Orchestrate] Orchestrate Start");
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    threads_ = resCtx.threads;

    maxTmpMemSize_ = resCtx.cclMem.size;
    myRank_ = resCtx.topoInfo.userRank;
    rankSize_ = resCtx.topoInfo.userRankSize;
    dataCount_ = param.DataDes.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;
    strideCount_ = param.DataDes.strideCount;
    HCCL_DEBUG("[InsV2AllGatherConcurrentExecutor][Orchestrate] strideCount[%lu]", strideCount_);

    // 拆分algHierarchyInfo
    CHK_PRT_RET(
        algHierarchyInfo_.infos.empty() || algHierarchyInfo_.infos[0].size() < 2,
        HCCL_ERROR("[%s] algHierarchyInfo.infos[0] is invalid (empty or size < 2).", __func__), HCCL_E_PARA);
    std::vector<std::vector<u32>> temp0HierarchyInfo = {algHierarchyInfo_.infos[0][0]};
    std::vector<std::vector<u32>> temp1HierarchyInfo = {algHierarchyInfo_.infos[0][1]};

    // 构建template
    InsAlgTemplate0 algTemplate0(param, myRank_, temp0HierarchyInfo);
    InsAlgTemplate1 algTemplate1(param, myRank_, temp1HierarchyInfo);

    // 分配threads
    CHK_RET(PrepareResForTemplate(algTemplate0, algTemplate1));

    // 分配channels或者ccuKernels
    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        tmp0CcuKernels_.assign(resCtx.ccuKernels.begin(), resCtx.ccuKernels.begin() + resCtx.ccuKernelNum[0]);
        tmp1CcuKernels_.assign(
            resCtx.ccuKernels.begin() + resCtx.ccuKernelNum[0],
            resCtx.ccuKernels.begin() + resCtx.ccuKernelNum[0] + resCtx.ccuKernelNum[1]);
    } else {
        const u64 splitIndex = rankSize_ - 1; // 默认第一个算法为mesh，使用 rankSize_ - 1 个 link
        for (u64 i = 0; i < resCtx.channels[0].size(); ++i) {
            const auto& channel = resCtx.channels[0][i];
            auto& targetMap = (i < splitIndex) ? tmp0LinkMap_ : tmp1LinkMap_;
            targetMap[channel.remoteRank].push_back(channel);
        }
    }

    // 算法展开
    HcclResult ret = OrchestrateLoop(param, resCtx, algTemplate0, algTemplate1);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsV2AllGatherConcurrentExecutor][Orchestrate]errNo[0x%016llx] All gather executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllGatherConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& algTemplate0,
    InsAlgTemplate1& algTemplate1)
{
    HCCL_INFO("[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] Start");
    // 构造Mesh拓扑template资源
    TemplateResource templateAlgResforTemp0;
    templateAlgResforTemp0.threads = tmp0Threads_;
    // 构造Nhr拓扑template资源
    TemplateResource templateAlgResforTemp1;
    templateAlgResforTemp1.threads = tmp1Threads_;

    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        templateAlgResforTemp0.ccuKernels = tmp0CcuKernels_;
        templateAlgResforTemp1.ccuKernels = tmp1CcuKernels_;
    } else {
        templateAlgResforTemp0.channels = tmp0LinkMap_;
        templateAlgResforTemp1.channels = tmp1LinkMap_;
    }

    TemplateDataParams AlgParamsforTemp0;
    TemplateDataParams AlgParamsforTemp1;

    // 计算数据切分比例
    std::vector<float> dataSplitSize;
    GetParallelDataSplit(param, dataSplitSize);

    // 缓存切分
    u32 scratchMultiplierforTemp0 =
        algTemplate0.CalcScratchMultiple(AlgParamsforTemp0.buffInfo.inBuffType, AlgParamsforTemp0.buffInfo.outBuffType);
    u32 scratchMultiplierforTemp1 =
        algTemplate1.CalcScratchMultiple(AlgParamsforTemp1.buffInfo.inBuffType, AlgParamsforTemp1.buffInfo.outBuffType);
    u32 ScratchMultiplier0 = static_cast<u32>(std::ceil(dataSplitSize[0] * scratchMultiplierforTemp0));
    u32 ScratchMultiplier1 = static_cast<u32>(std::ceil(dataSplitSize[1] * scratchMultiplierforTemp1));
    u32 totalScratchMultiple = ScratchMultiplier0 + ScratchMultiplier1;
    u64 scratchMemBlockSize = maxTmpMemSize_;
    HCCL_INFO("[InsV2AllGatherConcurrentExecutor]maxTmpMemSize_ [%u]", maxTmpMemSize_);
    if (totalScratchMultiple > 0) {
        scratchMemBlockSize = (maxTmpMemSize_ / HCCL_MIN_SLICE_ALIGN / totalScratchMultiple) * HCCL_MIN_SLICE_ALIGN;
    }
    u64 scratchSizeforTemp0 = ScratchMultiplier0 * scratchMemBlockSize;
    u64 scratchSizeforTemp1 = maxTmpMemSize_ - scratchSizeforTemp0;
    u64 scratchOffsetforTemp0 = 0;
    u64 scratchOffsetforTemp1 = scratchSizeforTemp0;

    // 分别计算两个template的maxCountPerLoop
    const u64 maxCountUBLimit = UB_MAX_DATA_SIZE / dataTypeSize_;
    u64 maxCountPerLoopforTemp0 = maxCountUBLimit;
    u64 maxCountPerLoopforTemp1 = maxCountUBLimit;
    if (scratchMultiplierforTemp0 > 0) {
        maxCountPerLoopforTemp0 = std::min(
            maxCountUBLimit, scratchSizeforTemp0 / scratchMultiplierforTemp0 / HCCL_MIN_SLICE_ALIGN *
                                 HCCL_MIN_SLICE_ALIGN / dataTypeSize_);
    }
    if (scratchMultiplierforTemp1 > 0) {
        maxCountPerLoopforTemp1 = std::min(
            maxCountUBLimit, scratchSizeforTemp1 / scratchMultiplierforTemp1 / HCCL_MIN_SLICE_ALIGN *
                                 HCCL_MIN_SLICE_ALIGN / dataTypeSize_);
    }
    CHK_PRT_RET(
        maxCountPerLoopforTemp0 == 0,
        HCCL_ERROR("[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] maxDataCountPerLoop0 is 0"), HCCL_E_INTERNAL);
    CHK_PRT_RET(
        maxCountPerLoopforTemp1 == 0,
        HCCL_ERROR("[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] maxDataCountPerLoop1 is 0"), HCCL_E_INTERNAL);

    // 按比例切分数据，并计算loopTimes
    const u64 sliceAlignCount = HCCL_MIN_SLICE_ALIGN / dataTypeSize_;
    const u64 totalCount0 =
        static_cast<u64>(std::floor(dataSplitSize[0] * dataCount_)) / sliceAlignCount * sliceAlignCount;
    const u64 totalCount1 = dataCount_ - totalCount0;
    const u64 initOffsetforTemp1 = totalCount0 * dataTypeSize_;
    u64 loopTimesforTemp0 =
        totalCount0 / maxCountPerLoopforTemp0 + static_cast<u64>(totalCount0 % maxCountPerLoopforTemp0 != 0);
    u64 loopTimesforTemp1 =
        totalCount1 / maxCountPerLoopforTemp1 + static_cast<u64>(totalCount1 % maxCountPerLoopforTemp1 != 0);

    HCCL_INFO(
        "[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] maxCountPerLoopforTemp0[%llu], "
        "maxCountPerLoopforTemp1[%llu], "
        "transportBoundDataCount[%llu], totalScratchMultiple[%llu], loopTimesforTemp0[%llu], loopTimesforTemp1[%llu], "
        "totalCount0[%llu], totalCount1[%llu]",
        maxCountPerLoopforTemp0, maxCountPerLoopforTemp1, maxCountUBLimit, totalScratchMultiple, loopTimesforTemp0,
        loopTimesforTemp1, totalCount0, totalCount1);

    // 前同步
    CHK_RET(PreSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnTemplates_));

    // 交替下发两个template的任务
    for (u32 loopIndex = 0; loopIndex < loopTimesforTemp0 || loopIndex < loopTimesforTemp1; loopIndex++) {
        if (loopIndex < loopTimesforTemp0) {
            u64 currCountforTemp0 = (loopIndex == loopTimesforTemp0 - 1) ?
                                        (totalCount0 - loopIndex * maxCountPerLoopforTemp0) :
                                        maxCountPerLoopforTemp0;
            u64 dataOffsetforTemp0 = loopIndex * maxCountPerLoopforTemp0 * dataTypeSize_;
            GenTemplateAlgParams(
                param, resCtx, dataOffsetforTemp0, currCountforTemp0, scratchOffsetforTemp0, AlgParamsforTemp0);
            HCCL_INFO(
                "[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] loopIndex[%u], currCountforTemp0[%u], "
                "dataOffsetforTemp0[%u]",
                loopIndex, currCountforTemp0, dataOffsetforTemp0);
            CHK_RET(algTemplate0.KernelRun(param, AlgParamsforTemp0, templateAlgResforTemp0));
        }

        if (loopIndex < loopTimesforTemp1) {
            u64 currCountforTemp1 = (loopIndex == loopTimesforTemp1 - 1) ?
                                        (totalCount1 - loopIndex * maxCountPerLoopforTemp1) :
                                        maxCountPerLoopforTemp1;
            u64 dataOffsetforTemp1 = initOffsetforTemp1 + loopIndex * maxCountPerLoopforTemp1 * dataTypeSize_;
            GenTemplateAlgParams(
                param, resCtx, dataOffsetforTemp1, currCountforTemp1, scratchOffsetforTemp1, AlgParamsforTemp1);
            HCCL_INFO(
                "[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] loopIndex[%u], currCountforTemp1[%u], "
                "dataOffsetforTemp1[%u]",
                loopIndex, currCountforTemp1, dataOffsetforTemp1);
            CHK_RET(algTemplate1.KernelRun(param, AlgParamsforTemp1, templateAlgResforTemp1));
        }
    }

    // 尾同步
    CHK_RET(PostSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnMain_));
    HCCL_INFO("[InsV2AllGatherConcurrentExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

// 算法注册
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLGATHER, AicpuAllGatherConcurMeshNHR, InsV2AllGatherConcurrentExecutor, TopoMatchUBX,
    InsTempAllGatherMesh1D, InsTempAllGatherNHR);

#if !defined(AICPU_COMPILE) && MC2_CLIENT_ENABLE_CCU
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLGATHER, CcuSchedAllGatherConcurMeshNHRMultiLink, InsV2AllGatherConcurrentExecutor,
    TopoMatchUBX, CcuTempKfcAllGatherMesh1DMem2Mem, CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem);
#endif

} // namespace mc2_ops_hccl
