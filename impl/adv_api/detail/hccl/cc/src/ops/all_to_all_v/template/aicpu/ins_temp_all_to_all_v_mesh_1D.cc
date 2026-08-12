/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_temp_all_to_all_v_mesh_1D.h"

#include <algorithm>
#include <limits>

namespace mc2_ops_hccl {
namespace {
constexpr u64 ALLTOALL_MULTI_CHANNEL_DATA_SIZE_LIMIT = 150ULL * 1024ULL * 1024ULL;
constexpr const char* ALLTOALL_SINGLE_CHANNEL_ALG = "InsAlltoAllMesh1DSingleChannel";
constexpr const char* ALLTOALL_MULTI_CHANNEL_ALG = "InsAlltoAllMesh1D";
constexpr const char* ALLTOALLV_MESH_ALG = "InsAlltoAllVMesh1D";

bool AddOverflows(u64 lhs, u64 rhs) { return lhs > std::numeric_limits<u64>::max() - rhs; }

bool MultiplyOverflows(u64 lhs, u64 rhs) { return lhs != 0U && rhs > std::numeric_limits<u64>::max() / lhs; }

HcclResult CalcUserSliceOffset(u64 displacement, u64 dataTypeSize, u64 splitOffset, u64& byteOffset)
{
    CHK_PRT_RET(
        MultiplyOverflows(displacement, dataTypeSize),
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] user buffer displacement overflows."), HCCL_E_PARA);
    const u64 displacementBytes = displacement * dataTypeSize;
    CHK_PRT_RET(
        AddOverflows(displacementBytes, splitOffset),
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] user buffer offset overflows."), HCCL_E_PARA);
    byteOffset = displacementBytes + splitOffset;
    return HCCL_SUCCESS;
}

HcclResult CalcCclSliceOffset(u32 bufferIndex, u64 sliceStride, u64 baseOffset, u64 splitOffset, u64& byteOffset)
{
    CHK_PRT_RET(
        MultiplyOverflows(bufferIndex, sliceStride),
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] CCL buffer stride overflows."), HCCL_E_PARA);
    const u64 bufferOffset = static_cast<u64>(bufferIndex) * sliceStride;
    CHK_PRT_RET(
        AddOverflows(bufferOffset, baseOffset) || AddOverflows(bufferOffset + baseOffset, splitOffset),
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] CCL buffer offset overflows."), HCCL_E_PARA);
    byteOffset = bufferOffset + baseOffset + splitOffset;
    return HCCL_SUCCESS;
}

HcclResult CalcDataSize(u64 count, u64 dataTypeSize, u64& dataSize)
{
    CHK_PRT_RET(
        MultiplyOverflows(count, dataTypeSize), HCCL_ERROR("[InsTempAlltoAllVMesh1D] data size overflows."),
        HCCL_E_PARA);
    dataSize = count * dataTypeSize;
    return HCCL_SUCCESS;
}
} // namespace

bool ShouldUseMultiChannelForAlltoAll(u64 sendCount, u64 dataTypeSize, u32 rankSize)
{
    if (sendCount == 0U || dataTypeSize == 0U || rankSize == 0U) {
        return false;
    }
    if (dataTypeSize > ALLTOALL_MULTI_CHANNEL_DATA_SIZE_LIMIT / rankSize) {
        return true;
    }
    return sendCount > ALLTOALL_MULTI_CHANNEL_DATA_SIZE_LIMIT / (dataTypeSize * rankSize);
}

std::vector<u32> CalcAlltoallMesh1DCommRanks(
    u32 myRank, u32 rankSize, u32 concurrentRankNum, u32 roundIdx, u32 remainRankSize)
{
    std::vector<u32> commRanks;
    if (rankSize <= 1U || myRank >= rankSize || concurrentRankNum == 0U || remainRankSize == 0U) {
        return commRanks;
    }
    const u32 pairNumPerRound = concurrentRankNum / 2U + concurrentRankNum % 2U;
    const u32 pairSize =
        remainRankSize < concurrentRankNum ? remainRankSize / 2U + remainRankSize % 2U : pairNumPerRound;
    const u64 firstPair = static_cast<u64>(roundIdx) * pairNumPerRound + 1U;
    const u64 pairEnd = firstPair + pairSize;
    commRanks.reserve(std::min(concurrentRankNum, remainRankSize));
    for (u64 i = firstPair; i < pairEnd; ++i) {
        const u32 rankOffset = static_cast<u32>(i % rankSize);
        const u32 leftRemoteRank = static_cast<u32>((static_cast<u64>(myRank) + rankSize - rankOffset) % rankSize);
        const u32 rightRemoteRank = static_cast<u32>((static_cast<u64>(myRank) + rankOffset) % rankSize);
        commRanks.push_back(leftRemoteRank);
        if (leftRemoteRank == rightRemoteRank) {
            break;
        }
        commRanks.push_back(rightRemoteRank);
    }
    return commRanks;
}

InsTempAlltoAllVMesh1D::InsTempAlltoAllVMesh1D(
    const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

std::string InsTempAlltoAllVMesh1D::Describe() const
{
    return "Template of AllToAllV Mesh1D with rank size " + std::to_string(templateRankSize_);
}

HcclResult InsTempAlltoAllVMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        topoInfo->level0Topo != Level0Shape::MESH_1D && topoInfo->level0Topo != Level0Shape::CLOS,
        HCCL_ERROR(
            "[InsTempAlltoAllVMesh1D][CalcRes] unsupported level0Topo[%u].", static_cast<u32>(topoInfo->level0Topo)),
        HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        subCommRanks_.empty() || templateRankSize_ == 0U,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D][CalcRes] sub communication ranks are empty."), HCCL_E_PARA);

    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    resourceRequest.channels.push_back(level0Channels);

