/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_v2_all_reduce_parallel_executor.h"
#include "ins_temp_reduce_scatter_mesh_1D.h"
#include "ins_temp_reduce_scatter_nhr.h"
#include "ins_temp_all_gather_mesh_1D.h"
#include "ins_temp_all_gather_nhr.h"
#include "topo_match_pcie_mix.h"
#include <cmath>

namespace mc2_ops_hccl {
template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
InsAllReduceParallelExecutor<
    AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::InsAllReduceParallelExecutor()
{}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    // 使用topo match计算AlgHierarchyInfoForAllLevel
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    constexpr u32 minTopoLevels = 2;
    if (algHierarchyInfo.infos.empty() || algHierarchyInfo.infos.size() < minTopoLevels) {
        HCCL_ERROR("[%s] algHierarchyInfo.infos is invalid (empty or size < %u).", __func__, minTopoLevels);
        return HcclResult::HCCL_E_PARA;
    }
    myRank_ = topoInfo->userRank;
    HCCL_INFO("[InsAllReduceParallelExecutor] CalcRes start, rank[%d]", myRank_);

    std::vector<std::vector<u32>> temp0HierarchyInfo;
    std::vector<std::vector<u32>> temp1HierarchyInfo;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        if (algHierarchyInfo.infos[0].size() < 2) {
            HCCL_ERROR(
                "[%s] algHierarchyInfo.infos[0] size[%zu] is less than 2.", __func__, algHierarchyInfo.infos[0].size());
            return HcclResult::HCCL_E_PARA;
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
    // 实例化算法模板类
    // 构建template
    std::shared_ptr<InsAlgTemplate0> algTemplate0 =
        std::make_shared<InsAlgTemplate0>(param, topoInfo->userRank, temp0HierarchyInfo); // RS 框内
    std::shared_ptr<InsAlgTemplate1> algTemplate1 =
        std::make_shared<InsAlgTemplate1>(param, topoInfo->userRank, temp1HierarchyInfo); // RS 框间
    std::shared_ptr<InsAlgTemplate2> algTemplate2 =
        std::make_shared<InsAlgTemplate2>(param, topoInfo->userRank, temp0HierarchyInfo); // AG 框内
    std::shared_ptr<InsAlgTemplate3> algTemplate3 =
        std::make_shared<InsAlgTemplate3>(param, topoInfo->userRank, temp1HierarchyInfo); // AG 框间

    // 计算资源
    AlgResourceRequest intraTempRequest;
    AlgResourceRequest interTempRequest;
    AlgResourceRequest intraTempRequest1;
    AlgResourceRequest interTempRequest1;
    AlgResourceRequest intraTempRequestFinal;
    AlgResourceRequest interTempRequestFinal;

    CHK_RET(algTemplate0->CalcRes(comm, param, topoInfo, intraTempRequest));
    CHK_RET(algTemplate1->CalcRes(comm, param, topoInfo, interTempRequest));
    CHK_RET(algTemplate2->CalcRes(comm, param, topoInfo, intraTempRequest1));
    CHK_RET(algTemplate3->CalcRes(comm, param, topoInfo, interTempRequest1));

    u32 slaveThreadNumIntra = intraTempRequest.slaveThreadNum;
    if (intraTempRequest.slaveThreadNum >= intraTempRequest1.slaveThreadNum) {
        intraTempRequestFinal.notifyNumPerThread = intraTempRequest.notifyNumPerThread;
    } else {
        slaveThreadNumIntra = intraTempRequest1.slaveThreadNum;
        intraTempRequestFinal.notifyNumPerThread = intraTempRequest1.notifyNumPerThread;
    }
    u32 slaveThreadNumInter = interTempRequest.slaveThreadNum;
    if (interTempRequest.slaveThreadNum >= interTempRequest1.slaveThreadNum) {
        interTempRequestFinal.notifyNumPerThread = interTempRequest.notifyNumPerThread;
    } else {
        slaveThreadNumInter = interTempRequest1.slaveThreadNum;
        interTempRequestFinal.notifyNumPerThread = interTempRequest1.notifyNumPerThread;
    }

    resourceRequest.notifyNumOnMainThread = 2; // allreduce用于两个template间同步
    resourceRequest.slaveThreadNum = slaveThreadNumIntra + slaveThreadNumInter + 4;
    resourceRequest.notifyNumPerThread.emplace_back(intraTempRequest.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.emplace_back(intraTempRequest1.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), intraTempRequestFinal.notifyNumPerThread.begin(),
        intraTempRequestFinal.notifyNumPerThread.end());
    resourceRequest.notifyNumPerThread.emplace_back(interTempRequest.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.emplace_back(interTempRequest1.notifyNumOnMainThread + 1);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), interTempRequestFinal.notifyNumPerThread.begin(),
        interTempRequestFinal.notifyNumPerThread.end());

    resourceRequest.channels.emplace_back(intraTempRequest.channels[0]);
    resourceRequest.channels.emplace_back(interTempRequest.channels[0]);

