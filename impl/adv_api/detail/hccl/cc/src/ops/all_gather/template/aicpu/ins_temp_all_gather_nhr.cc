/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_temp_all_gather_nhr.h"
#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "template_utils.h"

namespace mc2_ops_hccl {
namespace {
constexpr u32 POST_COPY_NOTIFY_IDX = 1;
} // namespace

InsTempAllGatherNHR::InsTempAllGatherNHR(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllGatherNHR::~InsTempAllGatherNHR() {}

HcclResult InsTempAllGatherNHR::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    std::vector<HcclChannelDesc> level1Channels;
    std::vector<HcclChannelDesc> myChannelDescs;
    const u64 perDataSize = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    const u64 dataSize = param.DataDes.count * perDataSize;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        const bool isIsolation =
            !(IsAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH) || dataSize <= SMALL_SIZE_512KB);
        CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, subCommRanks_, myChannelDescs, isIsolation));
        for (const auto& channel : myChannelDescs) {
            if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
                level1Channels.push_back(channel);
            }
        }
    } else {
        CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, myChannelDescs));
        level1Channels = myChannelDescs;
    }
    CHK_PRT_RET(
        level1Channels.empty(), HCCL_ERROR("[InsTempAllGatherNHR][CalcRes] level1Channels is empty."),
        HcclResult::HCCL_E_INTERNAL);
    resourceRequest.channels.push_back(level1Channels);
    channelsPerRank_ = CalcChannelsPerRankMin(level1Channels);
    maxChannelsPerRank_ = CalcChannelsPerRank(level1Channels);
    if (channelsPerRank_ > MAX_JETTY_NUM) {
        HCCL_WARNING(
            "[InsTempAllGatherNHR][CalcRes] channelsPerRank_ %u is greater than MAX_JETTY_NUM %u", channelsPerRank_,
            MAX_JETTY_NUM);
    } else {
        HCCL_DEBUG(
            "[InsTempAllGatherNHR][CalcRes] channelsPerRank_ is %u, maxChannelsPerRank_ is %u", channelsPerRank_,
            maxChannelsPerRank_);
    }
    CHK_RET(GetRes(resourceRequest));
    return HCCL_SUCCESS;
}
HcclResult InsTempAllGatherNHR::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = GetThreadNum();
    HCCL_INFO("[InsTempAllGatherNHR][GetRes] threadNum[%u]", threadNum);
    resourceRequest.slaveThreadNum = threadNum - 1;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 2);
    resourceRequest.notifyNumOnMainThread = threadNum - 1;
    return HCCL_SUCCESS;
}
u64 InsTempAllGatherNHR::GetThreadNum() const { return maxChannelsPerRank_ * 2; }

u64 InsTempAllGatherNHR::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 scratchMultiple = templateRankSize_;
    return scratchMultiple;
}