    channelsPerRank_ = 1U;
    if (std::string(param.algName) != ALLTOALL_SINGLE_CHANNEL_ALG && templateRankSize_ > 1U) {
        CHK_PRT_RET(
            level0Channels.empty(), HCCL_ERROR("[InsTempAlltoAllVMesh1D][CalcRes] channels are empty."),
            HCCL_E_INTERNAL);
        channelsPerRank_ = CalcChannelsPerRank(level0Channels);
    }
    CHK_PRT_RET(
        channelsPerRank_ == 0U, HCCL_ERROR("[InsTempAlltoAllVMesh1D][CalcRes] channelsPerRank is zero."),
        HCCL_E_INTERNAL);

    const u32 remoteRankNum = templateRankSize_ > 0U ? templateRankSize_ - 1U : 0U;
    const u32 concurrentRankNum = std::min(ALLTOALLV_DIRECT_FULLMESH_CONCURRENT_SIZE, remoteRankNum);
    CHK_PRT_RET(
        concurrentRankNum > 0U && channelsPerRank_ > std::numeric_limits<u32>::max() / concurrentRankNum,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D][CalcRes] slave thread number overflows."), HCCL_E_PARA);
    resourceRequest.slaveThreadNum = concurrentRankNum * channelsPerRank_;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, channelsPerRank_);
    resourceRequest.notifyNumOnMainThread = resourceRequest.slaveThreadNum;
    HCCL_INFO(
        "[InsTempAlltoAllVMesh1D][CalcRes] algName[%s], channelsPerRank[%u], slaveThreadNum[%u].", param.algName,
        channelsPerRank_, resourceRequest.slaveThreadNum);
    return HCCL_SUCCESS;
}

u64 InsTempAlltoAllVMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    const u32 remoteRankNum = templateRankSize_ > 0U ? templateRankSize_ - 1U : 0U;
    concurrentSendRecvNum_ = std::min(ALLTOALLV_DIRECT_FULLMESH_CONCURRENT_SIZE, remoteRankNum);
    return concurrentSendRecvNum_;
}

u32 InsTempAlltoAllVMesh1D::CalcCommLoops() const
{
    if (templateRankSize_ <= 1U || concurrentSendRecvNum_ == 0U) {
        return 0U;
    }
    const u32 remoteRankNum = templateRankSize_ - 1U;
    return remoteRankNum / concurrentSendRecvNum_ + static_cast<u32>(remoteRankNum % concurrentSendRecvNum_ != 0U);
}