    HCCL_DEBUG(
        "[InsAllReduceParallelExecutor][CalcRes] myRank[%u], notifyNumOnMainThread[%u], slaveThreadNum[%u], "
        "channels[%u]",
        myRank_, resourceRequest.notifyNumOnMainThread, resourceRequest.slaveThreadNum,
        resourceRequest.channels.size());
    for (auto i = 0; i < resourceRequest.notifyNumPerThread.size(); i++) {
        HCCL_DEBUG(
            "[InsAllReduceParallelExecutor][CalcRes] myRank[%u], notifyNumPerThread[%u]=[%u]", myRank_, i,
            resourceRequest.notifyNumPerThread[i]);
    }

    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult InsAllReduceParallelExecutor<
    AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2,
    InsAlgTemplate3>::Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    constexpr u32 minTopoLevels = 2;
    if (resCtx.algHierarchyInfo.infos.empty() || resCtx.algHierarchyInfo.infos.size() < minTopoLevels) {
        HCCL_ERROR("[%s] algHierarchyInfo.infos is invalid (empty or size < %u).", __func__, minTopoLevels);
        return HcclResult::HCCL_E_PARA;
    }
    HCCL_INFO("[InsAllReduceParallelExecutor][Orchestrate] Orchestrate Start");

    maxTmpMemSize_ = resCtx.cclMem.size;
    myRank_ = resCtx.topoInfo.userRank;
    // 给channels_和threads_赋值
    threads_ = resCtx.threads;
    HCCL_INFO("[InsAllReduceParallelExecutor][Orchestrate] threads_size[%d]", threads_.size());
    if (param.engine != CommEngine::COMM_ENGINE_AIV && param.engine != CommEngine::COMM_ENGINE_CCU) {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
        intraLinks_ = remoteRankToChannelInfo_[0];
        interLinks_ = remoteRankToChannelInfo_[1];
    }
    dataCount_ = param.DataDes.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    dataSize_ = dataCount_ * dataTypeSize_;

    // 获取算法Topo信息
    vTopo_ = resCtx.algHierarchyInfo.infos; // 本通信域内的通信平面

    // 计算localRankSize
    if (resCtx.topoInfo.level0Topo == Level0Shape::MESH_1D_CLOS && !resCtx.topoInfo.level0PcieMix) {
        temp0HierarchyInfo_ = {resCtx.algHierarchyInfo.infos[0][0]};
        std::vector<u32> closRanks;
        u32 meshSize = resCtx.algHierarchyInfo.infos[0][0].size();
        for (auto rank : resCtx.algHierarchyInfo.infos[0][1]) {
            if (rank % meshSize == resCtx.topoInfo.userRank % meshSize) {
                closRanks.push_back(rank);
            }
        }
        temp1HierarchyInfo_ = {closRanks};
    } else {
        temp0HierarchyInfo_ = resCtx.algHierarchyInfo.infos[0];
        temp1HierarchyInfo_ = resCtx.algHierarchyInfo.infos[1];
    }

    intraLocalRankSize_ = GetRankSize(temp0HierarchyInfo_);
    interLocalRankSize_ = GetRankSize(temp1HierarchyInfo_);
    rankSize_ = intraLocalRankSize_ * interLocalRankSize_;
    HCCL_INFO(
        "[Orchestrate] localRankSize: myRank[%d] intraLocalRankSize[%u] interLocalRankSize[%u] rankSize_[%u]", myRank_,
        intraLocalRankSize_, interLocalRankSize_, rankSize_);

    // 实例化算法模板类
    InsAlgTemplate0 tempAlgIntra(param, resCtx.topoInfo.userRank, temp0HierarchyInfo_);
    InsAlgTemplate1 tempAlgInter(param, resCtx.topoInfo.userRank, temp1HierarchyInfo_);
    InsAlgTemplate2 tempAlgIntra1(param, resCtx.topoInfo.userRank, temp0HierarchyInfo_);
    InsAlgTemplate3 tempAlgInter1(param, resCtx.topoInfo.userRank, temp1HierarchyInfo_);
    if (param.engine == CommEngine::COMM_ENGINE_AICPU_TS) {
        tempAlgInter.SetchannelsPerRank(interLinks_);
        tempAlgInter1.SetchannelsPerRank(interLinks_);
    }