HcclResult InsTempAllGatherNHR::PrepareDataSplitForMultiChannel(const TemplateResource& templateResource)
{
    u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 totalDataCount = tempAlgParams_.sliceSize / dataTypeSize;
    if (templateResource.channels.empty() || templateResource.channels.begin()->second.empty()) {
        dataSplit_.assign(1, tempAlgParams_.sliceSize);
        dataOffset_.assign(1, 0);
        if (tempAlgParams_.tailSize > 0) {
            dataSplitTail_.assign(1, tempAlgParams_.tailSize);
            dataOffsetTail_.assign(1, 0);
        }
        return HCCL_SUCCESS;
    }
    std::vector<u64> elemCountOut;
    CHK_RET(CalcDataSplitByPortGroup(
        totalDataCount, dataTypeSize, templateResource.channels.begin()->second, elemCountOut, dataSplit_,
        dataOffset_));
    if (tempAlgParams_.tailSize > 0) {
        u64 totalDataCountTail = tempAlgParams_.tailSize / dataTypeSize;
        CHK_RET(CalcDataSplitByPortGroup(
            totalDataCountTail, dataTypeSize, templateResource.channels.begin()->second, elemCountOut, dataSplitTail_,
            dataOffsetTail_));
    }

    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::ValidateKernelRunParams(const TemplateResource& templateResource) const
{
    CHK_PRT_RET(
        dataType_ >= HcclDataType::HCCL_DATA_TYPE_RESERVED || DATATYPE_SIZE_TABLE[dataType_] == 0,
        HCCL_ERROR("[InsTempAllGatherNHR] Rank [%u], invalid dataType [%u].", myRank_, static_cast<u32>(dataType_)),
        HcclResult::HCCL_E_PARA);
    const u32 checkDataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(
        tempAlgParams_.sliceSize % checkDataTypeSize != 0 || tempAlgParams_.tailSize % checkDataTypeSize != 0,
        HCCL_ERROR(
            "[InsTempAllGatherNHR] Rank [%u], sliceSize [%llu] or tailSize [%llu] is not aligned to "
            "dataTypeSize [%u].",
            myRank_, tempAlgParams_.sliceSize, tempAlgParams_.tailSize, checkDataTypeSize),
        HcclResult::HCCL_E_PARA);
    CHK_PRT_RET(
        threadNum_ > templateResource.threads.size(),
        HCCL_ERROR(
            "[InsTempAllGatherNHR] Rank [%u], requiredQueNum [%u] is greater than templateQueNum [%zu].", myRank_,
            threadNum_, templateResource.threads.size()),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        channelsPerRank_ == 0 || channelsPerRank_ > maxChannelsPerRank_,
        HCCL_ERROR(
            "[InsTempAllGatherNHR] Rank [%u], channelsPerRank_[%u] is invalid, maxChannelsPerRank_[%u].", myRank_,
            channelsPerRank_, maxChannelsPerRank_),
        HcclResult::HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllGatherNHR] Run start");
    if (tempAlgParams.sliceSize == 0 && tempAlgParams.tailSize == 0) {
        HCCL_INFO("[InsTempAllGatherNHR] Rank [%d], get slicesize zero.", myRank_);
        return HCCL_SUCCESS;
    }
    tempAlgParams_ = tempAlgParams;
    dataType_ = param.DataDes.dataType;
    enableRemoteMemAccess_ = tempAlgParams.enableRemoteMemAccess;
    threadNum_ = GetThreadNum();
    CHK_RET(ValidateKernelRunParams(templateResource));

    isDmaRead_ = IsPcieProtocol(templateResource.channels);
    HCCL_DEBUG("[InsTempAllGatherNHR] Use Dma Read[%d]", isDmaRead_);
    CHK_RET(PrepareDataSplitForMultiChannel(templateResource));
    readLastStepToOutput_ = CanReadLastStepToOutput();
    skipOwnSliceCopy_ = readLastStepToOutput_ && CanSkipOwnSliceCopy();
    HCCL_DEBUG(
        "[InsTempAllGatherNHR] Read last step to output[%d], skip own slice copy[%d]", readLastStepToOutput_,
        skipOwnSliceCopy_);

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        bool postLocalCopyLaunched = false;
        CHK_RET(LocalDataCopy(templateResource.threads, channelIdx));
        if (templateRankSize_ > 1) {
            CHK_RET(RunAllGatherNHR(
                templateResource.threads, templateResource.channels, channelIdx, postLocalCopyLaunched));
        }
        if (!postLocalCopyLaunched) {
            CHK_RET(PostLocalCopy(templateResource.threads[channelIdx], channelIdx));
        }
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }

    HCCL_INFO("[InsTempAllGatherNHR] Run End");
    return HCCL_SUCCESS;
}

bool InsTempAllGatherNHR::CanReadLastStepToOutput() const
{
    return !isDmaRead_ && !enableRemoteMemAccess_ && tempAlgParams_.buffInfo.outBuffType == BufferType::OUTPUT &&
           tempAlgParams_.buffInfo.outputPtr != tempAlgParams_.buffInfo.hcclBuff.addr;
}