void InsTempAlltoAllVMesh1D::CalcCclBuffIdx(u32 remoteRank, u32& myRankCclBuffIdx, u32& remoteCclBuffIdx) const
{
    const u32 pairNum = concurrentSendRecvNum_ / 2U + concurrentSendRecvNum_ % 2U;
    const u32 gapRight =
        static_cast<u32>((static_cast<u64>(templateRankSize_) + remoteRank - myRank_) % templateRankSize_);
    const u32 gapLeft =
        static_cast<u32>((static_cast<u64>(templateRankSize_) + myRank_ - remoteRank) % templateRankSize_);
    if (gapLeft < gapRight) {
        myRankCclBuffIdx = pairNum - 1U - ((gapLeft - 1U) % pairNum);
        remoteCclBuffIdx = pairNum + ((gapLeft - 1U) % pairNum);
    } else if (gapLeft > gapRight) {
        myRankCclBuffIdx = pairNum + ((gapRight - 1U) % pairNum);
        remoteCclBuffIdx = pairNum - 1U - ((gapRight - 1U) % pairNum);
    } else {
        myRankCclBuffIdx = 0U;
        remoteCclBuffIdx = 0U;
    }
}

HcclResult InsTempAlltoAllVMesh1D::SetRuntimeChannelsPerRank(
    const OpParam& param, const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    const std::string algName(param.algName);
    if (algName == ALLTOALL_SINGLE_CHANNEL_ALG) {
        channelsPerRank_ = 1U;
        return HCCL_SUCCESS;
    }

    CHK_PRT_RET(
        channels.empty() && templateRankSize_ > 1U, HCCL_ERROR("[InsTempAlltoAllVMesh1D] runtime channels are empty."),
        HCCL_E_PARA);
    const u32 allocatedChannelsPerRank = CalcChannelsPerRank(channels);
    CHK_PRT_RET(
        allocatedChannelsPerRank == 0U, HCCL_ERROR("[InsTempAlltoAllVMesh1D] allocated channels per rank is zero."),
        HCCL_E_INTERNAL);

    if (algName == ALLTOALLV_MESH_ALG) {
        channelsPerRank_ = allocatedChannelsPerRank;
        return HCCL_SUCCESS;
    }
    CHK_PRT_RET(
        algName != ALLTOALL_MULTI_CHANNEL_ALG,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] unsupported algName[%s].", param.algName), HCCL_E_NOT_SUPPORT);
    CHK_PTR_NULL(param.all2AllVDataDes.sendCounts);
    CHK_PRT_RET(
        param.all2AllVDataDes.sendType >= HCCL_DATA_TYPE_RESERVED,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] invalid sendType[%u].", static_cast<u32>(param.all2AllVDataDes.sendType)),
        HCCL_E_PARA);
    const u64 dataTypeSize = DATATYPE_SIZE_TABLE[param.all2AllVDataDes.sendType];
    CHK_PRT_RET(
        dataTypeSize == 0U || templateRankSize_ == 0U,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] invalid datatype size or rank size."), HCCL_E_PARA);
    const u64 sendCount = static_cast<const u64*>(param.all2AllVDataDes.sendCounts)[0];
    const bool useMultiChannel = ShouldUseMultiChannelForAlltoAll(sendCount, dataTypeSize, templateRankSize_);
    channelsPerRank_ = useMultiChannel ? allocatedChannelsPerRank : 1U;
    HCCL_INFO(
        "[InsTempAlltoAllVMesh1D] algName[%s], sendCount[%llu], dataTypeSize[%llu], rankSize[%u], "
        "allocatedChannelsPerRank[%u], effectiveChannelsPerRank[%u].",
        param.algName, static_cast<unsigned long long>(sendCount), static_cast<unsigned long long>(dataTypeSize),
        templateRankSize_, allocatedChannelsPerRank, channelsPerRank_);
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::ValidateKernelInput(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource) const
{
    CHK_PRT_RET(
        subCommRanks_.empty() || templateRankSize_ == 0U,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] sub communication ranks are empty."), HCCL_E_PARA);
    CHK_PRT_RET(
        templateResource.threads.empty(), HCCL_ERROR("[InsTempAlltoAllVMesh1D] threads are empty."), HCCL_E_PARA);
    CHK_PRT_RET(
        tempAlgParams.sendCounts.size() != templateRankSize_ || tempAlgParams.recvCounts.size() != templateRankSize_ ||
            tempAlgParams.sdispls.size() != templateRankSize_ || tempAlgParams.rdispls.size() != templateRankSize_,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] count/displacement vector size mismatch."), HCCL_E_PARA);
    const auto rankIter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    CHK_PRT_RET(
        rankIter == subCommRanks_[0].end(), HCCL_ERROR("[InsTempAlltoAllVMesh1D] local rank is absent."), HCCL_E_PARA);
    const u32 myAlgRank = static_cast<u32>(std::distance(subCommRanks_[0].begin(), rankIter));
    CHK_PRT_RET(
        tempAlgParams.sendCounts[myAlgRank] != tempAlgParams.recvCounts[myAlgRank],
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] self send/recv counts differ."), HCCL_E_PARA);
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    CHK_RET(ValidateKernelInput(tempAlgParams, templateResource));
    CHK_PRT_RET(
        param.all2AllVDataDes.sendType >= HCCL_DATA_TYPE_RESERVED,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] invalid datatype."), HCCL_E_PARA);
    threadNum_ = static_cast<u32>(templateResource.threads.size());
    dataType_ = param.all2AllVDataDes.sendType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(dataTypeSize_ == 0U, HCCL_ERROR("[InsTempAlltoAllVMesh1D] datatype size is zero."), HCCL_E_PARA);
    CHK_RET(SetRuntimeChannelsPerRank(param, templateResource.channels));

    isDmaRead_ = IsPcieProtocol(templateResource.channels);
    const auto rankIter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    const u32 myAlgRank = static_cast<u32>(std::distance(subCommRanks_[0].begin(), rankIter));
    CHK_RET(RunALLtoALL(templateResource.channels, templateResource.threads, tempAlgParams, myAlgRank));
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::LocalCopyForMyRank(
    const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, u32 myAlgRank, u32 queIdx) const
{
    if (tempAlgParams.sendCounts[myAlgRank] == 0U) {
        return HCCL_SUCCESS;
    }
    u64 sourceOffset = 0U;
    u64 destinationOffset = 0U;
    u64 sourceSize = 0U;
    u64 destinationSize = 0U;
    CHK_RET(CalcUserSliceOffset(tempAlgParams.sdispls[myAlgRank], dataTypeSize_, 0U, sourceOffset));
    CHK_RET(CalcUserSliceOffset(tempAlgParams.rdispls[myAlgRank], dataTypeSize_, 0U, destinationOffset));
    CHK_RET(CalcDataSize(tempAlgParams.sendCounts[myAlgRank], dataTypeSize_, sourceSize));
    CHK_RET(CalcDataSize(tempAlgParams.recvCounts[myAlgRank], dataTypeSize_, destinationSize));
    const DataSlice srcSlice(
        tempAlgParams.buffInfo.inputPtr, sourceOffset, sourceSize, tempAlgParams.sendCounts[myAlgRank]);
    const DataSlice dstSlice(
        tempAlgParams.buffInfo.outputPtr, destinationOffset, destinationSize, tempAlgParams.recvCounts[myAlgRank]);
    CHK_RET(static_cast<HcclResult>(LocalCopy(thread, srcSlice, dstSlice)));
    HCCL_DEBUG(
        "[InsTempAlltoAllVMesh1D] local copy on thread[%u], data size[%llu].", queIdx,
        static_cast<unsigned long long>(sourceSize));
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::RunALLtoALL(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParams, u32 myAlgRank)
{
    const u32 commLoops = CalcCommLoops();
    u32 remainRankSize = templateRankSize_ - 1U;
    std::vector<u32> commRanks;
    std::vector<ThreadHandle> subThreads;
    if (threadNum_ > 1U) {
        subThreads.assign(threads.begin() + 1, threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
    }

    for (u32 roundIdx = 0U; roundIdx < commLoops && remainRankSize > 0U; ++roundIdx) {
        commRanks =
            CalcAlltoallMesh1DCommRanks(myRank_, templateRankSize_, concurrentSendRecvNum_, roundIdx, remainRankSize);
        CHK_PRT_RET(
            commRanks.empty(), HCCL_ERROR("[InsTempAlltoAllVMesh1D] communication rank set is empty."),
            HCCL_E_INTERNAL);
        if (isDmaRead_ && roundIdx == 0U) {
            CHK_RET(PreCopyByLoop(commRanks, channels, threads, tempAlgParams));
            if (threadNum_ > 1U) {
                GetNotifyIdxSubToMain(notifyIdxSubToMain_);
                CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
                CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
            }
            CHK_RET(LocalCopyForMyRank(tempAlgParams, threads[0], myAlgRank, 0U));
        } else if (!isDmaRead_ && roundIdx == 0U) {
            CHK_RET(LocalCopyForMyRank(tempAlgParams, threads[0], myAlgRank, 0U));
        }
        CHK_RET(RunSendRecvByLoop(commRanks, tempAlgParams, channels, threads, roundIdx, commLoops));
        remainRankSize -= static_cast<u32>(commRanks.size());
    }

    if (threadNum_ > 1U) {
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::RunSendRecvByLoop(
    const std::vector<u32>& commRanks, const TemplateDataParams& tempAlgParams,
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads, u32 roundIdx,
    u32 commLoops)
{
    for (u32 remoteRank : commRanks) {
        const auto channelIter = channels.find(remoteRank);
        CHK_PRT_RET(
            channelIter == channels.end() || channelIter->second.empty(),
            HCCL_ERROR("[InsTempAlltoAllVMesh1D] no channel for remote rank[%u].", remoteRank), HCCL_E_PARA);
        const std::vector<ChannelInfo>& curChannels = channelIter->second;
        const u32 curValidChannelsSize = std::min(static_cast<u32>(curChannels.size()), channelsPerRank_);
        CHK_PRT_RET(
            curValidChannelsSize == 0U,
            HCCL_ERROR("[InsTempAlltoAllVMesh1D] valid channel count is zero for rank[%u].", remoteRank), HCCL_E_PARA);
        CHK_RET(CalcDataSplitByPortGroupCommon(
            tempAlgParams.sendCounts[remoteRank], dataTypeSize_, curChannels, sendCountsSplit_, sendSizeSplit_,
            sendOffsetSplit_, curValidChannelsSize));
        CHK_RET(CalcDataSplitByPortGroupCommon(
            tempAlgParams.recvCounts[remoteRank], dataTypeSize_, curChannels, recvCountsSplit_, recvSizeSplit_,
            recvOffsetSplit_, curValidChannelsSize));
        CHK_RET(RunSendRecvForRank(
            tempAlgParams, roundIdx, curValidChannelsSize, curChannels, remoteRank, threads, commLoops));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::PreSyncInterThreadsPerRank(
    const ThreadHandle& mainThreadCurRank, const std::vector<ThreadHandle>& subThreadsCurRank) const
{
    std::vector<u32> notifyIndexes(subThreadsCurRank.size(), 1U);
    return PreSyncInterThreads(mainThreadCurRank, subThreadsCurRank, notifyIndexes);
}

HcclResult InsTempAlltoAllVMesh1D::PostSyncInterThreadsPerRank(
    const ThreadHandle& mainThreadCurRank, const std::vector<ThreadHandle>& subThreadsCurRank) const
{
    std::vector<u32> notifyIndexes;
    notifyIndexes.reserve(subThreadsCurRank.size());
    for (u32 index = 0U; index < subThreadsCurRank.size(); ++index) {
        notifyIndexes.push_back(index + 1U);
    }
    return PostSyncInterThreads(mainThreadCurRank, subThreadsCurRank, notifyIndexes);
}

HcclResult InsTempAlltoAllVMesh1D::RunSendRecvForRank(
    const TemplateDataParams& tempAlgParams, u32 roundIdx, u32 curValidChannelsSize,
    const std::vector<ChannelInfo>& curChannels, u32 remoteRank, const std::vector<ThreadHandle>& threads,
    u32 commLoops) const
{
    u32 myRankCclBuffIdx = 0U;
    u32 remoteCclBuffIdx = 0U;
    CalcCclBuffIdx(remoteRank, myRankCclBuffIdx, remoteCclBuffIdx);
    CHK_PRT_RET(
        myRankCclBuffIdx > (std::numeric_limits<u32>::max() - 1U) / channelsPerRank_,
        HCCL_ERROR("[InsTempAlltoAllVMesh1D] thread index overflows."), HCCL_E_PARA);
    const u32 baseQueIdx = myRankCclBuffIdx * channelsPerRank_ + 1U;
    CHK_PRT_RET(
        baseQueIdx >= threads.size() || curValidChannelsSize > threads.size() - baseQueIdx,
        HCCL_ERROR(
            "[InsTempAlltoAllVMesh1D] insufficient threads, index[%u], channels[%u], size[%zu].", baseQueIdx,
            curValidChannelsSize, threads.size()),
        HCCL_E_INTERNAL);

    const ThreadHandle& mainThreadCurRank = threads[baseQueIdx];
    std::vector<ThreadHandle> subThreadsCurRank;
    if (curValidChannelsSize > 1U) {
        subThreadsCurRank.assign(
            threads.begin() + baseQueIdx + 1U, threads.begin() + baseQueIdx + curValidChannelsSize);
        if (roundIdx != 0U) {
            CHK_RET(PreSyncInterThreadsPerRank(mainThreadCurRank, subThreadsCurRank));
        }
    }

    const bool needPreCopy = roundIdx != 0U && isDmaRead_;
    for (u32 channelId = 0U; channelId < curValidChannelsSize; ++channelId) {
        CHK_RET(RunSendRecvOnChannel(
            tempAlgParams, curChannels[channelId], threads[baseQueIdx + channelId], channelId, myRankCclBuffIdx,
            remoteCclBuffIdx, remoteRank, needPreCopy));
    }
    if (curValidChannelsSize > 1U && roundIdx + 1U < commLoops) {
        CHK_RET(PostSyncInterThreadsPerRank(mainThreadCurRank, subThreadsCurRank));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::RunSendRecvOnChannel(
    const TemplateDataParams& tempAlgParams, const ChannelInfo& channel, const ThreadHandle& thread, u32 channelId,
    u32 myRankCclBuffIdx, u32 remoteCclBuffIdx, u32 remoteRank, bool needPreCopy) const
{
    if (needPreCopy && sendSizeSplit_[channelId] > 0U) {
        CHK_RET(PreCopy(
            tempAlgParams, thread, myRankCclBuffIdx, remoteRank, sendSizeSplit_[channelId], sendCountsSplit_[channelId],
            sendOffsetSplit_[channelId]));
    }

    void* remoteCclBuffAddr = channel.remoteCclMem.addr;
    u64 txSourceOffset = 0U;
    u64 txDestinationOffset = 0U;
    u64 rxSourceOffset = 0U;
    u64 rxDestinationOffset = 0U;
    CHK_RET(CalcUserSliceOffset(
        tempAlgParams.sdispls[remoteRank], dataTypeSize_, sendOffsetSplit_[channelId], txSourceOffset));
    CHK_RET(CalcCclSliceOffset(
        remoteCclBuffIdx, tempAlgParams.inputSliceStride, tempAlgParams.buffInfo.hcclBuffBaseOff,
        sendOffsetSplit_[channelId], txDestinationOffset));
    CHK_RET(CalcCclSliceOffset(
        remoteCclBuffIdx, tempAlgParams.inputSliceStride, tempAlgParams.buffInfo.hcclBuffBaseOff,
        recvOffsetSplit_[channelId], rxSourceOffset));
    CHK_RET(CalcUserSliceOffset(
        tempAlgParams.rdispls[remoteRank], dataTypeSize_, recvOffsetSplit_[channelId], rxDestinationOffset));
    const DataSlice txSrcSlice(
        tempAlgParams.buffInfo.inputPtr, txSourceOffset, sendSizeSplit_[channelId], sendCountsSplit_[channelId]);
    const DataSlice txDstSlice(
        remoteCclBuffAddr, txDestinationOffset, sendSizeSplit_[channelId], sendCountsSplit_[channelId]);
    const DataSlice rxSrcSlice(
        remoteCclBuffAddr, rxSourceOffset, recvSizeSplit_[channelId], recvCountsSplit_[channelId]);
    const DataSlice rxDstSlice(
        tempAlgParams.buffInfo.outputPtr, rxDestinationOffset, recvSizeSplit_[channelId], recvCountsSplit_[channelId]);

    const std::vector<DataSlice> txSrcSlices{txSrcSlice};
    const std::vector<DataSlice> txDstSlices{txDstSlice};
    const std::vector<DataSlice> rxSrcSlices{rxSrcSlice};
    const std::vector<DataSlice> rxDstSlices{rxDstSlice};
    const DataInfo sendInfo{channel, {txSrcSlices, txDstSlices}, dataType_};
    const DataInfo recvInfo{channel, {rxSrcSlices, rxDstSlices}, dataType_};
    const SendRecvInfo sendRecvInfo{
        {channel, channel}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_};
    CHK_RET(RunSendRecv(sendRecvInfo, sendInfo, recvInfo, thread, channelId));
    if (!isDmaRead_ && recvSizeSplit_[channelId] > 0U) {
        CHK_RET(PostCopy(
            tempAlgParams, thread, myRankCclBuffIdx, remoteRank, recvSizeSplit_[channelId], recvCountsSplit_[channelId],
            recvOffsetSplit_[channelId]));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::RunSendRecv(
    const SendRecvInfo& sendRecvInfo, const DataInfo& sendInfo, const DataInfo& recvInfo, const ThreadHandle& thread,
    u32 channelId) const
{
    if (isDmaRead_) {
        if (sendSizeSplit_[channelId] > 0U && recvSizeSplit_[channelId] > 0U) {
            return static_cast<HcclResult>(SendRecvRead(sendRecvInfo, thread));
        }
        if (sendSizeSplit_[channelId] > 0U) {
            return static_cast<HcclResult>(SendRead(sendInfo, thread));
        }
        if (recvSizeSplit_[channelId] > 0U) {
            return static_cast<HcclResult>(RecvRead(recvInfo, thread));
        }
        return HCCL_SUCCESS;
    }

    if (sendSizeSplit_[channelId] > 0U && recvSizeSplit_[channelId] > 0U) {
        return static_cast<HcclResult>(SendRecvWrite(sendRecvInfo, thread));
    }
    if (sendSizeSplit_[channelId] > 0U) {
        CHK_RET(static_cast<HcclResult>(SendWrite(sendInfo, thread)));
    }
    if (recvSizeSplit_[channelId] > 0U) {
        CHK_RET(static_cast<HcclResult>(RecvWrite(recvInfo, thread)));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::PreCopyByLoop(
    const std::vector<u32>& commRanks, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads, const TemplateDataParams& tempAlgParams)
{
    for (u32 remoteRank : commRanks) {
        u32 myRankCclBuffIdx = 0U;
        u32 remoteCclBuffIdx = 0U;
        CalcCclBuffIdx(remoteRank, myRankCclBuffIdx, remoteCclBuffIdx);
        (void)remoteCclBuffIdx;
        CHK_PRT_RET(
            myRankCclBuffIdx > (std::numeric_limits<u32>::max() - 1U) / channelsPerRank_,
            HCCL_ERROR("[InsTempAlltoAllVMesh1D][PreCopy] thread index overflows."), HCCL_E_PARA);
        u32 queIdx = myRankCclBuffIdx * channelsPerRank_ + 1U;
        const auto channelIter = channels.find(remoteRank);
        CHK_PRT_RET(
            channelIter == channels.end() || channelIter->second.empty(),
            HCCL_ERROR("[InsTempAlltoAllVMesh1D][PreCopy] no channel for rank[%u].", remoteRank), HCCL_E_PARA);
        const std::vector<ChannelInfo>& curChannels = channelIter->second;
        const u32 curValidChannelsSize = std::min(static_cast<u32>(curChannels.size()), channelsPerRank_);
        CHK_PRT_RET(
            curValidChannelsSize == 0U || queIdx >= threads.size() || curValidChannelsSize > threads.size() - queIdx,
            HCCL_ERROR("[InsTempAlltoAllVMesh1D][PreCopy] insufficient channels or threads."), HCCL_E_INTERNAL);
        CHK_RET(CalcDataSplitByPortGroupCommon(
            tempAlgParams.sendCounts[remoteRank], dataTypeSize_, curChannels, sendCountsSplit_, sendSizeSplit_,
            sendOffsetSplit_, curValidChannelsSize));
        for (u32 channelId = 0U; channelId < curValidChannelsSize; ++channelId, ++queIdx) {
            if (sendSizeSplit_[channelId] > 0U) {
                CHK_RET(PreCopy(
                    tempAlgParams, threads[queIdx], myRankCclBuffIdx, remoteRank, sendSizeSplit_[channelId],
                    sendCountsSplit_[channelId], sendOffsetSplit_[channelId]));
            }
        }
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAlltoAllVMesh1D::PreCopy(
    const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, u32 myRankCclBuffIdx, u32 remoteRank,
    const u64& sendSize, const u64& sendCount, const u64& sendOffset) const
{
    u64 sourceOffset = 0U;
    u64 destinationOffset = 0U;
    CHK_RET(CalcUserSliceOffset(tempAlgParams.sdispls[remoteRank], dataTypeSize_, sendOffset, sourceOffset));
    CHK_RET(CalcCclSliceOffset(
        myRankCclBuffIdx, tempAlgParams.inputSliceStride, tempAlgParams.buffInfo.hcclBuffBaseOff, sendOffset,
        destinationOffset));
    const DataSlice source(tempAlgParams.buffInfo.inputPtr, sourceOffset, sendSize, sendCount);
    const DataSlice destination(tempAlgParams.buffInfo.hcclBuff.addr, destinationOffset, sendSize, sendCount);
    return static_cast<HcclResult>(LocalCopy(thread, source, destination));
}

HcclResult InsTempAlltoAllVMesh1D::PostCopy(
    const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, u32 myRankCclBuffIdx, u32 remoteRank,
    const u64& recvSize, const u64& recvCount, const u64& recvOffset) const
{
    u64 sourceOffset = 0U;
    u64 destinationOffset = 0U;
    CHK_RET(CalcCclSliceOffset(
        myRankCclBuffIdx, tempAlgParams.inputSliceStride, tempAlgParams.buffInfo.hcclBuffBaseOff, recvOffset,
        sourceOffset));
    CHK_RET(CalcUserSliceOffset(tempAlgParams.rdispls[remoteRank], dataTypeSize_, recvOffset, destinationOffset));
    const DataSlice source(tempAlgParams.buffInfo.hcclBuff.addr, sourceOffset, recvSize, recvCount);
    const DataSlice destination(tempAlgParams.buffInfo.outputPtr, destinationOffset, recvSize, recvCount);
    return static_cast<HcclResult>(LocalCopy(thread, source, destination));
}

void InsTempAlltoAllVMesh1D::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.assign(threadNum_ > 0U ? threadNum_ - 1U : 0U, 0U);
}

void InsTempAlltoAllVMesh1D::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    const u32 notifyNum = threadNum_ > 0U ? threadNum_ - 1U : 0U;
    notifyIdxSubToMain.reserve(notifyNum);
    for (u32 notifyIdx = 0U; notifyIdx < notifyNum; ++notifyIdx) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}
} // namespace mc2_ops_hccl
