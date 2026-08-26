/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_v2_all_to_all_concurrent_executor.h"

#include <algorithm>
#include <limits>
#include <set>

#include "alg_data_trans_wrapper.h"
#include "ins_temp_all_to_all_v_mesh_1D.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 CONCURRENT_TEMPLATE_NUM = 2U;
constexpr u32 CLOS_PORT_NUM = 4U;
constexpr u32 MESH_BW_AICPU_TS = 10U;
constexpr u32 CLOS_BW_AICPU_TS = 12U;

bool AddOverflows(u64 lhs, u64 rhs) { return lhs > std::numeric_limits<u64>::max() - rhs; }

bool MultiplyOverflows(u64 lhs, u64 rhs) { return lhs != 0U && rhs > std::numeric_limits<u64>::max() / lhs; }

bool IsAicpuEngine(CommEngine engine)
{
    return engine == CommEngine::COMM_ENGINE_AICPU || engine == CommEngine::COMM_ENGINE_AICPU_TS;
}

bool ContainsRank(const std::vector<u32>& ranks, u32 rank)
{
    return std::find(ranks.begin(), ranks.end(), rank) != ranks.end();
}

} // namespace

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::ValidateParam(
    const OpParam& param, u32 rankSize, u32 myRank) const
{
    CHK_PRT_RET(
        !IsAicpuEngine(param.engine),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] unsupported engine[%u].", static_cast<u32>(param.engine)),
        HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        rankSize <= 1U || rankSize > 4U,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] unsupported rankSize[%u].", rankSize), HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        myRank >= rankSize,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid userRank[%u] for rankSize[%u].", myRank, rankSize),
        HCCL_E_PARA);
    CHK_PRT_RET(
        param.all2AllVDataDes.sendType >= HCCL_DATA_TYPE_RESERVED ||
            param.all2AllVDataDes.recvType >= HCCL_DATA_TYPE_RESERVED ||
            param.all2AllVDataDes.sendType != param.all2AllVDataDes.recvType,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid send/recv datatype."), HCCL_E_PARA);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS || topoInfo->level0PcieMix,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] unsupported topology."), HCCL_E_NOT_SUPPORT);
    AlgTopoMatch topoMatch;
    return topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo);
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    CHK_RET(ValidateParam(param, topoInfo->userRankSize, topoInfo->userRank));
    CHK_PRT_RET(
        topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS || topoInfo->level0PcieMix,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor][CalcRes] unsupported topology."), HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        algHierarchyInfo.infos.empty() || algHierarchyInfo.infos[0].size() != CONCURRENT_TEMPLATE_NUM,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor][CalcRes] expected two level0 sub-topologies."), HCCL_E_PARA);
    const std::vector<u32>& meshRanks = algHierarchyInfo.infos[0][0];
    const std::vector<u32>& closRanks = algHierarchyInfo.infos[0][1];
    CHK_PRT_RET(
        meshRanks.empty() || closRanks.empty() || !ContainsRank(meshRanks, topoInfo->userRank) ||
            !ContainsRank(closRanks, topoInfo->userRank),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor][CalcRes] invalid sub-topology ranks."), HCCL_E_PARA);

    const std::vector<std::vector<u32>> meshSubComm = {meshRanks};
    const std::vector<std::vector<u32>> closSubComm = {closRanks};
    std::vector<HcclChannelDesc> meshChannels;
    std::vector<HcclChannelDesc> closChannels;
    CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
        comm, param, topoInfo, meshSubComm, meshChannels, CommTopo::COMM_TOPO_1DMESH));
    CHK_RET(CalcChannelRequestMeshClosMultiJetty(comm, param, topoInfo, closSubComm, closChannels, false, false));
    CHK_PRT_RET(
        meshChannels.size() != meshRanks.size() - 1U,
        HCCL_ERROR(
            "[InsV2AllToAllConcurrentExecutor][CalcRes] mesh channel count[%zu] does not match remote ranks[%zu].",
            meshChannels.size(), meshRanks.size() - 1U),
        HCCL_E_INTERNAL);

    InsAlgTemplate0 template0(param, topoInfo->userRank, meshSubComm);
    InsAlgTemplate1 template1(param, topoInfo->userRank, closSubComm);
    AlgResourceRequest request0;
    AlgResourceRequest request1;
    CHK_RET(template0.CalcResByChannelDescs(param, meshChannels, request0));
    CHK_RET(template1.CalcResByChannelDescs(param, closChannels, request1));
    CHK_PRT_RET(
        request1.slaveThreadNum == std::numeric_limits<u32>::max() ||
            request0.slaveThreadNum > std::numeric_limits<u32>::max() - request1.slaveThreadNum - 1U ||
            request0.notifyNumOnMainThread == std::numeric_limits<u32>::max() ||
            request1.notifyNumOnMainThread == std::numeric_limits<u32>::max(),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor][CalcRes] resource count overflows."), HCCL_E_PARA);

    resourceRequest.slaveThreadNum = request0.slaveThreadNum + request1.slaveThreadNum + 1U;
    resourceRequest.notifyNumOnMainThread = request0.notifyNumOnMainThread + 1U;
    resourceRequest.notifyNumPerThread = request0.notifyNumPerThread;
    resourceRequest.notifyNumPerThread.push_back(request1.notifyNumOnMainThread + 1U);
    resourceRequest.notifyNumPerThread.insert(
        resourceRequest.notifyNumPerThread.end(), request1.notifyNumPerThread.begin(),
        request1.notifyNumPerThread.end());
    resourceRequest.channels.resize(1U);
    resourceRequest.channels[0].reserve(meshChannels.size() + closChannels.size());
    resourceRequest.channels[0].insert(resourceRequest.channels[0].end(), meshChannels.begin(), meshChannels.end());
    resourceRequest.channels[0].insert(resourceRequest.channels[0].end(), closChannels.begin(), closChannels.end());
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::RestoreChannels(
    const AlgResourceCtxSerializable& resCtx, const std::vector<u32>& meshRanks, const std::vector<u32>& closRanks)
{
    CHK_PRT_RET(
        meshRanks.empty() || closRanks.empty(),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] Mesh/CLOS ranks are empty."), HCCL_E_PARA);
    CHK_PRT_RET(
        resCtx.channels.size() != 1U,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] expected one channel level, got[%zu].", resCtx.channels.size()),
        HCCL_E_INTERNAL);
    const size_t meshChannelNum = meshRanks.size() - 1U;
    CHK_PRT_RET(
        resCtx.channels[0].size() < meshChannelNum,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] channel list is shorter than mesh prefix."), HCCL_E_INTERNAL);

    templateChannels_.assign(CONCURRENT_TEMPLATE_NUM, {});
    std::set<u32> meshRemoteRanks;
    for (size_t index = 0U; index < resCtx.channels[0].size(); ++index) {
        const ChannelInfo& channel = resCtx.channels[0][index];
        if (index < meshChannelNum) {
            CHK_PRT_RET(
                channel.remoteRank == resCtx.topoInfo.userRank || !ContainsRank(meshRanks, channel.remoteRank) ||
                    !meshRemoteRanks.insert(channel.remoteRank).second,
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid mesh channel prefix."), HCCL_E_INTERNAL);
            templateChannels_[0][channel.remoteRank].push_back(channel);
        } else {
            CHK_PRT_RET(
                channel.remoteRank == resCtx.topoInfo.userRank || !ContainsRank(closRanks, channel.remoteRank),
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid CLOS channel suffix."), HCCL_E_INTERNAL);
            templateChannels_[1][channel.remoteRank].push_back(channel);
        }
    }
    CHK_PRT_RET(
        meshRemoteRanks.size() != meshChannelNum || templateChannels_[1].size() != closRanks.size() - 1U,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] incomplete Mesh/CLOS channel coverage."), HCCL_E_INTERNAL);

    const u32 meshChannelsPerRank = CalcChannelsPerRank(templateChannels_[0]);
    const u32 closChannelsPerRank = CalcChannelsPerRank(templateChannels_[1]);
    const u32 meshConcurrentRanks =
        std::min(ALLTOALLV_DIRECT_FULLMESH_CONCURRENT_SIZE, static_cast<u32>(meshRanks.size() - 1U));
    const u32 closConcurrentRanks =
        std::min(ALLTOALLV_DIRECT_FULLMESH_CONCURRENT_SIZE, static_cast<u32>(closRanks.size() - 1U));
    CHK_PRT_RET(
        meshChannelsPerRank == 0U || closChannelsPerRank == 0U ||
            meshConcurrentRanks > std::numeric_limits<u32>::max() / meshChannelsPerRank ||
            closConcurrentRanks > std::numeric_limits<u32>::max() / closChannelsPerRank,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid channel/thread count."), HCCL_E_INTERNAL);
    const size_t template0ThreadNum = static_cast<size_t>(meshConcurrentRanks) * meshChannelsPerRank + 1U;
    const size_t template1ThreadNum = static_cast<size_t>(closConcurrentRanks) * closChannelsPerRank + 1U;
    CHK_PRT_RET(
        resCtx.threads.size() != template0ThreadNum + template1ThreadNum,
        HCCL_ERROR(
            "[InsV2AllToAllConcurrentExecutor] thread count[%zu], expected[%zu].", resCtx.threads.size(),
            template0ThreadNum + template1ThreadNum),
        HCCL_E_INTERNAL);
    template0Threads_.assign(resCtx.threads.begin(), resCtx.threads.begin() + template0ThreadNum);
    template1Threads_.assign(resCtx.threads.begin() + template0ThreadNum, resCtx.threads.end());
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::SplitSendRecvData(
    const OpParam& param, std::vector<SendRecvData>& splitData) const
{
    const auto* sendCounts = static_cast<const u64*>(param.all2AllVDataDes.sendCounts);
    const auto* recvCounts = static_cast<const u64*>(param.all2AllVDataDes.recvCounts);
    const auto* sdispls = static_cast<const u64*>(param.all2AllVDataDes.sdispls);
    const auto* rdispls = static_cast<const u64*>(param.all2AllVDataDes.rdispls);
    splitData.assign(CONCURRENT_TEMPLATE_NUM, {});
    for (SendRecvData& data : splitData) {
        data.sendCounts.resize(rankSize_);
        data.recvCounts.resize(rankSize_);
        data.sdispls.resize(rankSize_);
        data.rdispls.resize(rankSize_);
    }
    const u32 meshFactor = param.engine == CommEngine::COMM_ENGINE_AICPU_TS ? MESH_BW_AICPU_TS : rankSize_ - 1U;
    const u32 closFactor = param.engine == CommEngine::COMM_ENGINE_AICPU_TS ? CLOS_BW_AICPU_TS : CLOS_PORT_NUM;
    const u32 totalFactor = meshFactor + closFactor;
    CHK_PRT_RET(
        totalFactor == 0U, HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] split factor is zero."), HCCL_E_INTERNAL);
    for (u32 rank = 0U; rank < rankSize_; ++rank) {
        const u64 meshSend = sendCounts[rank] / totalFactor * meshFactor;
        const u64 meshRecv = recvCounts[rank] / totalFactor * meshFactor;
        CHK_PRT_RET(
            AddOverflows(sdispls[rank], meshSend) || AddOverflows(rdispls[rank], meshRecv),
            HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] split displacement overflows."), HCCL_E_PARA);
        splitData[0].sendCounts[rank] = meshSend;
        splitData[0].recvCounts[rank] = meshRecv;
        splitData[0].sdispls[rank] = sdispls[rank];
        splitData[0].rdispls[rank] = rdispls[rank];
        splitData[1].sendCounts[rank] = sendCounts[rank] - meshSend;
        splitData[1].recvCounts[rank] = recvCounts[rank] - meshRecv;
        splitData[1].sdispls[rank] = sdispls[rank] + meshSend;
        splitData[1].rdispls[rank] = rdispls[rank] + meshRecv;
    }
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::CalcMaxDataCountPerLoop(
    const std::vector<u64>& scratchMultipliers, std::vector<u64>& maxDataCountPerLoop) const
{
    CHK_PRT_RET(
        scratchMultipliers.size() != CONCURRENT_TEMPLATE_NUM || dataTypeSize_ == 0U || rankSize_ == 0U,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid scratch calculation input."), HCCL_E_PARA);
    u64 multiplierSum = 0U;
    for (u64 multiplier : scratchMultipliers) {
        CHK_PRT_RET(
            AddOverflows(multiplierSum, multiplier),
            HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] scratch multiplier overflows."), HCCL_E_PARA);
        multiplierSum += multiplier;
    }
    maxDataCountPerLoop.assign(CONCURRENT_TEMPLATE_NUM, 0U);
    for (u32 index = 0U; index < CONCURRENT_TEMPLATE_NUM; ++index) {
        u64 maxDataSize = UB_MAX_DATA_SIZE;
        if (scratchMultipliers[index] != 0U && multiplierSum != 0U) {
            const u64 scratchBound = maxTmpMemSize_ / multiplierSum / HCCL_MIN_SLICE_ALIGN * HCCL_MIN_SLICE_ALIGN;
            maxDataSize = std::min(maxDataSize, scratchBound);
        }
        maxDataCountPerLoop[index] = maxDataSize / dataTypeSize_ / rankSize_;
        CHK_PRT_RET(
            maxDataCountPerLoop[index] == 0U,
            HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] max data count per loop is zero."), HCCL_E_INTERNAL);
    }
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::SetTemplateDataParams(
    TemplateDataParams& params, const SendRecvData& splitData, u64 currentCount, u64 processedCount,
    u64 maxDataCountPerLoop) const
{
    CHK_PRT_RET(
        MultiplyOverflows(currentCount, dataTypeSize_) || MultiplyOverflows(maxDataCountPerLoop, dataTypeSize_),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] template byte size overflows."), HCCL_E_PARA);
    params.count = currentCount;
    params.dataType = dataType_;
    params.processedDataCount = processedCount;
    params.sliceSize = currentCount * dataTypeSize_;
    params.tailSize = params.sliceSize;
    params.inputSliceStride = maxDataCountPerLoop * dataTypeSize_;
    params.outputSliceStride = params.inputSliceStride;
    params.repeatNum = 1U;
    params.inputRepeatStride = 0U;
    params.outputRepeatStride = 0U;
    for (u32 rank = 0U; rank < rankSize_; ++rank) {
        if (splitData.sendCounts[rank] > processedCount) {
            CHK_PRT_RET(
                AddOverflows(splitData.sdispls[rank], processedCount),
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] send displacement overflows."), HCCL_E_PARA);
            params.sendCounts[rank] = std::min(currentCount, splitData.sendCounts[rank] - processedCount);
            params.sdispls[rank] = splitData.sdispls[rank] + processedCount;
        } else {
            CHK_PRT_RET(
                AddOverflows(splitData.sdispls[rank], splitData.sendCounts[rank]),
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] final send displacement overflows."), HCCL_E_PARA);
            params.sendCounts[rank] = 0U;
            params.sdispls[rank] = splitData.sdispls[rank] + splitData.sendCounts[rank];
        }
        if (splitData.recvCounts[rank] > processedCount) {
            CHK_PRT_RET(
                AddOverflows(splitData.rdispls[rank], processedCount),
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] receive displacement overflows."), HCCL_E_PARA);
            params.recvCounts[rank] = std::min(currentCount, splitData.recvCounts[rank] - processedCount);
            params.rdispls[rank] = splitData.rdispls[rank] + processedCount;
        } else {
            CHK_PRT_RET(
                AddOverflows(splitData.rdispls[rank], splitData.recvCounts[rank]),
                HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] final receive displacement overflows."), HCCL_E_PARA);
            params.recvCounts[rank] = 0U;
            params.rdispls[rank] = splitData.rdispls[rank] + splitData.recvCounts[rank];
        }
    }
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    rankSize_ = resCtx.topoInfo.userRankSize;
    myRank_ = resCtx.topoInfo.userRank;
    CHK_RET(ValidateParam(param, rankSize_, myRank_));
    CHK_PTR_NULL(param.all2AllVDataDes.sendCounts);
    CHK_PTR_NULL(param.all2AllVDataDes.recvCounts);
    CHK_PTR_NULL(param.all2AllVDataDes.sdispls);
    CHK_PTR_NULL(param.all2AllVDataDes.rdispls);
    CHK_PRT_RET(
        resCtx.topoInfo.level0Topo != Level0Shape::MESH_1D_CLOS || resCtx.topoInfo.level0PcieMix ||
            resCtx.algHierarchyInfo.infos.empty() || resCtx.algHierarchyInfo.infos[0].size() != CONCURRENT_TEMPLATE_NUM,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid topology context."), HCCL_E_PARA);
    algHierarchyInfo_ = resCtx.algHierarchyInfo;
    dataType_ = param.all2AllVDataDes.sendType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(
        dataTypeSize_ == 0U, HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] datatype size is zero."), HCCL_E_PARA);
    maxTmpMemSize_ = resCtx.cclMem.size;
    CHK_RET(RestoreChannels(resCtx, algHierarchyInfo_.infos[0][0], algHierarchyInfo_.infos[0][1]));
    return OrchestrateLoop(param, resCtx);
}

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
HcclResult InsV2AllToAllConcurrentExecutor<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    const std::vector<std::vector<u32>> meshSubComm = {algHierarchyInfo_.infos[0][0]};
    const std::vector<std::vector<u32>> closSubComm = {algHierarchyInfo_.infos[0][1]};
    InsAlgTemplate0 template0(param, myRank_, meshSubComm);
    InsAlgTemplate1 template1(param, myRank_, closSubComm);

    TemplateResource resource0{};
    TemplateResource resource1{};
    resource0.channels = templateChannels_[0];
    resource1.channels = templateChannels_[1];
    resource0.threads = template0Threads_;
    resource1.threads = template1Threads_;
    for (TemplateResource* resource : {&resource0, &resource1}) {
        resource->aivCommInfoPtr = resCtx.aivCommInfoPtr;
        resource->npu2DpuShmemPtr = resCtx.npu2DpuShmemPtr;
        resource->dpu2NpuShmemPtr = resCtx.dpu2NpuShmemPtr;
    }

    TemplateDataParams params0{};
    TemplateDataParams params1{};
    for (TemplateDataParams* params : {&params0, &params1}) {
        params->buffInfo.inputPtr = param.inputPtr;
        params->buffInfo.outputPtr = param.outputPtr;
        params->buffInfo.inputSize = param.inputSize;
        params->buffInfo.outputSize = param.outputSize;
        params->buffInfo.hcclBuff = resCtx.cclMem;
        params->buffInfo.inBuffType = BufferType::INPUT;
        params->buffInfo.outBuffType = BufferType::OUTPUT;
        params->buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
        params->sendCounts.assign(rankSize_, 0U);
        params->recvCounts.assign(rankSize_, 0U);
        params->sdispls.assign(rankSize_, 0U);
        params->rdispls.assign(rankSize_, 0U);
    }

    std::vector<SendRecvData> splitData;
    CHK_RET(SplitSendRecvData(param, splitData));
    const std::vector<u64> scratchMultipliers = {
        template0.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT),
        template1.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT)};
    std::vector<u64> maxDataCountPerLoop;
    CHK_RET(CalcMaxDataCountPerLoop(scratchMultipliers, maxDataCountPerLoop));
    CHK_PRT_RET(
        MultiplyOverflows(scratchMultipliers[0], maxDataCountPerLoop[0]) ||
            MultiplyOverflows(scratchMultipliers[0] * maxDataCountPerLoop[0], dataTypeSize_),
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] scratch offset overflows."), HCCL_E_PARA);
    params0.buffInfo.hcclBuffBaseOff = 0U;
    params1.buffInfo.hcclBuffBaseOff = scratchMultipliers[0] * maxDataCountPerLoop[0] * dataTypeSize_;

    u64 maxCount0 = 0U;
    u64 maxCount1 = 0U;
    for (u32 rank = 0U; rank < rankSize_; ++rank) {
        maxCount0 = std::max({maxCount0, splitData[0].sendCounts[rank], splitData[0].recvCounts[rank]});
        maxCount1 = std::max({maxCount1, splitData[1].sendCounts[rank], splitData[1].recvCounts[rank]});
    }
    const u64 loopTimes0 =
        maxCount0 / maxDataCountPerLoop[0] + static_cast<u64>(maxCount0 % maxDataCountPerLoop[0] != 0U);
    const u64 loopTimes1 =
        maxCount1 / maxDataCountPerLoop[1] + static_cast<u64>(maxCount1 % maxDataCountPerLoop[1] != 0U);
    CHK_PRT_RET(
        resCtx.slaveThreadNum < resCtx.notifyNumOnMainThread || resCtx.notifyNumOnMainThread == 0U,
        HCCL_ERROR("[InsV2AllToAllConcurrentExecutor] invalid synchronization notify counts."), HCCL_E_INTERNAL);
    const std::vector<u32> preSyncNotify = {resCtx.slaveThreadNum - resCtx.notifyNumOnMainThread};
    const std::vector<u32> postSyncNotify = {resCtx.notifyNumOnMainThread - 1U};
    CHK_RET(PreSyncInterThreads(resource0.threads[0], {resource1.threads[0]}, preSyncNotify));
    u64 processed0 = 0U;
    u64 processed1 = 0U;
    for (u64 loop = 0U; loop < std::max(loopTimes0, loopTimes1); ++loop) {
        if (loop < loopTimes0) {
            const u64 current = loop + 1U == loopTimes0 ? maxCount0 - processed0 : maxDataCountPerLoop[0];
            CHK_RET(SetTemplateDataParams(params0, splitData[0], current, processed0, maxDataCountPerLoop[0]));
            CHK_RET(template0.KernelRun(param, params0, resource0));
            processed0 += current;
        }
        if (loop < loopTimes1) {
            const u64 current = loop + 1U == loopTimes1 ? maxCount1 - processed1 : maxDataCountPerLoop[1];
            CHK_RET(SetTemplateDataParams(params1, splitData[1], current, processed1, maxDataCountPerLoop[1]));
            CHK_RET(template1.KernelRun(param, params1, resource1));
            processed1 += current;
        }
    }
    CHK_RET(PostSyncInterThreads(resource0.threads[0], {resource1.threads[0]}, postSyncNotify));
    return HCCL_SUCCESS;
}

REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLTOALL, AicpuAllToAllSoleMeshConcurrent, InsV2AllToAllConcurrentExecutor, TopoMatchUBX,
    InsTempAlltoAllVMesh1D, InsTempAlltoAllVMesh1D);
REGISTER_EXECUTOR_BY_TWO_TEMPS(
    HcclCMDType::HCCL_CMD_ALLTOALLV, AicpuAllToAllVSoleMeshConcurrent, InsV2AllToAllConcurrentExecutor, TopoMatchUBX,
    InsTempAlltoAllVMesh1D, InsTempAlltoAllVMesh1D);

} // namespace mc2_ops_hccl