bool InsTempAllGatherNHR::CanSkipOwnSliceCopy() const
{
    return tempAlgParams_.buffInfo.inBuffType == BufferType::OUTPUT &&
           tempAlgParams_.buffInfo.outBuffType == BufferType::OUTPUT &&
           tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.outputPtr &&
           tempAlgParams_.buffInfo.inBuffBaseOff == tempAlgParams_.buffInfo.outBuffBaseOff &&
           tempAlgParams_.inputSliceStride == tempAlgParams_.outputSliceStride &&
           tempAlgParams_.inputRepeatStride == tempAlgParams_.outputRepeatStride;
}

bool InsTempAllGatherNHR::IsLastStepReadSlice(u32 algRank) const
{
    for (u32 readSliceIdx : lastStepReadSliceIdxs_) {
        if (readSliceIdx == algRank) {
            return true;
        }
    }
    return false;
}

InsTempAllGatherNHR::SliceCalcInfo InsTempAllGatherNHR::CalcSliceInfo(
    const AicpuNHRStepInfo& stepInfo, u32 rpt, u32 i, u32 channelIdx) const
{
    SliceCalcInfo info;
    info.txIdx = stepInfo.txSliceIdxs[i];
    info.rxIdx = stepInfo.rxSliceIdxs[i];
    info.txPartialOffset = (info.txIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize != 0) ?
                               dataOffsetTail_[channelIdx] :
                               dataOffset_[channelIdx];
    info.rxPartialOffset = (info.rxIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize != 0) ?
                               dataOffsetTail_[channelIdx] :
                               dataOffset_[channelIdx];
    const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
    info.scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;
    info.txScratchOff = info.scratchBase + tempAlgParams_.sliceSize * info.txIdx + info.txPartialOffset;
    info.rxScratchOff = info.scratchBase + tempAlgParams_.sliceSize * info.rxIdx + info.rxPartialOffset;
    info.txSliceSize = (info.txIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize != 0) ?
                           dataSplitTail_[channelIdx] :
                           dataSplit_[channelIdx];
    info.rxSliceSize = (info.rxIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize != 0) ?
                           dataSplitTail_[channelIdx] :
                           dataSplit_[channelIdx];
    return info;
}