    // 算法展开
    HcclResult ret = GenInsQues(param, resCtx, tempAlgIntra, tempAlgInter, tempAlgIntra1, tempAlgInter1);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsAllReduceParallelExecutor][Orchestrate]errNo[0x%016llx] AllReduce executor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    GetParallelDataSplit(std::vector<float>& splitDataSize) const
{
    constexpr double ratio = 0.5;
    splitDataSize.push_back(ratio);
    splitDataSize.push_back(ratio);
    HCCL_INFO(
        "[InsAllReduceParallelExecutor] meshFirstRatio[%f], closFirstRatio[%f]", splitDataSize[0], splitDataSize[1]);
    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
uint64_t InsAllReduceParallelExecutor<
    AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2,
    InsAlgTemplate3>::GetRankSize(const std::vector<std::vector<u32>>& vTopo)
{
    uint64_t count = 1;
    for (const auto& i : vTopo) {
        count *= i.size();
    }
    return count;
}

// Aicpu
template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    PrepareResForTemplate(InsAlgTemplate0& tempAlgIntra, InsAlgTemplate1& tempAlgInter, InsAlgTemplate2& tempAlgIntra1)
{
    AlgResourceRequest intraTempRequest;
    AlgResourceRequest interTempRequest;
    AlgResourceRequest intraTempRequest1;
    tempAlgIntra.GetRes(intraTempRequest);
    tempAlgInter.GetRes(interTempRequest);
    tempAlgIntra1.GetRes(intraTempRequest1);
    auto intraThreadsNum = intraTempRequest.slaveThreadNum + 1;
    auto intraThreadsNum1 = intraTempRequest1.slaveThreadNum + 1;
    auto intraThreadsNumFinal = std::max(intraThreadsNum, intraThreadsNum1);
    auto intraNotifyOnMainThread = intraTempRequest.notifyNumOnMainThread;
    auto interNotifyOnMainThread = interTempRequest.notifyNumOnMainThread;

    intraThreads_.clear();
    intraThreads_.emplace_back(threads_[1]);
    if (intraThreadsNum + 2 >= 3) {
        for (u32 i = 3; i < intraThreadsNum + 2; i++) {
            intraThreads_.emplace_back(threads_[i]);
        }
    }

    interThreads_.clear();
    interThreads_.emplace_back(threads_[intraThreadsNumFinal + 2]);
    if (threads_.size() >= intraThreadsNumFinal + 4) {
        for (u32 i = intraThreadsNumFinal + 4; i < threads_.size(); i++) {
            interThreads_.emplace_back(threads_[i]);
        }
    }

    // 用于两个算法同步
    mainThread_ = threads_.at(0);
    templateMainThreads_.clear();
    templateMainThreads_.emplace_back(intraThreads_.at(0));
    templateMainThreads_.emplace_back(interThreads_.at(0));
    syncNotifyOnTemplates_ = {intraNotifyOnMainThread, interNotifyOnMainThread};
    syncNotifyOnMain_ = {0, 1};

    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    PrepareResForTemplate23(
        InsAlgTemplate0& tempAlgIntra, InsAlgTemplate2& tempAlgIntra1, InsAlgTemplate3& tempAlgInter1)
{
    AlgResourceRequest intraTempRequest;
    AlgResourceRequest interTempRequest1;
    AlgResourceRequest intraTempRequest1;
    tempAlgIntra.GetRes(intraTempRequest);
    tempAlgInter1.GetRes(interTempRequest1);
    tempAlgIntra1.GetRes(intraTempRequest1);
    auto intraNotifyOnMainThread = intraTempRequest1.notifyNumOnMainThread;
    auto interNotifyOnMainThread = interTempRequest1.notifyNumOnMainThread;
    auto intraThreadsNum = intraTempRequest.slaveThreadNum + 1;
    auto intraThreadsNum1 = intraTempRequest1.slaveThreadNum + 1;
    auto intraThreadsNumFinal = std::max(intraThreadsNum, intraThreadsNum1);

    intraThreads_.assign(threads_.begin() + 2, threads_.begin() + intraThreadsNum1 + 2);
    interThreads_.assign(threads_.begin() + intraThreadsNumFinal + 3, threads_.end());
    // 用于两个算法同步
    mainThread_ = threads_.at(0);

    templateMainThreads_.clear();
    templateMainThreads_.emplace_back(intraThreads_.at(0));
    templateMainThreads_.emplace_back(interThreads_.at(0));
    syncNotifyOnMain_ = {0, 1};
    syncNotifyOnTemplates_ = {intraNotifyOnMainThread, interNotifyOnMainThread};

    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    PrepareResForTemplateResource(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, TemplateResource& intraTempAlgRes,
        TemplateResource& interTempAlgRes, bool isRsStage)
{
    intraTempAlgRes.channels = intraLinks_;
    interTempAlgRes.channels = interLinks_;
    intraTempAlgRes.threads = intraThreads_;
    intraTempAlgRes.aivCommInfoPtr = resCtx.aivCommInfoPtr;
    for (auto i : intraTempAlgRes.channels) {
        HCCL_DEBUG(
            "[InsAllReduceParallelExecutor][PrepareResForTemplateResource],intraTempAlgRes.channels, myRank_[%u], "
            "channels[%u]= size[%u] ",
            myRank_, i.first, i.second.size());
    }
    interTempAlgRes.threads = interThreads_;
    interTempAlgRes.aivCommInfoPtr = resCtx.aivCommInfoPtr;
    for (auto i : interTempAlgRes.channels) {
        HCCL_DEBUG(
            "[InsAllReduceParallelExecutor][PrepareResForTemplateResource],interTempAlgRes.channels, myRank_[%u], "
            "channels[%u]= size[%u] ",
            myRank_, i.first, i.second.size());
    }
    HCCL_DEBUG(
        "[InsAllReduceParallelExecutor][PrepareResForTemplateResource] AlgTemplate intraThreads_size[%d] "
        "interThreads_size[%d]",
        intraThreads_.size(), interThreads_.size());

    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    GenDataParamstempAlg(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 sliceCount,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, const u32 LocalRankSize, const u64 inputOffset,
        const u64 outputOffset, const u64 hcclBuffOffset) const
{
    GenDataParamsAllRank(sliceCount, LocalRankSize, dataParams);

    dataParams.buffInfo.inputPtr =
        dataParams.buffInfo.inBuffType == BufferType::HCCL_BUFFER ? resCtx.cclMem.addr : param.inputPtr;
    dataParams.buffInfo.inputSize =
        dataParams.buffInfo.inBuffType == BufferType::HCCL_BUFFER ? resCtx.cclMem.size : param.inputSize;
    dataParams.buffInfo.outputPtr =
        dataParams.buffInfo.outBuffType == BufferType::HCCL_BUFFER ? resCtx.cclMem.addr : param.outputPtr;
    dataParams.buffInfo.outputSize =
        dataParams.buffInfo.outBuffType == BufferType::HCCL_BUFFER ? resCtx.cclMem.size : param.outputSize;
    dataParams.buffInfo.hcclBuff = resCtx.cclMem;
    dataParams.buffInfo.hcclBuffSize = resCtx.cclMem.size;

    dataParams.buffInfo.hcclBuffBaseOff = scratchOffsetCount * dataTypeSize_ + hcclBuffOffset;
    dataParams.buffInfo.inBuffBaseOff = dataParams.buffInfo.inBuffType == BufferType::HCCL_BUFFER ?
                                            scratchOffsetCount * dataTypeSize_ + inputOffset :
                                            dataOffset;
    dataParams.buffInfo.outBuffBaseOff = dataParams.buffInfo.outBuffType == BufferType::HCCL_BUFFER ?
                                             scratchOffsetCount * dataTypeSize_ + outputOffset :
                                             dataOffset;
    dataParams.repeatNum = 1;
    dataParams.inputRepeatStride = 0;
    dataParams.outputRepeatStride = 0;

    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    CalcInterDataAllRank(
        const u64 sliceCount, const u32 LocalRankSizePart1, const u32 LocalRankSizePart2,
        std::map<u32, std::pair<u64, u64>>& dataMap)
{
    dataMap.clear();
    rankBaseOffIntraAGMap_.clear();

    FillDataMap(sliceCount, LocalRankSizePart1, LocalRankSizePart2, dataMap, true);
    FillOffsetMaps(dataMap, LocalRankSizePart1, LocalRankSizePart2, rankBaseOffIntraAGMap_, true);
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    CalcIntraDataAllRank(
        const u64 sliceCount, const u32 LocalRankSizePart1, const u32 LocalRankSizePart2,
        std::map<u32, std::pair<u64, u64>>& dataMap)
{
    dataMap.clear();
    rankBaseOffInterAGMap_.clear();

    FillDataMap(sliceCount, LocalRankSizePart1, LocalRankSizePart2, dataMap, false);
    FillOffsetMaps(dataMap, LocalRankSizePart1, LocalRankSizePart2, rankBaseOffInterAGMap_, false);
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
u32 InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    CalcRankIdInter(u32 i, u32 j, u32 part2)
{
    return i * part2 + j;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
u32 InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    CalcRankIdIntra(u32 i, u32 j, u32 part1)
{
    return j * part1 + i;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    FillDataMap(
        const u64 sliceCount, const u32 part1, const u32 part2, std::map<u32, std::pair<u64, u64>>& dataMap,
        bool isInter)
{
    u64 sliceSize = sliceCount * dataTypeSize_;
    u64 rankStridePart1 = RoundDown(sliceSize, (part1 * dataTypeSize_)) * dataTypeSize_;
    u32 rankId = 0;
    u64 curSizePart1 = 0;
    u64 rankStridePart2 = 0;
    u64 curSizePart2 = 0;

    for (u32 i = 0; i < part1; ++i) {
        curSizePart1 = (i == (part1 - 1)) ? (sliceSize - rankStridePart1 * i) : rankStridePart1;
        rankStridePart2 = RoundDown(curSizePart1, (part2 * dataTypeSize_)) * dataTypeSize_;
        for (u32 j = 0; j < part2; ++j) {
            curSizePart2 = (j == (part2 - 1)) ? (curSizePart1 - rankStridePart2 * j) : rankStridePart2;

            if (isInter) {
                rankId = CalcRankIdInter(i, j, part2);
            } else {
                rankId = CalcRankIdIntra(i, j, part1);
            }

            dataMap[rankId] = std::make_pair(rankStridePart2, curSizePart2);
        }
    }
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    FillOffsetMaps(
        const std::map<u32, std::pair<u64, u64>>& dataMap, const u32 part1, const u32 part2, std::map<u32, u64>& agMap,
        bool isInter)
{
    u64 baseOffAG = 0;
    u64 accumAG = 0;
    u32 rankId = 0;
    u64 result = 0;

    for (u32 i = 0; i < part1; ++i) {
        for (u32 j = 0; j < part2; ++j) {
            if (isInter) {
                rankId = CalcRankIdInter(i, j, part2);
            } else {
                rankId = CalcRankIdIntra(i, j, part1);
            }

            agMap.insert(std::make_pair(rankId, baseOffAG));

            result = (j == (part2 - 1)) ? dataMap.find(rankId)->second.second : dataMap.find(rankId)->second.first;
            accumAG += result;
        }
        baseOffAG += accumAG;
        accumAG = 0;
    }
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    GenDataParamsAllRank(const u64 sliceCount, const u32 LocalRankSize, TemplateDataParams& dataParams) const
{
    u64 curSize = 0;
    u64 sliceSize = sliceCount * dataTypeSize_;
    u64 rankStride = RoundDown(sliceSize, (LocalRankSize * dataTypeSize_)) * dataTypeSize_;
    for (u64 i = 0; i < LocalRankSize; i++) {
        curSize = (i == (LocalRankSize - 1)) ? (sliceSize - rankStride * i) : rankStride;
    }
    dataParams.tailSize = curSize;
    dataParams.sliceSize = rankStride;
    dataParams.count = dataParams.sliceSize / dataTypeSize_;
    dataParams.inputSliceStride = rankStride;
    dataParams.outputSliceStride = rankStride;
    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    GenDataParamsBufferType(
        const BufferType inBuffType, const BufferType outBuffType, const BufferType hcclBuffType,
        TemplateDataParams& dataParams) const
{
    dataParams.buffInfo.inBuffType = inBuffType;
    dataParams.buffInfo.outBuffType = outBuffType;
    dataParams.buffInfo.hcclBuffType = hcclBuffType;

    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    PrePareDataParamstempAlgInter(const u64 dataOffset, const u64 currCountPart, const u64 scratchOffsetCount)
{
    u32 myRankIdx = tempVirtRankMapInter_.at(myRank_);
    u64 sendDataSize = 0;
    u64 curSize = 0;
    allRankDisplsInter_.clear();
    allRankSliceSizeInter_.clear();
    u64 rankStride = RoundDown(currCountPart * dataTypeSize_, (intraLocalRankSize_ * dataTypeSize_)) * dataTypeSize_;
    for (u64 i = 0; i < intraLocalRankSize_; i++) {
        curSize = (i == (intraLocalRankSize_ - 1)) ? (currCountPart * dataTypeSize_ - rankStride * i) : rankStride;
        allRankSliceSizeInter_.push_back(curSize);
        allRankDisplsInter_.push_back(sendDataSize);
        sendDataSize += curSize;
    }
    currCountPart0_ = allRankSliceSizeInter_[myRankIdx] / dataTypeSize_;
    dataOffset0Inter_ = dataOffset;
    scratchOffsetCountInterStage1_ = scratchOffsetCount;
    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
void InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    PrePareDataParamstempAlgIntra(const u64 dataOffset, const u64 currCountPart, const u64 scratchOffsetCount)
{
    u64 myRankIdx = tempVirtRankMapIntra_.at(myRank_);
    u64 sendDataSize = 0;
    u64 curSize = 0;
    allRankDisplsInter_.clear();
    allRankSliceSizeInter_.clear();
    u64 rankStride = RoundDown(currCountPart * dataTypeSize_, (interLocalRankSize_ * dataTypeSize_)) * dataTypeSize_;
    for (u64 i = 0; i < interLocalRankSize_; i++) {
        curSize = (i == (interLocalRankSize_ - 1)) ? (currCountPart * dataTypeSize_ - rankStride * i) : rankStride;
        allRankSliceSizeInter_.push_back(curSize);
        allRankDisplsInter_.push_back(sendDataSize);
        sendDataSize += curSize;
    }
    currCountPart1_ = allRankSliceSizeInter_[myRankIdx] / dataTypeSize_;
    dataOffset0Intra_ = dataOffset;
    scratchOffsetCountIntraStage1_ = scratchOffsetCount;

    return;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    GenInsQues(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& tempAlgIntra,
        InsAlgTemplate1& tempAlgInter, InsAlgTemplate2& tempAlgIntra1, InsAlgTemplate3& tempAlgInter1)
{
    HCCL_INFO("[InsAllReduceParallelExecutor] AlgTemplate intra server is [%s]", tempAlgIntra.Describe().c_str());
    HCCL_INFO("[InsAllReduceParallelExecutor] AlgTemplate inter server is [%s]", tempAlgInter.Describe().c_str());

    std::vector<float> dataSplitSize;
    GetParallelDataSplit(dataSplitSize);

    u32 multipleIntra = tempAlgIntra.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT);
    u32 multipleInter = tempAlgInter.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT);
    u32 defaultMultiple = 2;
    if (multipleIntra > 0 || multipleInter > 0) {
        multipleIntra = defaultMultiple;
        multipleInter = defaultMultiple;
    }

    // 按照intraData0+interData1，以及intraData1+interData0两种方式分别计算，取multiple最大需求
    float multiple0 = dataSplitSize.at(0) * float(multipleIntra) + dataSplitSize.at(1) * float(multipleInter);
    float multiple1 = dataSplitSize.at(1) * float(multipleIntra) + dataSplitSize.at(0) * float(multipleInter);
    float multiple = std::max(multiple0, multiple1);

    // 数据切分
    u64 sliceCountUB = std::min(static_cast<u64>(UB_MAX_DATA_SIZE) / dataTypeSize_, dataCount_);
    float sliceCountPercent = std::max(
        dataSplitSize.at(0) * float(1.0 / intraLocalRankSize_), dataSplitSize.at(1) * float(1.0 / interLocalRankSize_));
    u64 sliceCountUB0 = sliceCountPercent > 0 ? std::floor(double(sliceCountUB) / sliceCountPercent) : sliceCountUB;
    u64 sliceCount = sliceCountUB;
    if (multiple > 0 && maxTmpMemSize_ > 0) {
        u64 scratchCount = maxTmpMemSize_ / dataTypeSize_; // 按照count来切分
        sliceCount = std::min(static_cast<u64>(std::floor(double(scratchCount) / multiple)), sliceCountUB0);
    }
    HCCL_DEBUG(
        "[InsAllReduceParallelExecutor][GenInsQues] dataCount_[%lu], myRank_[%d], sliceCountUB[%d], sliceCountUB0[%d], "
        "sliceCount[%d]",
        dataCount_, myRank_, sliceCountUB, sliceCountUB0, sliceCount);

    u64 alignSize = AICPU_ALIGN_SIZE;
    u64 sliceCountPart0 = static_cast<u64>(std::floor(double(sliceCount) * dataSplitSize.at(0)));
    if (sliceCountPart0 * dataTypeSize_ >= alignSize) {
        sliceCountPart0 = sliceCountPart0 * dataTypeSize_ / alignSize * alignSize / dataTypeSize_;
    }

    if (sliceCount == 0) {
        HCCL_WARNING("The divisor cannot be zero.");
        return HcclResult::HCCL_SUCCESS;
    }
    // 计算循环次数
    u32 loopTimes = dataCount_ / sliceCount + ((dataCount_ % sliceCount == 0) ? 0 : 1);
    // 计算Scratch偏移，数据尾块必然小于常规块，不用额外计算尾块时的Scratch偏移
    u64 scratchOffsetCountIntraStage0 = 0;
    u64 scratchOffsetCountInterStage0 = sliceCountPart0 * multipleIntra;
    u64 scratchOffsetCountInterStage1 = 0;
    u64 scratchOffsetCountIntraStage1 = sliceCountPart0 * multipleInter;
    HCCL_DEBUG(
        "[InsAllReduceParallelExecutor][GenInsQues] dataCount_[%lu], myRank_[%d], sliceCountPart0[%d], "
        "multipleIntra[%d]",
        dataCount_, myRank_, sliceCountPart0, multipleIntra);
    HCCL_DEBUG(
        "[InsAllReduceParallelExecutor][GenInsQues] myRank_[%d],scratchOffsetCountInterStage0[%d], "
        "scratchOffsetCountIntraStage1[%d]",
        myRank_, scratchOffsetCountInterStage0, scratchOffsetCountIntraStage1);
    TemplateDataParams tempAlgParamsIntra0;
    TemplateDataParams tempAlgParamsInter0;
    TemplateDataParams tempAlgParamsInter1;
    TemplateDataParams tempAlgParamsIntra1;
    TemplateDataParams tempAlgParamsIntra01;
    TemplateDataParams tempAlgParamsInter01;
    TemplateDataParams tempAlgParamsInter11;
    TemplateDataParams tempAlgParamsIntra11;

    u64 processedCount = 0;
    u32 loopIndex = 0;
    while (processedCount < dataCount_) {
        u64 remainingCount = dataCount_ - processedCount;
        u32 remainingLoopTimes = (loopIndex < loopTimes) ? (loopTimes - loopIndex) : 1;
        u64 currCount = (remainingCount + remainingLoopTimes - 1) / remainingLoopTimes;
        currCount = std::min(currCount, sliceCount);
        u64 currCountPart0 = static_cast<u64>(std::floor(double(currCount) * dataSplitSize.at(0)));
        u64 currCountPart1 = currCount - currCountPart0;
        if (remainingLoopTimes > 1) {
            u64 alignedCountPart0 = currCountPart0;
            u64 alignedCountPart1 = currCountPart1;
            alignedCountPart0 = alignedCountPart0 * dataTypeSize_ / alignSize * alignSize / dataTypeSize_;
            alignedCountPart1 = alignedCountPart1 * dataTypeSize_ / alignSize * alignSize / dataTypeSize_;
            if (alignedCountPart0 + alignedCountPart1 > 0) {
                currCountPart0 = alignedCountPart0;
                currCountPart1 = alignedCountPart1;
            }
        }
        CHK_PRT_RET(
            currCountPart0 + currCountPart1 == 0,
            HCCL_ERROR("[InsAllReduceParallelExecutor][GenInsQues] currCount is 0"), HcclResult::HCCL_E_INTERNAL);
        u64 dataOffset0 = processedCount * dataTypeSize_;
        u64 dataOffset1 = dataOffset0 + currCountPart0 * dataTypeSize_;
        // 计算算法模板所需资源
        TemplateResource intraTempAlgRes;
        TemplateResource interTempAlgRes;
        CHK_RET(PrepareResForTemplate(tempAlgIntra, tempAlgInter, tempAlgIntra1));
        CHK_RET(PrepareResForTemplateResource(param, resCtx, intraTempAlgRes, interTempAlgRes, true));
        // server 间地址偏移
        for (int i = 0; i < temp0HierarchyInfo_[0].size(); i++) {
            tempVirtRankMapInter_.insert(std::make_pair(temp0HierarchyInfo_[0][i], i));
        }
        // server 内地址偏移
        for (int i = 0; i < temp1HierarchyInfo_[0].size(); i++) {
            tempVirtRankMapIntra_.insert(std::make_pair(temp1HierarchyInfo_[0][i], i));
        }
        CalcIntraDataAllRank(currCountPart0, intraLocalRankSize_, interLocalRankSize_, meshPartDataMap_);
        CalcInterDataAllRank(currCountPart1, interLocalRankSize_, intraLocalRankSize_, nhrPartDataMap_);
        // 第一步开始前同步
        CHK_RET(PreSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnTemplates_));
        CHK_RET(RunTemplateIntra0(
            param, resCtx, dataOffset0, currCountPart0, scratchOffsetCountIntraStage0, tempAlgParamsIntra0,
            intraTempAlgRes, tempAlgIntra));
        CHK_RET(RunTemplateInter1(
            param, resCtx, dataOffset1, currCountPart1, scratchOffsetCountInterStage0, tempAlgParamsInter1,
            interTempAlgRes, tempAlgInter));
        // 第一步做完后回到主流做尾同步
        CHK_RET(PostSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnMain_));

        // 第二步开始前同步
        CHK_RET(PreSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnTemplates_));
        CHK_RET(RunTemplateInter0(
            param, resCtx, dataOffset0, currCountPart0, scratchOffsetCountInterStage1, tempAlgParamsInter0,
            interTempAlgRes, tempAlgInter));
        CHK_RET(RunTemplateIntra1(
            param, resCtx, dataOffset1, currCountPart1, scratchOffsetCountIntraStage1, tempAlgParamsIntra1,
            intraTempAlgRes, tempAlgIntra));
        // 尾同步
        CHK_RET(PostSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnMain_));

        // 计算算法模板所需资源 InsAlgTemplate2 &tempAlgIntra1, InsAlgTemplate3 &tempAlgInter1
        TemplateResource intraTempAlgRes1;
        TemplateResource interTempAlgRes1;
        CHK_RET(PrepareResForTemplate23(tempAlgIntra, tempAlgIntra1, tempAlgInter1));
        CHK_RET(PrepareResForTemplateResource(param, resCtx, intraTempAlgRes1, interTempAlgRes1, false));

        // 第三步开始前同步
        CHK_RET(PreSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnTemplates_));
        CHK_RET(RunTemplateInter01(
            param, resCtx, dataOffset0, currCountPart0, scratchOffsetCountInterStage1, tempAlgParamsInter01,
            interTempAlgRes1, tempAlgInter1));
        CHK_RET(RunTemplateIntra11(
            param, resCtx, dataOffset1, currCountPart1, scratchOffsetCountIntraStage1, tempAlgParamsIntra11,
            intraTempAlgRes1, tempAlgIntra1));
        // 尾同步
        CHK_RET(PostSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnMain_));

        // 第四步开始前同步
        CHK_RET(PreSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnTemplates_));
        CHK_RET(RunTemplateIntra01(
            param, resCtx, dataOffset0, currCountPart0, scratchOffsetCountIntraStage0, tempAlgParamsIntra01,
            intraTempAlgRes1, tempAlgIntra1));
        CHK_RET(RunTemplateInter11(
            param, resCtx, dataOffset1, currCountPart1, scratchOffsetCountInterStage0, tempAlgParamsInter11,
            interTempAlgRes1, tempAlgInter1));
        // 第四步做完后回到主流做尾同步
        CHK_RET(PostSyncInterThreads(mainThread_, templateMainThreads_, syncNotifyOnMain_));

        processedCount += currCountPart0 + currCountPart1;
        loopIndex++;
    }

    HCCL_INFO("[InsAllReduceParallelExecutor][GenInsQues] End.myRank[%d]", myRank_);
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateIntra0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate0& tempAlgIntra)
{
    // step1 - parallel 0: RS mesh
    u64 inputOffset = 0;
    u64 outputOffset = 0;
    u64 hcclBuffOffset = 1 * currCountPart * dataTypeSize_;
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::INPUT, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset, currCountPart, scratchOffsetCount, dataParams, intraLocalRankSize_, inputOffset,
            outputOffset, hcclBuffOffset);
        CHK_RET(tempAlgIntra.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateInter1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate1& tempAlgInter)
{
    // step1 - parallel 1: RS nhr
    u64 inputOffset = 0;
    u64 outputOffset = 0;
    u64 hcclBuffOffset = 0;
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::INPUT, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset, currCountPart, scratchOffsetCount, dataParams, interLocalRankSize_, inputOffset,
            outputOffset, hcclBuffOffset);
        CHK_RET(tempAlgInter.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateInter0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate1& tempAlgInter)
{
    // step2 - parallel 0: RS nhr
    u64 inputOffset = 0;
    u64 hcclBuffOffset = 0;
    u64 outputOffset = 0;
    PrePareDataParamstempAlgInter(dataOffset, currCountPart, scratchOffsetCount);
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset0Inter_, currCountPart0_, scratchOffsetCountInterStage1_, dataParams,
            interLocalRankSize_, inputOffset, outputOffset, hcclBuffOffset);
        dataParams.buffInfo.inBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        dataParams.buffInfo.hcclBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        dataParams.buffInfo.outBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        CHK_RET(tempAlgInter.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateIntra1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate0& tempAlgIntra)
{
    // step2 - parallel 1: RS mesh
    u64 inputOffset = 0;
    u64 hcclBuffOffset = 1 * currCountPart * dataTypeSize_;
    u64 outputOffset = 0;
    PrePareDataParamstempAlgIntra(dataOffset, currCountPart, scratchOffsetCount);
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset0Intra_, currCountPart1_, scratchOffsetCountIntraStage1_, dataParams,
            intraLocalRankSize_, inputOffset, outputOffset, hcclBuffOffset);
        dataParams.buffInfo.inBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        dataParams.buffInfo.hcclBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        dataParams.buffInfo.outBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        CHK_RET(tempAlgIntra.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateInter01(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate3& tempAlgInter1)
{
    // step3 - parallel 0: AG nhr
    u64 inputOffset = 0;
    u64 outputOffset = 0;
    u64 hcclBuffOffset = 0;
    PrePareDataParamstempAlgInter(dataOffset, currCountPart, scratchOffsetCount);
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset0Inter_, currCountPart0_, scratchOffsetCountInterStage1_, dataParams,
            interLocalRankSize_, inputOffset, outputOffset, hcclBuffOffset);
        dataParams.buffInfo.inBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        dataParams.buffInfo.hcclBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        dataParams.buffInfo.outBuffBaseOff += rankBaseOffInterAGMap_.at(myRank_);
        CHK_RET(tempAlgInter1.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateIntra11(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate2& tempAlgIntra1)
{
    // step3 - parallel 1: AG mesh
    u64 inputOffset = 0;
    u64 hcclBuffOffset = 0;
    u64 outputOffset = 0;
    PrePareDataParamstempAlgIntra(dataOffset, currCountPart, scratchOffsetCount);
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset0Intra_, currCountPart1_, scratchOffsetCountIntraStage1_, dataParams,
            intraLocalRankSize_, inputOffset, outputOffset, hcclBuffOffset);
        dataParams.buffInfo.inBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        dataParams.buffInfo.hcclBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        dataParams.buffInfo.outBuffBaseOff += rankBaseOffIntraAGMap_.at(myRank_);
        CHK_RET(tempAlgIntra1.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateIntra01(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate2& tempAlgIntra1)
{
    // step4 - parallel 0: AG mesh
    u64 inputOffset = 0;
    u64 hcclBuffOffset = 0;
    u64 outputOffset = 0;
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::OUTPUT, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset, currCountPart, scratchOffsetCount, dataParams, intraLocalRankSize_, inputOffset,
            outputOffset, hcclBuffOffset);
        CHK_RET(tempAlgIntra1.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1, typename InsAlgTemplate2,
    typename InsAlgTemplate3>
HcclResult
InsAllReduceParallelExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>::
    RunTemplateInter11(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset, const u64 currCountPart,
        const u64 scratchOffsetCount, TemplateDataParams& dataParams, TemplateResource& templateResource,
        InsAlgTemplate3& tempAlgInter1)
{
    // step4 - parallel 1: AG nhr
    u64 inputOffset = 0;
    u64 hcclBuffOffset = 0;
    u64 outputOffset = 0;
    if (currCountPart > 0) {
        GenDataParamsBufferType(BufferType::HCCL_BUFFER, BufferType::OUTPUT, BufferType::HCCL_BUFFER, dataParams);
        GenDataParamstempAlg(
            param, resCtx, dataOffset, currCountPart, scratchOffsetCount, dataParams, interLocalRankSize_, inputOffset,
            outputOffset, hcclBuffOffset);
        CHK_RET(tempAlgInter1.KernelRun(param, dataParams, templateResource));
    }
    return HcclResult::HCCL_SUCCESS;
}

// 算法注册
REGISTER_EXECUTOR_BY_FOUR_TEMPS(
    HcclCMDType::HCCL_CMD_ALLREDUCE, AicpuAllreduceParallelMeshNHRPcie, InsAllReduceParallelExecutor, TopoMatchPcieMix,
    InsTempReduceScatterMesh1D, InsTempReduceScatterNHR, InsTempAllGatherMesh1D, InsTempAllGatherNHR);
} // namespace mc2_ops_hccl