HcclResult InsTempAllGatherNHR::BuildStepSlices(
    const ChannelInfo& channelSend, const ChannelInfo& channelRecv, const AicpuNHRStepInfo& stepInfo,
    const u32& channelIdx, StepBuildMode mode, std::vector<DataSlice>& txSrcSlices, std::vector<DataSlice>& txDstSlices,
    std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices)
{
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    void* sendCclBuffAddr = channelSend.remoteCclMem.addr;
    void* recvCclBuffAddr = channelRecv.remoteCclMem.addr;

    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        for (u32 i = 0; i < stepInfo.nSlices; ++i) {
            SliceCalcInfo info = CalcSliceInfo(stepInfo, rpt, i, channelIdx);

            if (mode == StepBuildMode::LAST_STEP_READ_TO_OUTPUT) {
                const u64 rxOutputOff = tempAlgParams_.buffInfo.outBuffBaseOff +
                                        rpt * tempAlgParams_.outputRepeatStride +
                                        tempAlgParams_.outputSliceStride * info.rxIdx + info.rxPartialOffset;
                rxSrcSlices.emplace_back(
                    recvCclBuffAddr, info.rxScratchOff, info.rxSliceSize, info.rxSliceSize / dataTypeSize);
                rxDstSlices.emplace_back(
                    tempAlgParams_.buffInfo.outputPtr, rxOutputOff, info.rxSliceSize, info.rxSliceSize / dataTypeSize);
                if (rpt == 0) {
                    lastStepReadSliceIdxs_.push_back(info.rxIdx);
                }
            } else {
                txSrcSlices.emplace_back(
                    tempAlgParams_.buffInfo.hcclBuff.addr, info.txScratchOff, info.txSliceSize,
                    info.txSliceSize / dataTypeSize);
                txDstSlices.emplace_back(
                    sendCclBuffAddr, info.txScratchOff, info.txSliceSize, info.txSliceSize / dataTypeSize);
                rxSrcSlices.emplace_back(
                    recvCclBuffAddr, info.rxScratchOff, info.rxSliceSize, info.rxSliceSize / dataTypeSize);
                rxDstSlices.emplace_back(
                    tempAlgParams_.buffInfo.hcclBuff.addr, info.rxScratchOff, info.rxSliceSize,
                    info.rxSliceSize / dataTypeSize);
            }
        }
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::RunLastStepReadToOutput(
    const std::vector<ThreadHandle>& threads, const ChannelInfo& channelSend, const ChannelInfo& channelRecv,
    const AicpuNHRStepInfo& stepInfo, const u32& channelIdx, u32 step, bool& postLocalCopyLaunched)
{
    std::vector<DataSlice> txSrcSlices;
    std::vector<DataSlice> txDstSlices;
    std::vector<DataSlice> rxSrcSlices;
    std::vector<DataSlice> rxDstSlices;
    CHK_RET(BuildLastStepReadToOutputSlices(
        channelSend, channelRecv, stepInfo, channelIdx, txSrcSlices, txDstSlices, rxSrcSlices, rxDstSlices));

    if (rxSrcSlices.empty()) {
        return HcclResult::HCCL_SUCCESS;
    }

    const u32 postCopyThreadIdx = channelsPerRank_ + channelIdx;
    CHK_PRT_RET(
        postCopyThreadIdx >= threads.size(),
        HCCL_ERROR(
            "[InsTempAllGatherNHR] post copy thread index[%u] is out of range[%zu].", postCopyThreadIdx,
            threads.size()),
        HcclResult::HCCL_E_INTERNAL);

    CHK_RET(PreSyncInterThreads(threads[channelIdx], {threads[postCopyThreadIdx]}, {POST_COPY_NOTIFY_IDX}));
    CHK_RET(PostLocalCopy(threads[postCopyThreadIdx], channelIdx));
    postLocalCopyLaunched = true;

    const std::vector<DataSlice> emptySlices;
    TxRxChannels sendRecvChannels(channelSend, channelRecv);
    TxRxSlicesList readSlicesList({emptySlices, emptySlices}, {rxSrcSlices, rxDstSlices});
    SendRecvInfo readInfo(sendRecvChannels, readSlicesList, dataType_);
    CHK_PRT_RET(
        SendRecvBatchRead(readInfo, threads[channelIdx]),
        HCCL_ERROR("[InsTempAllGatherNHR] last step read failed (step=%u)", step), HcclResult::HCCL_E_INTERNAL);

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::RunStepNHR(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const u32& channelIdx, u32 step, u32 nSteps, bool& postLocalCopyLaunched)
{
    AicpuNHRStepInfo stepInfo;
    CHK_RET(GetStepInfo(step, nSteps, stepInfo));
    u32 fromRankKey = GetRankFromMap(stepInfo.fromRank);
    u32 toRankKey = GetRankFromMap(stepInfo.toRank);
    CHK_PRT_RET(
        channels.count(fromRankKey) == 0 || channelIdx >= channels.at(fromRankKey).size() ||
            channels.count(toRankKey) == 0 || channelIdx >= channels.at(toRankKey).size(),
        HCCL_ERROR(
            "[InsTempAllGatherNHR][%s] rank[%u] invalid channel access, fromRankKey[%u] toRankKey[%u] channelIdx[%u] "
            "channels.size[%zu] fromChannelSize[%zu] toChannelSize[%zu]",
            __func__, myRank_, fromRankKey, toRankKey, channelIdx, channels.size(),
            channels.count(fromRankKey) != 0 ? channels.at(fromRankKey).size() : 0,
            channels.count(toRankKey) != 0 ? channels.at(toRankKey).size() : 0),
        HCCL_E_INTERNAL);
    const ChannelInfo& channelRecv = channels.at(fromRankKey)[channelIdx];
    const ChannelInfo& channelSend = channels.at(toRankKey)[channelIdx];
    HCCL_DEBUG(
        "[InsTempAllGatherNHR] rank[%d] rankSize[%u] recvFrom[%u] sendTo[%u] step[%u] nSteps[%u] nSlices[%u]", myRank_,
        templateRankSize_, stepInfo.fromRank, stepInfo.toRank, step, nSteps, stepInfo.nSlices);

    const bool readLastStepToOutput = readLastStepToOutput_ && step == nSteps - 1 && stepInfo.nSlices > 1;
    if (readLastStepToOutput) {
        CHK_RET(RunLastStepReadToOutput(
            threads, channelSend, channelRecv, stepInfo, channelIdx, step, postLocalCopyLaunched));
        return HCCL_SUCCESS;
    }

    std::vector<DataSlice> txSrcSlices;
    std::vector<DataSlice> txDstSlices;
    std::vector<DataSlice> rxSrcSlices;
    std::vector<DataSlice> rxDstSlices;
    CHK_RET(BuildNormalStepSlices(
        channelSend, channelRecv, stepInfo, channelIdx, txSrcSlices, txDstSlices, rxSrcSlices, rxDstSlices));

    TxRxSlicesList sendRecvSlicesList({txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices});
    TxRxChannels sendRecvChannels(channelSend, channelRecv);
    SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList, dataType_);

    if (isDmaRead_) {
        CHK_PRT_RET(
            SendRecvRead(sendRecvInfo, threads[channelIdx]),
            HCCL_ERROR("[InsTempAllGatherNHR] sendrecv batch failed (step=%u)", step), HCCL_E_INTERNAL);
    } else {
        CHK_PRT_RET(
            SendRecvBatchWrite(sendRecvInfo, threads[channelIdx]),
            HCCL_ERROR("[InsTempAllGatherNHR] sendrecv batch failed (step=%u)", step), HCCL_E_INTERNAL);
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::RunAllGatherNHR(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const u32& channelIdx, bool& postLocalCopyLaunched)
{
    const u32 nSteps = GetNHRStepNum(templateRankSize_);
    lastStepReadSliceIdxs_.clear();

    for (u32 step = 0; step < nSteps; ++step) {
        CHK_RET(RunStepNHR(threads, channels, channelIdx, step, nSteps, postLocalCopyLaunched));
    }
    return HCCL_SUCCESS;
}

u32 InsTempAllGatherNHR::GetRankFromMap(const u32 algRankIdx) const { return subCommRanks_[0].at(algRankIdx); }
HcclResult InsTempAllGatherNHR::GetStepInfo(u32 step, u32 nSteps, AicpuNHRStepInfo& stepInfo)
{
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    stepInfo.step = step;
    stepInfo.myRank = myAlgRank;
    stepInfo.txSliceIdxs.clear();
    stepInfo.rxSliceIdxs.clear();

    u32 deltaRank = 1 << (nSteps - 1 - step);
    u32 recvFrom = (myAlgRank + templateRankSize_ - deltaRank) % templateRankSize_;
    u32 sendTo = (myAlgRank + deltaRank) % templateRankSize_;

    u32 nSlices = (templateRankSize_ - 1 + (1 << (nSteps - 1 - step))) / (1 << (nSteps - step));
    u32 deltaSliceIndex = 1 << (nSteps - step);
    u32 txSliceIdx = myAlgRank;
    u32 rxSliceIdx = (myAlgRank - (1 << (nSteps - 1 - step)) + templateRankSize_) % templateRankSize_;

    stepInfo.toRank = sendTo;
    stepInfo.fromRank = recvFrom;
    stepInfo.nSlices = nSlices;

    for (u32 i = 0; i < nSlices; i++) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);

        HCCL_DEBUG("[AllGatherNHR][GetStepInfo] i[%u] txSliceIdx[%u] rxSliceIdx[%u]", i, txSliceIdx, rxSliceIdx);

        txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::LocalDataCopy(const std::vector<ThreadHandle>& threads, const u32& channelIdx)

{
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));

    u64 sliceSize = tempAlgParams_.sliceSize;
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u64 partialSliceSize = dataSplit_[channelIdx];
    u64 partialOffset = dataOffset_[channelIdx];
    if (tempAlgParams_.tailSize != 0 && myAlgRank == templateRankSize_ - 1) {
        partialSliceSize = dataSplitTail_[channelIdx];
        partialOffset = dataOffsetTail_[channelIdx];
    }
    for (u64 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
        const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
        const u64 scratchBaseoff = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;

        const u64 inOff = tempAlgParams_.inputSliceStride * myAlgRank + inBaseOff + partialOffset;
        const u64 scOff = tempAlgParams_.sliceSize * myAlgRank + scratchBaseoff + partialOffset;
        if (tempAlgParams_.buffInfo.inputPtr == tempAlgParams_.buffInfo.hcclBuff.addr && inOff == scOff) {
            continue;
        }
        u64 sliceCount = partialSliceSize / dataTypeSize;
        DataSlice srcSlices(tempAlgParams_.buffInfo.inputPtr, inOff, partialSliceSize, sliceCount);
        DataSlice dstSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scOff, partialSliceSize, sliceCount);
        CHK_RET(LocalCopy(threads[channelIdx], srcSlices, dstSlice));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherNHR::PostLocalCopy(const ThreadHandle& thread, const u32& channelIdx)
{
    if (tempAlgParams_.buffInfo.outputPtr == tempAlgParams_.buffInfo.hcclBuff.addr) {
        HCCL_INFO("[InsTempAllGatherNHR] PostLocalCopy skip because output is scratch");
        return HCCL_SUCCESS;
    }
    const u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    for (u32 rpt = 0; rpt < tempAlgParams_.repeatNum; ++rpt) {
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 scratchRepeatStride = tempAlgParams_.sliceSize * templateRankSize_;
        const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * scratchRepeatStride;

        u64 partialSliceSize = dataSplit_[channelIdx];
        u64 partialOffset = dataOffset_[channelIdx];

        for (auto rank : subCommRanks_[0]) {
            u32 algRank = 0;
            CHK_RET(GetAlgRank(rank, subCommRanks_[0], algRank));
            if (readLastStepToOutput_ && skipOwnSliceCopy_ && algRank == myAlgRank) {
                continue;
            }
            if (readLastStepToOutput_ && IsLastStepReadSlice(algRank)) {
                continue;
            }
            if (tempAlgParams_.tailSize != 0 && algRank == templateRankSize_ - 1) {
                partialSliceSize = dataSplitTail_[channelIdx];
                partialOffset = dataOffsetTail_[channelIdx];
            }
            u64 sliceCount = partialSliceSize / dataTypeSize;
            u64 scratchOffset = tempAlgParams_.sliceSize * algRank + scratchBase + partialOffset;
            u64 outOffset = tempAlgParams_.outputSliceStride * algRank + outBaseOff + partialOffset;
            DataSlice srcSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scratchOffset, partialSliceSize, sliceCount);
            DataSlice dstSlice(tempAlgParams_.buffInfo.outputPtr, outOffset, partialSliceSize, sliceCount);
            CHK_RET(LocalCopy(thread, srcSlice, dstSlice));
        }
    }
    return HCCL_SUCCESS;
}
void InsTempAllGatherNHR::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub)
{
    notifyIdxMianToSub.clear();
    u32 threadNum = GetThreadNum();
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMianToSub.push_back(0);
    }
}

void InsTempAllGatherNHR::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = GetThreadNum();
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

} // namespace mc2_ops_hccl
